#ifndef SIP_CLIENT_HPP
#define SIP_CLIENT_HPP

#include "Defines.hpp"
#include "rtprecver.hpp"
#include "ThreadPool.h"


class SipClient
{
private:
    TransportContext m_tsxContext;
    SIPClient m_sipClientparam;
    pjsip_dialog *m_invitedlg;
    pjsip_inv_session *m_invsession;

    pj_caching_pool m_caching_pool;
    pj_pool_t *m_sipPool;

    static pjsip_endpoint *m_sipEndpt;
    static ThreadPool *executor;
    static bool quit_flag;
    bool m_invInit = false;
    pj_thread_t *m_keepalivethread;
    pj_thread_t *m_eventloopthread;
    LocalSipType m_localSipType;
    RtpRecver *m_rtpRecver;

public:
    SipClient();
    pj_status_t initPJlib();
    pj_status_t initSipMoudle(std::string endpointName , unsigned short tsxPort);
    pj_status_t registerClient(SIPClient &cltparam);
    void registerClient2();
    //callback
    static pj_bool_t on_rx_request( pjsip_rx_data *rdata );
    static pj_bool_t on_rx_response( pjsip_rx_data *rdata );

    pj_status_t unregisterClient(SIPClient &cltparam);
    int initInvParam(TransportContext &tsxContext);
    void onVidoPlay();
    static void runRtpServer( RtpRecver *rtpRecver);
    void startEventLoop();
    static int keepAlive_thread(void *arg);
    void sendKeepAlive(std::string deviceid);
    void setClientParamContext();
    std::string createPlaySDP(std::string fromDeviceid, std::string mediaRecvIp, unsigned short mediaRecvPort);
    bool sendInvite(std::string deviceid, std::string mediaRecvIp, short mediaRecvPort);
    ~SipClient();

};




#endif // !1