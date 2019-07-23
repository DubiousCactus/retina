/*
 * libArkit_Frame.cpp
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 */

#include <cassert>
#include <cstdint>
#include <libavcodec/avcodec.h>

#define INBUF_SIZE 4096

namespace ARKIT
{
    class Frame
    {
        private:
            uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
            AVPacket *pkt;

        public:
            Frame(uint8_t* data)
            {
                this->pkt = av_packet_alloc();
                assert(pkt);
                /* set end of buffer to 0 (this ensures that no overreading
                 * happens for damaged MPEG streams)
                 */
                memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);

            }
    };
}

