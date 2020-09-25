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

    ep = new Endpoint();

    ep->libCreate();

    // Initialize endpoint
    EpConfig ep_cfg;
    ep->libInit(ep_cfg);

    // Create SIP transport. Error handling sample is shown
    TransportConfig tcfg;
    tcfg.port = 5061;
    try
    {
        ep->transportCreate(PJSIP_TRANSPORT_UDP, tcfg);
    }
    catch (Error &err)
    {
        std::cout << err.info() << std::endl;
    }

    // Start the library (worker threads etc)
    ep->libStart();
    std::cout << "*** PJSUA2 STARTED ***" << std::endl;

    //Add account
    AccountConfig accCfg;
    accCfg.idUri = "sip:34020000022000000002@172.18.64.51:5061";
    accCfg.regConfig.registrarUri = "sip:34020000002000000001@172.18.64.231:15060";
    AuthCredInfo cred("digest", "3402000000", "34020000002000000001", 0, "12345678");
    accCfg.sipConfig.authCreds.push_back(cred);
    std::cout << std::endl << "REGISTER" << std::endl << std::endl;



    // Create the account
    MyAccount *acc = new MyAccount;
    acc->create(accCfg);

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
    delete ep;
}