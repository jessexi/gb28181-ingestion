#ifndef H264_2_IMAGE_HPP
#define H264_2_IMAGE_HPP

#include <string>
#include <iostream>
extern "C" {
  #include <libavcodec/avcodec.h>
}

#define INBUF_SIZE 4096

class H264ToImage
{
private:
    const AVCodec *codec;
    AVCodecParserContext *parser;
    AVCodecContext *c = NULL;
    AVPacket *pkt;
    uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    uint8_t *data;
    size_t data_size;
    AVFrame *frame;

public:
    H264ToImage(/* args */);
    static void decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt,
                   const char *filename);
    void init();
    void save2Image(char *h264Buffer, int h264length, char *filename);
    ~H264ToImage();
};

#endif