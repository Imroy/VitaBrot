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
#include <SDL2/SDL_timer.h>

Display::Display() :
  _window(nullptr),
  _renderer(nullptr),
  _texture(nullptr),
  _last_redraw(-1)
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    return;
  }

  _window = SDL_CreateWindow("VitaBrot", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _screen_width, _screen_height, SDL_WINDOW_SHOWN);
  if (_window == nullptr) {
    return;
  }

  _renderer = SDL_CreateRenderer(_window, -1, 0);
  if (_renderer == nullptr) {
    return;
  }

  // Clear the window
  SDL_RenderClear(_renderer);
  SDL_RenderPresent(_renderer);
  _last_redraw = SDL_GetTicks();

  _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, _screen_width, _screen_height);
  if (_texture == nullptr) {
    return;
  }

  // TODO: Clear the texture to solid black
}

Display::~Display() {
  if (_texture != nullptr)
    SDL_DestroyTexture(_texture);

  if (_renderer != nullptr)
    SDL_DestroyRenderer(_renderer);

  if (_window != nullptr)
    SDL_DestroyWindow(_window);

  SDL_Quit();
}

void Display::Draw_pixel(int32_t x, int32_t y, int32_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  SDL_Rect rect = { x, y, size, size };
  uint32_t value = ((uint32_t)a << 24) | ((uint32_t)b << 16) | ((uint32_t)g << 8) | r;

  if (rect.x + rect.w >= _screen_width)
    rect.w = _screen_width - 1 - rect.x;
  if (rect.y + rect.h >= _screen_height)
    rect.h = _screen_height - 1 - rect.y;

  uint32_t *pixels;
  int pitch;

  SDL_LockTexture(_texture, &rect, (void**)&pixels, &pitch);

  for (int32_t py = 0; py < rect.h; py++) {
    uint32_t *p = pixels + (py * _screen_width);
    for (int32_t px = rect.w; px; px--, p++)
      *p = value;
  }

}

int Display::Refresh(void) {
  uint32_t now = SDL_GetTicks();
  if (now - _last_redraw <= 16)
    return 0;

  if (_texture != nullptr) {
    int rc = SDL_RenderCopy(_renderer, _texture, nullptr, nullptr);
    if (rc < 0) {
      return rc;
    }
  }

  SDL_RenderPresent(_renderer);
  _last_redraw = now;

  return 0;
}
