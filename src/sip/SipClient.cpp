#include "SipClient.hpp"
#include "ThreadPool.h"
#include "RequestBuilder.hpp"

bool SipClient::quit_flag = false;
pjsip_endpoint *SipClient::m_sipEndpt = NULL;
ThreadPool *SipClient::executor = new ThreadPool(2);

static pjsip_module clientSipMoudle =
    {
        NULL, NULL,                     /* prev, next.		*/
        {"gb28181-client", 12},         /* Name.			*/
        -1,                             /* Id			*/
        PJSIP_MOD_PRIORITY_APPLICATION, /* Priority			*/
        NULL,                           /* load()			*/
        NULL,                           /* start()			*/
        NULL,                           /* stop()			*/
        NULL,                           /* unload()			*/
        &SipClient::on_rx_request,      /* on_rx_request()		*/
        &SipClient::on_rx_response,     /* on_rx_response()		*/
        NULL,                           /* on_tx_request.		*/
        NULL,                           /* on_tx_response()		*/
        NULL,                           /* on_tsx_state()		*/
};

static void call_on_state_changed(pjsip_inv_session *inv, pjsip_event *e)
{
    std::cout << "*************  call_on_state_changed ......" << inv->state << std::endl;
}

static void call_on_forked(pjsip_inv_session *inv, pjsip_event *e)
{
    std::cout << "************* call_on_forked ......" << std::endl;
}

static void call_on_media_update(pjsip_inv_session *inv_ses,
                                 pj_status_t status)
{
    std::cout << "************* call_on_media_update ......" << std::endl;
}

static void call_on_send_ack(pjsip_inv_session *inv, pjsip_rx_data *rdata)
{
    std::cout << "************* call_on_send_ack ......" << std::endl;
    pj_status_t status;
    pjsip_tx_data *tdata;
    status = pjsip_inv_create_ack(inv, rdata->msg_info.cseq->cseq, &tdata);
    pj_assert(status == PJ_SUCCESS);
    status = pjsip_inv_send_msg(inv, tdata);
    pj_assert(status == PJ_SUCCESS);
}

/* regc callback */
static void register_cb(struct pjsip_regc_cbparam *param)
{
    std::cout<< "*********** register call back" << std::endl;

    pjsip_regc_info info;
    pj_status_t status;
    status = pjsip_regc_get_info(param->regc, &info);
    pj_assert(status == PJ_SUCCESS);
    SipClient *client = (SipClient *)param->token;
};

pj_status_t SipClient::initPJlib()
{
    pj_status_t status;
    /* Must init PJLIB first: */
    status = pj_init();
    PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);
    /* Then init PJLIB-UTIL: */
    status = pjlib_util_init();
    PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);
    /* Must create a pool factory before we can allocate any memory. */
    pj_caching_pool_init(&m_caching_pool, &pj_pool_factory_default_policy, 0);
    /*set log level*/
    pj_log_set_level(5);
    return status;
};

pj_status_t SipClient::initSipMoudle(std::string endptName, unsigned short tsxPort)
{
    pj_status_t status;
    /* Create the endpoint: */
    status = pjsip_endpt_create(&m_caching_pool.factory, endptName.c_str(), &m_sipEndpt);
    PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);
    m_sipPool = pjsip_endpt_create_pool(m_sipEndpt, "clientpool", 4000, 4000);
    /*Add UDP transport, with hard-coded port*/
    pj_sockaddr_in addr;
    addr.sin_family = pj_AF_INET();
    addr.sin_addr.s_addr = 0;
    addr.sin_port = pj_htons(tsxPort);
    status = pjsip_udp_transport_start(m_sipEndpt, &addr, NULL, 1, NULL);
    PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);

    status = pjsip_tsx_layer_init_module(m_sipEndpt);
    PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);

    status = pjsip_ua_init_module(m_sipEndpt, NULL);
    PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);

    /* Initialize 100rel support */
    status = pjsip_100rel_init_module(m_sipEndpt);
    PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);
    status = pjsip_endpt_register_module(m_sipEndpt, &clientSipMoudle);
    return status;
};

void SipClient::registerClient2()
{
    RequestBuilder *builder = new RequestBuilder();
    builder->clientRegister();
}

