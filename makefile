CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O1 -fsanitize=address -fno-omit-frame-pointer -g
LDFLAGS = -fsanitize=address -fno-omit-frame-pointer
COV_FLAGS = -fprofile-arcs -ftest-coverage
SRC = projeto_funcoes.c
TEST_SRC = testes.c

.PHONY: all jogo testar clean

all: jogo

jogo: projeto.o projeto_funcoes.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

testar: CFLAGS += $(COV_FLAGS)
testar: LDFLAGS += $(COV_FLAGS) -lgcov
testar: clean testes
	@echo "\nExecutando testes..."
	@./testes
	@echo "\nAnalisando cobertura..."
	@gcov -r projeto_funcoes.gcda

testes: $(TEST_SRC) projeto_funcoes.o
	$(CC) $(CFLAGS) -o $@ $(TEST_SRC) projeto_funcoes.o $(LDFLAGS)

%.o: %.c projeto.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf jogo testes *.o *.gcno *.gcda *.gcov