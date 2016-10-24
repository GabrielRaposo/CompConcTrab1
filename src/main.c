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
#include <sys/stat.h>
#include "constants.h"
#include "util.h"
#include "timer.h"

//variáveis globais
int nthreads;
int string_size = 500;
FILE *infile;
/*
* tabela estilo 'hash' no índice referente ao
* ascii do caracter estará sua frequencia
*/
int ascii_freq_global[ASCII_SIZE];

bool end = false;
double inicio, fim, delta1, delta2, delta3;
pthread_mutex_t mutex;
pthread_mutex_t mutex_end;
pthread_mutex_t mutex_ascii;

/*
* versão concorrente da primeira etapa do algoritmo de huffman,
* conta a frequência dos caracteres do arquivo f e armazena
* na tabela ascii_freq
*/
void ocorrencias_caracteres_arquivo(FILE *f, int *ascii_freq){
	char c;
	while ((c = fgetc(f)) != EOF){
		incrementa_ocorrencias_char(ascii_freq, c);
	}
}

void * thread_conta_char(void * arg){
	int tid = *(int *)arg;
	int i;
	int ascii_freq_local[ASCII_SIZE];
	char c;
	char *string_local;

	string_local = malloc(sizeof(char) * string_size);
	inicializa_ascii_freq(ascii_freq_local);
	
	pthread_mutex_lock(&mutex_end);
	while(!end) {
		pthread_mutex_unlock(&mutex_end);

		//lendo arquivo
		pthread_mutex_lock( &mutex );
		for (i = 0; (i < string_size); ++i)
		{
			if( (c = fgetc(infile)) == EOF ){
				printf("EOF\n");
				pthread_mutex_lock(&mutex_end);
				end = true;
				pthread_mutex_unlock(&mutex_end);
				string_local[i] = '\0'; // null terminator
				break;
			}
			else
				string_local[i] = c;

		}
		pthread_mutex_unlock( &mutex );

		//processando string local lida
		c = string_local[0];
		for (i = 0; (i < string_size && c!='\0'); ++i)
		{
			c = string_local[i];
			incrementa_ocorrencias_char(ascii_freq_local, c);
		}

		//fim do loop
		pthread_mutex_lock(&mutex_end);
	}
	pthread_mutex_unlock(&mutex_end);

	//armazenando resultados parciais nas variáveis globais
	pthread_mutex_lock( &mutex_ascii );
	for(i = 0; i < ASCII_SIZE; i++){
		ascii_freq_global[i] += ascii_freq_local[i];
	}
	pthread_mutex_unlock( &mutex_ascii );

	printf("Sai da thread_conta_char %d\n", tid);
	free(arg);
	free(string_local);
	pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
	FILE *outfile;
	char *infile_name, *outfile_name;
	bool use_threads = false;
	int size, i;
	struct stat st;

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
		if(stat(infile_name, &st))
			error("Falha ao tentar ler tamanho do arquivo de entrada");
		size = st.st_size;
		// cada thread repete 150 vezes, hard coded
		// definido empiricamente
		string_size = (int) size / (nthreads * 150);

		//inicializando variáveis de concorrência
		pthread_mutex_init(&mutex, NULL);
		pthread_mutex_init(&mutex_end, NULL);
		pthread_mutex_init(&mutex_ascii, NULL);
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
		GET_TIME(fim);
		delta1 = fim - inicio;
		GET_TIME(inicio);

		for (i = 0; i < nthreads; ++i) {
			int *tid;
			tid = (int *)malloc(sizeof(int));
			*tid = i;
			if( pthread_create(&tid_sist[i], NULL, thread_conta_char,
			   (void *)tid) )
				error("Erro ao criar thread");
		}

		for (i = 0; i < nthreads; ++i) {
			if(pthread_join(tid_sist[i], NULL))
				error("Erro ao esperar thread parar.");
		}

		pthread_mutex_destroy(&mutex);
		pthread_mutex_destroy(&mutex_end);
		pthread_mutex_destroy(&mutex_ascii);
		free(tid_sist);

		GET_TIME(fim);
		delta2 = fim - inicio;
		GET_TIME(inicio);
	}
	else {
		ocorrencias_caracteres_arquivo(infile, ascii_freq_global);
	}

	escreve_arquivo_saida(ascii_freq_global, outfile);

	GET_TIME(fim);
	delta3 = fim - inicio;

	if(use_threads){ 
		printf("\n\nFrequência de caracteres no arquivo: %s\nTempo sequencial 1: %f\nTempo concorrente: %f\nTempo sequencial 2: %f\nTempo total: %f\n\n", 
			outfile_name, 
			delta1, delta2, delta3, 
			delta1 + delta2 + delta3);
	}
	else		
		printf("\n\nFrequência de caracteres no arquivo: %s\nTempo total: %f\n\n", outfile_name, delta3);
	
	fclose(infile);
	fclose(outfile);
	return 0;
}