pj_status_t SipClient::registerClient(SIPClient &cltparam)
{
    pj_status_t status;
    pjsip_regc *regc;

    char local[64] = {0};
    char dst[64] = {0};
    status = pjsip_regc_create(m_sipEndpt, this, &register_cb, &regc);
    PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);

    pj_ansi_sprintf(local, "<sip:%s@%s:%d>", cltparam.localDeviceID.c_str(), cltparam.localAddress.c_str(), cltparam.localSipPort);

    pj_ansi_sprintf(dst, "<sip:%s@%s:%d>", cltparam.sipserverID.c_str(), cltparam.sipserverAddress.c_str(), cltparam.sipserverPort);
    pj_str_t contacts[] = {
        {local, static_cast<pj_ssize_t>(strlen(local))}};
    pj_str_t localstr = pj_str(local);
    pj_str_t dststr = pj_str(dst);
    status = pjsip_regc_init(regc, &dststr, &localstr, &localstr, 1, contacts, cltparam.regValidSeconds);
    PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);

    pjsip_cred_info cred;
    pj_bzero(&cred, sizeof(cred));
    cred.realm = pj_str("*");
    cred.scheme = pj_str("digest");
    char username[64] = {0};
    pj_ansi_snprintf(username, 64, "%s", cltparam.localDeviceID.c_str());
    cred.username = pj_str(username);
    cred.data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
    char passwd[64] = {0};
    pj_ansi_snprintf(passwd, 64, "%s", cltparam.localPasswd.c_str());
    cred.data = pj_str(passwd);
    status = pjsip_regc_set_credentials(regc, 1, &cred);

    /* Register */
    pjsip_tx_data *tdata;
    status = pjsip_regc_register(regc, PJ_TRUE, &tdata);
    status = pjsip_regc_send(regc, tdata);
    PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);
};

std::string SipClient::createPlaySDP(std::string fromDeviceid,
                                     std::string mediaRecvIp, unsigned short mediaRecvPort)
{
    std::string deviceId = fromDeviceid;
    std::string recvip = mediaRecvIp;
    char str[512] = {0};
    pj_ansi_snprintf(str, 512,
                     "v=0\n"
                     "o=%s 0 0 IN IP4 %s\n"
                     "s=Play\n"
                     "c=IN IP4 %s\n"
                     "t=0 0\n"
                     "m=video %d RTP/AVP 96 98 97\n"
                     "a=recvonly\n"
                     "a=rtpmap:96 PS/90000\n"
                     "a=rtpmap:98 H264/90000\n"
                     "a=rtpmap:97 MPEG4/90000\n"
                     "y=0100000001\n",
                     deviceId.data(), recvip.data(), recvip.data(), mediaRecvPort);
    return str;
};

int SipClient::sendBye()
{
    pj_status_t status;
    pjsip_tx_data *tdata;
    status = pjsip_inv_end_session(m_invsession, 603, NULL, &tdata);
    pjsip_inv_send_msg(m_invsession, tdata);
    pjsip_inv_terminate(m_invsession, 603, true);
    pjsip_endpt_unregister_module(m_sipEndpt, pjsip_inv_usage_instance());
    m_invInit = false;
}

int SipClient::initInvParam(TransportContext &tsxContext)
{
    if (m_invInit)
        return PJ_SUCCESS;
    pj_status_t status;
    pjsip_inv_callback inv_cb;
    /* Init the callback for INVITE session: */
    pj_bzero(&inv_cb, sizeof(inv_cb));
    inv_cb.on_state_changed = &call_on_state_changed;
    inv_cb.on_new_session = &call_on_forked;
    inv_cb.on_media_update = &call_on_media_update;
    inv_cb.on_send_ack = &call_on_send_ack; //must
    /* Initialize invite session module:  */
    status = pjsip_inv_usage_init(m_sipEndpt, &inv_cb);
    PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);

    char from[64] = {0};
    char to[64] = {0};
    char target[64] = {0};
    char contact[64] = {0};
    //transfer issue, must do this
    std::string tempid = tsxContext.fromID;
    std::string tempip = tsxContext.fromIP;
    pj_ansi_snprintf(from, 64, "sip:%s@%s:%d", tempid.data(), tempip.data(), tsxContext.fromPort);
    // contact means recver
    pj_ansi_snprintf(contact, 64, "sip:%s@%s:%d", tsxContext.fromID.data(), tsxContext.fromIP.data(), tsxContext.fromPort);
    tempid = tsxContext.toID;
    tempip = tsxContext.contactIP;
    unsigned short  port = tsxContext.contactPort;
    pj_ansi_snprintf(target, 64, "sip:%s@%s:%d", tempid.data(), tempip.data(), port);
    //    pj_ansi_snprintf(to, 64, "sip:%s@%s:%d", "34020000001320000002", tempip.data(), tsxContext.toPort);
    tempid = tsxContext.toID;
    tempip = tsxContext.toIP;
    port = tsxContext.toPort;

    pj_ansi_snprintf(to, 64, "sip:%s@%s:%d", tempid.data(), tempip.data(), port);

    pj_str_t fromstr = pj_str(from);
    pj_str_t targetstr = pj_str(target);
    pj_str_t tostr = pj_str(to);
    pj_str_t contactStr = pj_str(contact);
    /* Create UAC dialog */
    status = pjsip_dlg_create_uac(pjsip_ua_instance(),
                                  &fromstr,      /* local URI */
                                  NULL,          /* local Contact */
                                  &tostr,        /* remote URI */
                                  &tostr,        /* remote target */
                                  &m_invitedlg); /* dialog */
    PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);
    m_invInit = true;
    return PJ_SUCCESS;
};

