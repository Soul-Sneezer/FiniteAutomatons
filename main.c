#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "common.h"
#include "list.h"
#include "queue.h"

typedef struct
{
	int key;
	int value;
} Entry;

typedef struct
{
	int nr_stari;
	int nr_tranzitii;

	bool alphabet[27];

	int S;
	int nr_s_finale;
	int* S_finale;

	List* functie_tranzitie[27]; // e o lista de liste  dinamice
} FA;

// https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
static unsigned int hash(unsigned int x, unsigned int size) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x % size;
}

static FA* initFA()
{
	FA* new_fa = (FA*)malloc(sizeof(FA));
	new_fa->nr_stari = 0;
	new_fa->nr_tranzitii = 0;
	new_fa->S = 0;
	new_fa->S_finale = NULL;

	return new_fa;
}

static void freeFA(FA* fa)
{
	for(int i = 0; i < 26; i++)
	{
		if(fa->alphabet[i])
		{
			for(int j = 0; j < fa->nr_stari; j++)
				freeList(&fa->functie_tranzitie[i][j]);
		}
	}

	fa = initFA();
}

static int strToInt(char* s)
{
	int nr = 0;
	for(int i = 0; s[i] != '\0'; i++)
	{
		nr = nr * 10 + (int)(s[i] - '0');
	}
	return nr;
}

static bool isDelimiter(char c)
{
	return (c == ' ' || c == '\t' || c == '\r'); // in caz ca vreau sa adaug si alte caractere pe viitor
}

static bool endOfFile(char c)
{
	return (c == '\0');
}

static bool endOfLine(char c)
{
	return (c == '\n' || c ==  '\v' || c == '\e');
}

static char* parseWord(char* buffer, int* current_index)
{
	int start_index = *current_index;
	while(!endOfFile(buffer[*current_index]) && !isDelimiter(buffer[*current_index]) && !endOfLine(buffer[*current_index]))
	{
		(*current_index)++;
	}

	int size = (*current_index) - start_index;
	char* new_word = (char*)malloc((size + 1) * sizeof(char));
	new_word[size] = '\0';

	for(int i = 0; i < size; i++)
	{
		new_word[i] = buffer[start_index + i];
	}


	if(!endOfFile(buffer[*current_index]) && buffer[*current_index] != '\n')
		(*current_index)++;
	return new_word;
}

static int findEntry(Entry* entries, int size, int key)
{
	uint32_t hash_value = hash(key, size);
	while(entries[hash_value].key != key)
	{
		hash_value = (hash_value + 1) % size;
	}

	return entries[hash_value].value;
}

static FILE* readInput(char* source)
{
	FILE* fd = fopen(source, "r");

	return fd;
}

static void newLine(FILE* fd, int* buf_index, char** buffer, size_t* buf_size)
{
	getline(buffer, buf_size, fd);
	*buf_index = 0;
}

