#ifndef PPU_SDH_H
#define PPU_SDH_H

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"

#include "tile.h"

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Event event;
} SdlWrapper;

SDL_AppResult sdl_init(SdlWrapper *sdl);

void sdl_render_clear(SdlWrapper *sdl);
SDL_AppResult sdl_render(SdlWrapper *sdl);

void sdl_quit(SdlWrapper *sdl);

void sdl_put_color(const SdlWrapper *sdl, int x, int y, Color *color);

#endif // PPU_SDH_H
