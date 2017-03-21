/**
 * Author:	Andrew Robert Owens
 * Email:	arowens [at] ucalgary.ca
 * Date:	January, 2017
 * Course:	CPSC 587/687 Fundamental of Computer Animation
 * Organization: University of Calgary
 *
 * Copyright (c) 2017 - Please give credit to the author.
 *
 * File:	Vec3f.cpp
 */

#include "Vec3f.h"

#include <cmath>
#include <iostream>

std::ostream &operator<<(std::ostream &out, Vec3f const &vec) {
  return out << vec.x() << " " << vec.y() << " " << vec.z();
}

std::istream &operator>>(std::istream &in, Vec3f &vec) {
  return in >> vec.x() >> vec.y() >> vec.z();
}

Vec3f Vec3f::radRotateAboutX(double radians) const {
  double s = std::sin(radians);
  double c = std::cos(radians);

  return Vec3f(m_x, (m_y * c) + (m_z * -s), (m_y * s) + (m_z * c));
}

Vec3f Vec3f::radRotateAboutY(double radians) const {
  double s = std::sin(radians);
  double c = std::cos(radians);

  return Vec3f((m_x * c) + (m_z * s), m_y, (m_x * -s) + (m_z * c));
}

Vec3f Vec3f::radRotateAboutZ(double radians) const {
  double s = std::sin(radians);
  double c = std::cos(radians);

  return Vec3f((m_x * c) + (m_y * -s), (m_x * s) + (m_y * c), m_z);
}
