/*
 * libArkit_Frame.cpp
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 */

#include <map>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <libavcodec/avcodec.h>


namespace ARKIT
{
    /* TODO: Remove the Pixel structure, it's garbage */
    struct Pixel {
        int x;
        int y;
        unsigned char intensity;

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

        Pixel(unsigned char intensity)
        {
            this->intensity = intensity;
        }

        bool operator() (const Pixel* p) const
        {
            return (p->x == this->x) && (p->y == this->y);
        }
    };

    class Frame
    {
        private:
            uint8_t **data;
            //std::map<int, Pixel*> pixels_cache;
            unsigned int width;
            unsigned int height;

        public:
            Frame(uint8_t **data, int width, int height)
            {
                this->data = data;
                this->width = width;
                this->height = height;
            }

            Frame(int width, int height)
            {
                this->data = new uint8_t*[height];
                for (int i = 0; i < height; ++i) {
                    this->data[i] = new uint8_t[width] {0};
                }
                this->width = width;
                this->height = height;
            }

            ~Frame()
            {
                //delete this->data;
            }

            Pixel* PixelAt(unsigned int x, unsigned int y)
            {
                assert (x < this->width && y < this->height);
                // TODO: Use cache
                return new Pixel(x, y, this->data[y][x]);
            }

            uint8_t RawAt(unsigned int x, unsigned int y) const
            {
                assert (x < this->width && y < this->height);
                return this->data[y][x];
            }

            void WriteAt(unsigned int x, unsigned int y, unsigned char val)
            {
                assert (x < this->width && y < this->height);
                this->data[y][x] = val;
            }

            int Width() const
            {
                return this->width;
            }

            int Height() const
            {
                return this->height;
            }

            template <typename T, size_t rows, size_t cols>
            Frame Convolve(T (&filter)[rows][cols])
            {
				int n2, m2;
				T conv;
				n2 = cols/2;
				m2 = rows/2;
                Frame f(this->width, this->height);
				for (int i = n2; i < this->height - n2; ++i) {
					for (int j = m2; j < this->width - m2; ++j) {
						conv = 0;
						for (int l = -n2; l <= n2; ++l) {
							for (int k = -m2; k <= m2; k++) {
								conv += filter[k][l] * f.RawAt(j - l, i - k);
							}
						}
						f.WriteAt(j - m2, i - n2, (uint8_t)conv);
					}
				}
				return f;
            }

            Frame operator*(const Frame f) const
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
    };
}

