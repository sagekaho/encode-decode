// Author: Darrell Long
# include <stdint.h>
# include <stdbool.h>
# include <stdlib.h>
# include "queue.h"
# include "huffman.h"
# include "code.h"
# include <stdio.h>

queue *newQueue (uint32_t size) // Constructor
{
	queue *line = (queue *)malloc(sizeof(queue));	
	line -> size = size;
	line -> currentLength = 0;
	line -> Q = calloc(size, sizeof(things)); // array of struct things
	return line;
}

void delQueue (queue *q) // Destructor
{
	free(q->Q); // frees array of struct things 
	free(q);
	return;
}

bool emptyQ (queue *q) // Is it empty?
{
	return q->currentLength == 0;
}
bool fullQ (queue *q) // Is it full?
{
	return q->currentLength == q->size;
}

void copyNode(queue *q, item *i, int t) // Copies node
{
	q->Q[t].T = i;
}

bool enqueue (queue *q, item *i) // Add an item
{
	if (fullQ(q))
	{
		return false; 
	}
	else 
	{
		if (emptyQ(q)) // Becomes first element
		{   
			(q->Q[0]).val = i->count; // Sets frequency
			copyNode(q, i, 0); // Copies treeNode
			(q->currentLength)+=1; // Increments the current length
		}
		else // Put in and then run insertion sort, descending order
		{
			// Item gets put after current last element
			(q->Q[q->currentLength]).val = i->count;
			copyNode(q, i, q->currentLength);
			(q->currentLength)+=1;

			// Runs insertion sort on array
			uint64_t temp, j;
			item *tempItem;
			for(int k=1; k<(int)q->currentLength; k++)
			{
				temp = (q->Q[k]).val; // Stores value
				tempItem = (q->Q[k]).T; // Stores pointer to item

				j = k - 1;
				while(((int)j >= 0 ) && ((int)(q->Q[j]).val) < (int)temp)
				{
					(q->Q[j + 1]).val = (q->Q[j]).val;
					(q->Q[j + 1]).T = (q->Q[j]).T;
					j -= 1;
				}

				(q->Q[j + 1]).val = temp;
				(q->Q[j + 1]).T = tempItem;
			}		
		}
		return true;
	}
}

bool dequeue (queue *q, item **i) // Remove from the rear
{
	if (emptyQ(q))
	{
		return false; 
	}
	else 
	{
		// Decrements current length but keeps items in array
		(q->currentLength)--;
		*i = q->Q[q->currentLength].T;
		(q->Q[q->currentLength]).val = -1; 
		return true;
	}
}

void printQ(queue *q) // For debugging purposes
{
	printf("Queue Size: %i, ", (int)q->size);
	printf("Current Length: %i\n", (int)q->currentLength);
	for(int i=0; i<((int)(q->currentLength)); i++)
	{
		printf("Index %c: %i\n", ((q->Q[i]).T)->symbol, (int)(q->Q[i]).val);
	}
	printf("\n");
	return;
}