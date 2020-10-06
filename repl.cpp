#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <streambuf>

#include "linenoise.h"

#include "ft.h"

using namespace ft;

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

  for(size_t i = 0; i != do_files.size(); i += 1) {
    std::ifstream t(do_files[i]);

    std::string body((std::istreambuf_iterator<char>(t)), (std::istreambuf_iterator<char>()));

    Error e = state.exec(body.c_str());

    if(e != E_OK) {
      std::cout << "Error: " << error_description(e) << std::endl;
    }

  }

  if(do_repl) {
    std::cout << "ft.h \\o/" << std::endl;

    while((ln = linenoise("> "))) {
      Error e = state.exec(ln);

      if(e) {
        std::cout << "Error: " << error_description(e) << std::endl;
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