#include <stdio.h>
#include "projeto.h"

int main() {
    Jogo jogo = {0};
    char comando;
    int linha, coluna;

    printMenu();

    while (1) {
        comando = getchar();
        while (comando == '\n' || comando == ' ')
            comando = getchar();

        if (comando == 's')
            break;

        switch (comando) {
            case 'l': // Carregar
                while (getchar() != '\n'); // Limpa buffer
                if (carregar_matriz(&jogo, "tabuleiro.txt")) {
                    mostrar(&jogo);
                }
                break;

            case 'b': // Pintar
                if (scanf("%d %d", &linha, &coluna) == 2) {
                    if (linha >= 0 && linha < jogo.linhas &&
                        coluna >= 0 && coluna < jogo.colunas) {
                        copiaTabuleiro(&jogo);
                        jogo.atual[linha][coluna] = maiuscula(jogo.original[linha][coluna]);
                        mostrar(&jogo);
                    }
                }
                while (getchar() != '\n'); // Limpa buffer
                break;

            case 'r': // Riscar
                if (scanf("%d %d", &linha, &coluna) == 2) {
                    if (linha >= 0 && linha < jogo.linhas &&
                        coluna >= 0 && coluna < jogo.colunas) {
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

            case 'g': { // Gravar
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
            }

            case 'a': // Dar dica
                if (jogo.linhas == 0) {
                    printf("Carregue um tabuleiro primeiro!\n");
                } else {
                    copiaTabuleiro(&jogo);
                    if (!ajudar(&jogo))
                        printf("Nada mais a inferir.\n");
                    mostrar(&jogo);
                }
                while (getchar() != '\n');
                break;

            case 'A': // Dar todas as dicas
                if (jogo.linhas == 0) {
                    printf("Carregue um tabuleiro primeiro!\n");
                } else {
                    while (1) {
                        copiaTabuleiro(&jogo);
                        if (!ajudar(&jogo))
                            break;
                    }
                    mostrar(&jogo);
                }
                while (getchar() != '\n');
                break;

            case 'R': { // Resolver
                if (jogo.linhas == 0) {
                    printf("Carregue um tabuleiro primeiro!\n");
                    if (getchar() != '\n') {} // limpa eventual '\n'
                } else {
                    // Limpa o '\n' que havia ficado no buffer
                    if (getchar() != '\n') {}

                    // 1) Aplica todas as inferências (função já salva estados, mas não mostra nada)
                    resolve_jogo(&jogo);

                    // 2) Conta quantas violações esse tabuleiro final apresenta
                    int totalViol = contarViolacoes(&jogo);

                    if (totalViol > 0) {
                        // Só imprime “Impossivel” — não lista cada violação
                        printf("Impossivel\n");
                    } else {
                        // Se não houver violações, exibe o tabuleiro final resolvido
                        mostrar(&jogo);
                    }
                }
                break;
            }

            default:
                printf("Comando invalido!\n");
                while (getchar() != '\n');
        }
    }

    // Limpa histórico ao sair
    limparJogo(&jogo);
    return 0;
}
