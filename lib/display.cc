/*
  VitaBrot, Mandelbrot explorer for the Playstation Vita.
  Copyright (C) 2017 Ian Tester

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "display.hh"

SDL_Window *gWindow = nullptr;
SDL_Renderer *gRenderer = nullptr;

int display_init(void) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    return -1;

  if ((gWindow = SDL_CreateWindow("VitaBrot", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN)) == nullptr)
    return -1;

  if ((gRenderer = SDL_CreateRenderer(gWindow, -1, 0)) == nullptr)
    return -1;

  SDL_RenderPresent(gRenderer);
  return 0;
}

int display_exit(void) {
  SDL_DestroyRenderer(gRenderer);
  SDL_DestroyWindow(gWindow);

  gWindow = nullptr;
  gRenderer = nullptr;

  SDL_Quit();
  return 0;
}
