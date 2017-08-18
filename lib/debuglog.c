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

#include "debuglog.h"

SceUID log_fd;

int log_open(const char* filename) {
  log_fd = sceIoOpen(filename, SCE_O_WRONLY | SCE_O_CREAT, 0777);

  if (log_fd < 0)
    return log_fd;
  return 0;
}

int log_close(void) {
  return sceIoClose(log_fd);
}
