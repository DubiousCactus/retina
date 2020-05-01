/*
 * Frame.cpp
 * Copyright (C) 2019-2020 Théo Morales <theo.morales.fr@gmail.com>
 * Distributed under terms of the MIT license.
 */

#include "Frame.h"

namespace retina {
Frame::Frame(uint8_t** data, int width, int height)
{
    this->data = std::vector<std::vector<uint8_t>>(height, std::vector<uint8_t>(width));
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            this->data[i][j] = data[i][j];
        }
    }
    this->width = width;
    this->height = height;
}

Frame::Frame(int width, int height)
{
    this->data = std::vector<std::vector<uint8_t>>(height, std::vector<uint8_t>(width, 0));
    this->width = width;
    this->height = height;
}

Pixel
Frame::PixelAt(unsigned int x, unsigned int y) const
{
    assert(x < this->width && y < this->height);
    // TODO: Use cache
    return Pixel(x, y, this->data[y][x]);
}

uint8_t
Frame::RawAt(unsigned int x, unsigned int y) const
{
    assert(x < this->width && y < this->height);
    return this->data[y][x];
}

void
Frame::WriteAt(unsigned int x, unsigned int y, unsigned char val)
{
    assert(x < this->width && y < this->height);
    this->data[y][x] = val;
}

Matrix<int>
Frame::GetIntMatrix() const
{
    Matrix<int> intMat(this->height, this->width);
    for (int i = 0; i < this->height; ++i) {
        for (int j = 0; j < this->width; ++j) {
            intMat.At(i, j) = (int)this->RawAt(j, i);
        }
    }
    return intMat;
}

Matrix<int>
Frame::GetIntMatrix(int x, int y, int patch_size) const
{
    // TODO: Replace all assert() with exceptions!
    int offset = patch_size / 2;
    assert(y - offset >= 0);
    assert(x - offset >= 0);
    assert(y + offset < this->height);
    assert(x + offset < this->width);
    Matrix<int> intMat(patch_size, patch_size);
    for (int i = -offset; i < offset; ++i) {
        for (int j = -offset; j < offset; ++j) {
            intMat.At(i + offset, j + offset) = (int)this->data[y + j][x + i];
        }
    }
    return intMat;
}

Matrix<double>
Frame::GetDoubleMatrix() const
{
    Matrix<double> doubleMat(this->height, this->width);
    for (int i = 0; i < this->height; ++i) {
        for (int j = 0; j < this->width; ++j) {
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
    assert(y + offset < this->height);
    assert(x + offset < this->width);
    Matrix<double> doubleMat(patch_size, patch_size);
    for (int i = -offset; i < offset; ++i) {
        for (int j = -offset; j < offset; ++j) {
            doubleMat.At(i + offset, j + offset) =
              (double)this->data[y + j][x + i];
        }
    }
    return doubleMat;
}

unsigned int
Frame::Width() const
{
    return this->width;
}

unsigned int
Frame::Height() const
{
    return this->height;
}

Frame Frame::operator*(const Frame& f) const
{
    assert(f.Width() == this->width);
    assert(f.Height() == this->height);
    Frame mul(this->width, this->height);
    for (int j = 0; j < this->height; ++j) {
        for (int i = 0; i < this->width; ++i) {
            for (int k = 0; k < mul.Height(); ++k) {
                mul.WriteAt(i, j, this->RawAt(k, j) * f.RawAt(i, k));
            }
        }
    }
    return mul;
}
}

