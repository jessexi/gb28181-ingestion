#ifndef RTPRECVER_H
#define RTPRECVER_H

#include <jrtplib3/rtpsession.h>
#include <jrtplib3/rtpudpv4transmitter.h>
#include <jrtplib3/rtpipv4address.h>
#include <jrtplib3/rtpsessionparams.h>
#include <jrtplib3/rtperrors.h>
#include <jrtplib3/rtplibraryversion.h>
#include <jrtplib3/rtpsourcedata.h>
#include <jrtplib3/rtpsources.h>
#include <jrtplib3/rtppacket.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "H264ToImage.hpp"

using namespace jrtplib;

class RtpRecver
{
public:
    explicit RtpRecver();
    ~RtpRecver();
    void init(std::string recvIp, unsigned short recvPort);
     void run();
    void destroy();

protected:
   
    void ProcessRTPPacket(const RTPSourceData &srcdat, const RTPPacket &rtppack);
    ;

private:
    RTPSession m_rtpSession;
    bool m_brecvOver;
    char *m_frameBuffer;
    int m_bufferSize;
    int imageNo = 0;
    int m_lastSeqNumber;
    H264ToImage *mH264ToImage;
    // void sigRecvedFrame(QByteArray);
};

#endif // RTPRECVER_H
