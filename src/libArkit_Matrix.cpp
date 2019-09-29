/*
 * libArkit_Matrix.cpp
 * Copyright (C) 2019 transpalette <transpalette@translaptop>
 *
 * Distributed under terms of the MIint license.
 */


#include "libArkit_Matrix.h"

namespace ARKIT
{
    Matrix::Matrix(int **data, int rows, int cols)
    {
	this->data = data;
	this->rows = rows;
	this->cols = cols;
    }

    Matrix::Matrix(int rows, int cols)
    {
	this->data = new int*[rows];
	for (int i = 0; i < rows; ++i) {
	    this->data[i] = new int[cols];
	}
	this->rows = rows;
	this->cols = cols;
    }

    Matrix::~Matrix()
    {
	free(this->data);
    }

    int *Matrix::At(int m, int n)
    {
	assert(m < this->rows);
	assert(n < this->cols);
	assert(m >= 0);
	assert(n >= 0);
	return &this->data[m][n];
    }

    Matrix Matrix::Convolve(Frame *f, Matrix &m)
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

    int Matrix::Sum(Matrix &m, int xmin, int ymin, int xmax, int ymax)
    {
	int sum = 0;
	for (int i = ymin; i < ymax; ++i) {
	    for (int j = xmin; j < xmax; ++j) {
		sum += *m.At(i, j);
	    }
	}
	return sum;
    }

    Matrix Matrix::operator*(Matrix m)
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

    Matrix Matrix::Transpose()
    {
	Matrix t(this->cols, this->rows);
	for (int i = this->rows - 1; i >= 0; --i) {
	    for (int j = 0; j < this->cols; ++j) {
		*t.At(j , this->rows - 1 - i) = *this->At(i, j);
	    }
	}
	return t;
    }

    int Matrix::Rows()
    {
	return this->rows;
    }

    int Matrix::Cols()
    {
	return this->cols;
    }
}
