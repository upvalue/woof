#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "ft.h"

using namespace ft;

struct TestState {
  TestState(): cfg(), state(cfg) {}

  StaticStateConfig<8, 1024> cfg;
  State state;
};

TEST_CASE("ft.h") {
  TestState test_state;

  State& s = test_state.state;

  SUBCASE("reads a number") {
    s.exec("5");
    CHECK(s.si == 1);
    CHECK(s.stack[0].bits == 5);
  }
}
