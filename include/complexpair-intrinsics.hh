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

inline complexpair& complexpair::operator +=(const complexpair& other) {
  _reals = _reals + other._reals;
  _imags = _imags + other._imags;
  return *this;
}

inline complexpair& complexpair::operator -=(const complexpair& other) {
  _reals = _reals - other._reals;
  _imags = _imags - other._imags;
  return *this;
}

inline complexpair& complexpair::operator *=(const complexpair& other) {
  float32x2_t re = _reals * other._reals;
  re -= _imags * other._imags;

  float32x2_t im = _imags * other._reals;
  im += _reals * other._imags;

  _reals = re;
  _imags = im;

  return *this;
}

inline complexpair& complexpair::operator /=(const complexpair& other) {
  float32x2_t den = sqr(other._reals) + sqr(other._imags);
  float32x2_t r_den = 1.0f / den;

  float32x2_t re_num = (_reals * other._reals) + (_imags * other._imags);
  float32x2_t im_num = (_imags * other._reals) - (_reals * other._imags);

  _reals = re_num * r_den;
  _imags = im_num * r_den;

  return *this;
}

inline complexpair operator +(const complexpair& a, const complexpair& b) {
  return complexpair(a._reals + b._reals, a._imags + b._imags);
}

inline complexpair operator -(const complexpair& a, const complexpair& b) {
  return complexpair(a._reals - b._reals, a._imags - b._imags);
}

inline complexpair operator *(const complexpair& a, const complexpair& b) {
  float32x2_t re = (a._reals * b._reals) - (a._imags * b._imags);
  float32x2_t im = (a._imags * b._reals) + (a._reals * b._imags);

  return complexpair(re, im);
}

inline complexpair operator /(const complexpair& a, const complexpair& b) {
  float32x2_t den = sqr(b._reals) + sqr(b._imags);
  float32x2_t r_den = 1.0f / den;

  float32x2_t re_num = (a._reals * b._reals) + (a._imags * b._imags);
  float32x2_t im_num = (a._imags * b._reals) - (a._reals * b._imags);

  return complexpair(re_num * r_den, im_num * r_den);
}

inline boolx2_t operator ==(const complexpair& a, const complexpair& b) {
  return (a._reals == b._reals) && (a._imags == b._imags);
}

inline boolx2_t operator !=(const complexpair& a, const complexpair& b) {
  return !((a._reals == b._reals) || (a._imags == b._imags));
}

inline float32x2_t norm(const complexpair& a) {
  return sqr(a._reals) + sqr(a._imags);
}