bool SipClient::sendInvite(std::string deviceid, std::string mediaRecvIp, short mediaRecvPort)
{
    int ret = initInvParam(m_tsxContext);
    if (ret != PJ_SUCCESS)
        return false;
    pjsip_tx_data *tdata;
    if (PJ_SUCCESS != pjsip_inv_create_uac(m_invitedlg, nullptr, 0, &m_invsession))
    {
        return false;
    }
    if (PJ_SUCCESS != pjsip_inv_invite(m_invsession, &tdata))
        return false;
    pjsip_media_type type;
    type.type = pj_str("application");
    type.subtype = pj_str("sdp");
    std::string sdp = createPlaySDP(deviceid, mediaRecvIp, mediaRecvPort);
    std::string tempstring = sdp;
    pj_str_t sdptext = pj_str(const_cast<char *>(tempstring.data()));
    try
    {
        tdata->msg->body = pjsip_msg_body_create(m_sipPool, &type.type, &type.subtype, &sdptext);
        auto hName = pj_str("Subject");
        char subjectUrl[128] = {0};
        pj_ansi_snprintf(subjectUrl, 128, "%s:0, %s:0", m_tsxContext.toID.c_str(), deviceid.c_str());
        auto hValue = pj_str(const_cast<char *>(subjectUrl));
        auto hdr = pjsip_generic_string_hdr_create(m_sipPool, &hName, &hValue);
        pjsip_msg_add_hdr(tdata->msg, reinterpret_cast<pjsip_hdr *>(hdr));
        pjsip_inv_send_msg(m_invsession, tdata);
    }
    catch (...)
    {
    }
    return true;
};

void SipClient::setClientParamContext()
{
    std::string serverid;
    std::string serverip;
    std::string clientid;
    std::string clientip;
    std::string cameraId;
    std::string cameraIp;

    serverid = "34020000000020000001";

    serverip = "172.18.64.231";

    clientid = "34020000001320000003";

    clientip = "172.18.64.51";

    
    cameraId ="34020000001180000001";
    cameraIp ="172.18.64.151";

    m_tsxContext.fromID = clientid;
    m_tsxContext.fromIP = clientip;
    m_tsxContext.fromPort = 5060;

    m_tsxContext.toID = cameraId;
    m_tsxContext.toIP = cameraIp;
    m_tsxContext.toPort = 5060;

    m_tsxContext.contactID = serverid;
    m_tsxContext.contactIP = serverip;
    m_tsxContext.contactPort = 15060;

    m_sipClientparam.localAddress = clientip;
    m_sipClientparam.localDeviceID = clientid;
    m_sipClientparam.localSipPort = 5060;
    m_sipClientparam.localPasswd = "12345678";
    m_sipClientparam.sipserverAddress = serverip;
    m_sipClientparam.sipserverID = serverid;
    m_sipClientparam.sipserverPort = 15060;
    m_sipClientparam.heartbeatInterval = 30;
    m_sipClientparam.maxTimeoutCount = 3;
    m_sipClientparam.regValidSeconds = 600;

    m_localSipType = TYPE_SIPClient;
};

void SipClient::onVidoPlay()
{
    this->setClientParamContext();
    // this->registerClient2();
    this->registerClient(m_sipClientparam);

    pj_thread_sleep(1000);

    m_rtpRecver = new RtpRecver();
    // connect(m_rtpRecver);

    std::string deviceid = m_tsxContext.fromID;
    std::string recvip = "172.18.64.51";
    int recvport = 9000;

    m_rtpRecver->init("0.0.0.0", recvport);

    executor->enqueue(SipClient::runRtpServer, m_rtpRecver);

    // this->sendKeepAlive(m_tsxContext.fromID);
    this->sendInvite(deviceid, recvip, recvport);
};

