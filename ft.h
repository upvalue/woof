#ifndef _FT_H
#define _FT_H

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <string.h>

// TODO: Remove STL dependency
#include <iostream>

#define FT_ASSERT(x) assert(x)

/**
 * Check for an error and return if there was one
 */
#define FT_CHECK(e) do { ft::Error err = e; if(err != E_OK) { return err; }} while(0)

/**
 * Size of scratch buffer to use for things like formatting strings and reading input
 */
#ifndef FT_SCRATCH_SIZE
# define FT_SCRATCH_SIZE 512
#endif

/**
 * Number of pointers to share between C++ and Forth
 */
#ifndef FT_SHARED_SIZE 
# define FT_SHARED_SIZE 8
#endif

namespace ft {

struct Value {
  Value(): bits(0) {}
  Value(ptrdiff_t bits_): bits(bits_) {}
  ~Value() {}

  ptrdiff_t bits;

  template <class T> T* as() const {
    return (T*) bits;
  }

  template <class T> void set(T* ptr) {
    bits = (ptrdiff_t) ptr;
  }
};

/** 
 * Error codes, returned directly by functions
 */
enum Error {
  E_OK,
  E_STACK_UNDERFLOW,
  E_STACK_OVERFLOW,
  E_OUT_OF_MEMORY,
  /**
   * Encountered something that was too large for scratch space, such as a very long word name
   */
  E_OUT_OF_SCRATCH,
};

inline const char* error_description(const Error e) {
  switch(e) {
    case E_OK: return "ok";
    case E_STACK_UNDERFLOW: return "stack underflow";
    case E_STACK_OVERFLOW: return "stack overflow";
    case E_OUT_OF_MEMORY: return "out of memory";
    default: return "unknown";
  }
}

/**
 * StateConfig -- a struct used to initialize State and point it at 
 * whatever memory you've allocated for it.
 */
struct StateConfig {
  Value* stack;
  size_t stack_size;

  char* memory;
  size_t memory_size;

  Value* shared;
  size_t shared_size;
};


/** 
 * A convenience method for initializing StateConfig with statically allocated memory
 */
template <size_t stack_size_num = 1024, size_t shared_size_num = 8, size_t memory_size_num = 1024 * 1024>
struct StaticStateConfig : StateConfig {
  StaticStateConfig() {
    stack = (Value*) stack_store;
    stack_size = stack_size_num;

    memory = (char*) memory_store;
    memory_size = memory_size_num;

    shared = (Value*) shared_store;
    shared_size = shared_size_num;
  }

  Value* stack_store[stack_size_num];
  char* memory_store[memory_size_num];
  Value* shared_store[shared_size_num];
};

/** 
 * An entry in the Forth dictionary
 */
struct DictEntry {
  enum Flags {
    FLAG_NONE = 0,
    FLAG_IMMEDIATE = 1 << 1,
    FLAG_CWORD = 1 << 2,
  };

  /**
   * The previous dictionary entry (if any)
   */
  DictEntry* previous;

  /**
   * Flags on the word
   */
  Flags flags;

  size_t name_length;

  char name[1];
  
