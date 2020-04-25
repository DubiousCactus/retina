/*
 * Matrix.h
 * Copyright (C) 2019 transpalette <transpalette@translaptop>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef MATRIX_H
#define MATRIX_H

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>

namespace retina {

enum InterpolationMethod
{
  INTER_BILINEAR,
  INTER_BICUBIC,
  INTER_NEAREST,
  INTER_LANCZOS4,
  INTER_AREA
};

template<class T>
class Matrix
{
private:
  T** data;
  int rows{};
  int cols{};

public:
  template<size_t n_rows, size_t n_cols>
  explicit Matrix(T (&data)[n_rows][n_cols])
  {
    T** data_p = new T*[n_rows];
    for (size_t i = 0; i < n_rows; i++) {
      data_p[i] = data[i];
    }
    this->data = data_p;
    this->rows = n_rows;
    this->cols = n_cols;
  }
  Matrix(T** data, int rows, int cols);
  Matrix(int rows, int cols);
  Matrix(const Matrix& m);
  ~Matrix();
  Matrix& operator=(Matrix m);
  Matrix operator*(Matrix m);
  T* operator()(int m, int n) const;
  T* At(int m, int n) const;
  Matrix Transposed() const;
  void Transpose();
  void Print() const;
  int Rows() const;
  int Cols() const;
  /* Returns the central part of the convolution
     that is the same size as A.*/
  static Matrix Convolve(const Matrix& m, const Matrix& kernel);
  static Matrix Resize(const Matrix& m,
                       unsigned int mRows,
                       unsigned int mCols,
                       InterpolationMethod interpolation);
  static T
  Sum(const Matrix<T>& m, int row, int col, int windowSize);
  // Hadamard product of two matrices of the same size
  static Matrix ElementWiseProduct(const Matrix& m1, const Matrix& m2);
  static constexpr Matrix MakeGaussianKernel(int radius);
  static constexpr Matrix MakeGaussianKernel(int sigma, int size);
};

template<class T>
Matrix<T>::Matrix(T** data, int rows, int cols)
{
  this->data = data;
  this->rows = rows;
  this->cols = cols;
}

template<class T>
Matrix<T>::Matrix(int rows, int cols)
{
  this->data = new T*[rows];
  for (int i = 0; i < rows; ++i) {
    this->data[i] = new T[cols];
  }
  this->rows = rows;
  this->cols = cols;
}

template<class T>
Matrix<T>::Matrix(const Matrix<T>& m)
{
  this->rows = m.rows;
  this->cols = m.cols;
  this->data = new T*[rows];
  for (int i = 0; i < rows; ++i) {
    this->data[i] = new T[cols];
    for (int j = 0; j < cols; ++j) {
      this->data[i][j] = m.data[i][j];
    }
  }
}

template<class T>
Matrix<T>::~Matrix()
{
  free(this->data);
}

template<class T>
Matrix<T>&
Matrix<T>::operator=(Matrix<T> m)
{
  std::swap(rows, m.rows);
  std::swap(cols, m.cols);
  std::swap(data, m.data);
  return *this;
}

template<class T>
T*
Matrix<T>::At(const int m, const int n) const
{
  assert(m < this->rows);
  assert(n < this->cols);
  assert(m >= 0);
  assert(n >= 0);
  return &this->data[m][n];
}

template<class T>
T*
Matrix<T>::operator()(int m, int n) const
{
  assert(m < this->rows);
  assert(n < this->cols);
  assert(m >= 0);
  assert(n >= 0);
  return &this->data[m][n];
}

template<class T>
Matrix<T>
Matrix<T>::Convolve(const Matrix<T>& m, const Matrix<T>& kernel)
{
  assert(kernel.Rows() == kernel.Cols()); // Only work with square kernels
  assert((kernel.Rows() % 2) != 0);       // Only work with odd kernels
  int n2, m2;
  int conv;
  n2 = kernel.cols / 2;
  m2 = kernel.rows / 2;
  Matrix<T> c(m.Rows(), m.Cols());
  auto ZeroPaddedAccess = [](auto& mat, int i, int j) {
    return (i < 0 || i >= mat.Rows() || j < 0 || j >= mat.Cols()) ? 0
                                                                  : *mat(i, j);
  };
  for (int i = 0; i < m.Rows(); ++i) {
    for (int j = 0; j < m.Cols(); ++j) {
      conv = 0;
      for (int k = -n2; k <= n2; ++k) {
        for (int l = -m2; l <= m2; ++l) {
          conv += *kernel(k + n2, l + m2) * ZeroPaddedAccess(m, i - k, j - l);
        }
      }
      *c(i, j) = conv;
    }
  }
  return c;
}

template<class T>
constexpr Matrix<T>
Matrix<T>::MakeGaussianKernel(const int radius)
{
  assert((radius % 2) !=
         0); // Kernel must be odd in order to have a central element
  Matrix<T> kernel(2 * radius + 1, 2 * radius + 1);
  double sum = 0, sigma = radius / 2.0, x = 0;
  auto Gaussian = [](double x, double mu, double sigma) {
    return exp(-0.5 * pow((x - mu) / sigma, 2)) / (sigma * sqrt(2 * M_PI));
  };
  for (int i = 0; i < kernel.Rows(); ++i) {
    for (int j = 0; j < kernel.Cols(); ++j) {
      x = Gaussian(i, radius, sigma) * Gaussian(j, radius, sigma);
      *kernel(i, j) = x;
      sum += x;
    }
  }
  // Normalization
  for (int i = 0; i < kernel.Rows(); ++i) {
    for (int j = 0; j < kernel.Cols(); ++j) {
      *kernel(i, j) /= sum;
    }
  }
  return kernel;
}

