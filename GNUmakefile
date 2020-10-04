CC := clang
CXX := clang++

all: repl test

vendor/linenoise/linenoise.o: vendor/linenoise/linenoise.c
	$(CC) -O2 -c -o $@ $<

repl: repl.cpp vendor/linenoise/linenoise.o ft.h
	$(CXX) -Ivendor/linenoise -g3 -o $@ repl.cpp vendor/linenoise/linenoise.o

test: test.cpp ft.h
	$(CXX) -Ivendor/doctest -g3 -o $@ $<
