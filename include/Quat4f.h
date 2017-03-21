#ifndef QUAT4F_H
#define QUAT4F_H

#include <ostream>
#include <cmath>
#include "Vec3f.h"
#include "Mat4f.h"

class Quat4f {
public:
  Quat4f(float real = 0, float iVal = 0, float jVal = 0, float kVal = 0);

  Quat4f(float re, Vec3f const &im = Vec3f());
  Quat4f(Vec3f const &im);

  Quat4f const &operator=(float real);
  Quat4f const &operator=(Vec3f const &vec);

  float &operator[](int index);
  float const &operator[](int index) const;

  float &re();
  float const &re() const;
  Vec3f &im();
  Vec3f const &im() const;

  Quat4f operator+(Quat4f const &q) const;
  Quat4f operator-(Quat4f const &q) const;
  Quat4f operator-(void) const;
  Quat4f operator*(float scalar) const;
  Quat4f operator/(float scalar) const;

  Quat4f operator*(Quat4f const &q) const;
  Vec3f operator*(Vec3f const &v) const;
  void operator*=(Quat4f const &q);
  Quat4f operator~() const;
  Quat4f inv() const;

  void operator+=(Quat4f const &q);
  void operator+=(float scalar);
  void operator-=(Quat4f const &q);
  void operator-=(float scalar);
  void operator*=(float scalar);
  void operator/=(float scalar);

  float norm() const;
  float normSquared() const;
  Quat4f normalized() const;
  void normalize();

  Mat4f matrix4f() const;

private:
  float m_re;
  Vec3f m_im;
};

Quat4f operator*(float scalar, Quat4f const &q);

std::ostream &operator<<(std::ostream &out, Quat4f const &q);

Quat4f slerp(Quat4f const &q0, Quat4f const &q1, float t);
Vec3f rotateAround(Vec3f const &vec, Vec3f const &axis, float radians);
void rotateAround(Vec3f &vec, Vec3f const &axis, float radians);

inline Quat4f::Quat4f(float re, float iV, float jV, float kV)
    : m_re(re), m_im(iV, jV, kV) {}

inline Quat4f::Quat4f(float re, Vec3f const &im) : m_re(re), m_im(im) {}

inline Quat4f::Quat4f(Vec3f const &im) : m_re(0), m_im(im) {}

inline Quat4f const &Quat4f::operator=(float real) {
  m_re = real;
  m_im = Vec3f();
  return *this;
}

inline Quat4f const &Quat4f::operator=(Vec3f const &imag) {
  m_re = 0.0f;
  m_im = imag;
  return *this;
}

inline float &Quat4f::operator[](int index) { return (&m_re)[index]; }

inline float const &Quat4f::operator[](int index) const {
  return (&m_re)[index];
}

inline float &Quat4f::re() { return m_re; }

inline float const &Quat4f::re() const { return m_re; }

inline Vec3f &Quat4f::im() { return m_im; }

inline Vec3f const &Quat4f::im() const { return m_im; }

inline Quat4f Quat4f::operator+(Quat4f const &rhs) const {
  return Quat4f(m_re + rhs.m_re, m_im + rhs.m_im);
}

inline Quat4f Quat4f::operator-(Quat4f const &rhs) const {
  return Quat4f(m_re - rhs.m_re, m_im - rhs.m_im);
}

inline Quat4f Quat4f::operator-() const { return Quat4f(-m_re, -m_im); }

inline Quat4f Quat4f::operator*(float scalar) const {
  return Quat4f(scalar * m_re, scalar * m_im);
}

inline Quat4f Quat4f::operator/(float scalar) const {
  return Quat4f(m_re / scalar, m_im / scalar);
}

inline void Quat4f::operator+=(Quat4f const &rhs) {
  m_re += rhs.m_re;
  m_im += rhs.m_im;
}

inline void Quat4f::operator+=(float scalar) { m_re += scalar; }

inline void Quat4f::operator-=(Quat4f const &rhs) {
  m_re -= rhs.m_re;
  m_im -= rhs.m_im;
}

inline void Quat4f::operator-=(float scalar) { m_re -= scalar; }

inline void Quat4f::operator*=(float scalar) {
  m_re *= scalar;
  m_im *= scalar;
}

inline void Quat4f::operator/=(float scalar) {
  m_re /= scalar;
  m_im /= scalar;
}

inline Quat4f Quat4f::operator*(Quat4f const &q) const {
  double const &s1(m_re);
  double const &s2(q.m_re);
  Vec3f const &v1(m_im);
  Vec3f const &v2(q.m_im);

  return Quat4f(s1 * s2 - v1 * v2, s1 * v2 + s2 * v1 + (v1 ^ v2));
}

inline Vec3f Quat4f::operator*(Vec3f const &v) const {
  Quat4f qV(v);

  Quat4f result = qV * ~(*this);
  result = (*this) * result;

  return result.im();
}

inline Mat4f Quat4f::matrix4f() const {
  float x = m_im.x();
  float y = m_im.y();
  float z = m_im.z();
  float w = m_re;

  float wx = w * x;
  float wy = w * y;
  float wz = w * z;

  float xx = x * x;
  float xy = x * y;
  float xz = x * z;

  float yy = y * y;
  float yz = y * z;

  float zz = z * z;

  Mat4f result = {1.f - 2.f * (yy + zz), 2.f * (xy - wz), 2.f * (xz + wy), 0,
                  2.f * (xy + wz), 1.f - 2.f * (xx + zz), 2.f * (yz - wx), 0,
                  2.f * (xz - wy), 2.f * (yz + wx), 1.f - 2.f * (xx + yy), 0,
                  0, 0, 0, 1};

  return result;
}

inline void Quat4f::operator*=(Quat4f const &q) { *this = (*this * q); }

inline float Quat4f::norm() const { return std::sqrt(normSquared()); }

inline float Quat4f::normSquared() const { return m_re * m_re + m_im * m_im; }

inline Quat4f Quat4f::normalized() const { return *this / norm(); }

inline void Quat4f::normalize() { *this /= norm(); }

inline Quat4f Quat4f::operator~() const { return Quat4f(m_re, -m_im); }

inline Quat4f Quat4f::inv() const { return (~(*this)) / this->normSquared(); }

inline Quat4f operator*(float scalar, Quat4f const &q) { return q * scalar; }

#endif // QUAT4F_H
