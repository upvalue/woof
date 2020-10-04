#include <stdlib.h>
#include <iostream>

#include "linenoise.h"

#include "ft.h"

using namespace ft;

StaticStateConfig<> memory;

int main(void) {
  char *ln;

  State state(memory);

  std::cout << "ft.h \\o/" << std::endl;

  while((ln = linenoise("> "))) {
    state.exec(ln);

    for(size_t i = 0; i != state.si; i += 1) {
      std::cout << state.stack[i].bits << ' ';
    }
    std::cout << std::endl;

    free(ln);
  }

  return 0;
}