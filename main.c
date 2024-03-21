#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "common.h"
#include "list.h"

typedef struct
{
	int nr_stari;
	int nr_tranzitii;
	int S; 
	int* S_finale;
} DFA;

static bool isDelimiter(char c)
{
	return !(c - ' '); // in caz ca vreau sa adaug si alte caractere pe viitor
}

static bool endOfFile(char c)
{
	return c == '\0';
}

static bool endOfLine(char c)
{
	return c == '\n';
}

static char* parseWord(char* buffer, int* current_index)
{
	int start_index = *current_index;
	while(!endOfFile(buffer[*current_index]) && !endOfLine(buffer[*current_index]))
	{
		if(isDelimiter(buffer[*current_index]))
		{
			size_t size = *current_index - start_index;
			char* new_word = (char*)malloc((size + 1) * sizeof(char));
			new_word[size] = '\0';
			strncpy(buffer + start_index, new_word, size);
			(*current_index)++;
			return new_word;
		}
		(*current_index)++;
	}

	return NULL;
}

static void createStates(DFA* dfa)
{
	getline(&buffer, &buf_size, fd); // numarul de stari ale automatului N
	int N = strToInt(buffer);
	dfa.nr_stari = N;

	char* state;
	getline(&buffer, &buf_size, fd); // starile automatului
	
	while((state = parseWord(buffer, &buf_index)) != NULL && N > 0)
	{
		int nr = strToInt(state);
		N--; // schimba-l numai daca ai gasit o stare distincta
	}

}

static void readInput()
{
	FILE* fd = fopen("./tests/input.txt", "r");
	size_t buf_size = 256;
	char* buffer = (char*)malloc(buf_size * sizeof(char));
	DFA dfa;
	int buf_index = 0;

	createStates(&dfa);

	getline(&buffer, &buf_size, fd); // numarul de tranzitii M
	int M = strToInt(buffer);
	dfa.nr_tranzitii = M;
	while(M > 0)
	{
		getline(&buffer, &buf_size, fd); // descriere tranzitie
M--;
	}

	getline(&buffer, &buf_size, fd); // numar stari finale nrF
	dfa.S_finale = (int*)malloc(strToInt(buffer) * sizeof(int));
	getline(&buffer, &buf_size, fd); // starile finale, nrF ca numar
	while(parseWord(buffer, &buf_index) != NULL);

	getline(&buffer, &buf_size, fd); // numarul de cuvinte ce urmeaza a fi verificate, NrCuv
	int NrCuv = strToInt(buffer);	
	while(NrCuv > 0)
	{
		getline(&buffer, &buf_size, fd); // cuvant de verificat
		char* cuvant = parseWord(buffer, &buf_index);
		//verifica(cuvant);
		free(cuvant);
		NrCuv--;
	}
}

void writeOutput(char* output)
{
	printf("%s\n", output);
}

int main()
{
	readInput();
}
