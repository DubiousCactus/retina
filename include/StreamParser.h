/*
 * StreamParser.h
 * Copyright (C) 2019 transpalette <transpalette@translaptop>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef STREAMPARSER_H
#define STREAMPARSER_H

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

extern "C"
{
#include <libavcodec/avcodec.h>
}

#include "Frame.h"

#define INBUF_SIZE 4096

namespace arlite {
class StreamParser
{
  private:
    AVCodecParserContext* parser;
    AVCodecContext* context{ NULL };
    AVFrame* frame;
    AVPacket* pkt;
    std::ifstream file;
    uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    uint8_t* data;

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

#endif /* !STREAMPARSER_H */
