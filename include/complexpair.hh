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

template <typename T>
constexpr T sqr(T val) { return val * val; }

#ifdef __ARM_NEON__
#include <arm_neon.h>

typedef uint32x2_t boolx2_t;
#else
typedef float float32x2_t __attribute__ ((vector_size (8)));
typedef int boolx2_t __attribute__ ((vector_size (8)));
#endif

inline std::ostream& operator <<(std::ostream& os, const float32x2_t& p) {
  os << "{ " << p[0] << ", " << p[1] << " }";
  return os;
}

// NEON optimised complex-pair class

class complexpair {
private:
  float32x2_t _reals, _imags;

  // private constructor from two NEON vectors
  complexpair(float32x2_t re, float32x2_t im) :
    _reals(re), _imags(im)
  {}

public:
  complexpair(float a = 0, float b = 0, float c = 0, float d = 0) :
    _reals{a, c},
    _imags{b, d}
  {}

  complexpair(const complexpair& other) :
    _reals(other._reals),
    _imags(other._imags)
  {}

  complexpair& operator =(const complexpair& other) {
    _reals = other._reals;
    _imags = other._imags;

    return *this;
  }

  std::complex<float> get(uint8_t i) const { return std::complex<float>(_reals[i], _imags[i]); }

  void set(uint8_t i, float re = 0, float im = 0) {
    _reals[i] = re;
    _imags[i] = im;
  }

  void set(uint8_t i, std::complex<float> c) {
    _reals[i] = c.real();
    _imags[i] = c.imag();
  }

  void set(uint8_t i, std::complex<double> c) {
    _reals[i] = (float)c.real();
    _imags[i] = (float)c.imag();
  }

  float real(uint8_t i) const { return _reals[i]; }
  float imag(uint8_t i) const { return _imags[i]; }

  complexpair& operator +=(const complexpair& other);
  complexpair& operator -=(const complexpair& other);
  complexpair& operator *=(const complexpair& other);
  complexpair& operator /=(const complexpair& other);

  friend complexpair operator +(const complexpair& a)  {
    return complexpair(a._reals, a._imags);
  }

  friend complexpair operator +(const complexpair& a, const complexpair& b);
  friend complexpair operator -(const complexpair& a, const complexpair& b);
  friend complexpair operator *(const complexpair& a, const complexpair& b);
  friend complexpair operator /(const complexpair& a, const complexpair& b);

  friend boolx2_t operator ==(const complexpair& a, const complexpair& b);
  friend boolx2_t operator !=(const complexpair& a, const complexpair& b);

  friend constexpr float real(const complexpair& c, uint8_t i) { return c._reals[i]; }
  friend constexpr float imag(const complexpair& c, uint8_t i) { return c._imags[i]; }

  friend float32x2_t norm(const complexpair& a);

  friend std::ostream& operator <<(std::ostream& os, const complexpair& c) {
    os << "{ " << c._reals[0];
    if (c._imags[0] < 0)
      os << " - ";
    else
      os << " + ";
    os << std::abs(c._imags[0]) << "i, ";

    os << c._reals[1];
    if (c._imags[1] < 0)
      os << " - ";
    else
      os << " + ";
    os << std::abs(c._imags[1]) << "i }";

    return os;
  }

};

#ifdef __ARM_NEON__
#include "complexpair-neon.hh"
#else
#include "complexpair-intrinsics.hh"
#endif
