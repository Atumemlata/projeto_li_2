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
            
            // Inicializa tabuleiro atual
            jogo->atual[i][j] = jogo->original[i][j];
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

// Verifica se há células "preenchidas" ('#') adjacentes.
int tem_adjacente(Jogo *g) {
    for (int i = 0; i < g->linhas; i++) {
        for (int j = 0; j < g->colunas; j++) {
            if (g->atual[i][j] != '#')
                continue;
            if ((i > 0 && g->atual[i-1][j] == '#') ||
                (i < g->linhas - 1 && g->atual[i+1][j] == '#') ||
                (j > 0 && g->atual[i][j-1] == '#') ||
                (j < g->colunas - 1 && g->atual[i][j+1] == '#'))
                return 1;
        }
    }
    return 0;
}

// Procura o primeiro conflito encontrado.
// letras iguais na mesma linha ou coluna (exceto '#')
// ou duas coordenadas '#' adjacentes.
// Se encontrar, retorna 1 e preenche (x1, y1) e (x2, y2) com as coordenadas do conflito.
int encontra_violacoes(Jogo *g, int *x1, int *y1, int *x2, int *y2) {
    // Verifica duplicadas (mesma letra) na mesma linha e coluna
    for (int i = 0; i < g->linhas; i++) {
        for (int j = 0; j < g->colunas; j++) {
            if (g->atual[i][j] == '#')
                continue;
            // Verifica na mesma linha (apenas à direita)
            for (int k = j + 1; k < g->colunas; k++) {
                if (g->atual[i][k] == g->atual[i][j]) {
                    *x1 = i; *y1 = j;
                    *x2 = i; *y2 = k;
                    return 1;
                }
            }
            // Verifica na mesma coluna (apenas abaixo)
            for (int k = i + 1; k < g->linhas; k++) {
                if (g->atual[k][j] == g->atual[i][j]) {
                    *x1 = i; *y1 = j;
                    *x2 = k; *y2 = j;
                    return 1;
                }
            }
        }
    }
    // Verifica coordenadas '#' adjacentes:
    // So se vê cima e esquerda para nao se repetir
    for (int i = 0; i < g->linhas; i++) {
        for (int j = 0; j < g->colunas; j++) {
            if (g->atual[i][j] == '#') {
                if (i > 0 && g->atual[i-1][j] == '#') {
                    *x1 = i; *y1 = j;
                    *x2 = i - 1; *y2 = j;
                    return 1;
                }
                if (j > 0 && g->atual[i][j-1] == '#') {
                    *x1 = i; *y1 = j;
                    *x2 = i; *y2 = j - 1;
                    return 1;
                }
            }
        }
    }
    return 0;
}

// Backtracking recursivo: tenta resolver os conflitos do tabuleiro.
// Se conseguir resolver (ou seja, não há conflitos) retorna 1; senão, retorna 0.
int resolve(Jogo *g) {
    int x1, y1, x2, y2;
    
    // Se não há conflito, o tabuleiro está resolvido.
    if (!encontra_violacoes(g, &x1, &y1, &x2, &y2))
        return 1;
    
    // Salva os valores originais
    char cel1 = g->atual[x1][y1];
    char cel2 = g->atual[x2][y2];
    
    // Tenta remover (definindo como '#') a primeira celula em conflito
    g->atual[x1][y1] = '#';
    if (!tem_adjacente(g) && resolve(g))
        return 1;
    g->atual[x1][y1] = cel1;  // desfaz a alteração
    
    // Tenta remover a segunda célula
    g->atual[x2][y2] = '#';
    if (!tem_adjacente(g) && resolve(g))
        return 1;
    g->atual[x2][y2] = cel2;
    
    return 0;
}

// Sugere uma dica: qual a célula a remover a seguir.
// Se encontrar uma dica, retorna 1 e define (*x, *y) com as coordenadas.
int dar_dica(Jogo *g, int *x, int *y) {
    int x1, y1, x2, y2;
    
    if (!encontra_violacoes(g, &x1, &y1, &x2, &y2))
        return 0;
    
    // Testa a remoção da primeira célula num tabuleiro de cópia
    Jogo copia = *g;
    copia.atual[x1][y1] = '#';
    if (!tem_adjacente(&copia) && resolve(&copia)) {
        *x = x1; *y = y1;
        return 1;
    }
    
    // Caso contrário, sugere a segunda célula
    *x = x2; *y = y2;
    return 1;
}

// Função principal que aplica as dicas até que o jogo seja resolvido.
// Após resolver, converte todas as letras para maiúsculas e exibe o tabuleiro.
void resolve_jogo(Jogo *g) {
    int x, y;
    while (dar_dica(g, &x, &y)) {
        copiaTabuleiro(g);      // Assume que essa função faz uma cópia do tabuleiro (para histórico ou visual)
        g->atual[x][y] = '#';     // Remove a célula sugerida pela dica
    }
    // Converte todas as células para maiúsculas
    for (int i = 0; i < g->linhas; i++) {
        for (int j = 0; j < g->colunas; j++) {
            g->atual[i][j] = maiuscula(g->atual[i][j]);
        }
    }
    mostrar(g);  // Exibe o tabuleiro final
}
