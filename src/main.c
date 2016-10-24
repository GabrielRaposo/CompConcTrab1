/*
* Autores: Matheus Graciano 113093372 <graciano.dev@gmail.com>,
* 		   Gabriel Silva Raposo 115117041 <gabriel.s.raposo@gmail.com>
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

#define STRING_SIZE 50

//variáveis globais
int nthreads;
FILE *infile;

//variáveis para implementação produtor/consumidor
int count_buffer;
float inicio, fim, delta;
pthread_mutex_t mutex;

/*
* tabela estilo 'hash' no índice referente ao
* ascii do caracter estará sua frequencia
*/
int ascii_freq_global[ASCII_SIZE];
//exclusão mútua das posições na tabela
pthread_mutex_t mutex_ascii_freq[ASCII_SIZE];

/*
* versão concorrente da primeira etapa do algoritmo de huffman,
* conta a frequência dos caracteres do arquivo f e armazena
* na tabela ascii_freq
*/
void ocorrencias_caracteres_arquivo(FILE *f, int *ascii_freq){
	char c;
	while ((c = fgetc(f)) != EOF){
		incrementa_ocorrencias_char(ascii_freq, c, NULL);
	}
}

void * thread_conta_char(void * arg){
	int tid = *(int *)arg;
	int i;
	char c;
	char *string_local;
	bool end = false;

	string_local = malloc(sizeof(char) * STRING_SIZE);
	
	pthread_mutex_lock( &mutex );
	fgets( string_local, STRING_SIZE+1, infile);
	//printf("string>>%s pela thread %d \n\n", string_local, tid); 
	pthread_mutex_unlock( &mutex );
	
	while(!end){ 
		i = 0;
		c = string_local[i];
		incrementa_ocorrencias_char(ascii_freq_global, c, mutex_ascii_freq);

		while(i < STRING_SIZE && c != '\n'){
			i++;	
			c = string_local[i];
			incrementa_ocorrencias_char(ascii_freq_global, c, mutex_ascii_freq);
		}
		pthread_mutex_lock( &mutex );
		if( fgets( string_local, STRING_SIZE+1, infile) == NULL){
			pthread_mutex_unlock( &mutex );
			break;
		}
		//printf("string>>%s pela thread %d \n\n", string_local, tid); 
		pthread_mutex_unlock( &mutex );
	}

	printf("Sai da thread_conta_char %d\n", tid);
	pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
	FILE *outfile;
	char *infile_name, *outfile_name;
	bool use_threads = false;
	int size, i;

	pthread_t *tid_sist;
	
	GET_TIME(inicio);

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

	if(use_threads = (argc > 3)){
		nthreads=atoi(argv[3]);
		//inicializando variáveis de concorrência
		pthread_mutex_init(&mutex, NULL);
		for (i = 0; i < ASCII_SIZE; ++i)
		{
			pthread_mutex_init(&mutex_ascii_freq[i], NULL);
		}
		tid_sist = (pthread_t*) malloc(sizeof(pthread_t) * nthreads);
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
		for (i = 0; i < nthreads; ++i) {
			int *tid;
			tid = (int *)malloc(sizeof(int));
			*tid = i;
			if(pthread_create(&tid_sist[i], NULL, thread_conta_char, (void *)tid))
				error("Erro ao criar thread");
		}

		for (i = 0; i < nthreads; ++i) {
			if(pthread_join(tid_sist[i], NULL))
				error("Erro ao esperar thread parar.");
		}

		//alguma outra coisa para juntar resultado, se for necessário
		pthread_mutex_destroy(&mutex);
		for (i = 0; i < ASCII_SIZE; ++i)
		{
			pthread_mutex_destroy(&mutex_ascii_freq[i]);
		}
		free(tid_sist);
	}
	else {
		ocorrencias_caracteres_arquivo(infile, ascii_freq_global);
	}

	escreve_arquivo_saida(ascii_freq_global, outfile);
	GET_TIME(fim);
	printf("Frequência de caracteres no arquivo: %s\nTempo total: %f\n", outfile_name, fim - inicio);
	fclose(infile);
	fclose(outfile);
	return 0;
}