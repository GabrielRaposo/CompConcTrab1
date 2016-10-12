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


/*
* incrementa em cada caractere da tabela ascii_frew
* usando os caracteres da string de line
*/
void ocorrencias_caracteres_linha(int *ascii_freq, char *line);

/*
* pega a tabela de frequencia de caracteres e escreve
* num arquivo csv indicado
*/
void escreve_arquivo_saida(int *ascii_freq, FILE *f);

#endif
