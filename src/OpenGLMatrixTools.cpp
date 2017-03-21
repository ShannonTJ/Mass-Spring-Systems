/**
 * Author:	Andrew Robert Owens
 * Email:	arowens [at] ucalgary.ca
 * Date:	January, 2017
 * Course:	CPSC 587/687 Fundamental of Computer Animation
 * Organization: University of Calgary
 *
 * Copyright (c) 2017 - Please give credit to the author.
 *
 * File: OpenGLMatrixTools.cpp
 */

#include "OpenGLMatrixTools.h"

#include <cmath>

Mat4f IdentityMatrix() { return UniformScaleMatrix(1.0); }

Mat4f UniformScaleMatrix(float scale) {
  Mat4f uniform = {scale, 0, 0, 0, 0, scale, 0, 0, 0, 0, scale, 0, 0, 0, 0, 1};

  return uniform;
}

Mat4f ScaleMatrix(float x, float y, float z) {
  Mat4f scale = {x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1};

  return scale;
}

Mat4f ScaleMatrix(Vec3f const &s) {
  Mat4f scale = {s.x(), 0, 0, 0, 0, s.y(), 0, 0, 0, 0, s.z(), 0, 0, 0, 0, 1};

  return scale;
}

Mat4f TranslateMatrix(float x, float y, float z) {
  Mat4f trans = {1, 0, 0, x, 0, 1, 0, y, 0, 0, 1, z, 0, 0, 0, 1};

  return trans;
}

Mat4f TranslateMatrix(Vec3f const &pos) {
  Mat4f trans = {1, 0, 0, pos.x(), 0, 1, 0, pos.y(),
                 0, 0, 1, pos.z(), 0, 0, 0, 1};

  return trans;
}

Mat4f RotateAboutXMatrix(float angleDeg) {
  float angleRad = angleDeg * (M_PI / 180.0);

  float c = std::cos(angleRad);
  float s = std::sin(angleRad);

  Mat4f rot = {1, 0, 0, 0, 0, c, -s, 0, 0, s, c, 0, 0, 0, 0, 1};

  return rot;
}

Mat4f RotateAboutYMatrix(float angleDeg) {
  float angleRad = angleDeg * (M_PI / 180.0);

  float c = std::cos(angleRad);
  float s = std::sin(angleRad);

  Mat4f rot = {c, 0, s, 0, 0, 1, 0, 0, -s, 0, c, 0, 0, 0, 0, 1};

  return rot;
}

Mat4f RotateAboutZMatrix(float angleDeg) {
  float angleRad = angleDeg * (M_PI / 180.0);

  float c = std::cos(angleRad);
  float s = std::sin(angleRad);

  Mat4f rot = {c, -s, 0, 0, s, c, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

  return rot;
}

Mat4f OrthographicProjection(float left, float right, float bottom, float top,
                             float near, float far) {
  float xDistort = 2.0 / (right - left);
  float yDistort = 2.0 / (top - bottom);
  float zDistort = -2.0 / (far - near);
  float xShift = -(right + left) / (right - left);
  float yShift = -(top + bottom) / (top - bottom);
  float zShift = -(far + near) / (far - near);

  Mat4f ortho = {xDistort, 0, 0,        xShift, 0, yDistort, 0, yShift,
                 0,        0, zDistort, zShift, 0, 0,        0, 1};
  return ortho;
}

Mat4f PerspectiveProjection(float fov, float aspectRatio, float zNear,
                            float zFar) {
  float range = std::tan(fov * (M_PI / 360.0)) * zNear;
  //	float left = -range * aspectRatio;
  float right = range * aspectRatio;
  //	float bottom = -range;
  float top = range;

  //	float a00 = 2.0*zNear/(right - left);
  //	float a11 = 2.0*zNear/(top - bottom);
  //	float a22 = -(zFar + zNear)/(zFar - zNear);
  //	float a32 = -2.0*zFar*zNear/(zFar - zNear);
  //
  //	Mat4f persp = {
  //			a00,	0, 	0, 	0,
  //			0, 	a11, 	0, 	0,
  //			0, 	0, 	a22,	a32,
  //			0, 	0, 	-1, 	0
  //		};
  float a00 = zNear / (right);
  float a11 = zNear / (top);
  float a22 = -(zFar + zNear) / (zFar - zNear);
  float a32 = -2.0 * zFar * zNear / (zFar - zNear);

  Mat4f persp = {a00, 0, 0, 0, 0, a11, 0, 0, 0, 0, a22, a32, 0, 0, -1, 0};

  return persp;
}

Mat4f LookAtMatrix(const Vec3f &pos, const Vec3f &target, const Vec3f &up) {
  Vec3f f = pos - target; // inverted for R-handed CS
  f.normalize();
  Vec3f u = up.normalized();
  Vec3f r = u.crossProduct(f).normalized();
  u = f.crossProduct(r).normalized();

  Mat4f view = {r.x(), r.y(), r.z(), -r * pos, u.x(), u.y(), u.z(), -u * pos,
                f.x(), f.y(), f.z(), -f * pos, 0,     0,     0,     1};
  //	Mat4f view = {
  //		r.x(),	u.x(),	f.x(), -r*pos,
  //		r.y(),	u.y(),	f.y(), -u*pos,
  //		r.z(),	u.z(),	f.z(), -f*pos,
  //			0,		0,		0,		1
  //	};
  //	Mat4f view = {	r.x(),	r.y(),	r.z(),	-pos.x(),
  //        u.x(),	u.y(),	u.z(),	-pos.y(),
  //        f.x(),	f.y(),	f.z(),	-pos.z(),
  //        0,	0,	0,	1 };
  return view;
}
