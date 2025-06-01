#include <stdio.h>
#include <string.h>
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
            case 'l': { // Carregar com argumento imediato
                char filename[100];
                // Lê logo o nome do ficheiro após o 'l'
                if (scanf("%99s", filename) != 1) {
                    printf("Erro: espera um nome de ficheiro após 'l'.\n");
                } else {
                    if (carregar_matriz(&jogo, filename)) {
                        mostrar(&jogo);
                    } else {
                        printf("Falha ao carregar '%s'.\n", filename);
                    }
                }
                // Limpa o restante da linha (incluindo '\n')
                while (getchar() != '\n');
                break;
            }

            case 'b': // Pintar
                if (scanf("%d %d", &linha, &coluna) == 2) {
                    if (linha >= 0 && linha < jogo.linhas &&
                        coluna >= 0 && coluna < jogo.colunas) {
                        copiaTabuleiro(&jogo);
                        jogo.atual[linha][coluna] = maiuscula(jogo.original[linha][coluna]);
                        mostrar(&jogo);
                    }
                }
                while (getchar() != '\n');
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
                char filename_save[100];
                // Lê nome do ficheiro após 'g'
                if (scanf("%99s", filename_save) != 1) {
                    printf("Erro ao ler o nome do ficheiro.\n");
                } else {
                    salvar_jogo(&jogo, filename_save);
                }
                while (getchar() != '\n');
                break;
            }

            case 'a': // Dar dica (uma inferência)
                if (jogo.linhas == 0) {
                    printf("Carregue um tabuleiro primeiro!\n");
                } else {
                    copiaTabuleiro(&jogo);
                    if (ajudar(&jogo)) {
                        mostrar(&jogo);
                    } else {
                        printf("Nenhuma dica disponivel.\n");
                        if (jogo.historico != NULL) {
                            Estado *temp = jogo.historico;
                            jogo.historico = jogo.historico->prox;
                            free(temp);
                        }
                    }
                }
                while (getchar() != '\n');
                break;

            case 'A': // Dar todas as dicas possíveis
                if (jogo.linhas == 0) {
                    printf("Carregue um tabuleiro primeiro!\n");
                } else {
                    copiaTabuleiro(&jogo);
                    int totalMudancas = 0;
                    while (ajudar(&jogo)) {
                        totalMudancas = 1;
                    }
                    if (totalMudancas) {
                        mostrar(&jogo);
                    } else {
                        printf("Nenhuma dica disponivel.\n");
                        if (jogo.historico != NULL) {
                            Estado *temp = jogo.historico;
                            jogo.historico = jogo.historico->prox;
                            free(temp);
                        }
                    }
                }
                while (getchar() != '\n');
                break;

            case 'R': { // Resolver
                if (jogo.linhas == 0) {
                    printf("Carregue um tabuleiro primeiro!\n");
                } else {
                    copiaTabuleiro(&jogo);
                    if (resolver_completamente(&jogo)) {
                        int totalViol = contarViolacoes(&jogo);
                        int celulasNaoResolvidas = 0;
                        for (int i = 0; i < jogo.linhas && !celulasNaoResolvidas; i++) {
                            for (int j = 0; j < jogo.colunas; j++) {
                                if (islower(jogo.atual[i][j])) {
                                    celulasNaoResolvidas = 1;
                                    break;
                                }
                            }
                        }
                        if (totalViol == 0 && !celulasNaoResolvidas) {
                            printf("Jogo resolvido!\n");
                            mostrar(&jogo);
                        } else {
                            printf("Impossivel resolver.\n");
                            imprimeTabuleiro(&jogo);
                        }
                    } else {
                        printf("Impossivel resolver.\n");
                        imprimeTabuleiro(&jogo);
                    }
                }
                while (getchar() != '\n');
                break;
            }

            default:
                printf("Comando invalido!\n");
                while (getchar() != '\n');
        }
    }

    limparJogo(&jogo);
    return 0;
}
