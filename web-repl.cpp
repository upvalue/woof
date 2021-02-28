#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <streambuf>

#include <emscripten.h>

#include "woof.h"

using namespace woof;

StaticStateConfig<> memory;
State* state = 0;

int main(void) {
  return 0;
}

extern "C" {

EMSCRIPTEN_KEEPALIVE
extern void woof_initialize_cli(const char* string) {
  if(!state) {
    state = new woof::State(memory);
  }
}

EMSCRIPTEN_KEEPALIVE
extern void woof_eval_and_print(const char* ln) {

  std::cout << "> " << ln << std::endl;
  Error e = state->exec(ln);

  if(e) {
    std::cout << "Error: " << state->scratch << std::endl << error_description(e) << std::endl;
  }

  for(size_t i = 0; i != state->si; i += 1) {
    std::cout << state->stack[i].bits << ' ';
  }
  std::cout << std::endl;
}

}

