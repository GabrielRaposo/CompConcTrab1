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

#define BUFFER_SIZE 5

//variáveis globais
int nthreads;
FILE *infile;

//variáveis para implementação produtor/consumidor
char buffer[BUFFER_SIZE];
bool file_ended;
int count_buffer, in, out;
pthread_mutex_t mutex;
pthread_cond_t cond_prod, cond_cons;

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

void insere(char c) {
	pthread_mutex_lock(&mutex);
	while(count_buffer==BUFFER_SIZE){
		pthread_cond_wait(&cond_prod, &mutex);
	}
	count_buffer++;
	buffer[in] = c;
	in++;
	in %= BUFFER_SIZE;
	pthread_mutex_unlock(&mutex);
	pthread_cond_signal(&cond_cons);
}

char retira (void) {
	char c;
	pthread_mutex_lock(&mutex);
	while(count_buffer == 0) {
		pthread_cond_wait(&cond_cons, &mutex);
	}
	count_buffer--;
	c = buffer[out];
	out++;
	out %= BUFFER_SIZE;
	pthread_mutex_unlock(&mutex);
	pthread_cond_signal(&cond_prod);
	return c;
}

void * thread_produtora(void * arg) {
	char c;
	while(true) {
		if((c = fgetc(infile))!=EOF)
			insere(c);
		else{
			file_ended = true;
			 break;
		}
	}
	pthread_exit(NULL);
}

void * thread_consumidora(void * arg) {
	char c;
	while(!file_ended) {
		c = retira();
		incrementa_ocorrencias_char(ascii_freq_global, c, mutex_ascii_freq);
	}
	pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
	FILE *outfile;
	char *infile_name, *outfile_name;
	bool use_threads = false;
	int size, i;

	pthread_t tid_sist_prod, *tid_sist_cons;

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
		pthread_cond_init(&cond_cons, NULL);
		pthread_cond_init(&cond_prod, NULL);
		pthread_mutex_init(&mutex, NULL);
		count_buffer = in = out = 0;
		file_ended = false;
		for (i = 0; i < ASCII_SIZE; ++i)
		{
			pthread_mutex_init(&mutex_ascii_freq[i], NULL);
		}
		tid_sist_cons = (pthread_t*) malloc(sizeof(pthread_t) * nthreads);
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
		if(pthread_create(&tid_sist_prod, NULL, thread_produtora, NULL))
			error("Erro ao criar thread");
		for (i = 0; i < nthreads; ++i) {
			if(pthread_create(&tid_sist_cons[i], NULL, thread_consumidora, NULL))
				error("Erro ao criar thread");
		}

		//esperando threads terminarem
		if(pthread_join(tid_sist_prod, NULL))
			error("Erro ao esperar thread parar.");
		for (i = 0; i < nthreads; ++i) {
			if(pthread_join(tid_sist_cons[i], NULL))
				error("Erro ao esperar thread parar.");
		}

		//alguma outra coisa para juntar resulado, se for necessário
		pthread_cond_destroy(&cond_cons);
		pthread_cond_destroy(&cond_prod);
		pthread_mutex_destroy(&mutex);
		for (i = 0; i < ASCII_SIZE; ++i)
		{
			pthread_mutex_destroy(&mutex_ascii_freq[i]);
		}
		free(tid_sist_cons);
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
