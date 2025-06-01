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
    Estado *atual = jogo->historico;
    while (atual) {
        Estado *temp = atual->prox;
        free(atual);
        atual = temp;
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


// Função contarViolacoes para evitar falsos positivos
int contarViolacoes(Jogo *jogo) {
    int violacoes = 0;

    // 1) Verificar letras repetidas na mesma linha (apenas maiúsculas)
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            if (isupper(jogo->atual[i][j])) {
                for (int k = j + 1; k < jogo->colunas; k++) {
                    if (jogo->atual[i][j] == jogo->atual[i][k] && isupper(jogo->atual[i][k])) {
                        violacoes++;
                    }
                }
            }
        }
    }

    // 2) Verificar letras repetidas na mesma coluna (apenas maiúsculas)
    for (int j = 0; j < jogo->colunas; j++) {
        for (int i = 0; i < jogo->linhas; i++) {
            if (isupper(jogo->atual[i][j])) {
                for (int k = i + 1; k < jogo->linhas; k++) {
                    if (jogo->atual[i][j] == jogo->atual[k][j] && isupper(jogo->atual[k][j])) {
                        violacoes++;
                    }
                }
            }
        }
    }

    // 3) Verificar '#' consecutivos (horizontais)
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas - 1; j++) {
            if (jogo->atual[i][j] == '#' && jogo->atual[i][j + 1] == '#') {
                violacoes++;
            }
        }
    }

    // 4) Verificar '#' consecutivos (verticais)
    for (int j = 0; j < jogo->colunas; j++) {
        for (int i = 0; i < jogo->linhas - 1; i++) {
            if (jogo->atual[i][j] == '#' && jogo->atual[i + 1][j] == '#') {
                violacoes++;
            }
        }
    }

    // 5) Verificar conexão apenas se houver pelo menos uma maiúscula
    int temMaiuscula = 0;
    for (int i = 0; i < jogo->linhas && !temMaiuscula; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            if (isupper(jogo->atual[i][j])) {
                temMaiuscula = 1;
                break;
            }
        }
    }

    if (temMaiuscula) {
        int visitado[MAX][MAX] = {0};
        int startX = -1, startY = -1;

        // Encontra a primeira célula maiúscula
        for (int i = 0; i < jogo->linhas && startX < 0; i++) {
            for (int j = 0; j < jogo->colunas; j++) {
                if (isupper(jogo->atual[i][j])) {
                    startX = i;
                    startY = j;
                    break;
                }
            }
        }

        if (startX != -1) {
            verificarCasa(jogo, startX, startY, visitado);

            // Verifica se todas as maiúsculas estão conectadas
            for (int i = 0; i < jogo->linhas; i++) {
                for (int j = 0; j < jogo->colunas; j++) {
                    if (isupper(jogo->atual[i][j]) && !visitado[i][j]) {
                        violacoes++;
                    }
                }
            }
        }
    }

    return violacoes;
}

int verificarRestricoes(Jogo *jogo) {
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

    // Verificar '#' consecutivos (horizontais)
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas - 1; j++) {
            if (jogo->atual[i][j] == '#' && jogo->atual[i][j+1] == '#') {
                violacoes++;
                printf("Dois '#' consecutivos em (%d,%d)-(%d,%d)\n", i, j, i, j+1);
            }
        }
    }
    // Verificar '#' consecutivos (verticais)
    for (int j = 0; j < jogo->colunas; j++) {
        for (int i = 0; i < jogo->linhas - 1; i++) {
            if (jogo->atual[i][j] == '#' && jogo->atual[i+1][j] == '#') {
                violacoes++;
                printf("Dois '#' consecutivos em (%d,%d)-(%d,%d)\n", i, j, i+1, j);
            }
        }
    }

    if (violacoes == 0) {
        printf("\nTabuleiro valido!\n");
    } else {
        printf("\nTotal violacoes: %d\n", violacoes);
    }

    return violacoes;
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

// ————— Funções de apoio ao “ajudar” —————

// Deve riscar casa (x,y) se houver um espaço na mesma linha/coluna

