//Author: Darrell Long
# include <stdint.h>
# include <stdbool.h>
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h> // need for read & system call
# include <sys/types.h> // need for system call, fstat
# include <sys/stat.h> // need for system call
# include <fcntl.h> // need for system call
# include <errno.h> // need for errno
# include <ctype.h>
# include "code.h"

# ifndef NIL
# define NIL (void *) 0
# endif

typedef struct DAH treeNode;

struct DAH
{
	uint8_t symbol;
	uint64_t count;
	bool leaf;
	treeNode *left, *right;
};

// New node, with symbols, leaf or not, a count associated with it
treeNode *newNode (uint8_t s, bool l, uint64_t c)
{
	treeNode *tree = malloc(sizeof(treeNode));	
	tree -> count = c;
	tree -> leaf = l;
	tree -> symbol = s;
	tree -> left = NULL;
	tree -> right = NULL;
	return tree;
}

treeNode *join(treeNode *l, treeNode *r) // Joins two subtrees
{
	treeNode *together = newNode('$', false, (l->count+r->count));
	together->left = l;
	together->right = r;
	return together;
}

// TA was explaining this concept to us, followed his suggestions/psuedo code
// This recursively stores symbols for trees into n
int32_t dumpTreehelp(treeNode *tree, uint8_t buf[], int n, int file)
{
	if (n <= file) // Makes sure it only writes according to file size
	{
		if (tree != NULL)
		{
			if (tree->leaf) // If leaf, write L followed by the symbol
			{
				buf[n++] = 'L';
				buf[n++] = tree->symbol;
			}
			// Calls function recursively, once done writes I for interior node
			else
			{
				n = dumpTreehelp(tree->left, buf, n, file);
				n = dumpTreehelp(tree->right, buf, n, file);
				buf[n++] = 'I';
			}
		}
	}
	return n;
}

// Followed suggestions/psuedo code of TA
// Dumps a Huffman tree onto a file 
void dumpTree (treeNode *t, int file, int openF) 
{
	// Makes buffer of file size length
	uint8_t buf[file];
	ssize_t bytesWritten;

	// Calls our helper function to get symbols
	dumpTreehelp(t, buf, 0, file);

	// Writes tree to file
	bytesWritten = write(openF, buf, file);
	if (bytesWritten == -1)
	{	
		perror("Failed to write to file.");
		exit(errno);
	}
	return;
}

// Parses a Huffman tree to build codes
// Recieved help from TA on this part
// Pushes and pops onto code s to copy into code table
void buildCode (treeNode *t, code s, code table[256]) 
{
	// k stores popped value
	uint32_t *k = malloc(sizeof(uint32_t));
	if (t->leaf) // If leaf, copy into code table
	{
		table[t->symbol] = s; 
	}
	else
	{
		pushCode(&s, 0);
		buildCode(t->left, s, table);
		popCode(&s, k);
		pushCode(&s, 1);
		buildCode(t->right, s, table);
		popCode(&s, k);
	}
	free(k);
	return;
}

void delNode (treeNode *h) // Deletes a leaf
{
	free(h);
	return;
}

treeNode *delTree (treeNode *t) // Deletes a tree
{
	if (t != NULL)
	{
		delTree(t->left);
		delTree(t->right);
		delNode(t);
	}
	return NULL;
}

// Code provided by D. Long
static inline int8_t compare (treeNode *l, treeNode *r) // Compares original tree with new
{
	return l->count - r->count; // l<r if negative, 1 = r if 0, ...
}

// Code provided by D. Long
static inline void spaces(int c) 
{ 
	for (int i = 0; i < c; i += 1) 
	{ 
		putchar(' '); 
	} 
	return; 
}

// Code provided by D. Long
void printTree(treeNode *t, int depth)
{
	if (t) 
	{
		printTree(t->left, depth + 1);

		if (t->leaf)
		{
			if (isalnum(t->symbol))
			{
				spaces(4 * depth); printf("'%c' (%lu)\n", t->symbol, t->count);
			}
			else
			{
			spaces(4 * depth); printf("0x%X (%lu)\n", t->symbol, t->count);
			}
		}
		else
		{
			spaces(4 * depth); printf("$ (%lu)\n", t->count);
		}

		printTree(t->right, depth + 1); 
	}

	return;
}

