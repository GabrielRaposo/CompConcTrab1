/*
* Autores: Matheus Graciano 113093372 <graciano.dev@gmail.com>,
* 		   Gabriel Raposo 123dre-aqui123 <email>
* Universidade Federal do Rio de Janeiro
* Departamento de Ciência da Computação
* Computação Concorrente - trabalho 1
* Professora: Silvana Rossetto
*/

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "util.h"
#include "timer.h"

//variáveis globais
int nthreads;
FILE *infile;

/*
* tabela 'hash' no índice referente ao
* ascii do caracter estará sua frequencia
*/
int ascii_freq_global[ASCII_SIZE];

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
	int size;

	//avaliando argumentos passados
	if(argc <3){
		printf("Use ./%s <infile> <outfile> <nthreads|opcional>\n",
			argv[0]);
		error(NULL);
	}

	size = strlen(argv[1])+1;
	infile_name = (char*) malloc(size * sizeof(char));

	size = strlen(argv[2])+1;
	outfile_name = (char*) malloc(size 	* sizeof(char));
	
	strncpy(infile_name,  argv[1], strlen(argv[1]));
	strncpy(outfile_name, argv[2], strlen(argv[2]));

	use_threads = (argc >3);
	if(use_threads){
		nthreads=atoi(argv[3]);
	}

	//Lendo arquivos de entrada e saída
	printf("\n....Acessando arquivo: %s\n", infile_name);
	infile = fopen(infile_name, "r");
	if (infile == NULL) {
		error("Não foi possível abrir o arquivo."); 
	}
	printf("\nAcessando arquivo: %s\n", outfile_name);
	outfile = fopen(outfile_name, "w");
	if (outfile == NULL) {
		error("Não foi possível abrir o arquivo."); 
	}

	inicializa_ascii_freq(ascii_freq_global);


	//executando problema
	if(use_threads) {
		//criando threads

		//esperando threads terminarem

		//alguma outra coisa para juntar resulado, se for necessário
	}
	else {
		ocorrencias_caracteres_arquivo(infile, ascii_freq_global);
	}

	escreve_arquivo_saida(ascii_freq_global, outfile);
	printf("Frequência de caracteres no arquivo: %s\n", outfile_name);
	fclose(infile);
	fclose(outfile);
	return 0;
}
