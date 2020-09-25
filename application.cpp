#include <iostream>

// #include "RequestBuilder.hpp"
#include "SipClient.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    cout << "gb28181 init!  " << endl;

    // RequestBuilder *builder = new RequestBuilder();
    // builder->init();

    SipClient *client= new SipClient();
    client->initPJlib();
    client->initSipMoudle("sipenpt", 5060);
    client->startEventLoop();
    client->onVidoPlay();

    pj_thread_sleep(10000000);

    return 0;
}
