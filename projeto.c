#include <stdio.h>

#define LINHAS 5
#define COLUNAS 5

typedef struct {
    char original[LINHAS][COLUNAS];
    char atual[LINHAS][COLUNAS];
} Jogo;

// Matriz inicial pré-definida
Jogo jogo = {
    .original = {
        {'e','c','a','d','c'},
        {'d','c','d','e','c'},
        {'b','d','d','c','e'},
        {'c','d','e','e','b'},
        {'a','c','c','b','b'}
    },
    .atual = {
        {'e','c','a','d','c'},
        {'d','c','d','e','c'},
        {'b','d','d','c','e'},
        {'c','d','e','e','b'},
        {'a','c','c','b','b'}
    }
};

void mostrar() {
    printf("\n");
    for(int i = 0; i < LINHAS; i++) {
        for(int j = 0; j < COLUNAS; j++) {
            printf("%c ", jogo.atual[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void resetar_celula(int linha, int coluna) {
    jogo.atual[linha][coluna] = jogo.original[linha][coluna];
}

int main() {
    int linha, coluna;
    char acao;

    while(1) {
        mostrar();
        
        printf("Digite (ex: 0 0 M para maiúscula, 1 2 # riscar, 3 4 R resetar): ");
        scanf("%d %d %c", &linha, &coluna, &acao);
        
        if(linha < 0 || linha >= LINHAS || coluna < 0 || coluna >= COLUNAS) {
            printf("Coorden#include <stdio.h>

#define LINHAS 5
#define COLUNAS 5

char tabuleiro[LINHAS][COLUNAS] = {
    {'e','c','a','d','c'},
    {'d','c','d','e','c'},
    {'b','d','d','c','e'},
    {'c','d','e','e','b'},
    {'a','c','c','b','b'}
};

void mostrar() {
    printf("\n");
    for(int i = 0; i < LINHAS; i++) {
        for(int j = 0; j < COLUNAS; j++) {
            printf("%c ", tabuleiro[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main() {
    int linha, coluna;
    char acao;

    while(1) {
        mostrar();
        
        printf("Digite (ex: 0 0 M para maiúscula ou 1 2 #): ");
        scanf("%d %d %c", &linha, &coluna, &acao);
        
        if(linha < 0 || linha >= LINHAS || coluna < 0 || coluna >= COLUNAS) {
            printf("Coordenadas inválidas!\n");
            continue;
        }

        if(acao == 'M') tabuleiro[linha][coluna] -= 32;
        else if(acao == '#') tabuleiro[linha][coluna] = '#';
        else printf("Ação inválida! Use M ou #\n");
    }

    return 0;
}adas inválidas!\n");
            continue;
        }

        if(acao == 'M') { // Tornar maiúscula
            jogo.atual[linha][coluna] = jogo.original[linha][coluna] - 32;
        }
        else if(acao == '#') { // Riscar
            jogo.atual[linha][coluna] = '#';
        }
        else if(acao == 'R') { // Resetar para original
            resetar_celula(linha, coluna);
        }
        else {
            printf("Ação inválida! Use M, # ou R\n");
        }
    }

    return 0;
}