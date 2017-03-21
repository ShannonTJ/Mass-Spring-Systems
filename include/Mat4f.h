/**
 * Author:	Andrew Robert Owens
 * Email:	arowens [at] ucalgary.ca
 * Date:	January, 2017
 * Course:	CPSC 587/687 Fundamental of Computer Animation
 * Organization: University of Calgary
 *
 * Copyright (c) 2017 - Please give credit to the author.
 *
 * File:	Mat4f.cpp
 */

#ifndef MAT4F_H
#define MAT4F_H

#include <assert.h>
#include <memory>
#include <initializer_list>
#include <array>
#include <functional>
#include <algorithm>
#include <iterator>
#include <iostream>

// Stores a 4 by 4 Matrix in Row Major order.
// When passing to glUniform4x4fv, turn on transpose.

class Mat4f {
public:
  enum { DIM = 4, NUM_ELEM = 16 };

  typedef std::array<float, NUM_ELEM> ARRAY_16f;
  typedef std::unique_ptr<ARRAY_16f> Mat4fHandle;

public:
  explicit Mat4f();
  explicit Mat4f(float f);

  // not explicit, so Mat4f m = {1,...,16};
  Mat4f(std::initializer_list<float> list);
  // Move constructor
  Mat4f(Mat4f &&moved);
  // Copy Constructor
  Mat4f(const Mat4f &copied);
  ~Mat4f();

  float &operator()(int row, int column);
  float &operator[](int element);
  float operator()(int row, int column) const;
  float operator[](int element) const;

  void fill(float t);

  Mat4f operator+(Mat4f other) const;
  Mat4f operator*(const Mat4f &other) const;
  Mat4f operator*(float scalar) const;

  Mat4f &operator=(const Mat4f &copied);
  Mat4f &operator=(Mat4f &&moved);

  bool isValidDimIndex(int idx) const;
  bool isValidElementIndex(int idx) const;

  Mat4f transposed() const;

  ARRAY_16f::iterator begin();
  ARRAY_16f::iterator end();
  ARRAY_16f::const_iterator begin() const;
  ARRAY_16f::const_iterator end() const;

  float const *data() const;

private:
  Mat4fHandle m_ptr;
};

std::ostream &operator<<(std::ostream &, const Mat4f &mat);

#endif // MAT4F_H
