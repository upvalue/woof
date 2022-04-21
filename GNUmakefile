CC := clang
CXX := clang++

RAYLIB_LDFLAGS := $(shell pkg-config --libs raylib)

all: repl test

vendor/linenoise/linenoise.o: vendor/linenoise/linenoise.c
	$(CC) -O2 -c -o $@ $<

repl: repl.cpp vendor/linenoise/linenoise.o woof.h
	$(CXX) -O3 -lraylib -lSDL2 -Ivendor/linenoise -g3 -o $@ repl.cpp vendor/linenoise/linenoise.o $(RAYLIB_LDFLAGS)

web-repl.js: web-repl.cpp woof.h
	emcc -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' -g3 --preload-file prelude.fs -o $@ $< 

test: test.cpp woof.h
	$(CXX) -Ivendor/doctest -g3 -o $@ $<

clean:
	rm -f repl test