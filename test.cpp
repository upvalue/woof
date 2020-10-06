#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "ft.h"

using namespace ft;

struct TestState {
  TestState(): cfg(), state(cfg) {}

  StaticStateConfig<8, 8, 1024> cfg;
  State state;
};

TEST_CASE("ft.h") {
  TestState test_state;

  State& s = test_state.state;

  SUBCASE("reads a number") {
    CHECK(s.exec("5") == E_OK);
    CHECK(s.si == 1);
    CHECK(s.stack[0].bits == 5);
  }

  SUBCASE("can call a word") {
    CHECK(s.exec("2 2 +") == E_OK);
    CHECK(s.si == 1);
    CHECK(s.stack[0].bits == 4);
  }

  SUBCASE("can define and call a word") {
    CHECK(s.exec(": asdf 5 ; asdf") == E_OK);
    CHECK(s.si == 1);
    CHECK(s.stack[0].bits == 5);
  }

  SUBCASE("can execute an immediate forth word") {
    CHECK(s.exec(": asdf 5 ; immediate : asdf2 asdf ;") == E_OK);
    CHECK(s.si == 1);
    CHECK(s.stack[0].bits == 5);
  }

  SUBCASE("can modify code while compiling") {
    CHECK(s.exec(": exit-early 6 , ; immediate : asdf exit-early 1 ;") == E_OK);
    CHECK(s.si == 0);
  }

  SUBCASE("ignores comments") {
    CHECK(s.exec("1 \\ 2 3 4 5\r\n6") == E_OK);
    CHECK(s.si == 2);
    CHECK(s.stack[0].bits == 1);
    CHECK(s.stack[1].bits == 6);
  }
  
  SUBCASE("can write and read memory") {
    CHECK(s.exec("5 , here WORD - @") == E_OK);
    CHECK(s.si == 1);
    CHECK(s.stack[0].bits == 5);

    CHECK(s.exec("5 , 999 here WORD - ! here WORD - @") == E_OK);
    CHECK(s.si == 2);
    CHECK(s.stack[1].bits == 999);
  }
}