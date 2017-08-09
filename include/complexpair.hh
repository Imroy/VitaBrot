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

#include "complex.hh"

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

  complex get(uint8_t i) const { return complex(_reals[i], _imags[i]); }

  void set(uint8_t i, float re = 0, float im = 0) {
    _reals[i] = re;
    _imags[i] = im;
  }

  void set(uint8_t i, complex c) {
    _reals[i] = c.real();
    _imags[i] = c.imag();
  }

  float real(uint8_t i) const { return _reals[i]; }
  float imag(uint8_t i) const { return _imags[i]; }

  complexpair& operator +=(const complexpair& other) {
    _reals = vadd_f32(_reals, other._reals);
    _imags = vadd_f32(_imags, other._imags);
    return *this;
  }

  complexpair& operator -=(const complexpair& other) {
    _reals = vsub_f32(_reals, other._reals);
    _imags = vsub_f32(_imags, other._imags);
    return *this;
  }

  complexpair& operator *=(const complexpair& other) {
    float32x2_t re = vmul_f32(_reals, other._reals);	// ac
    re = vmls_f32(re, _imags, other._imags);		// ac - bd

    float32x2_t im = vmul_f32(_imags, other._reals);	// bc
    im = vmla_f32(im, _reals, other._imags);		// bc + ad

    _reals = re;
    _imags = im;

    return *this;
  }

  complexpair& operator /=(const complexpair& other) {
    float32x2_t den = vadd_f32(vmul_f32(other._reals, other._reals),
			       vmul_f32(other._imags, other._imags));
    float32x2_t r_den = vrecpe_f32(den);
    for (uint8_t i = 0; i < 4; i++)
      r_den = vmul_f32(vrecps_f32(den, r_den), r_den);

    float32x2_t re_num = vmul_f32(_reals, other._reals);
    re_num = vmla_f32(re_num, _imags, other._imags);
    float32x2_t im_num = vmul_f32(_imags, other._reals);
    im_num = vmls_f32(im_num, _reals, other._imags);

    _reals = vmul_f32(re_num, r_den);
    _imags = vmul_f32(im_num, r_den);

    return *this;
  }

  friend complexpair operator +(const complexpair& a) {
    return complexpair(a._reals, a._imags);
  }

  friend complexpair operator +(const complexpair& a, const complexpair& b) {
    return complexpair(vadd_f32(a._reals, b._reals), vadd_f32(a._imags, b._imags));
  }

  friend complexpair operator -(const complexpair& a, const complexpair& b) {
    return complexpair(vsub_f32(a._reals, b._reals), vsub_f32(a._imags, b._imags));
  }

  friend complexpair operator *(const complexpair& a, const complexpair& b) {
    float32x2_t re = vmul_f32(a._reals, b._reals);
    re = vmls_f32(re, a._imags, b._imags);

    float32x2_t im = vmul_f32(a._imags, b._reals);
    im = vmla_f32(im, a._reals, b._imags);

    return complexpair(re, im);
  }

  friend complexpair operator /(const complexpair& a, const complexpair& b) {
    float32x2_t den = vadd_f32(vmul_f32(b._reals, b._reals), vmul_f32(b._imags, b._imags));
    float32x2_t r_den = vrecpe_f32(den);
    for (uint8_t i = 0; i < 2; i++)
      r_den = vmul_f32(vrecps_f32(den, r_den), r_den);

    float32x2_t re_num = vmul_f32(a._reals, b._reals);
    re_num = vmla_f32(re_num, a._imags, b._imags);
    float32x2_t im_num = vmul_f32(a._imags, b._reals);
    im_num = vmls_f32(im_num, a._reals, b._imags);

    return complexpair(vmul_f32(re_num, r_den),
		       vmul_f32(im_num, r_den));
  }

  friend uint32x2_t operator ==(const complexpair& a, const complexpair& b) {
    uint32x2_t re = vceq_f32(a._reals, b._reals);
    uint32x2_t im = vceq_f32(a._imags, b._imags);
    return vand_u32(re, im);
  }

  friend uint32x2_t operator !=(const complexpair& a, const complexpair& b) {
    uint32x2_t re = vceq_f32(a._reals, b._reals);
    uint32x2_t im = vceq_f32(a._imags, b._imags);
    uint32x2_t vor = vorr_u32(re, im);
    return (uint32x2_t){ !vor[0], !vor[1] };
  }

  friend constexpr float real(const complexpair& c, uint8_t i) { return c._reals[i]; }
  friend constexpr float imag(const complexpair& c, uint8_t i) { return c._imags[i]; }

  friend float32x2_t norm(const complexpair& a) {
    float32x2_t re = vmul_f32(a._reals, a._reals);
    float32x2_t im = vmul_f32(a._imags, a._imags);
    return vadd_f32(re, im);
  }

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
