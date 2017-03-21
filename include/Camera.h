/**
 * Author:	Andrew Robert Owens
 * Email:	arowens [at] ucalgary.ca
 * Date:	January, 2017
 * Course:	CPSC 587/687 Fundamental of Computer Animation
 * Organization: University of Calgary
 *
 * Copyright (c) 2017 - Please give credit to the author.
 *
 * File:	Camera.h
 */

#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>

#include "Vec3f.h"
#include "Mat4f.h"
#include "Quat4f.h"
#include "OpenGLMatrixTools.h"

class Camera {
public:
  explicit Camera(Vec3f const &pos = Vec3f(), Vec3f const &forward = Vec3f(),
                  Vec3f const &up = Vec3f());

  void rotateAroundFocus(float deltaX, float deltaY);
  void rotateLeftRight(float t);
  void rotateUpDown(float t);
  void rotateRoll(float t);

  Mat4f lookatMatrix() const;

  void move(Vec3f const &offset);

  float focusDistance() const;
  Vec3f const &position() const;
  Vec3f const &forward() const;
  Vec3f const &up() const;
  Vec3f right() const;

private:
  float m_focusDist;
  Vec3f m_pos;
  Vec3f m_up;
  Vec3f m_forward;
};

// INLINE DEFINITIONS //

inline Camera::Camera(Vec3f const &pos, Vec3f const &forward, Vec3f const &up)
    : m_focusDist(forward.length()), m_pos(pos), m_up(up), m_forward(forward) {}

inline void Camera::rotateAroundFocus(float deltaX, float deltaY) {
  Vec3f focus = m_pos + m_forward * m_focusDist;
  Vec3f diff = m_pos - focus;

  rotateAround(diff, m_up, -deltaX);
  m_forward = -(diff.normalized());

  Vec3f bi = m_up ^ m_forward;
  bi.normalize();

  rotateAround(diff, bi, deltaY);
  rotateAround(m_up, bi, deltaY);
  m_forward = -(diff.normalized());

  m_pos = focus + diff;
}

inline void Camera::rotateUpDown(float t) {
  Vec3f bi = m_up ^ m_forward;
  rotateAround(m_forward, bi, t);

  m_forward.normalize();
  m_up = m_forward ^ bi;
  m_up.normalize();
}

inline void Camera::rotateLeftRight(float t) {
  rotateAround(m_forward, m_up, t);

  m_forward.normalize();
}

inline void Camera::rotateRoll(float t) {
  rotateAround(m_up, m_forward, t);

  m_up.normalize();
}

inline Mat4f Camera::lookatMatrix() const {

  return LookAtMatrix(m_pos, m_pos + m_forward * m_focusDist, m_up);
}

inline void Camera::move(Vec3f const &offset) {
  Vec3f bi = m_up ^ m_forward;
  bi.normalize();

  m_pos += bi * offset.x() + m_up * offset.y() + m_forward * offset.z();
}

inline float Camera::focusDistance() const { return m_focusDist; }
inline Vec3f const &Camera::position() const { return m_pos; }
inline Vec3f const &Camera::forward() const { return m_forward; }
inline Vec3f const &Camera::up() const { return m_up; }
inline Vec3f Camera::right() const { return (m_up ^ m_forward).normalized(); }
#endif /* defined(____Camera__) */
