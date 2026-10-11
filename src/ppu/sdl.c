#include <stddef.h>

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"

#include "constant.h"
#include "ppu/color.h"
#include "ppu/sdl.h"

SDL_AppResult sdl_init(SdlWrapper *sdl) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't init video: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer(
          "GB", WINDOW_WIDTH * WINDOW_SCALE, WINDOW_HEIGHT * WINDOW_SCALE,
          SDL_WINDOW_ALWAYS_ON_TOP, &sdl->window, &sdl->renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

void sdl_render_clear(SdlWrapper *sdl) { SDL_RenderClear(sdl->renderer); }

SDL_AppResult sdl_render(SdlWrapper *sdl) {
  while (SDL_PollEvent(&sdl->event)) {
    if (sdl->event.type == SDL_EVENT_QUIT) {
      return SDL_APP_SUCCESS;
    }
  }
  SDL_RenderPresent(sdl->renderer);
#ifdef GB_RENDER_TEST
  SDL_Surface *shot = SDL_RenderReadPixels(sdl->renderer, nullptr);
  if (!shot) {
    SDL_Log("Cannot capture");
  } else {
    SDL_SaveBMP(shot, "test_render.bmp");
  }
#endif
  return SDL_APP_CONTINUE;
}

void sdl_quit(SdlWrapper *sdl) {
  SDL_DestroyRenderer(sdl->renderer);
  SDL_DestroyWindow(sdl->window);
  SDL_Quit();
}

void sdl_put_color(const SdlWrapper *sdl, int x, int y, Color *color) {
  SDL_SetRenderDrawColor(sdl->renderer, color->r, color->g, color->b, 255);
  SDL_FRect rect = {(float)x * WINDOW_SCALE, (float)y * WINDOW_SCALE,
                    WINDOW_SCALE, WINDOW_SCALE};
  SDL_RenderFillRect(sdl->renderer, &rect);
}