static void createFA(FILE* fd, FA* fa)
{
	size_t buf_size = 256;
	char* buffer = (char*)malloc(buf_size * sizeof(char));
	int buf_index = 0;
	getline(&buffer, &buf_size, fd); // numarul de stari ale automatului N
	int N = strToInt(parseWord(buffer, &buf_index));
	fa->nr_stari = N;

	char* state_name = (char*)malloc(buf_size * sizeof(char));
	newLine(fd, &buf_index, &buffer, &buf_size);

	Entry* state_names = (Entry*)malloc(2 * N * sizeof(Entry));
	bool alphabet[27];
	int current = 0;
	for(int i = 0; i < 2 * N; i++)
	{
		state_names[i].value = -1;
	}

	for(int i = 0; i < 27; i++)
	{
		alphabet[i] = false;
	}

	while(!endOfFile(buffer[buf_index]) && buffer[buf_index] != '\n')
	{
		state_name = parseWord(buffer, &buf_index);
		int nr = strToInt(state_name);
		uint32_t hash_value = hash(nr, 2 * N);
		while(state_names[hash_value].value != -1)
		{
			hash_value = (hash_value + 1) % (2 * N);;
		}
		state_names[hash_value].key = nr;
		state_names[hash_value].value = current++;
	}
	newLine(fd, &buf_index, &buffer, &buf_size);
	int M = strToInt(parseWord(buffer, &buf_index));
	fa->nr_tranzitii = M;
	List* states[27];

	while(M > 0)
	{
		newLine(fd, &buf_index, &buffer, &buf_size);
		int x = strToInt(parseWord(buffer, &buf_index));
		x = findEntry(state_names, 2 * N, x);

		int y = strToInt(parseWord(buffer, &buf_index));
		y = findEntry(state_names, 2 * N, y);

		char* l = parseWord(buffer, &buf_index);

		int index;
		if(l[0] == '#')
			index = 26;
		else
			index = l[0] - 'a';

		if(alphabet[index] == false)	// litera face parte din alfabetul pentru DFA
		{
			alphabet[index] = true;
			fa->functie_tranzitie[index] = (List*)malloc(N * sizeof(List));
			for(int i = 0; i < N; i++)
			{
				initList(&fa->functie_tranzitie[index][x]);
			}
		}

		addToList(&fa->functie_tranzitie[index][x], y);
		M--;
	}
	
	for(int i = 0; i < 27; i++)
	{
		if(alphabet[i] == true)
			fa->alphabet[i] = true;
	}
	
	newLine(fd, &buf_index, &buffer, &buf_size);
	fa->S = findEntry(state_names , 2 * N, strToInt(parseWord(buffer, &buf_index)));
	
	newLine(fd, &buf_index, &buffer, &buf_size);
	int NrF = strToInt(parseWord(buffer, &buf_index));
	fa->nr_s_finale = NrF;
	fa->S_finale = (int*)malloc(NrF * sizeof(int));

	newLine(fd, &buf_index, &buffer, &buf_size);
	current = 0;
	while(NrF > 0)
	{
		fa->S_finale[current++] = findEntry(state_names, 2 * N, strToInt(parseWord(buffer, &buf_index)));
		NrF--;
	}
	free(state_names);
}


static void printFAInfo(FA* fa)
{
	printf("Nr stari: %d\n", fa->nr_stari);
	printf("Nr tranzitii: %d\n", fa->nr_tranzitii);
	printf("Alfabet: ");
	for(int i = 0; i < 27; i++)
	{
		if(i != 26 && fa->alphabet[i])
			printf("%c ", 'a' + i);
		else if(i == 26 && fa->alphabet[i])
			printf("# ");
	}
	printf("\nTranzitii: \n");
	for(int i = 0; i < 27; i++)
	{
		for(int j = 0; j < fa->nr_stari; j++)
		{
			if(i != 26 && fa->alphabet[i])
			{
				for(int k = 0; k < fa->functie_tranzitie[i][j].current; k++)
					printf("%d %c -> %d\n", j, i + 'a', fa->functie_tranzitie[i][j].elements[k]);
			}
			else if(i == 26 && fa->alphabet[i])
			{
				for(int k = 0; k < fa->functie_tranzitie[i][j].current; k++)
					printf("%d # -> %d\n", j, fa->functie_tranzitie[26][j].elements[k]);
			}
		}
	}
	printf("Stare initiala: %d\n", fa->S);
	
	printf("Stari finale: ");
	for(int i = 0; i < fa->nr_s_finale; i++)
	{
		printf("%d ", fa->S_finale[i]);
	}
	
}

static Queue* fromLambda(FA* fa, int n, Queue* q)
{
	List* set = (List*)malloc(sizeof(List));	
	initList(set);

	while(!isEmpty(q))
	{
		int start_state = pop(q);

		bool f[n];

		for(int i = 0; i < n; i++)
		{
			f[i] = false;
		}

		Queue* q2 = (Queue*)malloc(sizeof(Queue));
		initQueue(q2);
		f[start_state] = true;
		push(q2, start_state);

		while(!isEmpty(q2))
		{
			int current_state = pop(q2);
			addToList(set, current_state);

			List* list = fa->functie_tranzitie[26];
			for(int i = 0; i < list->current; i++)
			{
				f[list->elements[i]] = true;	
				push(q2, list->elements[i]);
			}
		}
	}

	Queue* q3 = (Queue*)malloc(sizeof(Queue));
	initQueue(q3);

	for(int i = 0; i < set->current; i++)
	{
		push(q3, set->elements[i]);
	}
	free(set);
	return q3;
}

