#include <stdio.h>
#include "projeto.h"

int main() {
    Jogo jogo = {0}; // Inicializa todas as variáveis para 0/NULL
    char comando;
    int linha, coluna;

    printMenu();

    while (1) {
        comando = getchar();
        while (comando == '\n' || comando == ' ') comando = getchar(); // Ignora newlines e espaços

        if (comando == 's') break;

        switch (comando) {
            case 'l': // Carregar
                while (getchar() != '\n'); // Limpa buffer
                if (carregar_matriz(&jogo, "tabuleiro.txt")) {
                    mostrar(&jogo);
                }
                break;

            case 'b': // Pintar
                if (scanf("%d %d", &linha, &coluna) == 2) {
                    if (linha >= 0 && linha < jogo.linhas && coluna >= 0 && coluna < jogo.colunas) {
                        copiaTabuleiro(&jogo);
                        jogo.atual[linha][coluna] = maiuscula(jogo.original[linha][coluna]);
                        mostrar(&jogo);
                    }
                }
                while (getchar() != '\n'); // Limpa buffer
                break;

            case 'r': // Riscar
                if (scanf("%d %d", &linha, &coluna) == 2) {
                    if (linha >= 0 && linha < jogo.linhas && coluna >= 0 && coluna < jogo.colunas) {
                        copiaTabuleiro(&jogo);
                        jogo.atual[linha][coluna] = '#';
                        mostrar(&jogo);
                    }
                }
                while (getchar() != '\n');
                break;

            case 'd': // Desfazer
                imprimeTabuleiro(&jogo);
                mostrar(&jogo);
                break;

            case 'v': // Verificar
                verificarRestricoes(&jogo);
                break;

            case 'c': // Menu
                printMenu();
                break;

            case 'g': // Gravar
                while (getchar() != '\n');
                char filename[100];
                printf("Nome do arquivo para salvar: ");
                if (scanf("%99s", filename) != 1) {
                    printf("Erro ao ler o nome do ficheiro.\n");
                    break;
                }
                
                salvar_jogo(&jogo, filename);
                while (getchar() != '\n');
                break;

            case 'a': // Dica
                if (jogo.linhas == 0) {
                    printf("Carregue um tabuleiro primeiro!\n");
                } else {
                    int x, y;
                    if (dar_dica(&jogo, &x, &y)) {
                        printf("Dica: Tente a posicao (%d, %d)\n", x, y);
                    } else {
                        printf("Nenhuma dica disponivel!\n");
                    }
                }
                while (getchar() != '\n');
                break;

            case 'R': // Resolver
                if (jogo.linhas == 0) {
                    printf("Carregue um tabuleiro primeiro!\n");
                } else {
                    resolver_jogo(&jogo);
                    mostrar(&jogo);
                }
                while (getchar() != '\n');
                break;

            default:
                printf("Comando invalido!\n");
                while (getchar() != '\n');
        }
    }

    // Limpa histórico ao sair
    while (jogo.historico != NULL) {
        imprimeTabuleiro(&jogo);
    }

    return 0;
}