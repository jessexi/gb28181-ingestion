#ifndef DEFINES_H
#define DEFINES_H

/* Include all headers. */
#include <pjlib.h>
#include <pjsip.h>
#include <stdlib.h>
#include <pjsip-simple/publish.h>
#include <pjlib-util.h>
#include <pjsip_ua.h>
#include <pjsip/sip_uri.h>
#include <pjsip/sip_transport.h>
#include <pjsip/sip_types.h>
#include <pjmedia.h>
#include <pjmedia/sdp_neg.h>
#include <pjmedia/sdp.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <stdio.h>


typedef struct
{
    std::string fromID;
    std::string fromIP;
    unsigned short fromPort;

    std::string toID;
    std::string toIP;
    unsigned short toPort;

    std::string contactID;
    std::string contactIP;
    unsigned short contactPort;

}TransportContext;


typedef struct tagSIPServer
{
    std::string sipserverID;
    std::string sipserverDomain;
    std::string sipserverPasswd;
    unsigned short sipserverPort;
    unsigned int heartbeatInterval;
    unsigned int maxTimeoutCount;
    tagSIPServer(){
        sipserverPort = 5060;
        heartbeatInterval = 60;
        maxTimeoutCount = 5;
    }
}SIPServer;

typedef struct tagSIPClient
{
    std::string sipserverID;
    std::string sipserverDomain;
    std::string sipserverAddress; //ip
    unsigned short sipserverPort;

    std::string localDeviceID;
    std::string localAddress; //ip
    std::string localDomain;
    unsigned short localSipPort;
    std::string localPasswd;
    unsigned long regValidSeconds; //s

    unsigned int heartbeatInterval;
    unsigned int maxTimeoutCount;
    tagSIPClient(){
        sipserverAddress = "172.0.0.1";
        localPasswd = "12345678";
        sipserverPort = 5060;
        localSipPort = 5060;
        regValidSeconds = 86400;
        heartbeatInterval = 60;
        maxTimeoutCount = 3;
    }
}SIPClient;

typedef enum{
    TYPE_None,
    TYPE_SIPServer,
    TYPE_SIPClient,
    TYPE_Other
}LocalSipType;



#endif // DEFINES_H
