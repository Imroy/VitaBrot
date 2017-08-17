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

#include <psp2/kernel/processmgr.h>
#include <SDL2/SDL_events.h>
#include "display.hh"
#include "mandelbrot.hh"

enum joystick_buttons {
  VITA_TRIANGLE,
  VITA_CIRCLE,
  VITA_CROSS,
  VITA_SQUARE,
  VITA_LTRIGGER,
  VITA_RTRIGGER,
  VITA_DOWN,
  VITA_LEFT,
  VITA_UP,
  VITA_RIGHT,
  VITA_SELECT,
  VITA_START,
};

int main(int argc, char *argv[]) {
  Display disp;

  SDL_InitSubSystem(SDL_INIT_JOYSTICK);
  SDL_Joystick *joy = SDL_JoystickOpen(0);

  Mandelbrot m(disp);
  m.move(-0.5, 0.0, 3);
  m.reset();
  m.set_limit(255);
  m.start_threads();

  bool running = true;
  while (running) {
    disp.Refresh();

    if (SDL_JoystickGetButton(joy, VITA_CIRCLE)) {
      running = false;
      continue;
    }

    bool changed = false;
    if (SDL_JoystickGetButton(joy, VITA_UP)) {
      m.move_rel(0, -0.01);
      changed = true;
    }

    if (SDL_JoystickGetButton(joy, VITA_RIGHT)) {
      m.move_rel(0.01, 0);
      changed = true;
    }

    if (SDL_JoystickGetButton(joy, VITA_DOWN)) {
      m.move_rel(0, 0.01);
      changed = true;
    }

    if (SDL_JoystickGetButton(joy, VITA_LEFT)) {
      m.move_rel(-0.01, 0);
      changed = true;
    }

    if (SDL_JoystickGetButton(joy, VITA_LTRIGGER)) {
      m.zoom_rel(0.9);
      changed = true;
    }

    if (SDL_JoystickGetButton(joy, VITA_RTRIGGER)) {
      m.zoom_rel(1.1);
      changed = true;
    }

    if (changed)
      m.reset();
  }

  m.stop_threads();

  SDL_JoystickClose(joy);

  sceKernelExitProcess(0);
  return 0;
}
