#ifndef REQUEST_BUILDER_HPP
#define REQUEST_BUILDER_HPP

#include <pjsua2.hpp>

class RequestBuilder
{
private:
  pj::Endpoint *ep;

public:
    RequestBuilder(/* args */);
    void init();
    ~RequestBuilder();
};




#endif