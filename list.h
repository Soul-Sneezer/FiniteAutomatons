#ifndef LFA_LIST
#define LFA_LIST

#define NEW_SIZE(n) ((n) == 0 ? 8 : (n * 2))
#define GROW_LIST(type, pointer, oldCount, newCount) \
	(type*)reallocate(pointer, sizeof(type) * (oldCount), sizeof(type) * (newCount))
#define FREE_LIST(type, pointer, oldCount) \
	reallocate(pointer, sizeof(type) * (oldCount), 0)

typedef struct
{
	int capacity;
	int current;
	int* elements; 
} List;

void initList(List* list);
void addToList(List* list, int n);
void freeList(List* list);

#endif
