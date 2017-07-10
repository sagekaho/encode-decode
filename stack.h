# ifndef _STACK_H
# define _STACK_H
# include <stdint.h>
# include <stdbool.h>
# include "huffman.h"

// Code provided by D. Long

typedef treeNode items;

typedef struct stack 
{
	uint32_t size; // How big?
	uint32_t top; // Where's the top?
	items *entries; // Array to hold it (via calloc)
} stack;

stack *newStack(void); // Constructor
void delStack(stack *); // Destructor

items pop (stack *); // Returns the top item
void push(stack *, items); // Adds an item to the top

bool empty(stack *); // Is it empty?
bool full(stack *); // Is it full?
void printStack (stack *s); // Prints the Stack

# endif