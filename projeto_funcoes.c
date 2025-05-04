#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "projeto.h"

int carregar_matriz(Jogo *jogo, const char *filename) {
    // Limpa histórico existente de forma segura
    while (jogo->historico != NULL) {
        Estado *temp = jogo->historico;
        jogo->historico = jogo->historico->prox;
        free(temp);
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Erro: Não foi possível abrir o arquivo %s\n", filename);
        return 0;
    }

    // Reinicia as dimensões
    jogo->linhas = 0;
    jogo->colunas = 0;

    // Lê dimensões do tabuleiro
    if (fscanf(file, "%d %d", &jogo->linhas, &jogo->colunas) != 2) {
        fclose(file);
        fprintf(stderr, "Erro: Formato inválido no arquivo (linhas/colunas)\n");
        return 0;
    }

    // Valida dimensões máximas
    if (jogo->linhas <= 0 || jogo->colunas <= 0 || 
        jogo->linhas > MAX || jogo->colunas > MAX) {
        fclose(file);
        fprintf(stderr, "Erro: Dimensões inválidas (%d x %d)\n", 
               jogo->linhas, jogo->colunas);
        return 0;
    }

    // Lê o tabuleiro caractere por caractere
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            if (fscanf(file, " %c", &jogo->original[i][j]) != 1) {
                fclose(file);
                fprintf(stderr, "Erro: Leitura do tabuleiro incompleta\n");
                return 0;
            }
            
            // Valida caractere
            if (jogo->original[i][j] != ' ' && 
               !isalpha(jogo->original[i][j]) && 
               jogo->original[i][j] != '#') {
                fclose(file);
                fprintf(stderr, "Erro: Caractere inválido '%c' na posição (%d,%d)\n",
                      jogo->original[i][j], i, j);
                return 0;
            }
            
            // Inicializa tabuleiro atual
            jogo->atual[i][j] = jogo->original[i][j];
        }
    }

    fclose(file);
    return 1;
}

void printMenu() {
    printf("\n--- Menu de Comandos ---\n");
    printf("s : Sair do jogo\n");
    printf("l : Carregar tabuleiro\n");
    printf("b : Pintar casa (linha coluna)\n");
    printf("r : Riscar casa (linha coluna)\n");
    printf("d : Desfazer jogada\n");
    printf("v : Verificar restricoes\n");
    printf("c : Mostrar menu\n");
    printf("g : Gravar jogo em arquivo\n");
    printf("a : Dar dica\n");
    printf("R : Resolver jogo\n");
    printf("--------------------------\n");
}

void mostrar(Jogo *jogo) {
    printf("\n");
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
    Estado *novo = malloc(sizeof(Estado));
    if (!novo) {
        printf("Erro ao alocar memoria!\n");
        return;
    }

    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            novo->estado[i][j] = jogo->atual[i][j];
        }
    }
    novo->prox = jogo->historico;
    jogo->historico = novo;
}

void imprimeTabuleiro(Jogo *jogo) {
    if (jogo->historico == NULL) {
        printf("Nenhum estado anterior para restaurar!\n");
        return;
    }

    Estado *topo = jogo->historico;
    
    // Restaura o estado anterior
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            jogo->atual[i][j] = topo->estado[i][j];
        }
    }
    
    // Atualiza o histórico
    jogo->historico = topo->prox;
    free(topo);
}

void verificarCasa(Jogo *jogo, int x, int y, int visitado[MAX][MAX]) {
    if (x < 0 || x >= jogo->linhas) return;
    if (y < 0 || y >= jogo->colunas) return;
    if (visitado[x][y]) return;
    if (!isupper(jogo->atual[x][y])) return;

    visitado[x][y] = 1;

    verificarCasa(jogo, x+1, y, visitado);
    verificarCasa(jogo, x-1, y, visitado);
    verificarCasa(jogo, x, y+1, visitado);
    verificarCasa(jogo, x, y-1, visitado);
}

void verificarRestricoes(Jogo *jogo) {
    int violacoes = 0;

    // Verificar linhas
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            for (int k = j+1; k < jogo->colunas; k++) {
                if (jogo->atual[i][j] == jogo->atual[i][k] && 
                    jogo->atual[i][j] != '#' && 
                    jogo->atual[i][j] != ' ') {
                    violacoes++;
                    printf("Violacao linha %d: %c repetido\n", i, jogo->atual[i][j]);
                }
            }
        }
    }

    // Verificar colunas
    for (int j = 0; j < jogo->colunas; j++) {
        for (int i = 0; i < jogo->linhas; i++) {
            for (int k = i+1; k < jogo->linhas; k++) {
                if (jogo->atual[i][j] == jogo->atual[k][j] && 
                    jogo->atual[i][j] != '#' && 
                    jogo->atual[i][j] != ' ') {
                    violacoes++;
                    printf("Violacao coluna %d: %c repetido\n", j, jogo->atual[i][j]);
                }
            }
        }
    }

    // Verificar conexão
    int visitado[MAX][MAX] = {0};
    int startX = -1, startY = -1;

    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            if (isupper(jogo->atual[i][j])) {
                startX = i;
                startY = j;
                break;
            }
        }
        if (startX != -1) break;
    }

    if (startX != -1) {
        verificarCasa(jogo, startX, startY, visitado);

        for (int i = 0; i < jogo->linhas; i++) {
            for (int j = 0; j < jogo->colunas; j++) {
                if (isupper(jogo->atual[i][j]) && !visitado[i][j]) {
                    violacoes++;
                    printf("Casa (%d,%d) desconectada\n", i, j);
                }
            }
        }
    }

    // Verificar '#' consecutivos
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas-1; j++) {
            if (jogo->atual[i][j] == '#' && jogo->atual[i][j+1] == '#') {
                violacoes++;
                printf("Dois '#' consecutivos em (%d,%d)-(%d,%d)\n", i, j, i, j+1);
            }
        }
    }

    for (int j = 0; j < jogo->colunas; j++) {
        for (int i = 0; i < jogo->linhas-1; i++) {
            if (jogo->atual[i][j] == '#' && jogo->atual[i+1][j] == '#') {
                violacoes++;
                printf("Dois '#' consecutivos em (%d,%d)-(%d,%d)\n", i, j, i+1, j);
            }
        }
    }

    printf(violacoes ? "\nTotal violacoes: %d\n" : "\nTabuleiro valido!\n", violacoes);
}

void salvar_jogo(Jogo *jogo, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Erro ao criar arquivo!\n");
        return;
    }

    fprintf(file, "%d %d\n", jogo->linhas, jogo->colunas);
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            fprintf(file, "%c ", jogo->atual[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
    printf("Tabuleiro salvo em %s\n", filename);
}

int dar_dica(Jogo *jogo, int *x, int *y) {
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            if (jogo->original[i][j] != ' ' && 
                jogo->atual[i][j] != maiuscula(jogo->original[i][j])) {
                *x = i;
                *y = j;
                return 1;
            }
        }
    }
    return 0;
}

void resolver_jogo(Jogo *jogo) {
    if (jogo->linhas == 0) {
        printf("Carregue um tabuleiro primeiro!\n");
        return;
    }

    copiaTabuleiro(jogo);
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            if (jogo->original[i][j] != ' ') {
                jogo->atual[i][j] = maiuscula(jogo->original[i][j]);
            }
        }
    }
    printf("Jogo resolvido!\n");
}