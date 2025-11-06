#ifndef ICONTROLLABLE_BROADCASTER_HPP
#define ICONTROLLABLE_BROADCASTER_HPP
#include "interface_public_broadcaster.hpp"
#include <asio/awaitable.hpp>
 
using asio::awaitable;

class IControllableBroadcaster : public IPublicBroadcaster
{
public:
    virtual ~IControllableBroadcaster() {}
    virtual std::string_view ip() const = 0;
    virtual awaitable<size_t> async_read_password(std::string& buff, size_t pass_buff_len, std::string_view delim) = 0;
    // virtual awaitable<size_t> async_respond(std::string buff, size_t pass_buff_len, std::string_view delim) = 0;
};
#endif