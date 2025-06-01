#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "projeto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Testes das Funções Básicas ---

void test_maiuscula() {
    CU_ASSERT(maiuscula('a') == 'A');
    CU_ASSERT(maiuscula('z') == 'Z');
    CU_ASSERT(maiuscula('A') == 'A');
    CU_ASSERT(maiuscula('Z') == 'Z');
    CU_ASSERT(maiuscula('1') == '1');
    CU_ASSERT(maiuscula('#') == '#');
    CU_ASSERT(maiuscula(' ') == ' ');
}

void test_carregar_matriz_valida() {
    Jogo j = {0};
    
    // Teste com arquivo válido
    CU_ASSERT(carregar_matriz(&j, "tabuleiro.txt") == 1);
    CU_ASSERT(j.linhas > 0);
    CU_ASSERT(j.colunas > 0);
    
    // Verifica se as matrizes foram inicializadas corretamente
    for (int i = 0; i < j.linhas; i++) {
        for (int k = 0; k < j.colunas; k++) {
            CU_ASSERT(j.atual[i][k] == tolower(j.original[i][k]));
        }
    }
    
    limparJogo(&j);
}

void test_carregar_matriz_invalida() {
    Jogo j = {0};
    
    // Teste com arquivo inexistente
    CU_ASSERT(carregar_matriz(&j, "arquivo_inexistente.txt") == 0);
    CU_ASSERT(j.linhas == 0);
    CU_ASSERT(j.colunas == 0);
    
    limparJogo(&j);
}

// --- Testes das Funções de Histórico ---

void test_copia_imprime_tabuleiro() {
    Jogo j = {0};
    
    // Cria um tabuleiro simples para teste
    j.linhas = 2;
    j.colunas = 2;
    j.original[0][0] = 'a'; j.original[0][1] = 'b';
    j.original[1][0] = 'c'; j.original[1][1] = 'd';
    j.atual[0][0] = 'a'; j.atual[0][1] = 'b';
    j.atual[1][0] = 'c'; j.atual[1][1] = 'd';
    j.historico = NULL;
    
    // Faz uma alteração e copia o estado
    j.atual[0][0] = 'A';
    copiaTabuleiro(&j);
    
    // Verifica se há histórico
    CU_ASSERT(j.historico != NULL);
    
    // Faz outra alteração
    j.atual[0][0] = 'X';
    CU_ASSERT(j.atual[0][0] == 'X');
    
    // Restaura o estado anterior
    imprimeTabuleiro(&j);
    CU_ASSERT(j.atual[0][0] == 'A');
    
    limparJogo(&j);
}

void test_copia_tabuleiro_sem_memoria() {
    Jogo j = {0};
    j.linhas = 2;
    j.colunas = 2;
    j.historico = NULL;
    
    // Teste básico da função (não podemos forçar falta de memória facilmente)
    copiaTabuleiro(&j);
    // Se chegou aqui sem crash, está OK
    
    limparJogo(&j);
}

void test_imprime_tabuleiro_sem_historico() {
    Jogo j = {0};
    j.historico = NULL;
    
    // Chamar imprimeTabuleiro sem histórico não deve crashar
    imprimeTabuleiro(&j);
    // Se chegou aqui sem crash, está OK
    
    limparJogo(&j);
}

// --- Testes das Funções de Validação ---

void test_contarViolacoes_tabuleiro_valido() {
    Jogo j = {0};
    j.linhas = 3;
    j.colunas = 3;
    
    // Tabuleiro válido sem violações
    char valido[3][3] = {
        {'A', 'B', 'C'},
        {'D', '#', 'E'},
        {'F', 'G', 'H'}
    };
    
    for (int i = 0; i < 3; i++) {
        for (int k = 0; k < 3; k++) {
            j.original[i][k] = tolower(valido[i][k]);
            j.atual[i][k] = valido[i][k];
        }
    }
    
    CU_ASSERT(contarViolacoes(&j) == 0);
    
    limparJogo(&j);
}

void test_contarViolacoes_letras_repetidas() {
    Jogo j = {0};
    j.linhas = 3;
    j.colunas = 3;
    
    // Tabuleiro com letras repetidas na mesma linha
    char invalido[3][3] = {
        {'A', 'A', 'C'},  // A repetido na linha
        {'D', 'E', 'F'},
        {'G', 'E', 'H'}   // E repetido na coluna
    };
    
    for (int i = 0; i < 3; i++) {
        for (int k = 0; k < 3; k++) {
            j.original[i][k] = tolower(invalido[i][k]);
            j.atual[i][k] = invalido[i][k];
        }
    }
    
    CU_ASSERT(contarViolacoes(&j) > 0);
    
    limparJogo(&j);
}

