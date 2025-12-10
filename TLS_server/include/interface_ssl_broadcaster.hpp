#ifndef ISSL_CONTROLLABLE_BROADCASTER_HPP
#define ISSL_CONTROLLABLE_BROADCASTER_HPP


#include "interface_controllable_broadcaster.hpp"
#include "interface_ssl.hpp"

class ISslBroadcaster : public IControllableBroadcaster, public ISecureSocketLayer
{
public:
    virtual ~ISslBroadcaster() = default;
};
#endif