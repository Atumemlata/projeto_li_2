#ifndef PROJETO_H
#define PROJETO_H

#define MAX 26

typedef struct {
    int linhas;
    int colunas;
    char original[MAX][MAX];
    char atual[MAX][MAX];
} Jogo;

int carregar_matriz(Jogo *jogo, const char *filename);
void mostrar(Jogo *jogo);
char maiuscula(char c);

#endif
