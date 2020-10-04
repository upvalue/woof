#ifndef _FT_H
#define _FT_H

namespace ft {

struct Value {
  ptrdiff_t bits;
};

/**
 * An instance of Forth. Self-contained and re-entrant
 */
struct State {
  /**
   * The data stack
   */
  Value **stack;


};

};

#endif