#include <stdlib.h>
#include <stddef.h>
#include "list.h"


List* initList()
{
	List* list = (List*)malloc(sizeof(List));
	list->capacity = 0;
	list->current = 0;
	list->elements = NULL;
}

void freeList(List* list)
{
	FREE_LIST(int, list->elements, list->capacity);
	initList(list);
}

void addToList(List* list, int n)
{
	if(list->current + 1 > list->capacity)
	{
		int capacity = NEW_SIZE(list->capacity);
		list->elements = GROW_LIST(int, list->elements, list->capacity, capacity);
		list->capacity = capacity;
	}

	list->elements[list->current] = n;
	list->current++;
}
