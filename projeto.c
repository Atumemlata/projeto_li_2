#include <stdio.h>
#include "projeto.h"

int main() {
    Jogo jogo;
    char comando;
    int linha, coluna;

    while (1) {
        comando = getchar();
        while (comando == '\n') comando = getchar();


        if (comando == 's') break;       // programa corre até pressionar a tecla s para sair 

        if (comando == 'l') {            // carrega o tabuleiro
            while (getchar() != '\n');
            if (carregar_matriz(&jogo, "tabuleiro.txt")) {
                mostrar(&jogo);
            }
        } else if (comando == 'b') {
            if (scanf("%d %d", &linha, &coluna) == 2) {
                while (getchar() != '\n');
                if (linha >= 0 && linha < jogo.linhas && coluna >= 0 && coluna < jogo.colunas) {
                    copiaTabuleiro(&jogo);
                    jogo.atual[linha][coluna] = maiuscula(jogo.original[linha][coluna]);
                    mostrar(&jogo);
                }
            } else {
                while (getchar() != '\n');
            }
        } else if (comando == 'r') {
            if (scanf("%d %d", &linha, &coluna) == 2) {
                while (getchar() != '\n');
                if (linha >= 0 && linha < jogo.linhas && coluna >= 0 && coluna < jogo.colunas) {
                    copiaTabuleiro(&jogo);
                    jogo.atual[linha][coluna] = '#';
                    mostrar(&jogo);
                }
            } else {
                while (getchar() != '\n');
            }  // limpa tudo o que não interessa ler
        } else if (comando == 'd') {
            imprimeTabuleiro(&jogo);
            mostrar(&jogo);
        } else if (comando == 'v') {
            verificarRestricoes(&jogo);
        } else {
            while (getchar() != '\n');
        }
    }
    
    while (jogo.historico != NULL) {
        imprimeTabuleiro(&jogo);
    }

    return 0;
}