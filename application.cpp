#include <iostream>

#include "RequestBuilder.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    cout << "gb28181 init!  " << endl;

    RequestBuilder *builder = new RequestBuilder();
    builder->init();
    return 0;
}
