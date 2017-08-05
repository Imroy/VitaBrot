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

#include <cstdint>
#include <cmath>

template <typename T>
constexpr T sqr(T val) { return val * val; }

// Yes, I know there is std::complex.
// I want to implement NEON optimisations.

class complex {
private:
  float _re, _im;

public:
  complex(float a = 0, float b = 0) :
    _re(a), _im(b)
  {}

  complex(const complex& other) :
    _re(other._re), _im(other._im)
  {}

  complex& operator =(const complex& other) {
    _re = other._re;
    _im = other._im;

    return *this;
  }

  complex& operator =(const float other) {
    _re = other;
    _im = 0;

    return *this;
  }

  float real(void) const { return _re; }
  float imag(void) const { return _im; }

  complex& operator +=(const complex& other) {
    _re += other._re;
    _im += other._im;
    return *this;
  }

  complex& operator +=(float other) {
    _re += other;
    return *this;
  }

  complex& operator -=(const complex& other) {
    _re -= other._re;
    _im -= other._im;
    return *this;
  }

  complex& operator -=(float other) {
    _re -= other;
    return *this;
  }

  complex& operator *=(const complex& other) {
    float a = (_re * other._re) - (_im * other._im);
    float b = (_re * other._im) + (_im * other._re);
    _re = a;
    _im = b;

    return *this;
  }

  complex& operator *=(float other) {
    _re *= other;
    _im *= other;

    return *this;
  }

  friend complex operator +(const complex& a) {
    return complex(a._re, a._im);
  }

  friend complex operator -(const complex& a) {
    return complex(-a._re, -a._im);
  }

  friend complex operator +(const complex& a, const complex& b) {
    return complex(a._re + b._re,
		   a._im + b._im);
  }

  friend complex operator +(const complex& a, const float b) {
    return complex(a._re + b,
		   a._im);
  }

  friend complex operator +(const float a, const complex& b) {
    return complex(a + b._re,
		   b._im);
  }

  friend complex operator -(const complex& a, const complex& b) {
    return complex(a._re - b._re,
		   a._im - b._im);
  }

  friend complex operator -(const complex& a, const float b) {
    return complex(a._re - b,
		   a._im);
  }

  friend complex operator -(const float a, const complex& b) {
    return complex(a - b._re,
		   b._im);
  }

  friend complex operator *(const complex& a, const complex& b) {
    return complex((a._re * b._re) - (a._im * b._im),
		   (a._re * b._im) + (a._im * b._re));
  }

  friend complex operator *(const complex& a, const float b) {
    return complex(a._re * b,
		   a._im * b);
  }

  friend complex operator *(const float a, const complex& b) {
    return complex(a * b._re,
		   a * b._im);
  }

  friend bool operator ==(const complex& a, const complex& b) {
    return (a._re == b._re) && (a._im == b._im);
  }

  friend bool operator !=(const complex& a, const complex& b) {
    return (a._re != b._re) && (a._im != b._im);
  }

  friend constexpr float real(const complex& c) { return c._re; }
  friend constexpr float imag(const complex& c) { return c._im; }

  friend constexpr float norm(const complex& a) { return sqr(a._re) + sqr(a._im); }
  friend constexpr float abs(const complex& a) { return sqrt(sqr(a._re) + sqr(a._im)); }

  friend complex sqr(const complex& a) {
    return complex(sqr(a._re) - sqr(a._im),
		   2 * a._re * a._im);
  }

};
