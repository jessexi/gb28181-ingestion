#include "RequestBuilder.hpp"
#include <pjsua2.hpp>
#include <iostream>

using namespace pj;

class MyAccount : public Account {
public:
    virtual void onRegState(OnRegStateParam &prm) {
        AccountInfo ai = Account::getInfo();
        std::cout << (ai.regIsActive? "*** Register:" : "*** Unregister:")
                  << " code=" << prm.code << std::endl;
    }
};

void RequestBuilder::init()
{
    std::cout << "builder init!" << std::endl;

    Endpoint ep;

    ep.libCreate();

    // Initialize endpoint
    EpConfig ep_cfg;
    ep.libInit(ep_cfg);

    // Create SIP transport. Error handling sample is shown
    TransportConfig tcfg;
    tcfg.port = 5060;
    try
    {
        ep.transportCreate(PJSIP_TRANSPORT_UDP, tcfg);
    }
    catch (Error &err)
    {
        std::cout << err.info() << std::endl;
    }

    // Start the library (worker threads etc)
    ep.libStart();
    std::cout << "*** PJSUA2 STARTED ***" << std::endl;

    // Configure an AccountConfig
    AccountConfig acfg;
    acfg.idUri = "sip:test@pjsip.org";
    acfg.regConfig.registrarUri = "sip:pjsip.org";
    AuthCredInfo cred("digest", "*", "test", 0, "secret");
    acfg.sipConfig.authCreds.push_back(cred);

    // Create the account
    MyAccount *acc = new MyAccount;
    acc->create(acfg);

    // Here we don't have anything else to do..
    pj_thread_sleep(10000);

    // Delete the account. This will unregister from server
    delete acc;

}

RequestBuilder::RequestBuilder(/* args */)
{
}

RequestBuilder::~RequestBuilder()
{
}