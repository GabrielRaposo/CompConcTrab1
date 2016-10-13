#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "constants.h"

void error(char *msg) {
	if(msg==NULL) printf("Erro.\n");
	else printf("%s\n", msg);
	exit(EXIT_FAILURE);
}

void inicializa_ascii_freq(int *ascii_freq) {
	int i;
	for(i=0; i<ASCII_SIZE; i++) ascii_freq[i] = 0;
}

void incrementa_ocorrencias_char(int *ascii_freq, char c) {
	int ascii_eq = (int) c; // equivalente da tabela ascii do char c

	if(ascii_eq>=MIN_ASCII_USING && ascii_eq<=MAX_ASCII_USING)
		ascii_freq[ascii_eq]++;
}

bool in_array(int arr[], int needle, int size){
	int i;
	for (i = 0; i < size; ++i) {
		if(arr[i]==needle) return true;
	}
	return false;
}

void escreve_arquivo_saida(int *ascii_freq, FILE *f) {
	int i;
	fprintf(f, "Caractere, Qtde\n");
	for(i=0; i<ASCII_SIZE; i++) {
		if(ascii_freq[i]>0 && !in_array(BLACKLIST, i, BLACKLIST_SIZE))
			fprintf(f, "%c, %d\n", i, ascii_freq[i]);
	}
}
