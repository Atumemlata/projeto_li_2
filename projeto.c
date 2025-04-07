#include <stdio.h>
#include "projeto.h"
#include "projeto_funcoes.c"

int main() {
    Jogo jogo;
    char comando;
    int linha, coluna;

    while (1) {
        comando = getchar();

        if (comando == 's') break;

        if (comando == 'l') {
            while (getchar() != '\n');
            if (carregar_matriz(&jogo, "tabuleiro.txt")) {
                mostrar(&jogo);
            }
        } else if (comando == 'b') {
            if (scanf("%d %d", &linha, &coluna) == 2) {
                if (linha >= 0 && linha < jogo.linhas && coluna >= 0 && coluna < jogo.colunas) {
                    jogo.atual[linha][coluna] = maiuscula(jogo.original[linha][coluna]);
                    mostrar(&jogo);
                }
            }
            while (getchar() != '\n');
        } else if (comando == 'r') {
            if (scanf("%d %d", &linha, &coluna) == 2) {
                if (linha >= 0 && linha < jogo.linhas && coluna >= 0 && coluna < jogo.colunas) {
                    jogo.atual[linha][coluna] = '#';
                    mostrar(&jogo);
                }
            }
            while (getchar() != '\n');
        } else {
            while (getchar() != '\n');
        }
    }

    return 0;
}
