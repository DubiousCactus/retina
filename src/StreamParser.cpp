/*
 * StreamParser.cpp
 * Copyright (C) 2019-2020 Théo Morales <theo.morales.fr@gmail.com>
 * Distributed under terms of the MIT license.
 */

#include "StreamParser.h"

namespace retina {
uint8_t*
StreamParser::GetRGBPixel(int x, int y)
{
    auto pixel = new uint8_t[3];
    pixel[0] = *(this->rgbFrame->data[0] + y * this->rgbFrame->linesize[0] + (x*3));
    pixel[1] = *(this->rgbFrame->data[0] + y * this->rgbFrame->linesize[0] + (x*3) + 1);
    pixel[2] = *(this->rgbFrame->data[0] + y * this->rgbFrame->linesize[0] + (x*3) + 2);
    return pixel;
}

uint8_t
StreamParser::GetGrayscalePixel(int x, int y)
{
    auto rgb = this->GetRGBPixel(x, y);
    uint8_t gray_pixel = 0.33 * rgb[0] + 0.33 * rgb[1] + 0.33 * rgb[2];
    delete[] rgb;
    return gray_pixel;
}


StreamParser::StreamParser(std::string file_path)
{
    // Check the file header and store it in the format context
    if (avformat_open_input(&this->fmtContext, file_path.c_str(), nullptr, nullptr) != 0) {
        std::cerr << "[!] Could not open the file " << file_path << " !" << std::endl;
        exit(1);
    }

    // Check the stream information
    if (avformat_find_stream_info(this->fmtContext, nullptr) != 0) {
        std::cerr << "[!] Could not read stream info !" << std::endl;
        exit(1);
    }

    av_dump_format(this->fmtContext, 0, file_path.c_str(), 0);
    for (int i = 0; i < this->fmtContext->nb_streams; ++i) {
        if (this->fmtContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            this->videoStreamIdx = i;
            break;
        }
    }

    if (this->videoStreamIdx == -1) {
        std::cerr << "[!] Could not find a video stream!" << std::endl;
        exit(1);
    }

    AVCodecContext *codecContextOrg = this->fmtContext->streams[this->videoStreamIdx]->codec;

    // Find the decoder for the video stream
    this->codec = avcodec_find_decoder(codecContextOrg->codec_id);
    if (this->codec == nullptr) {
        std::cerr << "[!] Could not find a decoder for the codec!" << std::endl;
        exit(1);
    }

    // Copy the context, because we must not use the codec from the stream directly
    this->codecContext = avcodec_alloc_context3(this->codec);
    if (avcodec_copy_context(this->codecContext, codecContextOrg) != 0) {
        std::cerr << "[!] Could not copy the codec context!" << std::endl;
        exit(1);
    }

    if (avcodec_open2(this->codecContext, this->codec, nullptr) < 0) {
        std::cerr << "[!] Could not open the codec!" << std::endl;
        exit(1);
    }

    this->frame = av_frame_alloc();
    this->rgbFrame = av_frame_alloc();
    if (this->frame == nullptr || this->rgbFrame == nullptr) {
        std::cerr << "[!] Could not allocate frames!" << std::endl;
        exit(1);
    }

    int nBytes = avpicture_get_size(AV_PIX_FMT_RGB24, this->codecContext->width, this->codecContext->height);
    this->buffer = (uint8_t *) av_malloc(nBytes * sizeof(uint8_t));

    // Assign appropriate parts of buffer to image planes in rgbFrame, which is an AVFrame,
    // a superset of AVPicture.
    avpicture_fill((AVPicture *)this->rgbFrame, this->buffer, AV_PIX_FMT_RGB24,
            this->codecContext->width, this->codecContext->height);

    this->sws_ctx = sws_getContext(this->codecContext->width,
                             this->codecContext->height,
                             this->codecContext->pix_fmt,
                             this->codecContext->width,
                             this->codecContext->height,
                             AV_PIX_FMT_RGB24,
                             SWS_BILINEAR, nullptr, nullptr, nullptr);
}

StreamParser::~StreamParser()
{
    av_frame_free(&this->frame);
    av_frame_free(&this->rgbFrame);
    sws_freeContext(this->sws_ctx);
    av_freep(&this->buffer);
    avcodec_free_context(&this->codecContext);
    avformat_close_input(&this->fmtContext);
    avformat_free_context(this->fmtContext);
}

std::optional<Frame>
StreamParser::NextFrame()
{
    std::optional<Frame> f = std::nullopt;
    AVPacket packet;
    int frameFinished = 0;
    if (av_read_frame(this->fmtContext, &packet) < 0) {
        av_packet_unref(&packet);
        return std::nullopt;
    }

    while (!frameFinished) {
        while (packet.stream_index != this->videoStreamIdx) {
            if (av_read_frame(this->fmtContext, &packet) < 0) {
                av_packet_unref(&packet);
                return std::nullopt;
            }
        }
        avcodec_decode_video2(this->codecContext, this->frame, &frameFinished, &packet);
        if (frameFinished) {
            // Convert the frame from its original format to an RGB frame
            sws_scale(this->sws_ctx, (uint8_t const * const *) this->frame->data,
                      this->frame->linesize, 0, this->codecContext->height,
                      this->rgbFrame->data, this->rgbFrame->linesize);
            auto buff = new uint8_t*[this->codecContext->height];
            for (int i = 0; i < this->codecContext->height; i++) {
                buff[i] = new uint8_t[this->codecContext->width];
                for (int j = 0; j < this->codecContext->width; j++) {
                    buff[i][j] = this->GetGrayscalePixel(j, i);
                }
            }
            f = Frame(buff, this->codecContext->width, this->codecContext->height);
            /* Clean up the buffer */
            for (int i = 0; i < this->codecContext->height; ++i)
                delete[] buff[i];
            delete[] buff;
        }
    }
    av_packet_unref(&packet);
    return f;
}
}
