# ifndef _HUFFMAN_H
# define _HUFFMAN_H
# include <stdint.h>
# include <stdbool.h>
# include "code.h"

# ifndef NIL
# define NIL (void *) 0
# endif

// Code provided by D. Long

typedef struct DAH treeNode;

struct DAH
{
	uint8_t symbol;
	uint64_t count;
	bool leaf;
	treeNode *left, *right;
};

// New node , with symbols , leaf or not , a count associated with it
treeNode *newNode(uint8_t s, bool l, uint64_t c);

// Delete a tree
treeNode *delTree(treeNode *t);

// Dump a Huffman tree onto a file
void dumpTree (treeNode *t, int file, int openF);

// Parse a Huffman tree to build codes
void buildCode(treeNode *t, code s, code table [256]);

void delNode (treeNode *h);

static inline int8_t compare(treeNode *l, treeNode *r)
{
	return l->count - r->count; // l < r if negative , l = r if 0 , ...
}

void printTree(treeNode *t, int depth); // Prints tree

treeNode *join(treeNode *l, treeNode *r); // Join two subtrees
# endif