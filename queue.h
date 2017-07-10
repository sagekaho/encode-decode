# ifndef _QUEUE_H
# define _QUEUE_H
# include <stdint.h>
# include <stdbool.h>
# include "huffman.h"

// Code provided by D. Long

typedef treeNode item; // treeNode defined in huffman.h

typedef struct things // used as array to store frequency and pointer
{
	uint64_t val; // holds value of item(frequency)
	item *T; // pointer to item(treeNode)
} things;

typedef struct queue
{
	uint32_t size; // How big is it?
	uint32_t currentLength; // Number of elements currently in queue
	things *Q; // Array of things which point to items
} queue;

// Copies item into struct things
void copyNode(queue *q, item *i, int t); 

queue *newQueue(uint32_t size); // Constructor
void delQueue(queue *q); // Destructor

bool emptyQ(queue *q); // Is it empty?
bool fullQ(queue *q); // Is it full?

bool enqueue(queue *q, item *i); // Add an item
bool dequeue(queue *q, item **i); // Remove from the rear

void printQ(queue *q); // Prints queue for debugging purposes

# endif