#ifndef H264_2_IMAGE_HPP
#define H264_2_IMAGE_HPP

class H264ToImage
{
private:
    /* data */
public:
    H264ToImage(/* args */);

    void init();
    void save2Image(char* h264Buffer, int h264length, char *filename);
    ~H264ToImage();
};



#endif