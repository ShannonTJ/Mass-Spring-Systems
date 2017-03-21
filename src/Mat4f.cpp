/**
 * Author:	Andrew Robert Owens
 * Email:	arowens [at] ucalgary.ca
 * Date:	January, 2017
 * Course:	CPSC 587/687 Fundamental of Computer Animation
 * Organization: University of Calgary
 *
 * Copyright (c) 2017 - Please give credit to the author.
 *
 * File: Mat4f.cpp
 */

#include "Mat4f.h"

// ====== CONSTRUCTORS (MOVE/COPY) / DESTRUCTORS ============================//
Mat4f::Mat4f() {
  Mat4fHandle tmp(new ARRAY_16f);
  m_ptr = std::move(tmp); // take ownership
}

Mat4f::Mat4f(float t) {
  Mat4fHandle tmp(new ARRAY_16f);
  tmp->fill(t);
  m_ptr = std::move(tmp); // take ownership
}

Mat4f::Mat4f(std::initializer_list<float> list) {
  assert(list.size() == NUM_ELEM);
  Mat4fHandle tmp(new ARRAY_16f);
  std::copy_n(list.begin(),  // source
              NUM_ELEM,      // number of copies
              tmp->begin()); // destination

  m_ptr = std::move(tmp); // take ownership
}

Mat4f::Mat4f(Mat4f &&moved) {
  m_ptr = std::move(moved.m_ptr); // take ownership
}

Mat4f::Mat4f(const Mat4f &copied) {
  Mat4fHandle tmp(new ARRAY_16f);
  std::copy_n(copied.begin(), // source
              NUM_ELEM,       // number of copies
              tmp->begin());  // destination

  m_ptr = std::move(tmp); // take ownership
}

Mat4f::~Mat4f() {}
// ==========================================================================//

// =========== OPERATORS ====================================================//

Mat4f &Mat4f::operator=(const Mat4f &copied) {
  if (this != &copied) {
    Mat4f tmp(copied);
    *this = std::move(tmp);
  }
  return *this;
}

Mat4f &Mat4f::operator=(Mat4f &&moved) {
  m_ptr = std::move(moved.m_ptr);
  return *this;
}

float &Mat4f::operator()(int row, int column) {
  assert(isValidDimIndex(row) && isValidDimIndex(column));
  return m_ptr->at(row * DIM + column);
}

float Mat4f::operator()(int row, int column) const {
  assert(isValidDimIndex(row) && isValidDimIndex(column));
  return m_ptr->at(row * DIM + column);
}

float &Mat4f::operator[](int element) {
  assert(isValidElementIndex(element));
  return m_ptr->at(element);
}

float Mat4f::operator[](int element) const {
  assert(isValidElementIndex(element));
  return m_ptr->at(element);
}

Mat4f Mat4f::operator+(Mat4f other) const {
  /* School Computers GCC doesn't support lambda funcs
  std::transform(	m_ptr->begin(),
                  m_ptr->end(),
                  other.m_ptr->begin(),
                  other.m_ptr->begin(),
                  []( float left, float right )
                  {
                          return left + right;
                  }
          );
  */

  std::transform(m_ptr->begin(), m_ptr->end(), other.m_ptr->begin(),
                 other.m_ptr->begin(), std::plus<float>());
  return other;
}

Mat4f Mat4f::operator*(const Mat4f &other) const {
  Mat4f result;

  float element;
  for (int i = 0; i < DIM; ++i) {
    for (int j = 0; j < DIM; ++j) {
      element = 0;
      for (int k = 0; k < DIM; ++k) {
        element += (*this)(i, k) * other(k, j);
      }
      result(i, j) = element;
    }
  }

  return result;
}

Mat4f Mat4f::operator*(float scalar) const {
  Mat4f result(*this);
  /*
  std::transform( result.m_ptr->begin(),
                  result.m_ptr->end(),
                  result.m_ptr->begin(),
                  [ &scalar ]( float f )
                  {
                          return f*scalar;
                  }
          );
  */

  for (auto iter = result.begin(); iter != result.end(); ++iter)
    (*iter) *= scalar;

  return result;
}

// 0	1	2	3
// 4	5	6	7
// 8	9	10	11
// 12	13	14	15

Mat4f Mat4f::transposed() const {
  Mat4f result;

  result[0] = (*this)[0];
  result[1] = (*this)[3];
  result[2] = (*this)[8];
  result[3] = (*this)[12];

  result[4] = (*this)[1];
  result[5] = (*this)[5];
  result[6] = (*this)[9];
  result[7] = (*this)[13];

  result[8] = (*this)[2];
  result[9] = (*this)[6];
  result[10] = (*this)[10];
  result[11] = (*this)[14];

  result[12] = (*this)[3];
  result[13] = (*this)[7];
  result[14] = (*this)[11];
  result[15] = (*this)[15];

  return result;
}

void Mat4f::fill(float t) { m_ptr->fill(t); }

// ==========================================================================//

float const *Mat4f::data() const { return m_ptr->data(); }

Mat4f::ARRAY_16f::iterator Mat4f::begin() { return m_ptr->begin(); }

Mat4f::ARRAY_16f::iterator Mat4f::end() { return m_ptr->end(); }

Mat4f::ARRAY_16f::const_iterator Mat4f::begin() const { return m_ptr->begin(); }

Mat4f::ARRAY_16f::const_iterator Mat4f::end() const { return m_ptr->end(); }

bool Mat4f::isValidDimIndex(int idx) const { return idx >= 0 && idx < DIM; }
bool Mat4f::isValidElementIndex(int idx) const {
  return idx >= 0 && idx < NUM_ELEM;
}

std::ostream &operator<<(std::ostream &out, const Mat4f &mat) {
  std::ostream_iterator<float> out_it(out, " ");
  std::copy(mat.begin(), mat.end(), out_it);
  return out;
}
