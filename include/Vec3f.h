/**
 * Author:	Andrew Robert Owens
 * Email:	arowens [at] ucalgary.ca
 * Date:	January, 2017
 * Course:	CPSC 587/687 Fundamental of Computer Animation
 * Organization: University of Calgary
 *
 * Copyright (c) 2017 - Please give credit to the author.
 *
 * File:	Vec3f.h
 */

#ifndef VEC3F_H
#define VEC3F_H

#include <iostream>  // std::{cout, endl, etc.}
#include <cmath>     // std::{sqrt, abs, etc.}
#include <algorithm> // std::swap

class Vec3f {
public:
  static float distance(Vec3f const &a, Vec3f const &b);

public:
  //  no explicit ... danger danger
  explicit Vec3f(float x = 0.f, float y = 0.f, float z = 0.f);
  Vec3f(Vec3f const &other) = default;

  // Getter/Setter
  float x() const;
  float &x();
  void x(float x);
  float y() const;
  float &y();
  void y(float y);
  float z() const;
  float &z();
  void z(float z);

  void set(float x, float y, float z);
  void zero();
  bool hasNans() const;
  bool hasInfs() const;

  float &operator[](int idx);
  float operator[](int idx) const;

  // Usefull Member Functions
  static Vec3f abs(Vec3f const &);
  Vec3f normalized() const;
  void normalize();
  float length() const;
  float lengthSquared() const;
  float distance(Vec3f const &other) const;
  float dotProduct(Vec3f const &other) const;
  Vec3f crossProduct(Vec3f const &other) const;
  Vec3f projectOnto(Vec3f const &other) const;

  // Usefull Member Operators

  Vec3f operator^(const Vec3f &other) const; // cross product
  float operator*(const Vec3f &other) const; // dot product

  Vec3f operator-() const;
  Vec3f operator*(float factor) const;
  Vec3f operator/(float factor) const;
  Vec3f operator+(const Vec3f &other) const;
  Vec3f operator-(const Vec3f &other) const;
  Vec3f &operator=(Vec3f other); // assignment operator
  void operator+=(const Vec3f &other);
  void operator-=(const Vec3f &other);
  void operator*=(float factor);
  void operator/=(float factor);

  bool operator==(Vec3f const &other) const;

  Vec3f radRotateAboutZ(double radians) const;
  Vec3f radRotateAboutY(double radians) const;
  Vec3f radRotateAboutX(double radians) const;

  Vec3f componentwiseMult(Vec3f const &rhs) const;

  float *data();
  float const *data() const;

  friend Vec3f operator*(float scalar, Vec3f const &vec);
  friend void swap(Vec3f &l, Vec3f &r);

  static Vec3f lerp(float t, Vec3f const &a, Vec3f const &b);
  static Vec3f slerp(float t, Vec3f const &a, Vec3f const &b);

private:
  union {
    struct {
      float m_x, m_y, m_z;
    };
    float m_coord[3];
  };
};

std::ostream &operator<<(std::ostream &out, Vec3f const &vec);
std::istream &operator>>(std::istream &in, Vec3f &vec);

inline float Vec3f::distance(Vec3f const &a, Vec3f const &b) {
  return a.distance(b);
}

inline Vec3f::Vec3f(float x, float y, float z) : m_x(x), m_y(y), m_z(z) {}

// Functions
inline Vec3f abs(const Vec3f &v) {
  Vec3f out(std::abs(v.x()), std::abs(v.y()), std::abs(v.z()));
  return out;
}

inline bool Vec3f::operator==(Vec3f const &other) const {
  return (m_x == other.m_x && m_y == other.m_y && m_z == other.m_z);
}

inline float Vec3f::length() const {
  return std::sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
}

inline float Vec3f::lengthSquared() const {
  return m_x * m_x + m_y * m_y + m_z * m_z;
}

inline float Vec3f::distance(Vec3f const &other) const {
  Vec3f tmp = *this - other; // blah... ha.
  return tmp.length();
}

inline float Vec3f::dotProduct(Vec3f const &other) const {
  return m_x * other.x() + m_y * other.y() + m_z * other.z();
}

inline Vec3f Vec3f::crossProduct(Vec3f const &other) const {
  return Vec3f((m_y * other.m_z) - (m_z * other.m_y),
               (m_z * other.m_x) - (m_x * other.m_z),
               (m_x * other.m_y) - (m_y * other.m_x));
}

inline Vec3f Vec3f::normalized() const {
  Vec3f v(*this);
  v.normalize();
  return v;
}

