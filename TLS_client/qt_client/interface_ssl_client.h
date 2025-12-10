#ifndef INTERFACE_SSL_CLIENT_H
#define INTERFACE_SSL_CLIENT_H
#include "interface_ssl.h"
#include "interface_tcp_client.h"

class ISslClient : public ITcpClient, public ISecureSocketLayer
{
public:
    virtual ~ISslClient() = default;
};


#endif // INTERFACE_SSL_CLIENT_H
