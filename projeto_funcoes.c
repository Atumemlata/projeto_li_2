#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "projeto.h"

int carregar_matriz(Jogo *jogo, const char *filename) {
    // Limpa histórico existente de forma segura
    jogo->historico = NULL;
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

            // Inicializa o tabuleiro atual com letras minúsculas
            jogo->atual[i][j] = tolower(jogo->original[i][j]);
        }
    }

    fclose(file);
    return 1;
}


void limparJogo(Jogo *jogo) {
    Estado *e = jogo->historico;
    while (e) {
        Estado *temp = e->prox;
        free(e);
        e = temp;
    }
    jogo->historico = NULL;
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
    if (x < 0 || x >= jogo->linhas || y < 0 || y >= jogo->colunas) return;
    if (visitado[x][y] || jogo->atual[x][y] == '#') return;

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
                if (islower(jogo->atual[i][j]) && !visitado[i][j]) {
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

//FUNCAO PARA GRAVAR O JOGO(COMANDO g)
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
//FUNCOES PARA RESOLVER 

// ————— Funções de apoio ao “ajuda” —————

// Deve riscar casa (x,y) se houver um espaço na mesma linha/coluna

static int precisaRascar(Jogo *j, int x, int y) {
    char c = j->atual[x][y];
    if (!islower(c)) return 0;
    char C = toupper(c);
    for (int i = 0; i < j->linhas; i++)
        if (i!=x && j->atual[i][y] == C) return 1;
    for (int k = 0; k < j->colunas; k++)
        if (k!=y && j->atual[x][k] == C) return 1;
    return 0;
}

// 2) testa se riscar (x,y) isolaria alguma célula branca (maiúscula)
static int isolaBrancos(Jogo *j, int x, int y) {
    int visitado[MAX][MAX] = {0};
    char salv = j->atual[x][y];
    j->atual[x][y] = '#';
    // procura primeira maiúscula
    int sx=-1, sy=-1;
    for (int i=0;i<j->linhas && sx<0;i++)
      for (int k=0;k<j->colunas;k++)
        if (isupper(j->atual[i][k])) { sx=i; sy=k; break; }
    if (sx<0) { j->atual[x][y]=salv; return 0; }
    verificarCasa(j, sx, sy, visitado);
    int descon = 0;
    for (int i=0;i<j->linhas;i++)
      for (int k=0;k<j->colunas;k++)
        if (isupper(j->atual[i][k]) && !visitado[i][k]) { descon=1; break; }
    j->atual[x][y] = salv;
    return descon;
}

// 3) a função de “ajuda” devolve 1 se alterou algo, 0 caso contrário
int ajudar(Jogo *j) {
    char novo[MAX][MAX];
    int mudou = 0;
    memcpy(novo, j->atual, sizeof(novo));

    // (a) riscar minúsculas repetidas
    for (int i = 0; i < j->linhas; i++) {
      for (int k = 0; k < j->colunas; k++) {
        if (precisaRascar(j, i, k) && novo[i][k] != '#') {
          novo[i][k] = '#';
          mudou = 1;
        }
      }
    }

    // (b) pintar vizinhas de riscado em maiúscula
    for (int i = 0; i < j->linhas; i++) {
      for (int k = 0; k < j->colunas; k++) {
        if (j->atual[i][k] == '#') {
          // revela em maiúscula a original vizinha
          if (i > 0 && novo[i-1][k] != toupper(j->original[i-1][k])) {
            novo[i-1][k] = toupper(j->original[i-1][k]);
            mudou = 1;
          }
          if (i < j->linhas - 1 && novo[i+1][k] != toupper(j->original[i+1][k])) {
            novo[i+1][k] = toupper(j->original[i+1][k]);
            mudou = 1;
          }
          if (k > 0 && novo[i][k-1] != toupper(j->original[i][k-1])) {
            novo[i][k-1] = toupper(j->original[i][k-1]);
            mudou = 1;
          }
          if (k < j->colunas - 1 && novo[i][k+1] != toupper(j->original[i][k+1])) {
            novo[i][k+1] = toupper(j->original[i][k+1]);
            mudou = 1;
          }
        }
      }
    }

    // (c) pintar aquelas minúsculas que isolariam brancos
    for (int i = 0; i < j->linhas; i++) {
      for (int k = 0; k < j->colunas; k++) {
        if (islower(novo[i][k]) && isolaBrancos(j, i, k)) {
          novo[i][k] = toupper(j->original[i][k]);
          mudou = 1;
        }
      }
    }

    if (mudou) memcpy(j->atual, novo, sizeof(novo));
    return mudou;
}


void resolve_jogo(Jogo *g) {
    if (g->linhas == 0) {
        printf("Carregue um tabuleiro primeiro!\n");
        return;
    }

    int mudou;
    do {
        copiaTabuleiro(g);  // Salva o estado antes de qualquer alteração
        mudou = ajudar(g);
    } while (mudou);

    mostrar(g);
}


