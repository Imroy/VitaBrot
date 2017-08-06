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

Display::Display() :
  _window(nullptr),
  _renderer(nullptr),
  _texture(nullptr),
  _update_lock(SDL_CreateMutex())
{}

Display::~Display() {
  SDL_DestroyMutex(_update_lock);

  if (_texture != nullptr)
    SDL_DestroyTexture(_texture);

  if (_renderer != nullptr)
    SDL_DestroyRenderer(_renderer);

  if (_window != nullptr)
    SDL_DestroyWindow(_window);

  SDL_Quit();
}

int Display::Init(void) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    return -1;

  if ((_window = SDL_CreateWindow("VitaBrot", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _screen_width, _screen_height, SDL_WINDOW_SHOWN)) == nullptr)
    return -1;

  if ((_renderer = SDL_CreateRenderer(_window, -1, 0)) == nullptr)
    return -1;

  // Clear the window
  SDL_RenderClear(_renderer);
  SDL_RenderPresent(_renderer);

  if ((_texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, _screen_width, _screen_height)) == nullptr)
    return -1;

  SDL_SetRenderTarget(_renderer, _texture);
  // Clear the texture to solid black
  SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
  SDL_RenderClear(_renderer);

  return 0;
}

void Display::Add_pixel(int32_t x, int32_t y, int32_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  // Lock access to the _pixel_updates list
  if (SDL_LockMutex(_update_lock) < 0)
    return;

  _pixel_updates.push_back({ x, y, size, r, g, b, a });
  SDL_UnlockMutex(_update_lock);
}

int Display::Draw_pixels(void) {
  if (_pixel_updates.empty())
    return 0;

  SDL_SetRenderTarget(_renderer, _texture);
  SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_NONE);
  while (1) {
    // Lock access to the _pixel_updates list
    if (SDL_LockMutex(_update_lock) < 0)
      return -1;

    if (_pixel_updates.empty()) {
      SDL_UnlockMutex(_update_lock);
      break;
    }

    _pixel_t pixel = _pixel_updates.front();
    _pixel_updates.pop_front();
    SDL_UnlockMutex(_update_lock);

    SDL_SetRenderDrawColor(_renderer, pixel.r, pixel.g, pixel.b, pixel.a);
    SDL_Rect rect = { pixel.x, pixel.y, pixel.size, pixel.size };
    SDL_RenderFillRect(_renderer, &rect);
  }
  SDL_SetRenderTarget(_renderer, nullptr);

  Refresh();

  return 0;
}

int Display::Refresh(void) {
  int rc = SDL_RenderCopy(_renderer, _texture, nullptr, nullptr);
  if (rc < 0)
    return rc;

  SDL_RenderPresent(_renderer);

  return 0;
}
