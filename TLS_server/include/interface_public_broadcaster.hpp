#ifndef IPUBLIC_BROADCASTER_HPP
#define IPUBLIC_BROADCASTER_HPP
#include <iostream>
#include <memory>

class IPublicBroadcaster
{
public:
    virtual ~IPublicBroadcaster() {}
    virtual void deliver(const std::string& msg) = 0;
};

typedef std::shared_ptr<IPublicBroadcaster> IBroadcaster_ptr;

#endif