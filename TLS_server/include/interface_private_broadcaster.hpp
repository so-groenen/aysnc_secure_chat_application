#ifndef IPRIVATE_BROADCASTER_HPP
#define IPRIVATE_BROADCASTER_HPP
#include "interface_public_broadcaster.hpp"
#include <iostream>

#include "interface_controllable_broadcaster.hpp"

class IPrivateBroadcaster
{
public:
    virtual ~IPrivateBroadcaster() {}
    virtual void deliver_private(const std::string& msg, IControllableBroadcaster* participant) = 0;  
    virtual std::string_view password() const = 0;
};

#endif