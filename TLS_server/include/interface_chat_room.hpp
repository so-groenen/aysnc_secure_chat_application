#ifndef ICHAT_ROOM_HPP
#define ICHAT_ROOM_HPP

#include "interface_chat_participant.hpp"
#include "interface_private_broadcaster.hpp"
#include "interface_public_broadcaster.hpp"

class IChatRoom : public IPublicBroadcaster, public IPrivateBroadcaster
{
public:
    virtual ~IChatRoom() = default;
    virtual void join(IChatParticipant_ptr participant) = 0;
    virtual void join_public(IChatParticipant* participant) = 0;
    virtual void leave(IChatParticipant* participant) = 0;
};

#endif