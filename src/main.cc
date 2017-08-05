#include <psp2/kernel/processmgr.h>
#include <SDL2/SDL.h>

//Screen dimension constants
#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 544

SDL_Window *gWindow = nullptr;
SDL_Renderer *gRenderer = nullptr;

int main(int argc, char *argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    return -1;

  if ((gWindow = SDL_CreateWindow("VitaBrot", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN)) == nullptr)
    return -1;

  if ((gRenderer = SDL_CreateRenderer(gWindow, -1, 0)) == nullptr)
    return -1;

  SDL_RenderPresent(gRenderer);
  SDL_Delay(4000);
  SDL_DestroyRenderer(gRenderer);
  SDL_DestroyWindow(gWindow);

  gWindow = nullptr;
  gRenderer = nullptr;
  
  SDL_Quit();
  sceKernelExitProcess(0);
  return 0;
}
