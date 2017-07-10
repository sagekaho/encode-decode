// encode.c — Encodes file

# define SIZE 512
# include <unistd.h>    // need for read & system call
# include <stdint.h>    // need for uint64_t
# include <stdio.h>     // need for printf
# include <stdlib.h>    // need for exit
# include <sys/types.h> // need for system call, fstat
# include <sys/stat.h>  // need for system call
# include <fcntl.h>     // need for system call
# include <errno.h>     // need for errno
# include <string.h>    // need for strcmp
# include "huffman.h"
# include "queue.h"
# include "code.h"
# include "bv.h"

int main(int argc, char * argv[])
{
	// Variables used for user input
	int option;
	bool outfile = false;
	bool verbose = false;
	bool input = false;
	bool printing = false;
	char *inputFile;
	char *outputFile;

	// Gets the user input
	while((option = getopt(argc, argv, "i:o:vp")) != -1)
	{
		switch(option)
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
			case 'p': // Prints tree
			{
				printing = true;
				break;
			}
			default: // Should never reach this
			{
				exit(1);
				break;
			}
		}
	}
	// If user did not put any input, exit program
	if(input == false)
	{
		perror("Input not specified");
		exit(errno);
	}

	// Creates histogram
	uint64_t histogram[256];

	// Zeroes out histogram
	for(int i = 0; i < 256; i++)
	{
		histogram[i] = 0;
	}

	// Makes sure tree has at least two elements
	histogram[0] = 1;
	histogram[255] = 1;

	// Opens input file
	int fd;
	fd = open(inputFile, O_RDONLY);
	if (fd == -1)
	{
		perror("Failed to open file");
		exit(errno);
	}
	
	// Gets the stats on how many bytes file is
	// Found out how to use fstat from man page 
	struct stat fileStat;
	if(fstat(fd, &fileStat) < 0)
	{
		perror("Failed to get stats");
		exit(errno);
	}

	unsigned char buffer[SIZE];
	int l;
	unsigned int c;

	// Reads in input file for histogram
	do
	{
		l = read(fd, &buffer, SIZE);
		if(l == -1)
		{
			perror("Failed to read file");
			exit(errno);
		}

		for(int i = 0; i < l; i++)
		{
			c = buffer[i];
			histogram[c]++; // Increments number of occurances
		}

	} while(l == SIZE); // while 512 bytes being read

	// Closes the file 
	int closed;
	closed = close(fd);
	if(closed == -1)
	{
		perror("Failed to close file");
		exit(errno);
	}

	// Makes the queue and stores values into the queue
	queue *storage = newQueue(256);
	uint16_t leafNodes = 0;
	treeNode *line;

	for (int i = 0; i < 256; i++)
	{
		if (histogram[i] >= 1)
		{
			leafNodes++;
			line = newNode(i, true, histogram[i]);
			enqueue(storage, line);
		}
	}
	treeNode *val1;
	treeNode *val2;
	treeNode *together;
	// Makes the tree
	while (!emptyQ(storage))
	{
		dequeue(storage, &val1);

		if (!emptyQ(storage))
		{
			dequeue(storage, &val2);
			together = join(val1, val2);
			enqueue(storage, together);
		}

	}

	// Initializes code table
	code s = newCode();
	code table[256];
	for (uint32_t i = 0; i<256; i++)
	{
		table[i] = newCode();
	}

	// Builds the code according to the position of value on tree
	buildCode(val1, s, table);

	// Reverses the code on the stack so that it could be popped off correctly
	code reverseTable[256];
	for (uint32_t i = 0; i<256; i++)
	{
		reverseTable[i] = newCode();
	}
	uint32_t *popper = malloc(sizeof(uint32_t));
	for(int i = 0; i < 256; i++)
	{
		code tempCode = newCode();
		tempCode = table[i];
		while (popCode(&tempCode, popper))
		{
			pushCode(&reverseTable[i], *popper);
		}
	}

	uint16_t leaves = leafNodes;

	// Finds the number of leaf Nodes
	leafNodes = leafNodes * 3 - 1;

	// If no output file is inputed, print to standard output
	int oFile;
	if (outfile == false)
	{
		oFile = STDOUT_FILENO;
	}
	else
	{
		// Creates text file to write to
		// O_CREAT creates new file, O_EXCL checks if file exists
		// Third argument is there to specify permissions of creat
		oFile = open(outputFile, O_WRONLY | O_EXCL | O_CREAT, 0666);
		if(oFile == -1)
		{
			perror("File already exists");
			exit(errno);
		}
	} 

	if (oFile == -1)
	{
		perror("Failed to open file");
		exit(errno);
	}

	// Writes magic number, file size, and bytes written to output
	uint32_t magicNumber = 0xdeadd00d;
	uint64_t fileSize = fileStat.st_size;
	ssize_t bytesWritten;
	uint16_t treeSize = leafNodes;
	bytesWritten = write(oFile, &magicNumber, sizeof(magicNumber));
	if (bytesWritten == -1)
	{	
		perror("Failed to write magic number to file.");
		exit(errno);
	}	
	bytesWritten = write(oFile, &fileSize, sizeof(fileSize));
	if (bytesWritten == -1)
	{	
		perror("Failed to write fize size to file.");
		exit(errno);
	}
	bytesWritten = write(oFile, &treeSize, sizeof(treeSize));
	if (bytesWritten == -1)
	{	
		perror("Failed to write tree size to file.");
		exit(errno);
	}

	// Prints stats
	if (verbose == true)
	{
		printf("Original %lu bits: ", fileSize * 8);
		printf("leaves %u", leaves);
		printf(" (%u bytes) ", treeSize);
	}

	// Prints instructions to create tree to output file
	dumpTree(val1, leafNodes, oFile);

	// Prints Tree
	if (printing == true)
	{
		printTree(val1, 3);
	}

	// Opens file again to append bits
	fd = open(inputFile, O_RDONLY);
	if (fd == -1)
	{
		perror("Failed to open file");
		exit(errno);
	}

	// Makes bit vector for purpose of appending bits
	bitV *bufferBits = newVec(8); 
	// Used in popping and pushing
	uint32_t *k = malloc(sizeof(uint32_t));
	code tempCode;
	int encodedBits = 0;
	int i = 0;
	uint8_t writeInt;

	// Reads in input file to output code to output file
	do
	{
		l = read(fd, buffer, SIZE);
		if(l == -1)
		{
			perror("Failed to read file");
			exit(errno);
		}

		// Appending bits here
		for(int j = 0; j < l; j++)
		{
			c = buffer[j]; // Gets the symbol
			tempCode = reverseTable[c];
			while (popCode(&tempCode, k)) // While code is not empty
			{
				encodedBits++;
				if (*k == 0)
				{
					clrBit(bufferBits, i);
				}
				else if (*k == 1)
				{
					setBit(bufferBits, i);
				}
				i++;
				if (i == 8) // If bit vector full, write it out
				{
					ssize_t bytesWritten;
					writeInt = bufferBits->v[0];
					bytesWritten = write(oFile, &writeInt, sizeof(writeInt));
					if (bytesWritten == -1)
					{	
						perror("Failed to write to file.");
						exit(errno);
					}
					i = 0;
				}
			}
		}
		
	} while(l == SIZE); // While 512 bytes being read

	// Writes out remaining bits
	bytesWritten = write(oFile, bufferBits->v, sizeof(uint8_t));

	// Closes the files 
	closed = close(fd);
	if(closed == -1)
	{
		perror("Failed to close file");
		exit(errno);
	}
	closed = close(oFile);
	if(closed == -1)
	{
		perror("Failed to close file");
		exit(errno);
	}

	if (verbose == true) // Prints stats
	{
		double percentage = (double)(encodedBits);
		percentage = percentage/((int)fileSize * 8);
		percentage *= 100;
		printf("encoding %d bits (%0.4f%%).\n", encodedBits, percentage);
	}

	// Frees allocated memory
	free(k);
	free(popper);
	free(inputFile);
	free(outputFile);
	delVec(bufferBits);
	delTree(val1);
	delQueue(storage);
	
	return 0;
}
