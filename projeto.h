#ifndef PROJETO_H
#define PROJETO_H
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX 256

typedef struct Estado {
    char estado[MAX][MAX];
    struct Estado *prox;
} Estado;

typedef struct {
    int linhas;
    int colunas;
    char original[MAX][MAX];
    char atual[MAX][MAX];
    Estado *historico; 
} Jogo;

int carregar_matriz(Jogo *jogo, const char *filename);
void mostrar(Jogo *jogo);
char maiuscula(char c);
void copiaTabuleiro(Jogo *jogo);
void imprimeTabuleiro(Jogo *jogo);
int contarViolacoes(Jogo *jogo);
int verificarRestricoes(Jogo *jogo);
void verificarCasa(Jogo *jogo, int x, int y, int visitado[MAX][MAX]);
void printMenu(void);
void salvar_jogo(Jogo *jogo, const char *filename);
void limparJogo(Jogo *jogo);
int ajudar(Jogo *jogo);
int resolver_completamente(Jogo *jogo);


#endif