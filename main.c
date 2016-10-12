/*
* Autores: Matheus Graciano 113093372 <graciano.dev@gmail.com>, Gabriel Raposo 123dre-aqui123 <email>
* Universidade Federal do Rio de Janeiro
* Departamento de Ciência da Computação
* Computação Concorrente - trabalho 1
* Professora: Silvana Rossetto
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <pthread.h>
#include "util.h"
#include "timer.h"

#define ASCII_SIZE 128
#define MAXIMUM_STR_LINE_READABLE INT_MAX

//variáveis globais
int nthreads;
FILE *infile;

/*
* tabela 'hash' no índice referente ao
* ascii do caracter estará sua frequencia
*/
int ascii_freq_global[ASCII_SIZE];

/*
* inicializa tabela de frequencia de caracteres
* passada como argumento
*/
void inicializa_ascii_freq(int *ascii_freq){
	int i;
	for(i=0; i<ASCII_SIZE; i++) {
		ascii_freq[i] = 0;
	}
}


/*
* incrementa em cada caractere da tabela ascii_frew
* usando os caracteres da string de line
*/
void ocorrencias_caracteres_linha(int *ascii_freq, char *line){
	int i, ascii_eq;
	for(i=0; i<strlen(line); i++){
			ascii_eq = (int) line[i];
			ascii_freq[ascii_eq]++;
		}
}

void ocorrencias_caracteres_arquivo(FILE *f, int *ascii_freq){
	char line[MAXIMUM_STR_LINE_READABLE];
	while (fgets(line, sizeof(line), f) != NULL){
		ocorrencias_caracteres_linha(ascii_freq_global, line);
	}
}

int main(int argc, char const *argv[])
{
	FILE *outfile;
	char *infile_name, *outfile_name;
	bool use_threads = false;

	//avaliando argumentos passados
	if(argc <3){
		printf("Use ./%s <infile> <outfile> <nthreads|opcional>\n",
			argv[0]);
		error(NULL);
	}
	strcpy(infile_name, argv[1]);
	strcpy(outfile_name, argv[2]);

	use_threads = (argc >3);
	if(use_threads){
		nthreads=atoi(argv[3]);
	}

	//Lendo arquivos de entrada e saída
	infile = fopen(infile_name, "r");
	if (infile == NULL) {
		error("Unable to open file."); 
	}
	outfile = fopen(outfile_name, "w");
	if (outfile == NULL) {
		error("Unable to open file.");
	}

	inicializa_ascii_freq(ascii_freq_global);


	//executando problema
	if(use_threads) {
		//criando threads

		//esperando threads terminarem

		//alguma outra coisa para juntar resulado, se for necessário
	}
	else {
		ocorrencias_caracteres(infile, ascii_freq_global);
	}

	fclose(infile);
	fclose(outfile);
	return 0;
}