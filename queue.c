#include "queue.h"
#include "list.h"

void initQueue(Queue* q)
{
	q->front = 0;
	q->list = (List*)malloc(sizeof(List));
	initList(q->list);
}

void freeQueue(Queue* q)
{
	freeList(q->list);
	free(q->list);
	free(q);
}

void push(Queue* q, int value)
{
	addToList(q->list, value);
}

int pop(Queue* q)
{
	if(q->list->current > q->front)
		return q->list->elements[q->front++];
	else
		return -1;
}

bool isEmpty(Queue* q)
{
	return q->list->current <= q->front;
}
