/*
 * libArkit_Matrix.h
 * Copyright (C) 2019 transpalette <transpalette@translaptop>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef LIBARKIT_MATRIX_H
#define LIBARKIT_MATRIX_H

#include <cstddef>
#include "libArkit_Frame.h"

namespace ARKIT
{
	template <class T>
	class Matrix
	{
		private:
			T **data;
			int rows;
			int cols;

		public:
			template <size_t n_rows, size_t n_cols>
			Matrix(T (&data)[n_rows][n_cols])
			{
				T **data_p = new T*[n_rows];
				for (size_t i = 0; i < n_rows; i++) {
					data_p[i] = data[i];
				}
				this->data = data_p;
				this->rows = n_rows;
				this->cols = n_cols;
			}

			Matrix(T **data, int rows, int cols);
			Matrix(int rows, int cols);
			~Matrix();
			T *At(int m, int n);
			int Rows();
			int Cols();
			static Matrix Convolve(Frame *f, Matrix<T> &m);
			static T Sum(Matrix<T> &m, int xmin, int ymin, int xmax, int ymax);
			Matrix Transpose();
			Matrix operator*(Matrix<T> m);

	};

	template <class T>
    Matrix<T>::Matrix(T **data, int rows, int cols)
    {
		this->data = data;
		this->rows = rows;
		this->cols = cols;
    }

	template <class T>
    Matrix<T>::Matrix(int rows, int cols)
    {
		this->data = new T*[rows];
		for (int i = 0; i < rows; ++i) {
			this->data[i] = new T[cols];
		}
		this->rows = rows;
		this->cols = cols;
    }

	template <class T>
    Matrix<T>::~Matrix()
    {
		free(this->data);
    }

	template <class T>
    T *Matrix<T>::At(int m, int n)
    {
		assert(m < this->rows);
		assert(n < this->cols);
		assert(m >= 0);
		assert(n >= 0);
		return &this->data[m][n];
    }

	template <class T>
    Matrix<T> Matrix<T>::Convolve(Frame *f, Matrix<T> &m)
    {
		int n2, m2;
		int conv;
		n2 = m.cols/2;
		m2 = m.rows/2;
		Matrix c(f->Width(), f->Height());
		for (int i = n2; i < f->Height() - n2; ++i) {
			for (int j = m2; j < f->Width() - m2; ++j) {
				conv = 0;
				for (int l = -n2; l <= n2; ++l) {
					for (int k = -m2; k <= m2; k++) {
						conv += *m.At(k+m2, l+n2) * (int) f->RawAt(j - l, i - k);
					}
				}
				*c.At(j - m2, i - n2) = conv;
			}
		}
		return c;
    }

	template <class T>
    T Matrix<T>::Sum(Matrix<T> &m, int xmin, int ymin, int xmax, int ymax)
    {
		int sum = 0;
		for (int i = ymin; i < ymax; ++i) {
			for (int j = xmin; j < xmax; ++j) {
				sum += *m.At(i, j);
			}
		}
		return sum;
    }

	template <class T>
    Matrix<T> Matrix<T>::operator*(Matrix<T> m)
    {
		assert(this->cols == m.Rows());
		int sum;
		Matrix mul(this->rows, m.Cols());
		for (int j = 0; j < this->rows; ++j) {
			for (int i = 0; i < m.Cols(); ++i) {
				sum = 0;
				for (int k = 0; k < this->cols; ++k) {
					sum += *this->At(j, k) + *m.At(k, j);
				}
				*mul.At(j, i) = sum;
			}
		}
		return mul;
    }

	template <class T>
    Matrix<T> Matrix<T>::Transpose()
    {
		Matrix t(this->cols, this->rows);
		for (int i = this->rows - 1; i >= 0; --i) {
			for (int j = 0; j < this->cols; ++j) {
				*t.At(j , this->rows - 1 - i) = *this->At(i, j);
			}
		}
		return t;
    }

	template <class T>
    int Matrix<T>::Rows()
    {
		return this->rows;
    }

	template <class T>
    int Matrix<T>::Cols()
    {
		return this->cols;
    }

}


#endif /* !LIBARKIT_MATRIX_H */
