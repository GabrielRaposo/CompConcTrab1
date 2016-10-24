#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

/*
* printa mensagem de erro e sai do programa
*/
void error(char *msg);

/*
* inicializa tabela de frequencia de caracteres
* passada como argumento
*/
void inicializa_ascii_freq(int *ascii_freq);


bool in_array(int haystack[], int needle, int size);

bool is_valid_char(char c);

/*
* se o caractere c estiver no universo de caracteres a serem contados,
* incrementa no índice equivalente de c da tabela ascii_freq
*/
void incrementa_ocorrencias_char(int *ascii_freq, char c);

/*
* pega a tabela de frequencia de caracteres e escreve
* num arquivo csv indicado
*/
void escreve_arquivo_saida(int *ascii_freq, FILE *f);

#endif
