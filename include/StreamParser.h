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
#include <optional>
#include <memory>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include "Frame.h"

#define INBUF_SIZE 4096

namespace retina {
class StreamParser
{
  private:
    struct SwsContext *sws_ctx = nullptr;
    AVCodecContext* codecContext{ nullptr };
    AVFormatContext* fmtContext{ nullptr };
    AVCodec* codec { nullptr };
    AVFrame* frame { nullptr }, *rgbFrame { nullptr };
    uint8_t* buffer {nullptr};
    int videoStreamIdx { -1 };

    uint8_t* GetRGBPixel(int x, int y);
    uint8_t GetGrayscalePixel(int x, int y);

  public:
    explicit StreamParser(std::string file_path);
    ~StreamParser();
    std::optional<Frame> NextFrame();
};
}

#endif /* !STREAMPARSER_H */
