#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "projeto.h"
#include <stdio.h>
#include <stdlib.h>

// --- Testes Individuais ---

void test_maiuscula() {
    Jogo j = {0};
    (void)j;

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

    for (int i = 0; i < 3; i++)
        for (int k = 0; k < 3; k++) {
            j.original[i][k] = valido[i][k];
            j.atual[i][k]    = valido[i][k];
        }

    verificarRestricoes(&j);

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

    limparJogo(&j);
}

void test_mostrar() {
    Jogo j = {0};
    carregar_matriz(&j, "tabuleiro.txt");
    mostrar(&j);
    limparJogo(&j);
}

void test_limparJogo_com_historico() {
    Jogo j = {0};
    Estado *e1 = malloc(sizeof(Estado));
    Estado *e2 = malloc(sizeof(Estado));
    e1->prox = e2;
    e2->prox = NULL;
    j.historico = e1;

    limparJogo(&j);
}

void test_salvar_jogo() {
    Jogo j = {0};
    carregar_matriz(&j, "tabuleiro.txt");

    salvar_jogo(&j, "saida.txt");  // não testamos retorno, só que não crasha

    limparJogo(&j);
}

void test_resolver_jogo() {
    Jogo j = {0};

    resolve_jogo(&j);  // sem matriz, não deve crashar

    carregar_matriz(&j, "tabuleiro.txt");
    resolve_jogo(&j);  // com matriz, também não deve crashar

    limparJogo(&j);
}

void test_dar_dica() {
    Jogo j = {0};
    carregar_matriz(&j, "tabuleiro.txt");

    int x = -1, y = -1;
    int resultado = dar_dica(&j, &x, &y);

    CU_ASSERT(resultado == 0 || resultado == 1);

    limparJogo(&j);
}

void test_printMenu() {
    printMenu();
}

// --- Main com CUnit ---

int main() {
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("Testes do Projeto", NULL, NULL);

    CU_add_test(suite, "maiuscula()", test_maiuscula);
    CU_add_test(suite, "carregar_matriz()", test_carregar_matriz);
    CU_add_test(suite, "copiaTabuleiro/imprimeTabuleiro", test_copia_imprime_tabuleiro);
    CU_add_test(suite, "verificarRestricoes sem erros", test_verificarRestricoes_sem_erros);
    CU_add_test(suite, "verificarRestricoes com erros", test_verificarRestricoes_com_erros);
    CU_add_test(suite, "mostrar()", test_mostrar);
    CU_add_test(suite, "limparJogo com histórico", test_limparJogo_com_historico);
    CU_add_test(suite, "salvar_jogo()", test_salvar_jogo);
    CU_add_test(suite, "resolver_jogo()", test_resolver_jogo);
    CU_add_test(suite, "dar_dica()", test_dar_dica);
    CU_add_test(suite, "printMenu()", test_printMenu);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}
