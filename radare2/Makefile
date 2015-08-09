AFL_CC ?= afl-clang-fast -O2 -fsanitize=address -g

LIBS=$(shell pkg-config --libs r_asm)
CFLAGS=$(shell pkg-config --cflags r_asm)

asm_test: asm_test.c
	$(AFL_CC) -o $@ $< $(LIBS) $(CFLAGS)
