/*
 * libArkit_StreamParser.h
 * Copyright (C) 2019 transpalette <transpalette@translaptop>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef LIBARKIT_STREAMPARSER_H
#define LIBARKIT_STREAMPARSER_H

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

extern "C"
{
    #include <libavcodec/avcodec.h>
}

#include "libArkit_Frame.h"

#define INBUF_SIZE 4096

namespace ARKIT
{
    class StreamParser
    {
        private:
            AVCodecParserContext *parser;
            AVCodecContext *context { NULL };
            AVFrame *frame;
            AVPacket *pkt;
            std::ifstream file;
            uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
            uint8_t *data;

            uint8_t* GetRGBPixel(int x, int y);
            uint8_t GetGrayscalePixel(int x, int y);
            Frame* Decode();

        public:
            /* MPEG-1 codec */
            StreamParser(std::string file_path);
            ~StreamParser();
            Frame* NextFrame();
    };
}

#endif /* !LIBARKIT_STREAMPARSER_H */