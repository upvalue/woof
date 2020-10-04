CC := clang
CXX := clang++

vendor/linenoise/linenoise.o: vendor/linenoise/linenoise.c
	$(CC) -O2 -c -o $@ $<

repl: repl.cpp vendor/linenoise/linenoise.o ft.h
	$(CXX) -Ivendor/linenoise -o $@ repl.cpp vendor/linenoise/linenoise.o

test: test.cpp ft.h
	$(CXX) -Ivendor/doctest -o $@ $<