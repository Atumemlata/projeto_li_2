#include <assert.h>
#include "projeto.h"


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
    
    // Criar tabuleiro válido manualmente para teste
    j.linhas = 3;
    j.colunas = 3;
    
    // Tabuleiro válido (sem repetições ou regras quebradas)
    char valido[3][3] = {
        {'A', 'B', 'C'},
        {'D', 'E', 'F'},
        {'G', 'H', 'I'}
    };
    
    for(int i = 0; i < 3; i++) {
        for(int k = 0; k < 3; k++) {
            j.original[i][k] = valido[i][k];
            j.atual[i][k] = valido[i][k];
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
    testeMaiuscula();
    testeCarregarMatriz();
    testeCopiaImprimeTabuleiro();
    testeVerificarRestricoes_sem_erros();
    testeVerificarRestricoes_com_erros();
    testeMostrar();

    printf("Todos os testes passaram com sucesso!\n");
    return 0;
}

