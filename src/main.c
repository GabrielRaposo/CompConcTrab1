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

#define __USE_LARGEFILE64
#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE
#include <sys/stat.h>

#if __WIN32__
#define stat64 _stat64
#endif

#include "constants.h"
#include "util.h"
#include "timer.h"

//variáveis globais
int nthreads;
unsigned long long int string_size = 500;
FILE *infile;
/*
* tabela estilo 'hash' no índice referente ao
* ascii do caracter estará sua frequencia
*/
int ascii_freq_global[ASCII_SIZE];

bool end = false;
double inicio, fim, delta1, delta2, delta3;
pthread_mutex_t mutex_file;
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
	long long unsigned int buffer_size;
	int ascii_freq_local[ASCII_SIZE];
	char c;
	char *string_local;

	string_local = malloc(sizeof(char) * string_size);
	inicializa_ascii_freq(ascii_freq_local);
	
	pthread_mutex_lock(&mutex_end);
	while(!end) {
		pthread_mutex_unlock(&mutex_end);

		//lendo arquivo
		pthread_mutex_lock( &mutex_file );
		buffer_size = fread(string_local, sizeof(char), string_size, infile);
		if(buffer_size != string_size){
			pthread_mutex_lock(&mutex_end);
			end = true;
			pthread_mutex_unlock(&mutex_end);
		}
		pthread_mutex_unlock( &mutex_file );

		//processando string local lida
		for (i = 0; (i < buffer_size); ++i)
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
	pthread_mutex_unlock(&mutex_ascii);

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
	int i;
	unsigned long long int size;
	struct stat64 st;

	pthread_t *tid_sist;
	
	GET_TIME(inicio);

	//avaliando argumentos passados
	if(argc <3){
		printf("Use ./%s <infile> <outfile> <nthreads|opcional>\n", argv[0]);
		error(NULL);
	}

	size = strlen(argv[1])+1;
	infile_name = (char*) malloc(size * sizeof(char));

	size = strlen(argv[2])+1;
	outfile_name = (char*) malloc(size 	* sizeof(char));
	
	strncpy(infile_name,  argv[1], strlen(argv[1]));
	strncpy(outfile_name, argv[2], strlen(argv[2]));

	//avaliando os argumentos no caso de um número de threads ser passado
	if(use_threads = (argc > 3)){
		nthreads=atoi(argv[3]);
		if(stat64(infile_name, &st))
			error("Falha ao tentar ler tamanho do arquivo de entrada");
		size = st.st_size;
		// cada thread repete 150 vezes, hard coded
		// definido empiricamente testando com arquivos grandes
		string_size = size / (nthreads * 150);

		//limitando por teto inferior de 50B e superior de 300MB
		//aqui estamos pensando em RAM, por isso o máximo 300MB (cada thread)
		string_size = llu_max(MIN_STRING_SIZE, string_size);
		string_size = llu_min(MAX_STRING_SIZE, string_size);

	  	printf("string_size: %llu\n", string_size);
	  	printf("file size: %llu\n", size);

		//inicializando variáveis de concorrência
		pthread_mutex_init(&mutex_file, NULL);
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

		//criando threads
		for (i = 0; i < nthreads; ++i) {
			int *tid;
			tid = (int *)malloc(sizeof(int));
			*tid = i;
			if( pthread_create(&tid_sist[i], NULL, thread_conta_char,
			   (void *)tid) )
				error("Erro ao criar thread");
		}

		//esperando threads terminarem
		for (i = 0; i < nthreads; ++i) {
			if(pthread_join(tid_sist[i], NULL))
				error("Erro ao esperar thread parar.");
		}

		//calculando tempo de execução concorrente
		GET_TIME(fim);
		delta2 = fim - inicio;
		GET_TIME(inicio);

		pthread_mutex_destroy(&mutex_file);
		pthread_mutex_destroy(&mutex_end);
		pthread_mutex_destroy(&mutex_ascii);
		free(tid_sist);
	}
	else {
		ocorrencias_caracteres_arquivo(infile, ascii_freq_global);
	}

	escreve_arquivo_saida(ascii_freq_global, outfile);

	fclose(infile);
	fclose(outfile);
	
	GET_TIME(fim);
	delta3 = fim - inicio;

	printf("\n\nFrequência de caracteres no arquivo: %s\n", outfile_name);
	
	free(infile_name);
	free(outfile_name);
	if(use_threads){ 
		printf("Tempo sequencial 1: %f\n", delta1);
		printf("Tempo concorrente: %f\n", delta2);
		printf("Tempo sequencial 2: %f\n", delta3);
		printf("Tempo total: %f\n\n", delta1 + delta2 + delta3);
	}
	else		
		printf("Tempo total: %f\n\n", delta3);
	
	return 0;
}