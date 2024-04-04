#ifndef LFA_QUEUE
#define LFA_QUEUE

#include <stdbool.h>
#include "list.h"

typedef struct
{ 
	List* list; // basically the elements.current represents the back
	int front; 
} Queue;

void initQueue(Queue* q);
void freeQueue(Queue* q);
void push(Queue* q, int value);
int pop(Queue* q);
bool isEmpty(Queue *q);

#endif
