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
#include <psp2/power.h>
#include "display.hh"
#include "mandelbrot.hh"
#include "debuglog.h"

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
  VITA_NUM_BUTTONS
};

int main(int argc, char *argv[]) {
  log_open("ux0:data/vitabrot.log");
  Display disp;

  SDL_InitSubSystem(SDL_INIT_JOYSTICK);
  SDL_Joystick *joy = SDL_JoystickOpen(0);
  if (joy == nullptr) {
    DEBUG_LOG(SDL_GetError());
  }


  // Save old clock frequencies
  int old_armclock, old_busclock, old_gpuxbarclock, old_gpuclock;
  old_armclock = scePowerGetArmClockFrequency();
  old_busclock = scePowerGetBusClockFrequency();
  old_gpuxbarclock = scePowerGetGpuXbarClockFrequency();
  old_gpuclock = scePowerGetGpuClockFrequency();

  // Set maximum performance
  scePowerSetArmClockFrequency(444);
  scePowerSetBusClockFrequency(222);
  scePowerSetGpuClockFrequency(222);
  scePowerSetGpuXbarClockFrequency(166);

  Mandelbrot m(disp);
  m.move(-0.5, 0.0, 4.0);
  m.reset();
  m.set_limit(1023);

  m.start_threads();

  bool buttons[VITA_NUM_BUTTONS];
  for (uint8_t i = 0; i < VITA_NUM_BUTTONS; i++)
    buttons[i] = false;

  uint32_t last_switch = 0, last_move = 0;
  bool running = true;
  while (running) {
    disp.Refresh();

    bool changed = false;
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
      switch (ev.type) {
      case SDL_QUIT:
	running = false;
	break;

      case SDL_JOYBUTTONDOWN:
	buttons[ev.jbutton.button] = true;
	break;

      case SDL_JOYBUTTONUP:
	buttons[ev.jbutton.button] = false;
	break;

      }
    }

    if (buttons[VITA_CIRCLE]) {
      running = false;
      continue;
    }


    if (buttons[VITA_SQUARE] && (SDL_GetTicks() > last_switch + 400)) {
      m.switch_type();
      changed = true;
      last_switch = SDL_GetTicks();
    }

    // Limit rate of moving/zooming to 10 Hz
    if (SDL_GetTicks() < last_move + 100)
      continue;

    if (buttons[VITA_UP]) {
      m.move_rel(0, -0.01);
      changed = true;
    }

    if (buttons[VITA_RIGHT]) {
      m.move_rel(0.01, 0);
      changed = true;
    }

    if (buttons[VITA_DOWN]) {
      m.move_rel(0, 0.01);
      changed = true;
    }

    if (buttons[VITA_LEFT]) {
      m.move_rel(-0.01, 0);
      changed = true;
    }

    if (buttons[VITA_LTRIGGER]) {
      m.zoom_rel(1.05);
      changed = true;
    }

    if (buttons[VITA_RTRIGGER]) {
      m.zoom_rel(0.95);
      changed = true;
    }

    if (changed) {
      last_move = SDL_GetTicks();
      m.reset();
    }
  }

  m.stop_threads();

  // Restore clock frequencies
  scePowerSetArmClockFrequency(old_armclock);
  scePowerSetBusClockFrequency(old_busclock);
  scePowerSetGpuXbarClockFrequency(old_gpuxbarclock);
  scePowerSetGpuClockFrequency(old_gpuclock);

  SDL_JoystickClose(joy);

  log_close();

  sceKernelExitProcess(0);
  return 0;
}
