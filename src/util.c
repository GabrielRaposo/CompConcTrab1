#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
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

bool in_array(int haystack[], int needle, int size){
	int i;
	for (i = 0; i < size; ++i) {
		if(haystack[i]==needle) return true;
	}
	return false;
}

bool is_valid_char(char c){
	int ascii_eq = (int) c;
	if(ascii_eq>=MIN_ASCII_USING && ascii_eq<=MAX_ASCII_USING){
		return !in_array(BLACKLIST, ascii_eq, BLACKLIST_SIZE);
	}
	return false;
}

void incrementa_ocorrencias_char(int *ascii_freq, char c) {
	int ascii_eq = (int) c; // equivalente da tabela ascii do char c
	if(is_valid_char(c)) {
		ascii_freq[ascii_eq]++;
	}
}


void escreve_arquivo_saida(int *ascii_freq, FILE *f) {
	int i;
	fprintf(f, "Caractere, Qtde\n");
	for(i=0; i<ASCII_SIZE; i++) {
		if(ascii_freq[i]>0)
			fprintf(f, "%c, %d\n", i, ascii_freq[i]);
	}
}

long long unsigned int llu_min(long long unsigned int a, long long unsigned int b){
	return a < b? a : b;
}

long long unsigned int llu_max(long long unsigned int a, long long unsigned int b){
	return a > b? a : b;
}
