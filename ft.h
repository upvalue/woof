#ifndef _FT_H
#define _FT_H

#include <ctype.h>
#include <stddef.h>

// TODO: Remove STL dependency
#include <iostream>

#define FT_CHECK(e) do { ft::Error err = e; if(err != E_OK) { return err; }} while(0)

namespace ft {

struct Value {
  Value(): bits(0) {}
  Value(ptrdiff_t bits_): bits(bits_) {}
  ~Value() {}

  ptrdiff_t bits;
};

enum Error {
  E_OK,
  E_STACK_UNDERFLOW,
  E_STACK_OVERFLOW
};

/**
 * StateConfig -- a struct used to initialize State and point it at 
 * whatever memory you've allocated for it
 */
struct StateConfig {
  Value* stack;
  size_t stack_size;

  Value* memory;
  size_t memory_size;
};

/** 
 * A convenience method for initializing StateConfig with statically allocated memory
 */
template <size_t stack_size_num = 8, size_t memory_size_num = 1024>
struct StaticStateConfig : StateConfig {
  StaticStateConfig() {
    stack = (Value*) stack_store;
    stack_size = stack_size_num;

    memory = (Value*) memory_store;
    memory_size = memory_size_num;
  }

  Value* stack_store[stack_size_num];
  Value* memory_store[memory_size_num];
};

/**
 * An instance of Forth. Self-contained and re-entrant
 */
struct State {
  State(const StateConfig& cfg):
    stack(cfg.stack),
    stack_size(cfg.stack_size),
    si(0),
    memory(cfg.memory),
    memory_size(cfg.memory_size),
    scratch_i(0) {


  }
  ~State() {}

  /**
   * The data stack
   */
  Value *stack;
  size_t stack_size, si;

  /**
   * Program memory
   */
  Value *memory;
  size_t memory_size;

  /**
   * Scratch space, for formatting errors and strings
   */
  char scratch[1024];
  size_t scratch_i;

  /***** STACK INTERACTION PRIMITIVES */

  Error push(Value v) {
    stack[si++] = v;
    return E_OK;    
  };
  
  /**
   * Pop a value into v
   */
  Error pop(Value& v) {
    if(si == 0) {
      return E_STACK_OVERFLOW;
    }

    v = stack[si-1];
    si--;
    return E_OK;
  }

  /** 
   * Execute arbitrary code
   */
  Error exec(const char* s) {
    char c;
    while((c = *s++)) {
      if(isdigit(c)) {
        ptrdiff_t n = c - '0';
        while((c = *s++)) {
          if(isdigit(c)) {
            n *= 10;
            n += (c - '0');
          } else {
            s--;
            break;
          }
        }
        Value num(n);
        FT_CHECK(push(num));
      }
    }
    return E_OK;
  }


};

};

#endif