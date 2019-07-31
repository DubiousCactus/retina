/*
 * libArkit_StreamParser.cpp
 * Copyright (C) 2019 transpalette <transpalette@arch-cactus>
 *
 * Distributed under terms of the MIT license.
 */

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

#include <libavcodec/avcodec.h>
#include "libArkit_Frame.cpp"

#define INBUF_SIZE 4096

namespace ARKIT
{
    class StreamParser
    {
        private:
            AVCodecParserContext *parser; 
            AVCodecContext *context;
            AVFrame *frame;
            AVPacket *pkt;
            std::ifstream file;
            uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
            uint8_t *data;

            Frame* Decode()
            {
                int ret;

                ret = avcodec_send_packet(this->context, this->pkt);
                if (ret < 0) {
                    std::cerr << "Error sending a packet for decoding" << std::endl;
                    exit(1);
                }

                while (ret >= 0) {
                    ret = avcodec_receive_frame(this->context, this->frame);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                        return NULL;
                    else if (ret < 0) {
                        std::cerr << "Error during decoding" << std::endl;
                        exit(1);
                    }

                    Frame f(frame->data[0], frame->width, frame->height);

                    return &f;
                }
            }

        public:
            /* MPEG-1 codec */
            StreamParser(std::string file_path)
            {
                this->file = std::ifstream(file_path, std::ifstream::in | std::ifstream::binary);
                if (!file.is_open()) {
                    std::cerr << "Could not open " << file_path << std::endl;
                    exit(1);
                }

                this->pkt = av_packet_alloc();
                assert(this->pkt);

                /* set end of buffer to 0 (this ensures that no overreading
                 * happens for damaged MPEG streams)
                 */
                memset(this->inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);

                const AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_MPEG1VIDEO);
                assert(codec);
                this->parser = av_parser_init(codec->id);
                assert(this->parser);
                this->context = avcodec_alloc_context3(codec);
                assert(this->context);
                /* For some codecs, such as msmpeg4 and mpeg4, width and height
                 * MUST be initialized there because this information is not
                 * available in the bitstream.
                 */
                if (avcodec_open2(context, codec, NULL) < 0) {
                    std::cerr << "Could not open codec" << std::endl;
                    exit(1);
                }

                this->frame = av_frame_alloc();
                assert(frame);
            }

            ~StreamParser()
            {
                this->file.close();
                av_parser_close(this->parser);
                avcodec_free_context(&this->context);
                av_frame_free(&this->frame);
                av_packet_free(&this->pkt);
            }

            Frame* NextFrame()
            {
                Frame *f;
                size_t bytes_read;
                int ret;

                if (this->file.eof()) {
                    return NULL;
                }
                this->file.read((char*)(&this->inbuf[0]), INBUF_SIZE);

                f = NULL;
                /* Split the data into frames */
                this->data = this->inbuf;
                bytes_read = file.gcount();
                while (bytes_read > 0) {
                    ret = av_parser_parse2(this->parser, this->context,
                            &this->pkt->data, &this->pkt->size, data,
                            bytes_read, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
                    if (ret < 0) {
                        std::cerr << "Error while parsing" << std::endl;
                        exit(1);
                    }
                    data += ret;
                    bytes_read -= ret;

                    if (pkt->size)
                        f = this->Decode();
                }

                return f;
            }
    };
}
