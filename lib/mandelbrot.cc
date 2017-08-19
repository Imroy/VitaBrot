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

#include "mandelbrot.hh"
#include <stdio.h>
#include "display.hh"
#include "complexpair.hh"

Mandelbrot::Mandelbrot(Display& d) :
  _display(&d),
  _centre(-0.5, 0.0),
  _window_size(3), _pixel_size(_window_size / _display->width()),
  _next_x(0), _next_y(0),
  _first_pass(6), _pass(_first_pass), _pass_size(1 << _pass),
  _running(false), _shutdown(false),
  _iteration_limit(0),
  _palette(nullptr),
  _coords_mutex(SDL_CreateMutex()),
  _restart_sem(0)
{}

Mandelbrot::~Mandelbrot() {
  SDL_DestroyMutex(_coords_mutex);

  if (_palette != nullptr)
    SDL_FreePalette(_palette);
}

void Mandelbrot::move(float c_re, float c_im, float size) {
  _centre = complex(c_re, c_im);
  _window_size = size;
  _pixel_size = size / _display->width();
}

void Mandelbrot::move_rel(float r_re, float r_im) {
  _centre += complex(r_re, r_im) * _window_size;
}

void Mandelbrot::zoom_rel(float rel) {
  _window_size *= rel;
  _pixel_size = _window_size / _display->width();
}

void Mandelbrot::reset(void) {
  _next_x = _next_y = 0;
  _pass = _first_pass;
  _pass_size = 1 << _pass;
  _running = true;
  _restart_sem++;
}

SDL_Color colours1[31] = {
  {   0,   0,   0, 255 },
  { 120, 119, 238, 255 },
  {  24,   7,  25, 255 },
  { 197,  66,  28, 255 },
  {  29,  18,  11, 255 },
  { 135,  46,  71, 255 },
  {  24,  27,  13, 255 },
  { 241, 230, 128, 255 },
  {  17,  31,  24, 255 },
  { 240, 162, 139, 255 },
  {  11,   4,  30, 255 },
  { 106,  87, 189, 255 },
  {  29,  21,  14, 255 },
  {  12, 140, 118, 255 },
  {  10,   6,  29, 255 },
  {  50, 144,  77, 255 },
  {  22,   0,  24, 255 },
  { 148, 188, 243, 255 },
  {   4,  32,   7, 255 },
  { 231, 146,  14, 255 },
  {  10,  13,  20, 255 },
  { 184, 147,  68, 255 },
  {  13,  28,   3, 255 },
  { 169, 248, 152, 255 },
  {   4,   0,  34, 255 },
  {  62,  83,  48, 255 },
  {   7,  21,  22, 255 },
  { 152,  97, 184, 255 },
  {   8,   3,  12, 255 },
  { 247,  92, 235, 255 },
  {  31,  32,  16, 255 }
};

void Mandelbrot::set_limit(uint32_t limit) {
  _iteration_limit = limit;

  if (_palette != nullptr)
    delete [] _palette;

  if (_palette != nullptr)
    SDL_FreePalette(_palette);
  _palette = SDL_AllocPalette(limit + 1);

  uint32_t n = 0;

  uint32_t segmentsize = 8;
  int32_t r_segmentsize = 256 / segmentsize;
  uint32_t nsegments = 31, setsegments = ((limit + 3) * r_segmentsize) >> 8;
  for (uint32_t i = 0; i < setsegments; i++) {
    if (i == (setsegments - 1)) {
      segmentsize = limit - n - 2;
      r_segmentsize = 256 / segmentsize;
    }

    SDL_Color &col1 = colours1[i % nsegments], &col2 = colours1[(i + 1) % setsegments % nsegments];
    int32_t r = col1.r << 8;
    int32_t g = col1.g << 8;
    int32_t b = col1.b << 8;
    int32_t rs = ((((int32_t)col2.r << 8) - r) * r_segmentsize) >> 8;
    int32_t gs = ((((int32_t)col2.g << 8) - g) * r_segmentsize) >> 8;
    int32_t bs = ((((int32_t)col2.b << 8) - b) * r_segmentsize) >> 8;

    for (uint32_t y = 0; y < segmentsize; y++) {
      SDL_Color col = { (uint8_t)(r >> 8), (uint8_t)(g >> 8), (uint8_t)(b >> 8), 255 };
      SDL_SetPaletteColors(_palette, &col, n++, 1);

      r += rs;
      g += gs;
      b += bs;
    }
  }

  while (n < limit + 1) {
    SDL_Color col = { 0, 0, 0, 255 };
    SDL_SetPaletteColors(_palette, &col, n++, 1);
  }
}

