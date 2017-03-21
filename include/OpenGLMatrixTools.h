/**
 * Author:	Andrew Robert Owens
 * Email:	arowens [at] ucalgary.ca
 * Date:	January, 2017
 * Course:	CPSC 587/687 Fundamental of Computer Animation
 * Organization: University of Calgary
 *
 * Copyright (c) 2017 - Please give credit to the author.
 *
 * File:	OpenGLMatrixTools.h
 */

#ifndef OPENGL_MAT_TOOLS_H
#define OPENGL_MAT_TOOLS_H

//#define _USE_MATH_DEFINES
#include <cmath>

#include "Mat4f.h"
#include "Vec3f.h"

Mat4f IdentityMatrix();

Mat4f UniformScaleMatrix(float scale);
Mat4f ScaleMatrix(float x, float y, float z);
Mat4f ScaleMatrix(Vec3f const &scale);
Mat4f TranslateMatrix(float x, float y, float z);
Mat4f TranslateMatrix(Vec3f const &pos);
Mat4f RotateAboutXMatrix(float angleDeg);
Mat4f RotateAboutYMatrix(float angleDeg);
Mat4f RotateAboutZMatrix(float angleDeg);

Mat4f OrthographicProjection(float left, float right, float top, float bottom,
                             float far, float near);

Mat4f PerspectiveProjection(float fov, float aspectRatio, float zNear,
                            float zFar);

Mat4f LookAtMatrix(const Vec3f &pos, const Vec3f &target, const Vec3f &up);

#endif // OPENGL_MAT_TOOLS_H
