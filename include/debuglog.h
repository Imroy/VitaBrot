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

#include <psp2/io/fcntl.h>

#define DEBUG_LOG(x) if (log_fd > 0) sceIoWrite(log_fd, (x), strlen(x));

#ifdef __cplusplus
extern "C" {
#endif

extern SceUID log_fd;

int log_open(const char* filename);
int log_close(void);

#ifdef __cplusplus
}
#endif
