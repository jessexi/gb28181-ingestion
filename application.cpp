#include <iostream>

#include "RequestBuilder.hpp"
#include "SipClient.hpp"
#include <signal.h>


using namespace std;

SipClient *client=  NULL;

void signalHandler(int signal) {

   if (client != NULL) {
       client->sendBye();
   }
   
    exit(signal);
}


int main(int argc, char *argv[])
{
    cout << "gb28181 init!  " << endl;

    // RequestBuilder *builder = new RequestBuilder();
    // builder->init();
    // delete builder;

    client= new SipClient();

    client->initPJlib();
    client->initSipMoudle("sipenpt", 5060);
 ;
    client->startEventLoop();
    client->onVidoPlay();


    // signal(SIGINT, signalHandler);
    pj_thread_sleep(60000);

    client->sendBye();

    return 0;
}

