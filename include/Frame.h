/*
 * Frame.h
 * Copyright (C) 2019 transpalette <transpalette@translaptop>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef FRAME_H
#define FRAME_H

#include "Matrix.h"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>

namespace retina {
/* TODO: Remove the Pixel structure, it's garbage */
struct Pixel
{
    int x{};
    int y{};
    unsigned char intensity;

    Pixel() {}

    Pixel(int x, int y)
    {
        this->x = x;
        this->y = y;
        this->intensity = 0;
    }

    Pixel(int x, int y, unsigned char i)
    {
        this->x = x;
        this->y = y;
        this->intensity = i;
    }

    Pixel(unsigned char intensity) { this->intensity = intensity; }

    bool operator()(const Pixel* p) const
    {
        return (p->x == this->x) && (p->y == this->y);
    }

    bool operator<(const Pixel& p) const
    {
        return this->intensity < p.intensity;
    }

    bool operator==(const Pixel& p) const
    {
        return this->x == p.x && this->y == p.y;
    }
};

class Frame: public Matrix<uint8_t>
{
  public:
    using Matrix<uint8_t>::Matrix;
    Frame(uint8_t** data, unsigned int width, unsigned int height) : Matrix<uint8_t>(data, height, width) { };
    Pixel PixelAt(unsigned int x, unsigned int y) const;
    void WriteAt(unsigned int x, unsigned int y, unsigned char val);
    unsigned int Width() const { return this->cols; };
    unsigned int Height() const { return this->rows; };
    Matrix<double> GetDoubleMatrix() const;
    Matrix<double> GetDoubleMatrix(int x, int y, int patch_size) const;
    Frame operator*(const Frame& f) const;
    uint8_t operator()(unsigned int x, unsigned int y) const { assert(y < this->rows); assert(x < this->cols); return this->data[y][x]; };
};
}

#endif /* !FRAME_H */
