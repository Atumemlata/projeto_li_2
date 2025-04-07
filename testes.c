#include <stdio.h>
#include <assert.h>
#include "projeto.h"
#include "projeto_funcoes.c" 

void test_maiuscula() {
    assert(maiuscula('a') == 'A');
    assert(maiuscula('z') == 'Z');
    assert(maiuscula('A') == 'A');
}

void test_carregar_matriz() {
    Jogo j;
    assert(carregar_matriz(&j, "tabuleiro.txt") == 1);
    assert(j.linhas > 0 && j.colunas > 0);
}

int main() {
    test_maiuscula();
    test_carregar_matriz();
    printf("Todos os testes passaram!\n");
    return 0;
}
