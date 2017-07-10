// decode.c — Decodes file

# include <unistd.h> 	// need for read & system call
# include <stdint.h> 	// need for uint32_t
# include <stdio.h>  	// need for printf
# include <stdlib.h> 	// need for exit and atoi
# include <sys/types.h> // need for system call, fstat
# include <sys/stat.h>  // need for system call
# include <fcntl.h>     // need for system call
# include <errno.h>     // need for errno
# include <string.h>    // need for strcmp
# include "huffman.h"
# include "stack.h"
# include "bv.h"

typedef treeNode items;

// Loads the tree from saved tree
treeNode *loadTree(uint8_t savedTree[], uint16_t treeBytes)
{
    // Creates new stack for treeNodes
	stack *myStack = newStack();

	// Keeps looping until length of tree reached
	for(int i = 0; i < (int)treeBytes; i++)
	{
		if(savedTree[i] == 'L') // Means leaf in next byte
		{
			// Stores symbol in new node
			treeNode *T = newNode(savedTree[++i], true, 0);

			// Pushes that node onto the stack
			push(myStack, *T);
			delNode(T);
		}
		else if(savedTree[i] == 'I') // Means join two nodes
		{
			// Pop top two nodes off of stack
			treeNode nodeR = pop(myStack); 
			treeNode nodeL = pop(myStack);

			// Creates pointers to left and right
			treeNode *right = newNode(nodeR.symbol, nodeR.leaf, nodeR.count);
			right->right = nodeR.right;
			right->left = nodeR.left;

			treeNode *left = newNode(nodeL.symbol, nodeL.leaf, nodeL.count);
			left->right = nodeL.right;
			left->left = nodeL.left;

			// Joins the two nodes with a node
			treeNode *together = join(left, right);

			// Pushes that new node back into the stack
			push(myStack, *together);
			delNode(together);
		}
	}
	// Pops the top node off the stack
	treeNode doneTree = pop(myStack);

	// Creates a pointer to the root of the tree
	treeNode *all = newNode(doneTree.symbol, doneTree.leaf, doneTree.count);
	all->left = doneTree.left;
	all->right = doneTree.right;

	delStack(myStack);

	return all;
}


int main(int argc, char * argv[])
{
	int c;
	bool outfile = false;
	bool verbose = false;
	bool input;
	char *inputFile;
	char *outputFile;

	// Gets the user input
	while((c = getopt(argc, argv, "i:o:v")) != -1)
	{
		switch(c)
		{
			case 'i': // Input file, must be specified
			{
				input = true;
				inputFile = strdup(optarg);
				break;
			}
			case 'o': // Output file, optional, else writes to stdout
			{
				outfile = true;
				outputFile = strdup(optarg);
				break;
			}
			case 'v': // Verbose mode prints helpful info
			{
				verbose = true;
				break;
			}
			default: // Should never reach this
			{
				exit(1);
				break;
			}
		}
	}

	if(input == false)
	{
		perror("Input not specified");
		exit(errno);
	}
	
	// Opens file and prints error if fails
	int fd;
	fd = open(inputFile, O_RDONLY);
	if (fd == -1)
	{
		perror("Failed to open file");
		exit(errno);
	}

	// Checks for magic number, displays error if not same
	uint32_t magicNumber;
	int bytesRead;

	// Reads in magic number, first 4 bytes of file
	bytesRead = read(fd, &magicNumber, sizeof(magicNumber));
	if (bytesRead == -1)
	{
		perror("Failed to read magic number");
		exit(errno);
	}

	// Making our magic number
	uint32_t deadDood = 0xdeadd00d;
	
	// Exits program and prints error if numbers aren't the same
	if(magicNumber != deadDood)
	{
		perror("Magic Number not found");
		exit(errno);
	}

	// Reads in size of oFile
	uint64_t fileSize;
	bytesRead = read(fd, &fileSize, sizeof(fileSize));
	if (bytesRead == -1)
	{
		perror("Failed to read filesize");
		exit(errno);
	}

	// Reads in size of tree
	uint16_t treeBytes;
	bytesRead = read(fd, &treeBytes, sizeof(treeBytes));
	if (bytesRead == -1)
	{
		perror("Failed to read tree bytes");
		exit(errno);
	}

	// Reads in entire tree and stores in array savedTree
	uint8_t savedTree[treeBytes];
	bytesRead = read(fd, &savedTree, sizeof(savedTree));
	if (bytesRead == -1)
	{
		perror("Failed to read saved tree");
		exit(errno);
	}

	if(verbose == true) // Prints helpful info
	{
		printf("Original %i bits: ", (int)fileSize*8);
		printf("tree (%i)\n", (int)treeBytes);
	}

	// Loads the tree and stores in root
	treeNode *root = loadTree(savedTree, treeBytes);

	// Holds the characters needed for output
	char allText[fileSize];

    // Recieved help from TA on this part 
    // Equivalent of stepTree, uses bits to get to characters
	int decoded = 0;
	uint8_t buffer[4096];
	int n;
	// Creating a pointer to the root
	treeNode *ptr = root;
	do
	{
		// Read in 4096 bytes to buffer
		n = read(fd, &buffer, 4096);
		if(n == -1)
		{
			perror("Failed to read file to buffer");
			exit(errno);
		}

		// Make length of our bit vector to equal bits read
		bitV *bufferBits = (bitV *) malloc(sizeof(bitV));
		bufferBits->l = n*8;

		// Puts the buffer in our bit vector
		bufferBits->v = buffer;

		// Loops through entire bit vector
		for(int j=0; j<(int)(bufferBits->l); j++)
		{
			// Gets value of current bit
			if (valBit(bufferBits, j) == 0) 
			{
				// Move pointer to left of tree
				ptr = ptr -> left;
			}
			else // valBit is 1
			{
				// Move pointer to right of tree
				ptr = ptr -> right;
			}
			// If we've hit a leaf
			if (ptr -> leaf) 
			{
				// Write symbol to our array
				allText[decoded++] = ptr->symbol;
				// Reset the pointer to the root
				ptr = root;
			}
		}
		// Frees the bitvector
		// Don't use delVec since didn't allocate memory for v->v
		free(bufferBits);

	}while(n == 4096); // Buffer is reading in 4096 bytes
	
	delTree(root);

	// If no output file specified, print to standard out
	if(outfile == false)
	{
		// Prints out decoded file
		for(int i=0; i<(int)fileSize; i++)
		{
			printf("%c", allText[i]);
		}
	}
	
	int bytesWritten;

	// Write to oFile
	if(outfile == true)
	{
		int fp;
		// Creates text file to write to
		// O_CREAT creates new file, O_EXCL checks if file exists
		// Third argument is there to specify permissions of creat
		fp = open(outputFile, O_WRONLY | O_EXCL | O_CREAT, 0666);
		if(fp == -1)
		{
			perror("File already exists");
			exit(errno);
		}

		bytesWritten = write(fp, allText, sizeof(allText));
		if(bytesWritten == -1)
		{
			perror("Failed to write to file");
			exit(errno);
		}
		int closeW;
		closeW = close(fp);
		if(closeW == -1)
		{
			perror("Failed to close file");
			exit(errno);
		}
	}

	// Frees things that were strduped
	free(inputFile);
	if(outfile == true)
	{
		free(outputFile);
	}

	// Closes file decoded and prints error if it fails
	int closed;
	closed = close(fd);
	if(closed == -1)
	{
		perror("Failed to close file");
		exit(errno);
	}

	return 0;
}