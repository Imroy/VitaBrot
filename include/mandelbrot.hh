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

#include <complex>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_thread.h>
#include "complexpair.hh"
#include "display.hh"

class Mandelbrot {
public:
  enum class Precision {
    Single,
    Double,
  };

private:
  Display *_display;
  std::complex<double> _centre[2];
  double _window_size[2], _pixel_size[2];
  uint32_t _iteration_limit;
  bool _running, _shutdown, _julia;
  SDL_Palette *_palette;

  int32_t _next_x, _next_y, _first_pass, _pass, _pass_size;

  SDL_mutex *_coords_mutex;
  void _get_coords(uint32_t& x, uint32_t& y, uint32_t& size);

  uint32_t _restart_sem;

  SDL_Thread *_threads[4];	// Four threads for the quad-core CPU on the Vita

  // Allow the thread function to access private data and methods
  friend int Mandelbrot_sp_thread(void* data);
  friend int Mandelbrot_dp_thread(void* data);

public:
  Mandelbrot(Display& d);
  ~Mandelbrot();

  double window_size(void) const { return _window_size[_julia]; }
  int32_t pass(void) const { return _pass; }

  void switch_type(void);

  // Move the window
  void move(double c_re, double c_im, double size);

  // Move the window relative to its size
  void move_rel(double r_re, double r_im);

  // Zoom the window relative to its size
  void zoom_rel(double rel);

  // Reset the drawing of pixels
  void reset(void);

  // Set the iteration limit
  void set_limit(uint32_t limit);

  void start_threads(Precision p = Precision::Single);

  void stop_threads(void);
};

int Mandelbrot_sp_thread(void* data);
int Mandelbrot_dp_thread(void* data);
