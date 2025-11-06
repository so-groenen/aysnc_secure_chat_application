#ifndef IPRIVATE_BROADCASTER_HPP
#define IPRIVATE_BROADCASTER_HPP
#include "interface_public_broadcaster.hpp"
#include <iostream>

class IPrivateBroadcaster
{
public:
    virtual ~IPrivateBroadcaster() {}
    virtual void deliver_private(const std::string& msg, IPublicBroadcaster* participant) = 0; //should be IPublicBroadcaster
    virtual std::string_view password() const = 0;
};

#endif