  // The actual data in the dictionary comes afterwards
  template <class T> T* data() const {
    return (T*) (((size_t) this) + sizeof(DictEntry) + name_length + 1);
  }
};

struct State;

/**
 * c_word_t is a C++ function that can do arbitrary things with a State, used to define Forth words
 * backed by C++ functions.
 */
typedef Error (*c_word_t)(State&);

/**
 * Reserved C++ and Forth variables. 
 * Start at G_USER_SHARED to define your own.
 */
enum {
  G_LATEST,
  G_USER_SHARED,
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
    memory_i(0),
    memory_size(cfg.memory_size),
    shared(cfg.shared),
    shared_i(0),
    shared_size(cfg.shared_size),
    scratch_i(0) {
      memset(stack, 0, stack_size);
      memset(memory, 0, memory_size);
      memset(scratch, 0, FT_SCRATCH_SIZE);
      memset(shared, 0, shared_size);

      // Define builtin words
      defw("hello", [](State& s) {
        std::cout << "Hello world." << std::endl;
        return E_OK;
      });
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
  char *memory;
  size_t memory_i, memory_size;

  /**
   * Scratch buffer, for doing things with strings
   */
  char scratch[FT_SCRATCH_SIZE];
  size_t scratch_i;

  Value* shared;
  size_t shared_i, shared_size;

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
   * Drop N values from the stack
   */
  Error drop(size_t n = 1) {
    if(si < n) {
      return E_STACK_UNDERFLOW;
    }
    si -= n;
    return E_OK;
  }

  /***** SCRATCH INTERACTION */

  Error scratch_put(char c) {
    if(scratch_i == FT_SCRATCH_SIZE) {
      return E_OUT_OF_SCRATCH;
    }
    scratch[scratch_i++] = c;
    return E_OK;
  }


  /***** DICTIONARY PRIMITIVES */

  /**
   * Allocate some memory for general purpose use
   */
  Error allot(size_t req, char*& addr) {
    if(memory_i + req > memory_size) {
      return E_OUT_OF_MEMORY;
    }

    addr = memory;
    
    memory_i += req;

    return E_OK;
  }

  /**
   * Add a Forth word backed by a C++ function
   */
  Error defw(const char* name, c_word_t word) {
    size_t name_length = strlen(name);
    size_t size = sizeof(DictEntry) + name_length + sizeof(c_word_t);
    char* dict_addr = 0;

    FT_CHECK(allot(size, dict_addr));

    DictEntry* d = (DictEntry*) dict_addr;

    d->previous = shared[G_LATEST].as<DictEntry>(); // (DictEntry*) shared[G_LATEST].bits;
    d->flags = DictEntry::FLAG_CWORD;
    d->name_length = name_length;
    strncpy(d->name, name, name_length);

    c_word_t* ptr = d->data<c_word_t>();

    // TODO: It's possible for Forth code to overwrite this and cause us to call an invalid value
    // which would make ft.h crash. One possibility would be registering all C functions in an array
    // and only calling known indexes in that array. That way, even corrupted forth code could not
    // segfault.

    (*ptr) = word;

    std::cout << "define c word: " << (size_t) (*d->data<size_t>()) << " = " << (size_t)word << std::endl;

    FT_ASSERT(d->previous == shared[G_LATEST].as<DictEntry>());
    FT_ASSERT(d->flags == DictEntry::FLAG_CWORD);
    FT_ASSERT(d->name_length == name_length);
    FT_ASSERT(strncmp(d->name, name, name_length) == 0);
    FT_ASSERT(*d->data<size_t>() == (size_t) word);
    shared[G_LATEST].set(d);

    return E_OK;
  }

  /**
   * Lookup a word in the dictionary
   */
  DictEntry* lookup(const char* name) const {
    DictEntry* e = shared[G_LATEST].as<DictEntry>();

    while(e) {
      if(strncmp(e->name, name, e->name_length) == 0) {
        return e;
      }
      e = e->previous;
    }

    return 0;
  }


  /***** MAIN INTERPRETER */

  /** 
   * Execute arbitrary code
   */
  Error exec(const char* s) {
    char c;
    size_t i = 0;
    while((c = *s++)) {
      i++;
      if(isdigit(c)) {
        // Number: read and push onto stack
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
      } else if(isspace(c)) {
        // Skip whitespace
        continue;
      } else {
        scratch_i = 1;
        scratch[0] = c;
        while((c = *s++)) {
          if(isspace(c)) {
            break;
          }
          FT_CHECK(scratch_put(c));
        }
        FT_CHECK(scratch_put('\0'));

        // We now have a word, look it up in the dictionary

        DictEntry* word = lookup(scratch);

        if(word) {
          if(word->flags & DictEntry::FLAG_CWORD) {
            c_word_t cw = *word->data<c_word_t>();
            cw(*this);
            // std::cout << "found cword" << (size_t) cw << std::endl;
          }
        }

      }

      if(c == '\0') break;

      // Allow words to read other words here
      // Switch on mode
      // If interpreting, call word immediately
      // If compiling, push code 
    }
    return E_OK;
  }
};

};

#endif