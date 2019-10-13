/*
 * libArkit_Matrix.h
 * Copyright (C) 2019 transpalette <transpalette@translaptop>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef LIBARKIT_MATRIX_H
#define LIBARKIT_MATRIX_H

#include <cmath>
#include <cstddef>

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
		    // TODO: Operator () instead of At()
		    T *At(int m, int n);
		    int Rows();
		    int Cols();
			/* Returns the central part of the convolution
                that is the same size as A.*/
			//static Matrix Convolve(Frame *f, Matrix<T> &m);
		    static Matrix Convolve(Matrix<T> &m, Matrix<T> &kernel);
		    static T Sum(Matrix<T> &m, int row, int col, int windowSize);
			// Hadamard product of two matrices of the same size
			static Matrix<T> ElementWiseProduct(Matrix<T> &m1, Matrix<T> &m2);
			static Matrix<T> MakeGaussianKernel(int radius);
		    Matrix Transpose();
		    Matrix operator*(Matrix<T> m);
		    void Print();
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
/*
	template <class T>
    Matrix<T> Matrix<T>::Convolve(Frame *f, Matrix<T> &m)
    {
		assert(m.Rows() == m.Cols()); // Only work with square ms
		assert((m.Rows() % 2) != 0); // Only work with odd ms
		int n2, m2;
		int conv;
		n2 = m.cols/2;
		m2 = m.rows/2;
		Matrix c(f->Height(), f->Width());
		auto ZeroPaddedAccess = [](auto *frame, int i, int j) {
			return (i < 0 || i >= frame->Width() || j < 0 || j >= frame->Height()) ? 0
				: (int)frame->RawAt(i, j);
		};
		for (int i = 0; i < f->Height(); ++i) {
			for (int j = 0; j < f->Width(); ++j) {
				conv = 0;
				for (int k = -n2; k <= n2; ++k) {
					for (int l = -m2; l <= m2; ++l) {
						conv += *m.At(k+n2, l+m2) * ZeroPaddedAccess(f, j-l, i-k);
					}
				}
				*c.At(i, j) = conv;
			}
		}
		[>for (int i = n2; i < m.Rows() - n2; ++i) {
			for (int j = m2; j < m.Cols() - m2; ++j) {
				conv = 0;
				for (int l = -n2; l <= n2; ++l) {
					for (int k = -m2; k <= m2; k++) {
						conv += *kernel.At(k+m2, l+n2) * *m.At( i - k, j - l);
					}
				}
				std::cout << conv << std::endl;
				*c.At(j - m2, i - n2) = conv;
			}
		}<]
		return c;
[>
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
		return c;<]
    }*/

	template <class T>
    Matrix<T> Matrix<T>::Convolve(Matrix<T> &m, Matrix<T> &kernel)
    {
		assert(kernel.Rows() == kernel.Cols()); // Only work with square kernels
		assert((kernel.Rows() % 2) != 0); // Only work with odd kernels
		int n2, m2;
		int conv;
		n2 = kernel.cols/2;
		m2 = kernel.rows/2;
		Matrix<T> c(m.Rows(), m.Cols());
		auto ZeroPaddedAccess = [](auto &mat, int i, int j) {
			return (i < 0 || i >= mat.Rows() || j < 0 || j >= mat.Cols()) ? 0
				: *mat.At(i, j);
		};
		for (int i = 0; i < m.Rows(); ++i) {
			for (int j = 0; j < m.Cols(); ++j) {
				conv = 0;
				for (int k = -n2; k <= n2; ++k) {
					for (int l = -m2; l <= m2; ++l) {
						conv += *kernel.At(k+n2, l+m2) * ZeroPaddedAccess(m, i-k, j-l);
					}
				}
				*c.At(i, j) = conv;
			}
		}
		/*for (int i = n2; i < m.Rows() - n2; ++i) {
			for (int j = m2; j < m.Cols() - m2; ++j) {
				conv = 0;
				for (int l = -n2; l <= n2; ++l) {
					for (int k = -m2; k <= m2; k++) {
						conv += *kernel.At(k+m2, l+n2) * *m.At( i - k, j - l);
					}
				}
				std::cout << conv << std::endl;
				*c.At(j - m2, i - n2) = conv;
			}
		}*/
		return c;
    }

    template <class T>
    Matrix<T> Matrix<T>::MakeGaussianKernel(int radius)
	{
		assert((radius % 2) != 0); // Kernel must be odd in order to have a central element
		Matrix<T> kernel(2*radius+1, 2*radius+1);
		double sum = 0, sigma = radius/2.0, x;
		auto Gaussian = [](double x, double mu, double sigma) {
			return exp(-0.5 * pow((x - mu) / sigma, 2));
		};
		for (int i = 0; i < kernel.Rows(); ++i) {
			for (int j = 0; j < kernel.Cols(); ++j) {
				x =  Gaussian(i, radius, sigma) *
					Gaussian(j, radius, sigma);
				*kernel.At(i, j) = x;
				sum += x;
			}
		}
		// Normalization
		for (int i = 0; i < kernel.Rows(); ++i) {
			for (int j = 0; j < kernel.Cols(); ++j) {
				*kernel.At(i, j) /= sum;
			}
		}
		return kernel;
	}