void Mandelbrot::_get_coords(uint32_t& x, uint32_t& y, uint32_t& size) {
  SDL_LockMutex(_coords_mutex);

  x = _next_x;
  y = _next_y;
  size = _pass_size;

 increment:
  _next_y += _pass_size;
  if (_next_y >= _display->height()) {
    _next_x += _pass_size;
    _next_y -= _display->height();

    if (_next_x >= _display->width()) {
      if (_pass > 0) {
	_next_x = _next_y = 0;
	_pass--;
	_pass_size = 1 << _pass;
      } else
	_running = false;
    }
  }

  if (_running && (_pass < _first_pass)) {
    for (uint8_t pass = _pass + 1; pass <= _first_pass; pass++) {
      uint32_t mask = (1 << pass) - 1;
      if (((_next_x & mask) == 0) && ((_next_y & mask) == 0))
	goto increment;	// Forgive me.
    }
  }

  SDL_UnlockMutex(_coords_mutex);
}

int Mandelbrot_thread(void* data);

void Mandelbrot::start_threads(void) {
  for (uint8_t i = 0; i < 4; i++) {
    char name[12];
    snprintf(name, 12, "Mandelbrot%d", i+1);
    _threads[i] = SDL_CreateThread(Mandelbrot_thread, name, this);
  }
}

void Mandelbrot::stop_threads(void) {
  _shutdown = true;
  for (uint8_t i = 0; i < 4; i++) {
    int status;
    SDL_WaitThread(_threads[i], &status);
  }
}

complex Mandelbrot::_calc_c(uint32_t x, uint32_t y) {
  return _centre + (complex(x - (_display->width() * 0.5),
			    (_display->height() / _display->height()) * (y - (_display->height() * 0.5))) * _pixel_size);
}

int Mandelbrot_thread(void* data) {
  Mandelbrot *m = (Mandelbrot*)data;

  uint32_t x[2], y[2], size[2];
  complexpair z, c;
  uint32_t iter[2];
  uint32_t restart_val;

 restart:
  restart_val = m->_restart_sem;
  m->_get_coords(x[0], y[0], size[0]);
  m->_get_coords(x[1], y[1], size[1]);
  c.set(0, m->_calc_c(x[0], y[0]));
  c.set(1, m->_calc_c(x[1], y[1]));
  iter[0] = iter[1] = 0;

  while (!m->_shutdown) {
    while (!m->_running && !m->_shutdown)
      SDL_Delay(1);
    if (m->_shutdown)
      return 0;

    if (m->_restart_sem != restart_val)
      goto restart;

    z = sqr(z) + c;
    iter[0]++;
    iter[1]++;

    float32x2_t n = norm(z);
    for (uint8_t i = 0; i < 2; i++) {
      if ((iter[i] >= m->_iteration_limit) || (n[i] >= 4)){
	SDL_Color &col = m->_palette->colors[iter[i]];
	m->_display->Draw_pixel(x[i], y[i], size[i], col.r, col.g, col.b, col.a);

	m->_get_coords(x[i], y[i], size[i]);
	z.set(i);
	c.set(i, m->_calc_c(x[i], y[i]));
	iter[i] = 0;
      }
    }
  }

  return 0;
}
