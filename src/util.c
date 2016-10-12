#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "constants.h"

void error(char *msg){
	if(msg==NULL) printf("Erro.\n");
	else printf("%s\n", msg);
	exit(EXIT_FAILURE);
}

void inicializa_ascii_freq(int *ascii_freq){
	int i;
	for(i=0; i<ASCII_SIZE; i++) {
		ascii_freq[i] = 0;
	}
}

void ocorrencias_caracteres_linha(int *ascii_freq, char *line){
	int i, ascii_eq;
	printf("%s\n", line);
	for(i=0; i<strlen(line)-1; i++){
		printf("i=%d, c=%c", i, line[i]);
		ascii_eq = (int) line[i];
		if(MIN_ASCII_USING>=ascii_eq || ascii_eq<=MAX_ASCII_USING){
			ascii_freq[ascii_eq]++;	
		}
	}
	printf("ae porra\n");
}

void escreve_arquivo_saida(int *ascii_freq, FILE *f) {
	int i;
	fprintf(f, "Caractere, Qtde\n");
	for(i=0; i<ASCII_SIZE; i++){
		fprintf(f, "%c, %d", i, ascii_freq[i]);
	}
}
