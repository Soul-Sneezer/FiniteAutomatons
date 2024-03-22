#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "common.h"
#include "list.h"

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

static void initTFunc(List* func[27])
{
	for(int i = 0; i < 26; i++)
	{
		//initList(); 
	}
}

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

	initTFunc(new_fa->functie_tranzitie);

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
	initTFunc(states);

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

static bool verifyWord(FA* fa, char* word)
{
	int n = strlen(word);
	int current_state = fa->S;
	for(int i = 0; i < n; i++)
	{
		current_state = fa->functie_tranzitie[word[i] - 'a'][current_state].elements[0];
	}

	for(int i = 0; i < fa->nr_s_finale; i++)
	{
		if(current_state == fa->S_finale[i])
			return true;
	}

	return false;
}

static void testFA(FA* fa, FILE* input_fd, char* output_file)
{
	FILE* fd = fopen(output_file, "w");

	int buf_index = 0;
	size_t buf_size = 256;
	char* buffer = (char*)malloc(buf_size * sizeof(char));

	newLine(input_fd, &buf_index, &buffer, &buf_size);
	int NrCuv = strToInt(parseWord(buffer, &buf_index));
	printf("%d ", NrCuv);
	while(NrCuv > 0)
	{
		newLine(input_fd, &buf_index, &buffer, &buf_size);
		char* cuvant = parseWord(buffer, &buf_index);
		if(verifyWord(fa, cuvant))
			fprintf(fd, "DA\n");
		else
			fprintf(fd, "NU\n");
		free(cuvant);
		NrCuv--;
	}
}

int main()
{
	FA* fa = initFA();
	FILE* fd = readInput("./tests/input.txt");
	createFA(fd, fa);
	printFAInfo(fa);
//	testFA(fa, fd, "output.txt");
	freeFA(fa);
}
