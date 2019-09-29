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
	class Matrix
	{
		private:
			int **data;
			int rows;
			int cols;

		public:
			template <size_t n_rows, size_t n_cols>
			Matrix(int (&data)[n_rows][n_cols])
			{
				int **data_p = new int*[n_rows];
				for (size_t i = 0; i < n_rows; i++) {
					data_p[i] = data[i];
				}
				this->data = data_p;
				this->rows = n_rows;
				this->cols = n_cols;
			}

			Matrix(int **data, int rows, int cols);
			Matrix(int rows, int cols);
			~Matrix();
			int *At(int m, int n);
			int Rows();
			int Cols();
			static Matrix Convolve(Frame *f, Matrix &m);
			static int Sum(Matrix &m, int xmin, int ymin, int xmax, int ymax);
			Matrix Transpose();
			Matrix operator*(Matrix m);

	};
}


#endif /* !LIBARKIT_MATRIX_H */
