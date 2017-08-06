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

#pragma once

#include <deque>
#include <SDL2/SDL.h>

class Display {
private:
  SDL_Window *_window;
  SDL_Renderer *_renderer;
  SDL_Texture *_texture;

  // Screen dimension constants
  const int32_t _screen_width = 960, _screen_height = 544;

  struct _pixel_t {
    int32_t x, y, size;
    uint8_t r, g, b, a;
  };

  SDL_mutex *_update_lock;
  std::deque<_pixel_t> _pixel_updates;
  uint32_t _last_redraw;

public:
  Display();
  ~Display();

  int Init(void);

  // Add a pixel to the list of pixels to be drawn
  void Add_pixel(int32_t x, int32_t y, int32_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

  // Draw pixels from the list	*** Only in the main thread! ***
  int Draw_pixels(void);

  // Refresh contents of window	*** Only in the main thread ***
  int Refresh(void);

  const int32_t width(void) const { return _screen_width; }
  const int32_t height(void) const { return _screen_height; }

};
