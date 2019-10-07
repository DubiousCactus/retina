/*
 * libArkit_Frame.cpp
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 */

#include "libArkit_Frame.h"

namespace ARKIT
{
    Frame::Frame(uint8_t **data, int width, int height)
    {
        this->data = data;
        this->width = width;
        this->height = height;
    }

    Frame::Frame(int width, int height)
    {
        this->data = new uint8_t*[height];
        for (int i = 0; i < height; ++i) {
            this->data[i] = new uint8_t[width] {0};
        }
        this->width = width;
        this->height = height;
    }

    Frame::~Frame()
    {
        //delete this->data;
    }

    Pixel* Frame::PixelAt(unsigned int x, unsigned int y)
    {
        assert (x < this->width && y < this->height);
        // TODO: Use cache
        return new Pixel(x, y, this->data[y][x]);
    }

    uint8_t Frame::RawAt(unsigned int x, unsigned int y) const
    {
        assert (x < this->width && y < this->height);
        return this->data[y][x];
    }

    void Frame::WriteAt(unsigned int x, unsigned int y, unsigned char val)
    {
        assert (x < this->width && y < this->height);
        this->data[y][x] = val;
    }

    Matrix<int> Frame::GetMatrix()
    {
        Matrix<int> intMat(this->height, this->width);
        for (int i = 0; i < this->height; ++i) {
            for (int j = 0; j < this->width; ++j) {
                *intMat.At(i, j) = (int)this->RawAt(j, i);
            }
        }
        return intMat;
    }

    int Frame::Width() const
    {
        return this->width;
    }

    int Frame::Height() const
    {
        return this->height;
    }

    Frame Frame::operator*(const Frame f) const
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

