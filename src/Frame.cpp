/*
 * Frame.cpp
 * Copyright (C) 2019-2020 Théo Morales <theo.morales.fr@gmail.com>
 * Distributed under terms of the MIT license.
 */

#include "Frame.h"

namespace retina {
Pixel
Frame::PixelAt(unsigned int x, unsigned int y) const
{
    assert(x < this->Width() && y < this->Height());
    // TODO: Use a cache
    return Pixel(x, y, this->data[y][x]);
}

void
Frame::WriteAt(unsigned int x, unsigned int y, unsigned char val)
{
    assert(x < this->Width() && y < this->Height());
    this->data[y][x] = val;
}

//Matrix<int>
//Frame::GetIntMatrix() const
//{
//    Matrix<int> intMat(this->Height(), this->Width());
//    for (int i = 0; i < this->Height(); ++i) {
//        for (int j = 0; j < this->Width(); ++j) {
//            intMat.At(i, j) = (int)this(j, i);
//        }
//    }
//    return intMat;
//}
//
//Matrix<int>
//Frame::GetIntMatrix(int x, int y, int patch_size) const
//{
//    // TODO: Replace all assert() with exceptions!
//    int offset = patch_size / 2;
//    assert(y - offset >= 0);
//    assert(x - offset >= 0);
//    assert(y + offset < this->Height());
//    assert(x + offset < this->Width());
//    Matrix<int> intMat(patch_size, patch_size);
//    for (int i = -offset; i < offset; ++i) {
//        for (int j = -offset; j < offset; ++j) {
//            intMat.At(i + offset, j + offset) = (int)this->data[y + j][x + i];
//        }
//    }
//    return intMat;
//}

Matrix<double>
Frame::GetDoubleMatrix() const
{
    Matrix<double> doubleMat(this->Height(), this->Width());
    for (int i = 0; i < this->Height(); ++i) {
        for (int j = 0; j < this->Width(); ++j) {
            doubleMat.At(i, j) = (double)this->data[i][j];
        }
    }
    return doubleMat;
}

Matrix<double>
Frame::GetDoubleMatrix(int x, int y, int patch_size) const
{
    // TODO: Replace all assert() with exceptions!
    int offset = patch_size / 2;
    assert(y - offset >= 0);
    assert(x - offset >= 0);
    assert(y + offset < this->Height());
    assert(x + offset < this->Width());
    Matrix<double> doubleMat(patch_size, patch_size);
    for (int i = -offset; i < offset; ++i) {
        for (int j = -offset; j < offset; ++j) {
            doubleMat.At(i + offset, j + offset) =
              (double)this->data[y + j][x + i];
        }
    }
    return doubleMat;
}

Frame Frame::operator*(const Frame& f) const
{
    assert(f.Width() == this->Width());
    assert(f.Height() == this->Height());
    Frame mul(this->Width(), this->Height());
    for (int j = 0; j < this->Height(); ++j) {
        for (int i = 0; i < this->Width(); ++i) {
            for (int k = 0; k < mul.Height(); ++k) {
                mul.WriteAt(i, j, this->operator()(k, j) * f(i, k));
            }
        }
    }
    return mul;
}
}