template<class T>
constexpr Matrix<T>
Matrix<T>::MakeGaussianKernel(const int sigma, const int size)
{
  Matrix<T> kernel(size, size);
  int radius = size / 2;
  double r = 0, sum = 0, s = 2.0 * sigma * sigma;
  // assert((radius % 2) != 0); // Kernel must be odd in order to have a
  // central element

  for (int x = -radius; x <= radius; ++x) {
    for (int y = -radius; y <= radius; ++y) {
      r = sqrt(x * x + y * y);
      *kernel(x + radius, y + radius) = (exp(-(r * r) / s)) / (M_PI * s);
      sum += *kernel(x + radius, y + radius);
    }
  }

  // Normalization
  for (int i = 0; i < kernel.Rows(); ++i) {
    for (int j = 0; j < kernel.Cols(); ++j) {
      *kernel(i, j) /= sum;
    }
  }
  return kernel;
}

template<class T>
T
Matrix<T>::Sum(const Matrix<T>& m,
               const int row,
               const int col,
               const int windowSize)
{
  int sum = 0;
  int offset = windowSize / 2;
  assert((row - offset) >= 0 && (row + offset) < m.Rows());
  assert((col - offset) >= 0 && (col + offset) < m.Cols());
  for (int i = -offset; i <= offset; ++i) {
    for (int j = -offset; j <= offset; ++j) {
      sum += *m(row + i, col + j);
    }
  }
  return sum;
}

template<class T>
Matrix<T>
Matrix<T>::ElementWiseProduct(const Matrix<T>& m1, const Matrix<T>& m2)
{
  assert(m1.Rows() == m2.Rows());
  assert(m1.Cols() == m2.Cols());
  Matrix<T> mul(m1.Rows(), m2.Rows());
  for (int i = 0; i < m1.Rows(); ++i) {
    for (int j = 0; j < m1.Cols(); ++j) {
      *mul(i, j) = *m1(i, j) * *m2(i, j);
    }
  }
  return mul;
}

template<class T>
Matrix<T>
Matrix<T>::Resize(const Matrix<T>& m,
                  unsigned int mRows,
                  unsigned int mCols,
                  InterpolationMethod interpolation)
{
  // TODO: Move to Frame?
  Matrix<T> resized(mRows, mCols);
  float ystep, xstep, area;
  // TODO: Handle float values for xstep and ystep
  ystep = m.Cols() / mCols;
  xstep = m.Rows() / mRows;
  area = ystep * xstep;
  if (mRows < m.Rows() || mCols < m.Cols()) {
    if (interpolation == InterpolationMethod::INTER_AREA) {
      for (int j = 0; j < (resized.Rows() - ystep); j++) {
        for (int i = 0; i < (resized.Cols() - xstep); i++) {
          for (int k = 0; k < (ystep - 1); k++) {
            for (int l = 0; l < (xstep - 1); l++) {
              *resized(j, i) += *m(ystep * j + k, xstep * i + l);
            }
          }
          *resized(j, i) /= area;
        }
      }
    }
  } else {
    // TODO: Upscale
  }

  return resized;
}

template<class T>
Matrix<T> Matrix<T>::operator*(const Matrix<T> m)
{
  assert(this->cols == m.Rows());
  int sum;
  Matrix<T> mul(this->rows, m.Cols());
  for (int j = 0; j < this->rows; ++j) {
    for (int i = 0; i < m.Cols(); ++i) {
      sum = 0;
      for (int k = 0; k < m.Rows(); ++k) {
        sum += *this->At(j, k) * *m(k, i);
      }
      *mul(j, i) = sum;
    }
  }
  return mul;
}

template<class T>
Matrix<T>
Matrix<T>::Transposed() const
{
  Matrix<T> t(this->cols, this->rows);
  for (int i = 0; i < this->rows; ++i) {
    for (int j = 0; j < this->cols; ++j) {
      *t(j, i) = *this->At(i, j);
    }
  }
  return t;
}

template<class T>
void
Matrix<T>::Transpose()
{
  Matrix<T> t(this->cols, this->rows);
  for (int i = 0; i < this->rows; ++i) {
    for (int j = 0; j < this->cols; ++j) {
      *t(j, i) = *this->At(i, j);
    }
  }
  this = t;
}

template<class T>
int
Matrix<T>::Rows() const
{
  return this->rows;
}

template<class T>
int
Matrix<T>::Cols() const
{
  return this->cols;
}

template<class T>
void
Matrix<T>::Print() const
{
  for (int i = 0; i < this->rows; ++i) {
    for (int j = 0; j < this->cols; ++j) {
      std::cout << *this->At(i, j) << " ";
    }
    std::cout << std::endl;
  }
}
}

#endif /* !MATRIX_H */
