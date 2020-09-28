#include "rtprecver.hpp"
#include <string>
#include <stdlib.h>
#include "pstoh264.hpp"
#include "H264ToImage.hpp"

#define BUFFERSIZE 1024 * 1024
#define SORT_MAX_PACKET_COUNT 200

RtpRecver::RtpRecver()
{
    m_brecvOver = false;
    m_frameBuffer = (char *)malloc(BUFFERSIZE);
    memset(m_frameBuffer, 0, BUFFERSIZE);
    m_lastSeqNumber = 0;
}

RtpRecver::~RtpRecver()
{
    this->destroy();
    if (m_frameBuffer)
    {
        free(m_frameBuffer);
    }
}

void checkerror(int rtperr)
{
    if (rtperr < 0)
    {
        std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
        exit(-1);
    }
}

void RtpRecver::init(std::string recvIp, unsigned short recvPort)
{
    mH264ToImage = new H264ToImage();
    mH264ToImage->init();

    RTPUDPv4TransmissionParams transparams;
    RTPSessionParams sessparams;
    uint16_t portbase = recvPort;
    uint32_t srcip;
    std::string ipsrc = recvIp;
    int status;
    srcip = inet_addr(ipsrc.c_str());
    srcip = ntohl(srcip);

    sessparams.SetOwnTimestampUnit(1.0 / 10.0);
    sessparams.SetAcceptOwnPackets(true);
    transparams.SetPortbase(portbase);
    transparams.SetBindIP(srcip);
    status = m_rtpSession.Create(sessparams, &transparams);
    transparams.SetRTPReceiveBuffer(transparams.GetRTPReceiveBuffer() * 100);
    checkerror(status);
}

void RtpRecver::destroy()
{
    m_brecvOver = true;
    m_rtpSession.BYEDestroy(RTPTime(10, 0), 0, 0);
}

void RtpRecver::run()
{
    m_rtpSession.BeginDataAccess();
    while (!m_brecvOver)
    {
        int status = m_rtpSession.Poll();
        checkerror(status);
        if (m_rtpSession.GotoFirstSourceWithData())
        {
            do
            {
                RTPPacket *pack;
                RTPSourceData *srcdat;
                srcdat = m_rtpSession.GetCurrentSourceInfo();
                while (!m_brecvOver && (pack = m_rtpSession.GetNextPacket()) != NULL)
                {
                    ProcessRTPPacket(*srcdat, *pack);
                    m_rtpSession.DeletePacket(pack);
                }
            } while (m_rtpSession.GotoNextSourceWithData());
        }
    }
    m_rtpSession.EndDataAccess();
}

void RtpRecver::ProcessRTPPacket(const RTPSourceData &srcdat, const RTPPacket &rtppack)
{
    // You can inspect the packet and the source's info here
    // std::cout << "Got new RTP packet" << rtppack.GetExtensionID() << std::endl;
    if ((rtppack.GetExtendedSequenceNumber() - m_lastSeqNumber) > 1)
    {
        std::cout << "Got packet error " << rtppack.GetExtendedSequenceNumber() << " xxx " << rtppack.GetExtendedSequenceNumber() - m_lastSeqNumber << "  time " << rtppack.GetTimestamp() << std::endl;
    }
    m_lastSeqNumber = rtppack.GetExtendedSequenceNumber();
    if (m_frameBuffer)
    {
        memcpy(m_frameBuffer + m_bufferSize, rtppack.GetPayloadData(), rtppack.GetPayloadLength());
        m_bufferSize += rtppack.GetPayloadLength();
        if (rtppack.HasMarker()) // ps frame
        {
            char *h264Buffer;
            int h264length = 0;
            int ret = PsToH264::GetH246FromPs(m_frameBuffer, m_bufferSize, &h264Buffer, &h264length);
            if (ret > 0)
            {
                    std::cout << "decode ps to h264 success" << std::endl;
                    std::string name = "./camera_"  +std::to_string(++imageNo) + "_img_";
                
                    mH264ToImage->save2Image(h264Buffer, h264length, const_cast<char *>(name.c_str()));
            }
            memset(m_frameBuffer, 0, BUFFERSIZE);
            m_bufferSize = 0;
        }
    }
}
