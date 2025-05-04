#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "projeto.h"
#include <stdio.h>

// --- Testes individuais ---
// Cada teste inicializa Jogo a zero e, no final, chama limparJogo(&j).

void test_maiuscula() {
    Jogo j = {0};  // garante campos a NULL/0
    (void)j;       // j não é usado aqui, mas iniciamos por consistência

    CU_ASSERT(maiuscula('a') == 'A');
    CU_ASSERT(maiuscula('z') == 'Z');
    CU_ASSERT(maiuscula('A') == 'A');
    CU_ASSERT(maiuscula('1') == '1');

    limparJogo(&j);
}

void test_carregar_matriz() {
    Jogo j = {0};

    CU_ASSERT(carregar_matriz(&j, "tabuleiro.txt") == 1);
    CU_ASSERT(j.linhas > 0 && j.colunas > 0);

    limparJogo(&j);
}

void test_copia_imprime_tabuleiro() {
    Jogo j = {0};
    carregar_matriz(&j, "tabuleiro.txt");

    j.atual[0][0] = 'X';
    copiaTabuleiro(&j);
    j.atual[0][0] = 'Y';

    imprimeTabuleiro(&j);
    CU_ASSERT(j.atual[0][0] == 'X');

    limparJogo(&j);
}

void test_verificarRestricoes_sem_erros() {
    Jogo j = {0};
    j.linhas = 3;
    j.colunas = 3;

    char valido[3][3] = {
        {'A', 'B', 'C'},
        {'D', 'E', 'F'},
        {'G', 'H', 'I'}
    };
    for(int i = 0; i < 3; i++)
        for(int k = 0; k < 3; k++) {
            j.original[i][k] = valido[i][k];
            j.atual[i][k]    = valido[i][k];
        }

    verificarRestricoes(&j);
    // não há retorno, apenas certifica-se de não crashar

    limparJogo(&j);
}

void test_verificarRestricoes_com_erros() {
    Jogo j = {0};
    carregar_matriz(&j, "tabuleiro.txt");

    j.atual[0][0] = 'A';
    j.atual[0][1] = 'A';
    j.atual[1][0] = '#';
    j.atual[2][0] = '#';

    verificarRestricoes(&j);
    // verifica mensagens no stdout, sem asserts

    limparJogo(&j);
}

void test_mostrar() {
    Jogo j = {0};
    carregar_matriz(&j, "tabuleiro.txt");
    mostrar(&j);

    limparJogo(&j);
}

// --- Main com CUnit ---

int main() {
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("Testes do Projeto", NULL, NULL);

    CU_add_test(suite, "maiuscula()",                test_maiuscula);
    CU_add_test(suite, "carregar_matriz()",          test_carregar_matriz);
    CU_add_test(suite, "copiaTabuleiro/imprimeTabuleiro", test_copia_imprime_tabuleiro);
    CU_add_test(suite, "verificarRestricoes sem erros",  test_verificarRestricoes_sem_erros);
    CU_add_test(suite, "verificarRestricoes com erros",  test_verificarRestricoes_com_erros);
    CU_add_test(suite, "mostrar()",                  test_mostrar);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}
