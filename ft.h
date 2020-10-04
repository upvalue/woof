#ifndef _FT_H
#define _FT_H

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
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

/**
 * A word (as in system word) sized integer.
 */
struct Cell {
  Cell(): bits(0) {}
  Cell(ptrdiff_t bits_): bits(bits_) {}
  ~Cell() {}

  ptrdiff_t bits;

  template <class T> T* as() const {
    return (T*) bits;
  }

  template <class T> void set(T* ptr) {
    bits = (ptrdiff_t) ptr;
  }

  ptrdiff_t operator*() const {
    return bits;
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
  /** Encountered something that was too large for scratch space, such as a very long word name */
  E_OUT_OF_SCRATCH,
  /** For defining words -- requests that a word is available in scratch space. */
  E_WANT_WORD,
  /** Word not found */
  E_WORD_NOT_FOUND
};

inline const char* error_description(const Error e) {
  switch(e) {
    case E_OK: return "ok";
    case E_STACK_UNDERFLOW: return "stack underflow";
    case E_STACK_OVERFLOW: return "stack overflow";
    case E_OUT_OF_MEMORY: return "out of memory";
    case E_WANT_WORD: return "wanted a word";
    case E_WORD_NOT_FOUND: return "word not found";
    default: return "unknown";
  }
}

/**
 * StateConfig -- a struct used to initialize State and point it at 
 * whatever memory you've allocated for it.
 */
struct StateConfig {
  Cell* stack;
  size_t stack_size;

  char* memory;
  size_t memory_size;

  Cell* shared;
  size_t shared_size;
};


/** 
 * A convenience method for struct StateConfig with statically allocated memory
 */
template <size_t stack_size_num = 1024, size_t shared_size_num = 8, size_t memory_size_num = 1024 * 1024>
struct StaticStateConfig : StateConfig {
  StaticStateConfig() {
    stack = (Cell*) stack_store;
    stack_size = stack_size_num;

    memory = (char*) memory_store;
    memory_size = memory_size_num;

    shared = (Cell*) shared_store;
    shared_size = shared_size_num;
  }

  Cell* stack_store[stack_size_num];
  char* memory_store[memory_size_num];
  Cell* shared_store[shared_size_num];
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
    // TODO: Should probably make this aligned
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
 * Start at S_USER_SHARED to define your own.
 */
enum {
  /** Latest dictionary entry */
  S_LATEST,
  /** Current location in memory */
  S_HERE, 
  /** Input mode */
  S_WORD_AVAILABLE,
  /** Compiling */
  S_COMPILING,
  S_USER_SHARED,
};

enum {
  INPUT_INTERPRET,
  INPUT_PASS_WORD,
};

enum Token {
  TK_NUMBER,
  TK_WORD,
  TK_END,
};

enum Opcode {
  /** Null -- should not be encountered */
  OP_UNKNOWN,
  /** Push an immediate value */
  OP_PUSH_IMMEDIATE,
  /** Call another Forth word */
  OP_CALL_FORTH,
  /** Call out to a C++ defined word. Must be followed by a C++ function address */
  OP_CALL_C,
  /** Exit current word */
  OP_EXIT,
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

      /** BUILTIN WORDS */
      defw("+", [](State& s) {
        Cell a, b;
        FT_CHECK(s.pop(a));
        FT_CHECK(s.pop(b));
        return s.push(a.bits + b.bits);
      });

      defw(":", [](State& s) {
        // Set input mode to wait for word
        if(*s.shared[S_WORD_AVAILABLE] == 0) {
          return E_WANT_WORD;
        }

        s.shared[S_WORD_AVAILABLE] = 0;
        s.shared[S_COMPILING] = 1;

        // TODO: Here we need to create the actual word
        DictEntry* d = 0;
        return s.create(s.scratch, d);
      });

      defw(";", [](State& s) {
        s.dict_push(OP_EXIT);
        s.shared[S_COMPILING] = 0;
        return E_OK;
      });

      // TODO: comma
      // TODO: if/else
      // TODO: throw
    }
  ~State() {}

  /**
   * The data stack
   */
  Cell *stack;
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

  Cell* shared;
  size_t shared_i, shared_size;

  /***** STACK INTERACTION PRIMITIVES */

  Error push(Cell v) {
    stack[si++] = v;
    return E_OK;    
  };
  
