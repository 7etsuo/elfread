#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;

void
draw_filled_circle (SDL_Renderer *renderer, int x, int y, int radius)
{
  for (int w = 0; w < radius * 2; w++)
    {
      for (int h = 0; h < radius * 2; h++)
        {
          int dx = radius - w;
          int dy = radius - h;
          if ((dx * dx + dy * dy) <= (radius * radius))
            {
              SDL_RenderDrawPoint (renderer, x + dx, y + dy);
            }
        }
    }
}

void
draw_smiley_face ()
{
  // Face
  SDL_SetRenderDrawColor (renderer, 255, 255, 0, 255);
  draw_filled_circle (renderer, 320, 240, 50);

  // Eyes
  SDL_SetRenderDrawColor (renderer, 0, 0, 0, 255);

  // Left eye
  draw_filled_circle (renderer, 300, 230, 10);

  // Right eye
  draw_filled_circle (renderer, 340, 230, 10);

  // Smile
  SDL_SetRenderDrawColor (renderer, 0, 0, 0, 255);
  SDL_Rect smileRect = { 270, 250, 100, 50 };
  SDL_RenderDrawRect (renderer, &smileRect);
}

void
main_loop ()
{
  SDL_RenderClear (renderer);
  draw_smiley_face ();
  SDL_RenderPresent (renderer);
}

int
main (int argc, char *argv[])
{
  SDL_Init (SDL_INIT_VIDEO);
  TTF_Init ();
  window = SDL_CreateWindow ("7etsuo", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
  renderer = SDL_CreateRenderer (window, -1, SDL_RENDERER_ACCELERATED);
  font = TTF_OpenFont ("OpenSans-Regular.ttf", 24);

  emscripten_set_main_loop (main_loop, 0, 1);
  j TTF_CloseFont (font);
  TTF_Quit ();
  SDL_DestroyRenderer (renderer);
  SDL_DestroyWindow (window);
  SDL_Quit ();
  return 0;
}