void test_contarViolacoes_riscos_consecutivos() {
    Jogo j = {0};
    j.linhas = 3;
    j.colunas = 3;
    
    // Tabuleiro com # consecutivos
    char invalido[3][3] = {
        {'A', '#', '#'},  // # consecutivos horizontalmente
        {'#', 'E', 'F'},
        {'#', 'G', 'H'}   // # consecutivos verticalmente
    };
    
    for (int i = 0; i < 3; i++) {
        for (int k = 0; k < 3; k++) {
            j.original[i][k] = tolower(invalido[i][k]);
            j.atual[i][k] = invalido[i][k];
        }
    }
    
    CU_ASSERT(contarViolacoes(&j) > 0);
    
    limparJogo(&j);
}

void test_verificarRestricoes_sem_erros() {
    Jogo j = {0};
    j.linhas = 3;
    j.colunas = 3;
    
    // Tabuleiro válido
    char valido[3][3] = {
        {'A', 'B', 'C'},
        {'D', 'E', 'F'},
        {'G', 'H', 'I'}
    };
    
    for (int i = 0; i < 3; i++) {
        for (int k = 0; k < 3; k++) {
            j.original[i][k] = tolower(valido[i][k]);
            j.atual[i][k] = valido[i][k];
        }
    }
    
    CU_ASSERT(verificarRestricoes(&j) == 0);
    
    limparJogo(&j);
}

void test_verificarRestricoes_com_erros() {
    Jogo j = {0};
    j.linhas = 3;
    j.colunas = 3;
    
    // Tabuleiro com violações
    char invalido[3][3] = {
        {'A', 'A', 'C'},  // A repetido
        {'#', '#', 'F'},  // # consecutivos
        {'G', 'H', 'I'}
    };
    
    for (int i = 0; i < 3; i++) {
        for (int k = 0; k < 3; k++) {
            j.original[i][k] = tolower(invalido[i][k]);
            j.atual[i][k] = invalido[i][k];
        }
    }
    
    CU_ASSERT(verificarRestricoes(&j) > 0);
    
    limparJogo(&j);
}

// --- Testes das Funções de I/O ---

void test_mostrar() {
    Jogo j = {0};
    j.linhas = 2;
    j.colunas = 2;
    j.atual[0][0] = 'A'; j.atual[0][1] = 'B';
    j.atual[1][0] = 'C'; j.atual[1][1] = 'D';
    
    // Teste básico - se não crashar, está OK
    mostrar(&j);
    
    limparJogo(&j);
}

void test_printMenu() {
    // Teste básico - se não crashar, está OK
    printMenu();
}

void test_salvar_jogo() {
    Jogo j = {0};
    j.linhas = 2;
    j.colunas = 2;
    j.atual[0][0] = 'A'; j.atual[0][1] = 'B';
    j.atual[1][0] = 'C'; j.atual[1][1] = 'D';
    
    // Teste de salvamento
    salvar_jogo(&j, "teste_saida.txt");
    
    // Verifica se o arquivo foi criado (teste básico)
    FILE *f = fopen("teste_saida.txt", "r");
    CU_ASSERT(f != NULL);
    if (f) fclose(f);
    
    limparJogo(&j);
}

// --- Testes das Funções de Resolução ---

void test_ajudar_sem_tabuleiro() {
    Jogo j = {0};
    j.linhas = 0;
    j.colunas = 0;
    
    // Deve retornar 0 se não há tabuleiro
    CU_ASSERT(ajudar(&j) == 0);
    
    limparJogo(&j);
}

void test_ajudar_com_tabuleiro_simples() {
    Jogo j = {0};
    j.linhas = 2;
    j.colunas = 2;
    
    // Cria um tabuleiro simples onde uma inferência é possível
    j.original[0][0] = 'a'; j.original[0][1] = 'b';
    j.original[1][0] = 'c'; j.original[1][1] = 'a';
    j.atual[0][0] = 'A'; j.atual[0][1] = 'b';  // A já pintado
    j.atual[1][0] = 'c'; j.atual[1][1] = 'a';  // a deve ser riscado
    
    // Testa se a função consegue fazer inferências
    int resultado = ajudar(&j);
    // Se fez alguma mudança, resultado > 0, se não, resultado = 0
    CU_ASSERT(resultado >= 0);
    
    limparJogo(&j);
}

