#include <stdio.h>

#define LINHAS 26
#define COLUNAS 26

typedef struct {
    char original[LINHAS][COLUNAS];
    char atual[LINHAS][COLUNAS];
} Jogo;

Jogo jogo;

void carregar_matriz(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo\n");
        return;
    }

    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            if (fscanf(file, " %c", &jogo.original[i][j]) != 1) {
                printf("Erro ao ler o arquivo\n");
                fclose(file);
                return;
            }
            jogo.atual[i][j] = jogo.original[i][j];
        }
    }

    fclose(file);
}

void mostrar() {
    printf("\n");
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
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

    carregar_matriz("tabuleiro.txt");

    while (1) {
        mostrar();

        printf("Digite (ex: 0 0 M para maiúscula, 1 2 # riscar, 3 4 R resetar): ");
        scanf("%d %d %c", &linha, &coluna, &acao);

        if (linha < 0 || linha >= LINHAS || coluna < 0 || coluna >= COLUNAS) {
            printf("Coordenadas inválidas!\n");
            continue;
        }

        if (acao == 'M') { // Tornar maiúscula
            jogo.atual[linha][coluna] = jogo.original[linha][coluna] - 32;
        } else if (acao == '#') { // Riscar
            jogo.atual[linha][coluna] = '#';
        } else if (acao == 'R') { // Resetar para original
            resetar_celula(linha, coluna);
        } else {
            printf("Ação inválida! Use M, # ou R\n");
        }
    }

    return 0;
}