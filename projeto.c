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

            case 'a': // Dar dica (uma inferência)
                if (jogo.linhas == 0) {
                    printf("Carregue um tabuleiro primeiro!\n");
                } else {
                    copiaTabuleiro(&jogo);
                    if (ajudar(&jogo)) {
                        mostrar(&jogo);
                    } else {
                        printf("Nenhuma dica disponivel.\n");
                        // Remove o estado salvo já que não houve mudança
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
                    // Salva o estado inicial uma única vez
                    copiaTabuleiro(&jogo);
                    
                    int totalMudancas = 0;
                    // Aplica inferências repetidamente até não haver mais mudanças
                    while (ajudar(&jogo)) {
                        totalMudancas = 1;
                    }
                    
                    if (totalMudancas) {
                        mostrar(&jogo);
                    } else {
                        printf("Nenhuma dica disponivel.\n");
                        // Remove o estado salvo já que não houve mudança
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
                    // Salva o estado atual antes de tentar resolver
                    copiaTabuleiro(&jogo);
                    
                    if (resolver_completamente(&jogo)) {
                        // Verifica se está realmente resolvido
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
                            // Restaura o estado anterior
                            imprimeTabuleiro(&jogo);
                        }
                    } else {
                        printf("Impossivel resolver.\n");
                        // Restaura o estado anterior
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

    // Limpa histórico ao sair
    limparJogo(&jogo);
    return 0;
}