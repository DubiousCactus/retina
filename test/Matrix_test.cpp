/*
 * Matrix_test.cpp
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 */

#include <gtest/gtest.h>
#include "../include/Matrix.h"

TEST(Matrix, Creation) {
    arlib::Matrix<int> m(4, 6);
    ASSERT_EQ(m.Rows(), 4);
    ASSERT_EQ(m.Cols(), 6);

    float data[2][3] = {
        { 1.23, 5.22, 9.16 },
        { 7.88, 3.46, 8.54 }
    };
    arlib::Matrix<float> m2(data);
    ASSERT_EQ(m2.Rows(), 2);
    ASSERT_EQ(m2.Cols(), 3);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            ASSERT_EQ(*m2.At(i, j), data[i][j]);
        }
    }

    double **data2 = new double*[3];
    for (int i = 0; i < 3; i++) {
        data2[i] = new double[4];
        for (int j = 0; j < 4; j++) {
            data2[i][j] = 1.23456 + (i*j);
        }
    }
    arlib::Matrix<double> m3(data2, 3, 4);
    ASSERT_EQ(m3.Rows(), 3);
    ASSERT_EQ(m3.Cols(), 4);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            ASSERT_EQ(*m3.At(i, j), data2[i][j]);
        }
    }
}


TEST(Matrix, Transpose) {
    int data[2][3] = {
        { 1, 2, 3 },
        { 4, 5, 6}
    };
    int data_t[3][2] = {
        {1, 4},
        {2, 5},
        {3, 6}
    };
    arlib::Matrix<int> m(data);
    auto transpose = m.Transposed();
    EXPECT_EQ(transpose.Rows(), m.Cols());
    EXPECT_EQ(transpose.Cols(), m.Rows());
    for (int i = 0; i < transpose.Rows(); i++) {
        for (int j = 0; j < transpose.Cols(); j++) {
            ASSERT_EQ(*transpose.At(i, j), data_t[i][j]);
        }
    }

    long square[5][5] = {
        { 1, 2, 3, 4, 5 },
        { 0, 0, 0, 0, 0 },
        { 1, 2, 3, 4, 5 },
        { 7, 8, 9, 10, 11 },
        { -12, -83, -99, -65, -44 }
    };
    long square_t[5][5] = {
        { 1, 0, 1, 7, -12 },
        { 2, 0, 2, 8, -83 },
        { 3, 0, 3, 9, -99 },
        { 4, 0, 4, 10, -65 },
        { 5, 0, 5, 11, -44 }
    };
    arlib::Matrix<long> m2(square);
    auto t = m2.Transposed();
    EXPECT_EQ(t.Rows(), m2.Cols());
    EXPECT_EQ(t.Cols(), m2.Rows());
    for (int i = 0; i < t.Rows(); i++) {
        for (int j = 0; j < t.Cols(); j++) {
            ASSERT_EQ(*t.At(i, j), square_t[i][j]);
        }
    }
}

/*TEST(Matrix, Sum) {
    int data[4][3] = {
        { 1, 2, 3 },
        { 4, 5, 6},
        { 7, 8, 9},
        { 10, 11, 12}
    };
    ARKIT::Matrix<int> m(data);
    auto sum = ARKIT::Matrix<int>::Sum(m, 0, 0, 1, 1);
    EXPECT_EQ(sum, 12);
    sum = ARKIT::Matrix<int>::Sum(m, 0, 0, 1, 2);
    EXPECT_EQ(sum, 21);
    sum = ARKIT::Matrix<int>::Sum(m, 1, 1, 2, 1);
    EXPECT_EQ(sum, 13);
     //TODO: Expect exception: ARKIT::Matrix<int>::Sum(m, 3, 1, 2, 1);
}*/

TEST(Matrix, Mul) {
    int data[4][3] = {
        { 1, 2, 3 },
        { 4, 5, 6},
        { 7, 8, 9},
        { 10, 11, 12}
    };
    int op[2][3] = {
        {2, 2, 2},
        {1, 1, 1}
    };
    int res[4][2] = {
        {12, 6},
        {30, 15},
        {48, 24},
        {66, 33}
    };
    arlib::Matrix<int> m(data);
    arlib::Matrix<int> o(op);
    arlib::Matrix<int> r(res);
    auto mul = m * o.Transposed();
    EXPECT_EQ(mul.Rows(), r.Rows());
    EXPECT_EQ(mul.Cols(), r.Cols());
    for (int i = 0; i < r.Rows(); i++) {
        for (int j = 0; j < r.Cols(); j++) {
            ASSERT_EQ(*mul.At(i, j), *r.At(i, j));
        }
    }
     //TODO: Expect exception for bad dimensions
}


TEST(Matrix, Convolution) {
    int data[4][3] = {
        { 1, 2, 3 },
        { 4, 5, 6},
        { 7, 8, 9},
        { 10, 11, 12}
    };
    int kernel[3][3] = {
        {1, 2, 3},
        {3, 2, 1},
        {1, 3, 1}
    };
    int res[4][3] = {
        { 21, 42, 35 },
        { 50, 88, 70 },
        { 86, 139, 106 },
        { 82, 108, 70 }
    };
    arlib::Matrix<int> m(data);
    arlib::Matrix<int> k(kernel);
    arlib::Matrix<int> r(res);
    auto conv2d = arlib::Matrix<int>::Convolve(m, k);
    EXPECT_EQ(conv2d.Rows(), r.Rows());
    EXPECT_EQ(conv2d.Cols(), r.Cols());
    for (int i = 0; i < r.Rows(); i++) {
        for (int j = 0; j < r.Cols(); j++) {
            ASSERT_EQ(*conv2d.At(i, j), *r.At(i, j));
        }
    }
     //TODO: Expect exception for bad dimensions
}
