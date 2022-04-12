#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <streambuf>

#include "linenoise.h"
#include "raylib.h"

#include "woof.h"

using namespace woof;

StaticStateConfig<> memory;

int main(int argc, char** argv) {
  bool do_repl = true;
  std::vector<std::string> do_files;

  if(argc > 1) {
    do_repl = false;
    for(size_t i = 1; i != argc; i++) {
      std::string s(argv[i]);
      if(s.compare("--repl") == 0) {
        do_repl = true;
      } else {
        do_files.push_back(s);
      }
    }
  }

  char *ln;

  State state(memory);

  state.defw("ray/init", [](State& s) {
    InitWindow(640, 480, "woof \\o/");
    return E_OK;
  });

  // TODO: window close
  // TODO: basic draw commands

  state.defw("ray/close", [](State&) {
    CloseWindow();
    return E_OK;
  });
 
  for(size_t i = 0; i != do_files.size(); i += 1) {
    std::ifstream t(do_files[i]);

    std::string body((std::istreambuf_iterator<char>(t)), (std::istreambuf_iterator<char>()));

    Error e = state.exec(body.c_str());

    if(e != E_OK) {
      std::cout << "Error: " << state.scratch << std::endl << error_description(e) << std::endl;;
    }

  }

  if(do_repl) {
    std::cout << "woof \\o/" << std::endl;

    while((ln = linenoise("> "))) {
      Error e = state.exec(ln);

      if(e) {
        std::cout << "Error: " << state.scratch << std::endl << error_description(e) << std::endl;
      }

      for(size_t i = 0; i != state.si; i += 1) {
        std::cout << state.stack[i].bits << ' ';
      }
      std::cout << std::endl;

      free(ln);
    }
  }

  return 0;
}
