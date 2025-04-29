CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O1 -fsanitize=address -fno-omit-frame-pointer -g
LDFLAGS = -fsanitize=address -fno-omit-frame-pointer
COV_FLAGS = -fprofile-arcs -ftest-coverage
SRC = projeto_funcoes.c
MAIN_SRC = projeto.c
TEST_SRC = testes.c
OBJ = $(SRC:.c=.o)
MAIN_OBJ = $(MAIN_SRC:.c=.o)
TEST_OBJ = $(TEST_SRC:.c=.o)

.PHONY: all jogo testar coverage clean

all: jogo

jogo: $(MAIN_OBJ) $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

testar: CFLAGS += $(COV_FLAGS)
testar: LDFLAGS += $(COV_FLAGS) -lgcov
testar: clean testes
	@echo "\nExecutando testes..."
	@./testes
	@echo "\nAnalisando cobertura..."
	@gcov -r $(SRC) $(TEST_SRC)

testes: $(TEST_OBJ) $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c projeto.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf jogo testes *.o *.gcno *.gcda *.gcov

coverage: testar