void SipClient::runRtpServer(RtpRecver *rtpRecver)
{
    std::cout << "start to . bind server" << std::endl;

    rtpRecver->run();
}

pj_bool_t SipClient::on_rx_response(pjsip_rx_data *rdata)
{

    char *rdata_info;
    pj_status_t status;
    pjsip_transaction *tsx;
    pjsip_tx_data *tdata;
    rdata_info = pjsip_rx_data_get_info(rdata);
    std::cout << "On_rx_response is called" << std::endl;
}

pj_bool_t SipClient::on_rx_request(pjsip_rx_data *rdata)
{
    char *rdata_info;
    pj_status_t status;
    pjsip_transaction *tsx;
    pjsip_tx_data *tdata;
    rdata_info = pjsip_rx_data_get_info(rdata);
    printf("The received transmission data info is %s \n", rdata_info);
    std::cout << "On_rx_response is called" << std::endl;

    status = pjsip_tsx_create_uas(&clientSipMoudle, rdata, &tsx);
    pjsip_tsx_recv_msg(tsx, rdata);
    status = pjsip_endpt_create_response(m_sipEndpt, rdata, 200, NULL, &tdata);
    pjsip_tsx_send_msg(tsx, tdata);

    return PJ_TRUE;
};

//send keep alive
void SipClient::startEventLoop()
{
    pj_status_t status;

    status = pj_thread_create(m_sipPool, "keepalive", &keepAlive_thread, this, 0, 0, &m_keepalivethread);
};

void SipClient::sendKeepAlive(std::string deviceid)
{
    char querInfo[256] = {0};

    char from[64] = {0};
    char to[64] = {0};
    char target[64] = {0};
    char contact[64] = {0};
    //transfer issue, must do this
    std::string tempid = m_tsxContext.fromID;
    std::string tempip = m_tsxContext.fromIP;
    unsigned int port = m_tsxContext.fromPort;

    pj_ansi_snprintf(from, 64, "sip:%s@%s:%d", tempid.data(), tempip.data(), m_tsxContext.fromPort);
    // contact means recver
    pj_ansi_snprintf(contact, 64, "sip:%s@%s:%d", m_tsxContext.fromID.data(), m_tsxContext.fromIP.data(), m_tsxContext.fromPort);
    tempid = m_tsxContext.contactID;
    tempip = m_tsxContext.contactIP;
    port = m_tsxContext.contactPort;
    pj_ansi_snprintf(target, 64, "sip:%s@%s:%d", tempid.data(), tempip.data(), port);

    tempid = m_tsxContext.toID;
    tempip = m_tsxContext.toIP;
    port = m_tsxContext.toPort;
    pj_ansi_snprintf(to, 64, "sip:%s@%s:%d", tempid.data(), tempip.data(), port);

    pj_ansi_snprintf(querInfo, 256, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                    "<Notify>\n"
                                    "<CmdType>Keepalive</CmdType>\n"
                                    "<SN>43</SN>\n"
                                    "<DeviceID>%s</DeviceID>\n"
                                    "</Notify>\n",
                     deviceid.c_str());
    pjsip_tx_data *tdata;
    pjsip_method method = {PJSIP_OTHER_METHOD, {"MESSAGE", 7}};

    auto infoStr = pj_str(querInfo);
    auto contactStr = pj_str(contact);
    auto fromStr = pj_str(from);
    auto toStr = pj_str(to);
    auto targetStr = pj_str(target);
    pjsip_endpt_create_request(m_sipEndpt, &method, &targetStr, &fromStr, &toStr, &contactStr, nullptr, -1, &infoStr, &tdata);

    tdata->msg->body->content_type.type = pj_str("Application");
    tdata->msg->body->content_type.subtype = pj_str("MANSCDP+xml");
    pjsip_endpt_send_request(m_sipEndpt, tdata, -1, nullptr, nullptr);
};

/* Worker thread */
int SipClient::keepAlive_thread(void *arg)
{

    SipClient *client = (SipClient *)arg;
    while (!quit_flag && client)
    {
        pj_thread_sleep(30000);
        std::string localid = "34020000001320000003";
        client->sendKeepAlive(localid);
    }
    return 0;
}

SipClient::SipClient(){};

SipClient::~SipClient(){};
