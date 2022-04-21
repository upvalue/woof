#include <SDL/SDL.h>

#include "woof.h"

using namespace woof;

static SDL_Surface* surface = nullptr;

static State* sdlstate = nullptr;
static Cell forth_loop;
static bool loop_started = false;

void mainloop(void* arg) {
  if(!loop_started) return;
  if(forth_loop.bits != 0) {
    sdlstate->exec((ptrdiff_t*) forth_loop.bits);
  }
}

void woof_sdl_init(State& s) {
  sdlstate = &s;
  s.defw("sdl/loop", [](State& s) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
      return E_LIBRARY;
    }

    surface = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE);

    loop_started = true;

    return E_OK;
  });

  s.defw("sdl/clear", [](State& s) {
    SDL_FillRect(surface, NULL, 255);
    return E_OK;
  });

  s.defw("sdl/delay", [](State& s) {
    Cell delay_time;
    WF_CHECK(s.pop(delay_time));
    SDL_Delay(delay_time.bits);

    return E_OK;
  });

  s.defw("sdl/fill-rect", [](State& s) {
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));
    return E_OK;
  });

  s.defw("sdl/set-loop", [](State& s) {
    WF_CHECK(s.pop(forth_loop));

#ifdef EMSCRIPTEN
    emscripten_set_main_loop_arg(mainloop, nullptr, 60, -1);
#endif
    return E_OK;

    /*
    SDL_Event e;

    bool exitloop = false;

    while(!exitloop) {
      std::cout << "hi" << std::endl;
      while(SDL_PollEvent(&e)) { 
        if(e.type == SDL_QUIT) {
          exitloop = true;
        }
      }
      s.exec((ptrdiff_t*) loop.bits);
    }
    return E_OK;
    */

  });

  s.defw("sdl/quit", [](State& s) {
    // SDL_DestroyWindow(window);
    // window = nullptr;
    SDL_Quit();
    return E_OK;
  });

  s.defw("sdl/register-main-loop", [](State& s) {
    WF_CHECK(s.pop(forth_loop));
    return E_OK;
  });

}