#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "ft.h"

using namespace ft;

Value stack[1024];
Value memory[1024];

TEST_CASE("forth starts") {
  State state;
  state.stack = (Value*) stack;
  state.memory = (Value*) memory;

  state.exec("5");
  std::cout <<"hiya " << std::endl;

}