  /**
   * Pop a value into v
   */
  Error pop(Cell& v) {
    if(si == 0) {
      return E_STACK_UNDERFLOW;
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

  static size_t align(int boundary, size_t value) {
    return (size_t)((value + (boundary - 1)) & -boundary);
  }

  /**
   * Allocate some memory for general purpose use
   */
  Error allot(size_t req, char*& addr) {
    // std::cout << "allocated " << req << " memory " << std::endl;
    if(memory_i + req > memory_size) {
      return E_OUT_OF_MEMORY;
    }

    addr = &memory[memory_i];

    memory_i += req;

    return E_OK;
  }

  /** Add a forth word */
  Error create(const char* name, DictEntry*& d) {
    size_t name_length = strlen(name);
    size_t size = sizeof(DictEntry) + name_length + 1;
    char* dict_addr = 0;

    FT_CHECK(allot(size, dict_addr));

    d = (DictEntry*) dict_addr;

    d->previous = shared[S_LATEST].as<DictEntry>();
    d->name_length = name_length;
    strncpy(d->name, name, name_length);

    FT_ASSERT(d->previous == shared[S_LATEST].as<DictEntry>());
    FT_ASSERT(d->name_length == name_length);
    FT_ASSERT(strncmp(d->name, name, name_length) == 0);

    shared[S_LATEST].set(d);

    return E_OK;
  }

  /**
   * Add a Forth word backed by a C++ function
   */
  Error defw(const char* name, c_word_t word) {
    DictEntry* d = 0;
    FT_CHECK(create(name, d));

    d->flags = DictEntry::FLAG_CWORD;

    dict_push((size_t)word);

    // TODO: It's possible for Forth code to overwrite this and cause us to call an invalid value
    // which would make ft.h crash. One possibility would be registering all C functions in an array
    // and only calling known indexes in that array. That way, even corrupted forth code could not
    // segfault.

    FT_ASSERT(*d->data<size_t>() == (size_t) word);
    FT_ASSERT(d->flags == DictEntry::FLAG_CWORD);

    return E_OK;
  }

  /** Push a cell into memory, comma in Forth */
  Error dict_push(Cell cell) {
    if(memory_i + sizeof(Cell) > memory_size) {
      return E_OUT_OF_MEMORY;
    }


    Cell* addr = (Cell*) &memory[memory_i];
    // std::cout << "push " << cell.bits << "@" << (ptrdiff_t) addr << std::endl;
    (*addr) = cell;

    memory_i += sizeof(Cell);

    return E_OK;
  }

  /**
   * Lookup a word in the dictionary
   */
  DictEntry* lookup(const char* name) const {
    DictEntry* e = shared[S_LATEST].as<DictEntry>();

    while(e) {
      if(strncmp(e->name, name, e->name_length) == 0) {
        return e;
      }
      e = e->previous;
    }

    return 0;
  }

  /***** MAIN INTERPRETER */

  const char* input;
  size_t input_i;
  size_t input_size;
  ptrdiff_t token_number;

  Error next_token(Token& tk) {
    char c;
    while(input_i < input_size) {
      char c = input[input_i++];
      if(isdigit(c)) {
        // Number: read and push onto stack
        ptrdiff_t n = c - '0';
        while(input_i < input_size) {
          c = input[input_i++];
          if(isdigit(c)) {
            n *= 10;
            n += (c - '0');
          } else {
            input_i--;
            break;
          }
        }
        token_number = n;
        tk = TK_NUMBER;
        return E_OK;
      } else if(isspace(c)) {
        // Skip whitespace
        continue;
      } else {
        // Read a word
        scratch_i = 1;
        scratch[0] = c;
        while(input_i < input_size) {
          c = input[input_i++];
          if(isspace(c)) { 
            break;
          }
          FT_CHECK(scratch_put(c));
        }
        FT_CHECK(scratch_put('\0'));
        tk = TK_WORD;
        return E_OK;
      }
    }
    tk = TK_END;
    return E_OK;
  }

  /** 
   * Execute arbitrary code
   */
  Error exec(const char* input_) {
    input = input_;
    input_size = strlen(input_);
    input_i = 0;

    Token tk;
    FT_CHECK(next_token(tk));
    while(tk != TK_END) {
      if(tk == TK_NUMBER) {
        // If interpreting, push directly
        if(*shared[S_COMPILING] == 0) {
          push(token_number);
        } else {
          // If compiling, push opcode
          dict_push(OP_PUSH_IMMEDIATE);
          dict_push(token_number);
        }
      } else if(tk == TK_WORD) {
        // We now have a word, look it up in the dictionary
        DictEntry* word = lookup(scratch);

        if(word) {
          // This is a C++ word, invoke and check return value

          // TODO: If in compiling mode, we should check immediacy first
          // and if not push OP_CALL_C
          if(word->flags & DictEntry::FLAG_CWORD) {
            c_word_t cw = *word->data<c_word_t>();

            Error e = cw(*this);
            if(e == E_WANT_WORD) {
              // Attempt to read additional word name to pass through
              Token tk2;
              FT_CHECK(next_token(tk2));

              // Whatever we got wasn't a word, so just pass the error through
              if(tk2 != TK_WORD) return E_WANT_WORD;

              shared[S_WORD_AVAILABLE] = 1;

              cw(*this);
            } else if(e != E_OK) {
              return e;
            }
          } else {
            ptrdiff_t* code = word->data<ptrdiff_t>();
            exec(code);
          }
        } else {
          return E_WORD_NOT_FOUND;
        }
      }
      FT_CHECK(next_token(tk));
    }

    return E_OK;
  }

  /** Execute user defined Forth code */
  Error exec(ptrdiff_t* code) {
    while(true) {
      switch(*code++) {
        case OP_PUSH_IMMEDIATE: {
          ptrdiff_t n = *code++;
          push(n);
          continue;
        }
        case OP_EXIT: {
          return E_OK;
        }
        case OP_UNKNOWN: default: {
          std::cout << "unknown opcode" << *code << std::endl;
        }
      }
      // TODO: Check that code addresses are valid.
      // Find until exit
      return E_OK;
    }
  }
};

};

#endif