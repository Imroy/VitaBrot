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
#include <ostream>
#include <arm_neon.h>

template <typename T>
constexpr T sqr(T val) { return val * val; }

#ifndef __arm__
#include <complex>
typedef std::complex<float> complex;

#else
// NEON optimised complex class

class complex {
private:
  float32x2_t _vec;

  // private constructor from a NEON vector type
  complex(float32x2_t v) :
    _vec(v)
  {}

public:
  complex(float a = 0, float b = 0) :
    _vec{a, b}
  {}

  complex(const complex& other) :
    _vec(other._vec)
  {}

  complex& operator =(const complex& other) {
    _vec = other._vec;

    return *this;
  }

  complex& operator =(const float other) {
    _vec[0] = other;
    _vec[1] = 0;

    return *this;
  }

  float real(void) const { return _vec[0]; }
  float imag(void) const { return _vec[1]; }

  complex& operator +=(const complex& other) {
    _vec = vadd_f32(_vec, other._vec);
    return *this;
  }

  complex& operator +=(float other) {
    _vec[0] += other;
    return *this;
  }

  complex& operator -=(const complex& other) {
    _vec = vsub_f32(_vec, other._vec);
    return *this;
  }

  complex& operator -=(float other) {
    _vec[0] -= other;
    return *this;
  }

  complex& operator *=(const complex& other) {
    float32x2_t mul1 = vmul_f32(_vec, other._vec);
    float32x2_t reals = { _vec[0], other._vec[0] }, imags = { other._vec[1], _vec[1] };
    float32x2_t mul2 = vmul_f32(reals, imags);

    mul1[1] = -mul1[1];
    _vec = vpadd_f32(mul1, mul2);

    return *this;
  }

  complex& operator *=(float other) {
    _vec = vmul_n_f32(_vec, other);

    return *this;
  }

  complex& operator /=(const complex& other) {
    float32x2_t re = vmul_f32(_vec, other._vec);
    float32x2_t a_inv = { _vec[1], -_vec[0] }, im = vmul_f32(a_inv, other._vec);
    float32x2_t num = vpadd_f32(re, im);

    float32x2_t c2d2 = vmul_f32(other._vec, other._vec);
    float r_den = 1.0f / (c2d2[0] + c2d2[1]);
    _vec = vmul_n_f32(num, r_den);

    return *this;
  }

  complex& operator /=(float other) {
    _vec = vmul_n_f32(_vec, 1.0f / other);

    return *this;
  }

  friend complex operator +(const complex& a) {
    return complex(a._vec);
  }

  friend complex operator -(const complex& a) {
    return complex(-a._vec[0], -a._vec[1]);
  }

  friend complex operator +(const complex& a, const complex& b) {
    return complex(vadd_f32(a._vec, b._vec));
  }

  friend complex operator +(const complex& a, const float b) {
    return complex(a._vec[0] + b,
		   a._vec[1]);
  }

  friend complex operator +(const float a, const complex& b) {
    return complex(a + b._vec[0],
		   b._vec[1]);
  }

  friend complex operator -(const complex& a, const complex& b) {
    return complex(vsub_f32(a._vec, b._vec));
  }

  friend complex operator -(const complex& a, const float b) {
    return complex(a._vec[0] - b,
		   a._vec[1]);
  }

  friend complex operator -(const float a, const complex& b) {
    return complex(a - b._vec[0],
		   b._vec[1]);
  }

  friend complex operator *(const complex& a, const complex& b) {
    float32x2_t mul1 = vmul_f32(a._vec, b._vec);
    float32x2_t reals = { a._vec[0], b._vec[0] }, imags = { b._vec[1], a._vec[1] };
    float32x2_t mul2 = vmul_f32(reals, imags);

    mul1[1] = -mul1[1];
    return complex(vpadd_f32(mul1, mul2));
  }

  friend complex operator *(const complex& a, const float b) {
    return complex(vmul_n_f32(a._vec, b));
  }

  friend complex operator *(const float a, const complex& b) {
    return complex(vmul_n_f32(b._vec, a));
  }

  friend complex operator /(const complex& a, const complex& b) {
    float32x2_t re = vmul_f32(a._vec, b._vec);
    float32x2_t a_inv = { a._vec[1], -a._vec[0] }, im = vmul_f32(a_inv, b._vec);
    float32x2_t num = vpadd_f32(re, im);

    float32x2_t c2d2 = vmul_f32(b._vec, b._vec);
    float r_den = 1.0f / (c2d2[0] + c2d2[1]);

    return complex(vmul_n_f32(num, r_den));
  }

  friend complex operator /(const complex& a, const float b) {
    return complex(vmul_n_f32(a._vec, 1.0f / b));
  }

  friend complex operator /(const float a, const complex& b) {
    return complex(vmul_n_f32(b._vec, 1.0f / a));
  }

  friend bool operator ==(const complex& a, const complex& b) {
    uint32x2_t eq = vceq_f32(a._vec, b._vec);
    return eq[0] && eq[1];
  }

  friend bool operator !=(const complex& a, const complex& b) {
    uint32x2_t eq = vceq_f32(a._vec, b._vec);
    return !(eq[0] || eq[1]);
  }

  friend constexpr float real(const complex& c) { return c._vec[0]; }
  friend constexpr float imag(const complex& c) { return c._vec[1]; }

  friend float norm(const complex& a) {
    float32x2_t vsqr = vmul_f32(a._vec, a._vec);
    return vsqr[0] + vsqr[1];
  }

  friend float abs(const complex& a) {
    return sqrt(norm(a));
  }

  friend complex sqr(const complex& a) {
    float32x2_t vsqr = vmul_f32(a._vec, a._vec);
    return complex(vsqr[0] - vsqr[1],
		   2 * a._vec[0] * a._vec[1]);
  }

  friend std::ostream& operator <<(std::ostream& os, const complex& c) {
    os << c._vec[0];
    if (c._vec[1] < 0)
      os << " - ";
    else
      os << " + ";
    os << std::abs(c._vec[1]) << "i";

    return os;
  }

};
#endif
