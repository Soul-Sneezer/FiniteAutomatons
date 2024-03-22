#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
	int key;
	int value;
} Entry;

typedef struct
{
	int nr_stari;
	int nr_tranzitii;

	int alphabet_size;
	bool alphabet[26];

	int S;
	int nr_s_finale;
	int* S_finale;

	int* functie_tranzitie[26]; // e o lista dinamica
} DFA;

static void initTFunc(int* func[26])
{
	for(int i = 0; i < 26; i++)
		func[i] = NULL;
}

static unsigned int hash(unsigned int x, unsigned int size) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x % size;
}

static DFA* initDFA()
{
	DFA* new_dfa = (DFA*)malloc(sizeof(DFA));
	new_dfa->nr_stari = 0;
	new_dfa->nr_tranzitii = 0;
	new_dfa->S = 0;
	new_dfa->S_finale = NULL;

	initTFunc(new_dfa->functie_tranzitie);

	return new_dfa;
}

static void freeDFA(DFA* dfa)
{
	for(int i = 0; i < 26; i++)
	{
		if(dfa->alphabet[i])
			free(dfa->functie_tranzitie[i]);
	}
	dfa = initDFA();
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

static void createDFA(FILE* fd, DFA* dfa)
{
	size_t buf_size = 256;
	char* buffer = (char*)malloc(buf_size * sizeof(char));
	int buf_index = 0;
	getline(&buffer, &buf_size, fd); // numarul de stari ale automatului N
	int N = strToInt(parseWord(buffer, &buf_index));
	dfa->nr_stari = N;

	char* state_name = (char*)malloc(buf_size * sizeof(char));
	newLine(fd, &buf_index, &buffer, &buf_size);

	Entry* state_names = (Entry*)malloc(2 * N * sizeof(Entry));
	bool alphabet[26];
	int current = 0;
	for(int i = 0; i < 2 * N; i++)
	{
		state_names[i].value = -1;
	}

	for(int i = 0; i < 26; i++)
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
	dfa->nr_tranzitii = M;
	int* states[26];
	initTFunc(states);
	while(M > 0)
	{
		newLine(fd, &buf_index, &buffer, &buf_size);
		int x = strToInt(parseWord(buffer, &buf_index));
		x = findEntry(state_names, 2 * N, x);

		int y = strToInt(parseWord(buffer, &buf_index));
		y = findEntry(state_names, 2 * N, y);

		char* l = parseWord(buffer, &buf_index);

		if(alphabet[l[0] - 'a'] == false)	// litera face parte din alfabetul pentru DFA
		{
			alphabet[l[0] - 'a'] = true;
			states[l[0] - 'a'] = (int*)malloc(N * sizeof(int));
		}
		states[l[0] - 'a'][x] = y; 
		M--;
	}

	for(int i = 0; i < 26; i++)
	{
		dfa->functie_tranzitie[i] = states[i];
		if(alphabet[i] == true)
			dfa->alphabet[i] = true;
	}
	
	newLine(fd, &buf_index, &buffer, &buf_size);
	dfa->S = findEntry(state_names , 2 * N, strToInt(parseWord(buffer, &buf_index)));
	
	newLine(fd, &buf_index, &buffer, &buf_size);
	int NrF = strToInt(parseWord(buffer, &buf_index));
	dfa->nr_s_finale = NrF;
	dfa->S_finale = (int*)malloc(NrF * sizeof(int));

	newLine(fd, &buf_index, &buffer, &buf_size);
	current = 0;
	while(NrF > 0)
	{
		dfa->S_finale[current++] = findEntry(state_names, 2 * N, strToInt(parseWord(buffer, &buf_index)));
		NrF--;
	}

	free(state_names);
}

static void printDFAInfo(DFA* dfa)
{
	printf("Nr stari: %d\n", dfa->nr_stari);
	printf("Nr tranzitii: %d\n", dfa->nr_tranzitii);
	printf("Alfabet: ");
	for(int i = 0; i < 26; i++)
	{
		if(dfa->alphabet[i])
			printf("%c ", 'a' + i);
	}
	printf("\nTranzitii: \n");
	for(int i = 0; i < 26; i++)
	{
		for(int j = 0; j < dfa->nr_stari; j++)
		{
			if(dfa->alphabet[i])
				printf("%d %c -> %d\n", j, i + 'a', dfa->functie_tranzitie[i][j]);
		}
	}

	printf("Stare initiala: %d\n", dfa->S);
	
	printf("Stari finale: ");
	for(int i = 0; i < dfa->nr_s_finale; i++)
	{
		printf("%d ", dfa->S_finale[i]);
	}
	
}

static bool verifyWordDFA(DFA* dfa, char* word)
{
	int n = strlen(word);
	int current_state = dfa->S;
	for(int i = 0; i < n; i++)
	{
		current_state = dfa->functie_tranzitie[word[i] - 'a'][current_state];
		
	}

	for(int i = 0; i < dfa->nr_s_finale; i++)
	{
		if(current_state == dfa->S_finale[i])
			return true;
	}

	return false;
}

static void testDFA(DFA* dfa, FILE* input_fd, char* output_file)
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
		if(verifyWordDFA(dfa, cuvant))
			fprintf(fd, "DA\n");
		else
			fprintf(fd, "NU\n");
		free(cuvant);
		NrCuv--;
	}
}

int main()
{
	DFA* dfa = initDFA();
	FILE* fd = readInput("./tests/input(1).txt");
	createDFA(fd, dfa);
	printDFAInfo(dfa);
	testDFA(dfa, fd, "output.txt");
	freeDFA(dfa);
}
