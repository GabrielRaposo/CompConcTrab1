#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

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

#endif