inline void Vec3f::normalize() {
  float len = length();
  // Check if zero?
  m_x /= len;
  m_y /= len;
  m_z /= len;
}

inline Vec3f Vec3f::componentwiseMult(Vec3f const &rhs) const {
  return Vec3f(m_x * rhs.m_x, m_y * rhs.m_y, m_z * rhs.m_z);
}

inline Vec3f Vec3f::projectOnto(Vec3f const &other) const {
  float scaleRatio = dotProduct(other) / lengthSquared();
  return other * scaleRatio;
}

// Operators
inline float Vec3f::operator*(Vec3f const &other) const {
  return dotProduct(other);
}

inline Vec3f Vec3f::operator^(Vec3f const &other) const {
  return crossProduct(other);
}

inline Vec3f &Vec3f::operator=(Vec3f other) {
  swap(other, *this);
  return *this;
}

inline Vec3f Vec3f::operator-() const {
  Vec3f v(*this);
  v *= -1.f;
  return v;
}

inline Vec3f Vec3f::operator+(Vec3f const &other) const {
  return Vec3f(m_x + other.x(), m_y + other.y(), m_z + other.z());
}

inline Vec3f Vec3f::operator-(Vec3f const &other) const {
  return Vec3f(m_x - other.x(), m_y - other.y(), m_z - other.z());
}

inline Vec3f Vec3f::operator*(float factor) const {
  return Vec3f(m_x * factor, m_y * factor, m_z * factor);
}

inline Vec3f Vec3f::operator/(float factor) const {
  return Vec3f(m_x / factor, m_y / factor, m_z / factor);
}

inline Vec3f operator*(float factor, Vec3f const &vec) { return vec * factor; }

inline void Vec3f::operator+=(const Vec3f &other) {
  m_x += other.m_x;
  m_y += other.m_y;
  m_z += other.m_z;
}

inline void Vec3f::operator-=(const Vec3f &other) {
  m_x -= other.m_x;
  m_y -= other.m_y;
  m_z -= other.m_z;
}

inline void Vec3f::operator*=(float factor) {
  m_x *= factor;
  m_y *= factor;
  m_z *= factor;
}

inline void Vec3f::operator/=(float factor) {
  m_x /= factor;
  m_y /= factor;
  m_z /= factor;
}

inline void swap(Vec3f &l, Vec3f &r) {
  std::swap(l.m_x, r.m_x);
  std::swap(l.m_y, r.m_y);
  std::swap(l.m_z, r.m_z);
}

// Getter/Setter Junk
inline float &Vec3f::operator[](int idx) { return m_coord[idx]; }

inline float Vec3f::operator[](int idx) const { return m_coord[idx]; }

inline void Vec3f::set(float x, float y, float z) {
  m_x = x;
  m_y = y;
  m_z = z;
}

inline float Vec3f::x() const { return m_x; }

inline float &Vec3f::x() { return m_x; }

inline void Vec3f::x(float x) { m_x = x; }

inline float Vec3f::y() const { return m_y; }

inline float &Vec3f::y() { return m_y; }

inline void Vec3f::y(float y) { m_y = y; }

inline float Vec3f::z() const { return m_z; }

inline float &Vec3f::z() { return m_z; }

inline void Vec3f::z(float z) { m_z = z; }

inline float *Vec3f::data() { return &(m_coord[0]); }

inline float const *Vec3f::data() const { return &(m_coord[0]); }

inline void Vec3f::zero() {
  m_x = 0.f;
  m_y = 0.f;
  m_z = 0.f;
}

// Static functions
inline Vec3f Vec3f::lerp(float t, Vec3f const &a, Vec3f const &b) {
  return (1.f - t) * a + t * b;
}

inline Vec3f Vec3f::slerp(float t, Vec3f const &a, Vec3f const &b) {
  using std::acos;
  using std::sin;

  // TODO make more efficient
  float omega = (a * b) / (a.length() * b.length());
  omega = acos(omega);
  float sinOmega = sin(omega);

  return (sin(omega - omega * t) / sinOmega) * a +
         (sin(omega * t) / sinOmega) * b;
}

inline bool Vec3f::hasNans() const {
  return std::isnan(m_x) || std::isnan(m_y) || std::isnan(m_y);
}

inline bool Vec3f::hasInfs() const {
  return std::isinf(m_x) || std::isinf(m_y) || std::isinf(m_z);
}

#endif // Vec3f
