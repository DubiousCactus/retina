/*
 * libArkit_Frame.cpp
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 */

#include <map>
#include <cassert>
#include <cstdint>
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
            std::map<int, Pixel*> pixels_cache;
            unsigned int width;
            unsigned int height;

        public:
            Frame(uint8_t **data, int width, int height)
            {
                this->data = data;
                this->width = width;
                this->height = height;
            }

            Pixel* PixelAt(unsigned int x, unsigned int y)
            {
                assert (x < this->width && y < this->height);
                return new Pixel(x, y, this->data[y][x]);
            }

            uint8_t RawAt(unsigned int x, unsigned int y)
            {
                assert (x < this->width && y < this->height);
                return this->data[y][x];
            }

            void WriteAt(unsigned int x, unsigned int y, unsigned char val)
            {
                assert (x < this->width && y < this->height);
                this->data[y][x] = val;
            }

            int Width()
            {
                return this->width;
            }

            int Height()
            {
                return this->height;
            }
    };
}

