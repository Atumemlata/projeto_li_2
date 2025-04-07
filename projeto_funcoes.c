#include <stdio.h>
#include "projeto.h"

int carregar_matriz(Jogo *jogo, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return 0;

    if (fscanf(file, "%d %d", &jogo->linhas, &jogo->colunas) != 2) {
        fclose(file);
        return 0;
    }

    if (jogo->linhas > MAX || jogo->colunas > MAX) {
        fclose(file);
        return 0;
    }

    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            if (fscanf(file, " %c", &jogo->original[i][j]) != 1) {
                fclose(file);
                return 0;
            }
            jogo->atual[i][j] = jogo->original[i][j];
        }
    }

    fclose(file);
    return 1;
}

void mostrar(Jogo *jogo) {
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            printf("%c ", jogo->atual[i][j]);
        }
        printf("\n");
    }
}

char maiuscula(char c) {
    if (c >= 'a' && c <= 'z') return c - 32;
    return c;
}