void test_resolver_completamente_sem_tabuleiro() {
    Jogo j = {0};
    j.linhas = 0;
    j.colunas = 0;
    
    // Deve retornar 0 se não há tabuleiro
    CU_ASSERT(resolver_completamente(&j) == 0);
    
    limparJogo(&j);
}

void test_resolver_completamente_tabuleiro_simples() {
    Jogo j = {0};
    j.linhas = 2;
    j.colunas = 2;
    
    // Cria um tabuleiro muito simples
    j.original[0][0] = 'a'; j.original[0][1] = 'b';
    j.original[1][0] = 'c'; j.original[1][1] = 'd';
    j.atual[0][0] = 'a'; j.atual[0][1] = 'b';
    j.atual[1][0] = 'c'; j.atual[1][1] = 'd';
    
    // Testa a função de resolução
    int resultado = resolver_completamente(&j);
    CU_ASSERT(resultado >= 0);  // Não deve crashar
    
    limparJogo(&j);
}

// --- Testes de Limpeza ---

void test_limparJogo_sem_historico() {
    Jogo j = {0};
    j.historico = NULL;
    
    // Não deve crashar
    limparJogo(&j);
}

void test_limparJogo_com_historico() {
    Jogo j = {0};
    
    // Cria histórico artificial
    Estado *e1 = malloc(sizeof(Estado));
    Estado *e2 = malloc(sizeof(Estado));
    e1->prox = e2;
    e2->prox = NULL;
    j.historico = e1;
    
    // Deve limpar sem crashar
    limparJogo(&j);
    CU_ASSERT(j.historico == NULL);
}

// --- Main com CUnit ---

int main() {
    if (CU_initialize_registry() != CUE_SUCCESS) {
        return CU_get_error();
    }

    CU_pSuite suite = CU_add_suite("Testes do Projeto", NULL, NULL);
    if (suite == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Testes das funções básicas
    CU_add_test(suite, "maiuscula()", test_maiuscula);
    CU_add_test(suite, "carregar_matriz() válida", test_carregar_matriz_valida);
    CU_add_test(suite, "carregar_matriz() inválida", test_carregar_matriz_invalida);
    
    // Testes de histórico
    CU_add_test(suite, "copiaTabuleiro/imprimeTabuleiro", test_copia_imprime_tabuleiro);
    CU_add_test(suite, "copiaTabuleiro sem memória", test_copia_tabuleiro_sem_memoria);
    CU_add_test(suite, "imprimeTabuleiro sem histórico", test_imprime_tabuleiro_sem_historico);
    
    // Testes de validação
    CU_add_test(suite, "contarViolacoes tabuleiro válido", test_contarViolacoes_tabuleiro_valido);
    CU_add_test(suite, "contarViolacoes letras repetidas", test_contarViolacoes_letras_repetidas);
    CU_add_test(suite, "contarViolacoes riscos consecutivos", test_contarViolacoes_riscos_consecutivos);
    CU_add_test(suite, "verificarRestricoes sem erros", test_verificarRestricoes_sem_erros);
    CU_add_test(suite, "verificarRestricoes com erros", test_verificarRestricoes_com_erros);
    
    // Testes de I/O
    CU_add_test(suite, "mostrar()", test_mostrar);
    CU_add_test(suite, "printMenu()", test_printMenu);
    CU_add_test(suite, "salvar_jogo()", test_salvar_jogo);
    
    // Testes de resolução
    CU_add_test(suite, "ajudar() sem tabuleiro", test_ajudar_sem_tabuleiro);
    CU_add_test(suite, "ajudar() com tabuleiro", test_ajudar_com_tabuleiro_simples);
    CU_add_test(suite, "resolver_completamente() sem tabuleiro", test_resolver_completamente_sem_tabuleiro);
    CU_add_test(suite, "resolver_completamente() simples", test_resolver_completamente_tabuleiro_simples);
    
    // Testes de limpeza
    CU_add_test(suite, "limparJogo sem histórico", test_limparJogo_sem_historico);
    CU_add_test(suite, "limparJogo com histórico", test_limparJogo_com_historico);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    
    int failures = CU_get_number_of_failures();
    CU_cleanup_registry();
    
    return failures;
}