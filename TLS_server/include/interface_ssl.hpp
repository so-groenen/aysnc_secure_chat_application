#ifndef I_SECURE_SOCKET_LAYER_HPP
#define I_SECURE_SOCKET_LAYER_HPP
#include "asio/awaitable.hpp"
 
using asio::awaitable;

class ISecureSocketLayer 
{
public:
    virtual ~ISecureSocketLayer() = default;
    virtual awaitable<void> async_perform_ssl_handshake() = 0;
};
#endif