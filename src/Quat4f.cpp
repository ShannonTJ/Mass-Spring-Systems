#include "Quat4f.h"

#include <limits>

Quat4f slerp(Quat4f const &a, Quat4f const &b, float t) {
  //  float m0 = a.norm();
  //  float m1 = b.norm();
  //
  //  float m = (1.0 - t) * m0 + t * m1;
  //
  //  Quat4f p0 = a / m0;
  //  Quat4f p1 = b / m1;
  //
  //  float real = ((~p0) * p1).re();
  //  if (real > 1.) {
  //    real = 1.;
  //  } else if (real < -1.) {
  //    real = -1.;
  //  }
  //
  //  float theta = std::acos(real);
  //
  //  float denom = std::sin(theta);
  //  if (denom < std::numeric_limits<float>::epsilon()) {
  //    denom = std::numeric_limits<float>::epsilon();
  //  }
  //
  //  Quat4f p =
  //      (std::sin((1.0 - t) * theta) * p0 + std::sin(t * theta) * p1) / denom;
  //
  //  return m * p;

  float flip = 1;

  float cosine = a.re() * b.re() + a.im() * b.im();

  if (cosine < 0) {
    cosine = -cosine;
    flip = -1;
  }

  if ((1 - cosine) < std::numeric_limits<float>::epsilon())
    return a * (1 - t) + b * (t * flip);

  float theta = (float)acos(cosine);
  float sine = (float)sin(theta);
  float beta = (float)sin((1 - t) * theta) / sine;
  float alpha = (float)sin(t * theta) / sine * flip;

  return a * beta + b * alpha;
}

Vec3f rotateAround(Vec3f const &vec, Vec3f const &axis, float radians) {
  radians *= 0.5;
  const float sinAngle = std::sin(radians);
  const float cosAngle = std::cos(radians);

  Vec3f n(axis);
  n.normalize();

  Quat4f qAxis(cosAngle, n * sinAngle);

  Vec3f rotated = qAxis * vec;
  return rotated;
}

void rotateAround(Vec3f &vec, Vec3f const &axis, float radians) {
  radians *= 0.5;
  const float sinAngle = std::sin(radians);
  const float cosAngle = std::cos(radians);

  Vec3f n(axis);
  n.normalize();

  Quat4f qAxis(cosAngle, n * sinAngle);

  vec = qAxis * vec;
}

std::ostream &operator<<(std::ostream &out, Quat4f const &q) {
  return out << q.re() << " " << q.im();
}