static int precisaRiscar(Jogo *j, int x, int y) {
    char c = j->atual[x][y];
    if (!islower(c)) return 0;
    
    char C = toupper(c);
    
    // Verifica se já existe essa letra maiúscula na mesma linha
    for (int k = 0; k < j->colunas; k++) {
        if (k != y && j->atual[x][k] == C) {
            return 1;
        }
    }
    
    // Verifica se já existe essa letra maiúscula na mesma coluna
    for (int i = 0; i < j->linhas; i++) {
        if (i != x && j->atual[i][y] == C) {
            return 1;
        }
    }
    
    return 0;
}

// 2) testa se riscar (x,y) isolaria alguma célula branca (maiúscula)
static int isolaBrancos(Jogo *j, int x, int y) {
    if (j->atual[x][y] == '#') return 0; // Já está riscada
    
    // Temporariamente risca a posição
    char original = j->atual[x][y];
    j->atual[x][y] = '#';
    
    // Verifica conectividade
    int visitado[MAX][MAX] = {0};
    int sx = -1, sy = -1;
    
    // Encontra primeira célula maiúscula
    for (int i = 0; i < j->linhas && sx < 0; i++) {
        for (int k = 0; k < j->colunas; k++) {
            if (isupper(j->atual[i][k])) {
                sx = i;
                sy = k;
                break;
            }
        }
    }
    
    int desconectado = 0;
    if (sx >= 0) {
        verificarCasa(j, sx, sy, visitado);
        
        // Verifica se alguma célula maiúscula ficou desconectada
        for (int i = 0; i < j->linhas && !desconectado; i++) {
            for (int k = 0; k < j->colunas; k++) {
                if (isupper(j->atual[i][k]) && !visitado[i][k]) {
                    desconectado = 1;
                    break;
                }
            }
        }
    }
    
    // Restaura o valor original
    j->atual[x][y] = original;
    return desconectado;
}

// 3) a função de “ajudar” devolve 1 se alterou algo, 0 caso contrário
int ajudar(Jogo *j) {
    if (j->linhas == 0) return 0;
    
    char novo[MAX][MAX];
    int mudou = 0;
    
    // Copia o estado atual
    for (int i = 0; i < j->linhas; i++) {
        for (int k = 0; k < j->colunas; k++) {
            novo[i][k] = j->atual[i][k];
        }
    }

    // Regra 1: Riscar células que criariam repetições
    for (int i = 0; i < j->linhas; i++) {
        for (int k = 0; k < j->colunas; k++) {
            if (islower(novo[i][k]) && precisaRiscar(j, i, k)) {
                novo[i][k] = '#';
                mudou = 1;
            }
        }
    }
    
    // Atualiza o tabuleiro com as mudanças até agora
    if (mudou) {
        for (int i = 0; i < j->linhas; i++) {
            for (int k = 0; k < j->colunas; k++) {
                j->atual[i][k] = novo[i][k];
            }
        }
    }

    // Regra 2: Pintar vizinhas de casas riscadas
    int mudou2 = 0;
    for (int i = 0; i < j->linhas; i++) {
        for (int k = 0; k < j->colunas; k++) {
            if (j->atual[i][k] == '#') {
                // Verifica vizinhos ortogonais
                int vizinhos[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};
                for (int v = 0; v < 4; v++) {
                    int ni = i + vizinhos[v][0];
                    int nk = k + vizinhos[v][1];
                    
                    if (ni >= 0 && ni < j->linhas && nk >= 0 && nk < j->colunas) {
                        if (islower(novo[ni][nk])) {
                            novo[ni][nk] = toupper(j->original[ni][nk]);
                            mudou2 = 1;
                        }
                    }
                }
            }
        }
    }
    
    if (mudou2) mudou = 1;

    // Regra 3: Pintar células que se riscadas isolariam outras
    for (int i = 0; i < j->linhas; i++) {
        for (int k = 0; k < j->colunas; k++) {
            if (islower(novo[i][k]) && isolaBrancos(j, i, k)) {
                novo[i][k] = toupper(j->original[i][k]);
                mudou = 1;
            }
        }
    }

    // Regra 4: Evitar dois '#' consecutivos
    for (int i = 0; i < j->linhas; i++) {
        for (int k = 0; k < j->colunas; k++) {
            if (islower(novo[i][k])) {
                int devePintar = 0;
                
                // Verifica se riscar criaria '#' consecutivos horizontalmente
                if ((k > 0 && novo[i][k-1] == '#') || 
                    (k < j->colunas-1 && novo[i][k+1] == '#')) {
                    devePintar = 1;
                }
                
                // Verifica se riscar criaria '#' consecutivos verticalmente
                if ((i > 0 && novo[i-1][k] == '#') || 
                    (i < j->linhas-1 && novo[i+1][k] == '#')) {
                    devePintar = 1;
                }
                
                if (devePintar) {
                    novo[i][k] = toupper(j->original[i][k]);
                    mudou = 1;
                }
            }
        }
    }

    // Aplica todas as mudanças
    if (mudou) {
        for (int i = 0; i < j->linhas; i++) {
            for (int k = 0; k < j->colunas; k++) {
                j->atual[i][k] = novo[i][k];
            }
        }
    }

    return mudou;
}