static void verifyLFAWord(FA* fa, char* word, int n, int index, int start_state, bool* result)
{
	if(*result ==  true)
		return;
	if(index == n)
	{
		for(int i = 0; i < fa->nr_s_finale; i++)
		{
			if(start_state == fa->S_finale[i])
			{
				*result = true;
			}
		}

		return;
	}

	Queue* q1 = (Queue*)malloc(sizeof(Queue));
	Queue* q2 = (Queue*)malloc(sizeof(Queue));
	Queue* q3 = (Queue*)malloc(sizeof(Queue));
	initQueue(q1);
	initQueue(q2);
	initQueue(q3);
	push(q1, start_state);
	int c = word[index] - 'a';
	if(c < 0 || c > 25)
		c = 26;
	List list = fa->functie_tranzitie[c][start_state];
	
	for(int i = 0; i < list.current; i++)
	{
		q3 = fromLambda(fa, n, q1);

		initQueue(q1);

		List* set = (List*)malloc(sizeof(List));
		initList(set);

		while(!isEmpty(q3))
		{
			start_state = pop(q3);
			List*	list = fa->functie_tranzitie[c];
			for(int i = 0; i < list->current; i++)
			{
				addToList(set, list->elements[i]);
			}
		}

		if(set->capacity != 0)
		{
			for(int j = 0; j < set->current; j++)
			{
				push(q1, set->elements[j]);
			}
			freeList(set);
			index++;
		}
		else
		{
			freeQueue(q1);
			freeQueue(q2);
			freeQueue(q3);
			return;
		}
	}

	q2 = fromLambda(fa, n, q1);

	for(int i = 0; i < fa->nr_s_finale; i++)
	{
		for(int j = 0; j < q2->list->current; j++)
		{
			if(fa->S_finale[i] == q2->list->elements[j])
			{
				*result = true;
			}
		}
	}
	
	freeQueue(q1);
	freeQueue(q2);
	freeQueue(q3);
	return;
}

static void verifyWord(FA* fa, char* word, int n, int index, int start_state, bool* result, int* path)
{
	if(*result == true)
		return;
	if(index == n)
	{
		for(int i = 0; i < fa->nr_s_finale; i++)
		{
			if(start_state == fa->S_finale[i])
			{
				printf("\n");
				for(int i = 0; i < n; i++)
					printf("%d->",path[i]);
				printf("%d", start_state);
				*result = true;
			}
		}

		return;
	}
	
	List list = fa->functie_tranzitie[word[index] - 'a'][start_state];
	
	for(int i = 0; i < list.current; i++)
	{
		path[index] = start_state;
		verifyWord(fa, word, n, index + 1, list.elements[i], result, path);
	}
	if(fa->alphabet[26]) 
	{
		List list2 = fa->functie_tranzitie[26][start_state];
		for(int i = 0; i < list2.current; i++)
		{
			if(list2.elements[i] != start_state)
				verifyWord(fa, word, n, index, list2.elements[i], result, path);
		}
	}
	return;
}

static void testFA(FA* fa, FILE* input_fd, char* output_file)
{
	FILE* fd = fopen(output_file, "w");

	int buf_index = 0;
	size_t buf_size = 256;
	char* buffer = (char*)malloc(buf_size * sizeof(char));

	newLine(input_fd, &buf_index, &buffer, &buf_size);
	int NrCuv = strToInt(parseWord(buffer, &buf_index));
	while(NrCuv > 0)
	{
		newLine(input_fd, &buf_index, &buffer, &buf_size);
		char* cuvant = parseWord(buffer, &buf_index);
		bool result = false;
		bool loop = false;
		int* path = (int*)malloc(strlen(cuvant) * sizeof(int));
		
		if(!fa->alphabet[26])
			verifyWord(fa, cuvant, strlen(cuvant), 0, fa->S, &result, path);
		else
			verifyLFAWord(fa, cuvant, strlen(cuvant), 0, fa->S, &result);
		if(result)
			fprintf(fd, "DA\n");
		else
			fprintf(fd, "NU\n");

		free(cuvant);
		free(path);
		NrCuv--;
	}
}

int main()
{
	FA* fa = initFA();
	FILE* fd = readInput("./tests/input6.txt");
	createFA(fd, fa);
	printFAInfo(fa);
	testFA(fa, fd, "output.txt");
	freeFA(fa);
}
