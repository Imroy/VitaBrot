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
#include <psp2/ctrl.h>
#include "display.hh"
#include "mandelbrot.hh"

int main(int argc, char *argv[]) {
  int rc = display_init();
  if (rc)
    return rc;

  Mandelbrot m;
  m.move(-0.5, 0.0, 3);
  m.reset();
  m.set_limit(255);
  m.start_threads();

  sceCtrlSetSamplingMode(SCE_CTRL_MODE_DIGITAL);
  SceCtrlData ctrl;
  while (1) {
    sceCtrlPeekBufferPositive(0, &ctrl, 1);

    bool changed = false;
    if (ctrl.buttons & SCE_CTRL_UP) {
      m.move_rel(0, -0.01);
      changed = true;
    }

    if (ctrl.buttons & SCE_CTRL_RIGHT) {
      m.move_rel(0.01, 0);
      changed = true;
    }

    if (ctrl.buttons & SCE_CTRL_DOWN) {
      m.move_rel(0, 0.01);
      changed = true;
    }

    if (ctrl.buttons & SCE_CTRL_LEFT) {
      m.move_rel(-0.01, 0);
      changed = true;
    }

    if (ctrl.buttons & SCE_CTRL_LTRIGGER) {
      m.zoom_rel(0.9);
      changed = true;
    }

    if (ctrl.buttons & SCE_CTRL_RTRIGGER) {
      m.zoom_rel(1.1);
      changed = true;
    }

    if (changed)
      m.reset();
  }

  m.stop_threads();

  rc = display_exit();
  if (rc)
    return rc;

  sceKernelExitProcess(0);
  return 0;
}
