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
  _reals = vadd_f32(_reals, other._reals);
  _imags = vadd_f32(_imags, other._imags);
  return *this;
}

inline complexpair& complexpair::operator -=(const complexpair& other) {
  _reals = vsub_f32(_reals, other._reals);
  _imags = vsub_f32(_imags, other._imags);
  return *this;
}

inline complexpair& complexpair::operator *=(const complexpair& other) {
  float32x2_t re = vmul_f32(_reals, other._reals);	// ac
  re = vmls_f32(re, _imags, other._imags);		// ac - bd

  float32x2_t im = vmul_f32(_imags, other._reals);	// bc
  im = vmla_f32(im, _reals, other._imags);		// bc + ad

  _reals = re;
  _imags = im;

  return *this;
}

inline complexpair& complexpair::operator /=(const complexpair& other) {
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

inline complexpair operator +(const complexpair& a, const complexpair& b) {
  return complexpair(vadd_f32(a._reals, b._reals), vadd_f32(a._imags, b._imags));
}

inline complexpair operator -(const complexpair& a, const complexpair& b) {
  return complexpair(vsub_f32(a._reals, b._reals), vsub_f32(a._imags, b._imags));
}

inline complexpair operator *(const complexpair& a, const complexpair& b) {
  float32x2_t re = vmul_f32(a._reals, b._reals);
  re = vmls_f32(re, a._imags, b._imags);

  float32x2_t im = vmul_f32(a._imags, b._reals);
  im = vmla_f32(im, a._reals, b._imags);

  return complexpair(re, im);
}

inline complexpair operator /(const complexpair& a, const complexpair& b) {
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

inline boolx2_t operator ==(const complexpair& a, const complexpair& b) {
  boolx2_t re = vceq_f32(a._reals, b._reals);
  boolx2_t im = vceq_f32(a._imags, b._imags);
  return vand_u32(re, im);
}

inline boolx2_t operator !=(const complexpair& a, const complexpair& b) {
  boolx2_t re = vceq_f32(a._reals, b._reals);
  boolx2_t im = vceq_f32(a._imags, b._imags);
  boolx2_t vor = vorr_u32(re, im);
  return (boolx2_t){ !vor[0], !vor[1] };
}

inline float32x2_t norm(const complexpair& a) {
  float32x2_t re = vmul_f32(a._reals, a._reals);
  float32x2_t im = vmul_f32(a._imags, a._imags);
  return vadd_f32(re, im);
}
