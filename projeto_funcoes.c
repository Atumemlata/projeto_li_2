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

void copiaTabuleiro(Jogo *jogo) {
    Estado *novo = malloc(sizeof(Estado)); // Aloca memória para um novo estado
    if (!novo) return; // Verifica se a alocação foi bem-sucedida

    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            novo->estado[i][j] = jogo->atual[i][j];
        }
    }
    novo->prox = jogo->historico;
    jogo->historico = novo;
}

void imprimeTabuleiro(Jogo *jogo) {
    if (jogo->historico == NULL) return;

    Estado *topo = jogo->historico;
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            jogo->atual[i][j] = topo->estado[i][j];
        }
    }
    jogo->historico = topo->prox;
    free(topo);
}

void verificarRestricoes(Jogo *jogo) {
    int violacoes = 0;

    // Verifica as linhas (símbolos repetidos)
    for (int i = 0; i < jogo->linhas; i++) {
        char simbolos[jogo->colunas];
        for (int j = 0; j < jogo->colunas; j++) {
            simbolos[j] = jogo->atual[i][j];
        }

        for (int j = 0; j < jogo->colunas; j++) {
            for (int k = j + 1; k < jogo->colunas; k++) {
                if (simbolos[j] == simbolos[k] && simbolos[j] != '#' && simbolos[j] != ' ') {
                    violacoes++;
                    printf("Violacao: Simbolo '%c' repete na linha %d\n", simbolos[j], i);
                    break;
                }
            }
        }
    }

    // Verifica as colunas (símbolos repetidos)
    for (int j = 0; j < jogo->colunas; j++) {
        char simbolos[jogo->linhas];
        for (int i = 0; i < jogo->linhas; i++) {
            simbolos[i] = jogo->atual[i][j];
        }

        for (int i = 0; i < jogo->linhas; i++) {
            for (int k = i + 1; k < jogo->linhas; k++) {
                if (simbolos[i] == simbolos[k] && simbolos[i] != '#' && simbolos[i] != ' ') {
                    violacoes++;
                    printf("Violacao: Simbolo '%c' repete na coluna %d\n", simbolos[i], j);
                    break;
                }
            }
        }
    }

    // Verifica dois '#' seguidos horizontalmente
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas - 1; j++) {
            if (jogo->atual[i][j] == '#' && jogo->atual[i][j + 1] == '#') {
                violacoes++;
                printf("Violacao: Dois '#' consecutivos na linha %d entre colunas %d e %d\n", i, j, j + 1);
            }
        }
    }

    // Verifica dois '#' seguidos verticalmente
    for (int j = 0; j < jogo->colunas; j++) {
        for (int i = 0; i < jogo->linhas - 1; i++) {
            if (jogo->atual[i][j] == '#' && jogo->atual[i + 1][j] == '#') {
                violacoes++;
                printf("Violacao: Dois '#' consecutivos na coluna %d entre linhas %d e %d\n", j, i, i + 1);
            }
        }
    }

    // Mensagem final
    if (violacoes == 0) {
        printf("Nenhuma violacao encontrada.\n");
    } else {
        printf("Foram encontradas %d violacoes das restricoes do tabuleiro.\n", violacoes);
    }
}