/*
	template <class T>
    T Matrix<T>::Sum(Matrix<T> &m, int xmin, int ymin, int xmax, int ymax)
    {
		int sum = 0;
		auto ZeroPaddedAccess = [](auto &mat, int i, int j) {
			return (i < 0 || i >= mat.Rows() || j < 0 || j >= mat.Cols()) ? 0
				: *mat.At(i, j);
		};
		for (int i = xmin; i <= xmax; ++i) {
			for (int j = ymin; j <= ymax; ++j) {
				sum += ZeroPaddedAccess(m, j, i);
			}
		}
		return sum;
    }*/

	template <class T>
    T Matrix<T>::Sum(Matrix<T> &m, int row, int col, int windowSize)
    {
		int sum = 0;
		int offset = windowSize/2;
		for (int i = -offset; i <= offset; ++i) {
			for (int j = -offset; j <= offset; ++j) {
				sum += *m.At(row+i, col+j);
			}
		}
		return sum;
    }

    template <class T>
    Matrix<T> Matrix<T>::ElementWiseProduct(Matrix<T> &m1, Matrix<T> &m2)
	{
		assert(m1.Rows() == m2.Rows());
		assert(m1.Cols() == m2.Cols());
		Matrix<T> mul(m1.Rows(), m2.Rows());
		for (int i = 0; i < m1.Rows(); ++i) {
			for (int j = 0; j < m1.Cols(); ++j) {
				*mul.At(i, j) = *m1.At(i, j) * *m2.At(i, j);
			}
		}
		return mul;
	}

	template <class T>
    Matrix<T> Matrix<T>::operator*(Matrix<T> m)
    {
		assert(this->cols == m.Rows());
		int sum;
		Matrix<T> mul(this->rows, m.Cols());
		for (int j = 0; j < this->rows; ++j) {
			for (int i = 0; i < m.Cols(); ++i) {
				sum = 0;
				for (int k = 0; k < m.Rows(); ++k) {
					sum += *this->At(j, k) * *m.At(k, i);
				}
				*mul.At(j, i) = sum;
			}
		}
		return mul;
    }

	/*template <class T>
	*T Matrix<T>::operator()(int row, int col)
	{
		return this->At(row, col);
	}*/

	template <class T>
    Matrix<T> Matrix<T>::Transpose()
    {
		Matrix<T> t(this->cols, this->rows);
		for (int i = 0; i < this->rows; ++i) {
			for (int j = 0; j < this->cols; ++j) {
				*t.At(j, i) = *this->At(i, j);
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

    template <class T>
	void Matrix<T>::Print()
	{
		for (int i = 0; i < this->rows; ++i) {
			for (int j = 0; j < this->cols; ++j) {
				std::cout << *this->At(i, j) << " ";
			}
			std::cout << std::endl;
		}
	}
}


#endif /* !LIBARKIT_MATRIX_H */
