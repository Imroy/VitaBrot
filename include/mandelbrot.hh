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
#include <psp2/kernel/threadmgr.h>
#include "complex.hh"

class Mandelbrot {
private:
  complex _centre;
  float _window_size, _pixel_size;
  uint32_t _next_x, _next_y;
  bool _running;
  uint32_t _iteration_limit;
  SDL_Color *_palette;

  SceUID _coords_mutex;
  void _get_coords(uint32_t& x, uint32_t& y);

  SceUID _threads[4];	// Four threads for the quad-core CPU on the Vita

  // Allow the thread function to access private data and methods
  friend int Mandelbrot_thread(SceSize argsize, void* argp);

public:
  Mandelbrot();
  ~Mandelbrot();

  // Move the window
  void move(float c_re, float c_im, float size);

  // Move the window relative to its size
  void move_rel(float r_re, float r_im);

  // Zoom the window relative to its size
  void zoom_rel(float rel);

  // Reset the drawing of pixels
  void reset(void);

  // Set the iteration limit
  void set_limit(uint32_t limit);

  void start_threads(void);

  void stop_threads(void);
};

