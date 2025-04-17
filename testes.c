#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "projeto.h"
#include "projeto_funcoes.c"  

void testeMaiuscula() {
    assert(maiuscula('a') == 'A');
    assert(maiuscula('z') == 'Z');
    assert(maiuscula('A') == 'A');
    assert(maiuscula('1') == '1');
    printf("testeMaiuscula passou.\n");
}

void testeCarregarMatriz() {
    Jogo j;
    assert(carregar_matriz(&j, "tabuleiro.txt") == 1);
    assert(j.linhas > 0 && j.colunas > 0);
    printf("testeCarregarMatriz passou.\n");
}

void testeCopiaImprimeTabuleiro() {
    Jogo j;
    carregar_matriz(&j, "tabuleiro.txt");

    // Faz alteração
    j.atual[0][0] = 'X';
    copiaTabuleiro(&j);
    j.atual[0][0] = 'Y';

    // Desfaz
    imprimeTabuleiro(&j);
    assert(j.atual[0][0] == 'X'); // Verifica se restaurou
    printf("testeCopiaImprimeTabuleiro passou.\n");
}

void testeVerificarRestricoes_sem_erros() {
    Jogo j;
    carregar_matriz(&j, "tabuleiro.txt");

    // Simula um estado válido
    for (int i = 0; i < j.linhas; i++) {
        for (int k = 0; k < j.colunas; k++) {
            j.atual[i][k] = j.original[i][k];
        }
    }

    verificarRestricoes(&j);
    printf("testeVerificarRestricoes_sem_erros passou.\n");
}

void testeVerificarRestricoes_com_erros() {
    Jogo j;
    carregar_matriz(&j, "tabuleiro.txt");

    // Simula erros
    j.atual[0][0] = 'A';
    j.atual[0][1] = 'A'; // símbolo repetido na linha
    j.atual[1][0] = '#';
    j.atual[2][0] = '#'; // dois # verticais

    verificarRestricoes(&j);
    printf("testeVerificarRestricoes_com_erros passou.\n");
}

void testeMostrar() {
    Jogo j;
    carregar_matriz(&j, "tabuleiro.txt");
    mostrar(&j);
    printf("testeMostrar passou.\n");
}

int main() {
    test_maiuscula();
    test_carregar_matriz();
    test_copia_e_imprimeTabuleiro();
    test_verificarRestricoes_sem_erros();
    test_verificarRestricoes_com_erros();
    test_mostrar();

    printf("✅ Todos os testes passaram com sucesso!\n");
    return 0;
}

