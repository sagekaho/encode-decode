# include <stdint.h>
# include <stdbool.h>
# include <stdlib.h>
# include "huffman.h"
# include "stack.h"

stack *newStack (void) //Constructor
{
	stack *s = (stack *)malloc(sizeof(stack));	
	s -> size = 256; 
	s -> top = 0;
	s -> entries = malloc(sizeof(items)*(s->size));
	return s;
}

void delStack (stack *s) //Destructor
{
	free(s->entries);
	free(s);
}

bool empty (stack *s) //Is it empty?
{
	return s->top == 0;
}

bool full (stack *s) //Is it full?
{
	return s->top == s->size;
}

items pop (stack *s) //Returns the top item
{
	items t;
	if (!empty(s))
	{
		t = s->entries[--s->top];
	}
	else 
	{
		t.count = 0;
	}
	return t;
}

void push (stack *s, items t) //Adds an item to the top
{
	if (!full(s))
	{
		s->entries[s->top++] = t;
		s->size++;
	}
	return;
}

void printStack (stack *s) // Prints Stack for debugging purposes
{
	for(uint32_t i = 0; i < s->top; i++)
	{
		printTree(&(s->entries[i]), 3);
	}
}