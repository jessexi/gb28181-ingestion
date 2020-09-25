#ifndef REQUEST_BUILDER_HPP
#define REQUEST_BUILDER_HPP

#include <pjsua2.hpp>

class RequestBuilder
{
private:
  pj::Endpoint *ep;
  std::string CONTENT_CRLF = "\r\n"; 

public:
    RequestBuilder(/* args */);
    void init();
    std::string createSDP();
    std::string buildInvite();
    pj_str_t pjHeaderBuilder();
    void invite();
    ~RequestBuilder();
};

#endif