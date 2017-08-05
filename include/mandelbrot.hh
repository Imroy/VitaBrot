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

#include <SDL2/SDL_pixels.h>
#include "complex.hh"

class Mandelbrot {
private:
  complex _centre;
  float _window_size, _pixel_size;
  uint32_t _next_x, _next_y;
  bool _running;
  uint32_t _iteration_limit;
  SDL_Color *_palette;

  void _get_coords(uint32_t& x, uint32_t& y);

public:
  Mandelbrot();

  void move(float c_re, float c_im, float size);
  void reset(void);
  void set_limit(uint32_t limit);
  void run(void);
};