// Função para copiar o estado atual do tabuleiro
static void copiarEstado(Jogo *origem, Jogo *destino) {
    destino->linhas = origem->linhas;
    destino->colunas = origem->colunas;
    
    for (int i = 0; i < origem->linhas; i++) {
        for (int j = 0; j < origem->colunas; j++) {
            destino->original[i][j] = origem->original[i][j];
            destino->atual[i][j] = origem->atual[i][j];
        }
    }
    // Não copia o histórico para evitar problemas de memória
    destino->historico = NULL;
}

// Função para verificar se o estado atual é válido (sem violações)
static int estadoValido(Jogo *jogo) {
    return contarViolacoes(jogo) == 0;
}

// Função para verificar se o jogo está completo (sem minúsculas)
static int jogoCompleto(Jogo *jogo) {
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            if (islower(jogo->atual[i][j])) {
                return 0;
            }
        }
    }
    return 1;
}

// Função para encontrar a próxima célula não resolvida
static int encontrarProximaCelula(Jogo *jogo, int *x, int *y) {
    for (int i = 0; i < jogo->linhas; i++) {
        for (int j = 0; j < jogo->colunas; j++) {
            if (islower(jogo->atual[i][j])) {
                *x = i;
                *y = j;
                return 1;
            }
        }
    }
    return 0;
}

// Função melhorada de resolução com backtracking
static int resolverComBacktracking(Jogo *jogo) {
    // Aplica inferências básicas repetidamente até não haver mais mudanças
    int mudancas;
    do {
        mudancas = ajudar(jogo);
        // Se as inferências geraram um estado inválido, falha
        if (!estadoValido(jogo)) {
            return 0;
        }
    } while (mudancas);
    
    // Verifica se o jogo está completo e válido
    if (jogoCompleto(jogo)) {
        return estadoValido(jogo);
    }
    
    // Encontra a próxima célula para tentar resolver
    int x, y;
    if (!encontrarProximaCelula(jogo, &x, &y)) {
        return estadoValido(jogo);
    }
    
    // Cria cópias para testar as duas possibilidades
    Jogo jogoPintado = {0};
    Jogo jogoRiscado = {0};
    
    // Tenta pintar a célula (maiúscula)
    copiarEstado(jogo, &jogoPintado);
    jogoPintado.atual[x][y] = toupper(jogoPintado.original[x][y]);
    
    // Verifica se pintar não viola imediatamente as regras
    if (estadoValido(&jogoPintado)) {
        if (resolverComBacktracking(&jogoPintado)) {
            copiarEstado(&jogoPintado, jogo);
            return 1;
        }
    }
    
    // Tenta riscar a célula (#)
    copiarEstado(jogo, &jogoRiscado);
    jogoRiscado.atual[x][y] = '#';
    
    // Verifica se riscar não viola imediatamente as regras
    if (estadoValido(&jogoRiscado)) {
        if (resolverComBacktracking(&jogoRiscado)) {
            copiarEstado(&jogoRiscado, jogo);
            return 1;
        }
    }
    
    // Nenhuma das opções funcionou
    return 0;
}



// Nova função para resolver completamente com backtracking
int resolver_completamente(Jogo *g) {
    if (g->linhas == 0) {
        return 0;
    }
    
    return resolverComBacktracking(g);
}

