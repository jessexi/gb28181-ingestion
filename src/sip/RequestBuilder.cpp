#include "RequestBuilder.hpp"
#include <pjsua2.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>

using namespace pj;

class MyAccount : public Account
{
public:
    virtual void onRegState(OnRegStateParam &prm)
    {
        AccountInfo ai = Account::getInfo();
        std::cout << (ai.regIsActive ? "*** Register:" : "*** Unregister:")
                  << " code=" << prm.code << std::endl;
    }
    std::vector<Call *> calls;
};

void RequestBuilder::init()
{
    // std::cout << "builder init!" << std::endl;

    // ep = new Endpoint();

    // ep->libCreate();

    // // Initialize endpoint
    // EpConfig ep_cfg;
    // ep->libInit(ep_cfg);

    // // Create SIP transport. Error handling sample is shown
    // TransportConfig tcfg;
    // tcfg.port = 5060;
    // try
    // {
    //     ep->transportCreate(PJSIP_TRANSPORT_UDP, tcfg);
    // }
    // catch (Error &err)
    // {
    //     std::cout << err.info() << std::endl;
    // }

    // // Start the library (worker threads etc)
    // ep->libStart();
    // std::cout << "*** PJSUA2 STARTED ***" << std::endl;

    // this->invite();
    // pj_thread_sleep(10000);


    //Add account
    AccountConfig accCfg;
    accCfg.idUri = "sip:34020000001320000003@172.18.64.51:5060";
    accCfg.regConfig.registrarUri = "sip:34020000002000000001@172.18.64.231:15060";
    AuthCredInfo cred("digest", "*", "34020000002000000001", 0, "12345678");
    accCfg.sipConfig.authCreds.push_back(cred);
    std::cout << std::endl
              << "REGISTER" << std::endl
              << std::endl;

    // Create the account
    acc = new MyAccount;
    acc->create(accCfg);

    // Here we don't have anything else to do..
    pj_thread_sleep(1000);

    // std::cout << std::endl
    //           << "INVITE" << std::endl
    //           << std::endl;

    // // send invide
    // pj_str_t target = pj_str(const_cast<char *>(accCfg.regConfig.registrarUri.c_str()));
    // pj_str_t from = pj_str(const_cast<char *>(accCfg.idUri.c_str()));
    // pjsip_tx_data *tdata;

    // pj_str_t text = pj_str(const_cast<char *>(this->buildInvite().c_str()));
    // pj_str_t contact = this->pjHeaderBuilder();

    


    // pjsip_endpt_create_request(pjsua_get_pjsip_endpt(), pjsip_get_invite_method(), &target, &from, &target, NULL , NULL, -1, &text, &tdata);
    // pjsip_endpt_send_request_stateless(pjsua_get_pjsip_endpt(), tdata, NULL, NULL);

    // pj_thread_sleep(1000);

    // pjsua_call_id call_id = 1;
    // Call *call = new Call(*acc, call_id);
    // acc->calls.push_back(call);

    // CallOpParam prm(true);
    // prm.opt.audioCount = 0;
    // prm.opt.videoCount = 0;
    // std::cout << std::endl
    //           << "CALL START" << std::endl
    //           << std::endl;
    // call->makeCall("sip:34020000002000000001@172.18.64.231:15060", prm);
    // std::cout << std::endl
    //           << "CALL END" << std::endl
    //           << std::endl;

    pj_thread_sleep(1000);

    // Delete the account. This will unregister from server
    // delete acc;
}

void RequestBuilder::clientRegister(){
     //Add account
    AccountConfig accCfg;
    accCfg.idUri = "sip:34020000001320000003@172.18.64.51:5060";
    accCfg.regConfig.registrarUri = "sip:34020000002000000001@172.18.64.231:15060";
    AuthCredInfo cred("digest", "*", "34020000002000000001", 0, "12345678");
    accCfg.sipConfig.authCreds.push_back(cred);
    std::cout << std::endl
              << "REGISTER" << std::endl
              << std::endl;

    // Create the account
    MyAccount *acc = new MyAccount;
    acc->create(accCfg);
    pj_thread_sleep(2000);
}

RequestBuilder::RequestBuilder(/* args */)
{
}

pj_str_t RequestBuilder::pjHeaderBuilder()
{
    std::stringstream header;

    header << "Content-Type: Application/SDP" << CONTENT_CRLF
           << "Max-Forwards: 70" << CONTENT_CRLF;

    return pj_str(const_cast<char *>(header.str().c_str()));
}

void RequestBuilder::invite() {

}

std::string RequestBuilder::buildInvite()
{

    std::stringstream xml;
    std::string xmlbody;

    xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << CONTENT_CRLF
        << "<Control>" << CONTENT_CRLF
        << "<CmdType>DeviceControl</CmdType>" << CONTENT_CRLF
        << "<SN>" << 1 << "</SN>" << CONTENT_CRLF
        << "<DeviceID>" << 1 << "</DeviceID>" << CONTENT_CRLF
        << "<PTZCmd>" << 1 << "</PTZCmd>" << CONTENT_CRLF
        << "<Info>" << CONTENT_CRLF
        << "<ControlPriority>" << 1 << "</ControlPriority>" << CONTENT_CRLF
        << "</Info>" << CONTENT_CRLF
        << "</Control>" << CONTENT_CRLF;
    xmlbody = xml.str();

    return xmlbody;
}

std::string RequestBuilder::createSDP()
{
	char str[500] = { 0 };
	pj_ansi_snprintf(str, 500,
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
	"1",
	"172.18.64.75",
	"172.18.64.75",
	9000
			);
	return str;
}

RequestBuilder::~RequestBuilder()
{
    ep->libDestroy();
    delete ep;
}