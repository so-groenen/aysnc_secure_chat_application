#ifndef CHATROOM_HPP
#define CHATROOM_HPP

#include "interface_chat_room.hpp"
#include <deque>

using namespace std::string_view_literals;

class ChatRoom : public IChatRoom
{
    std::string m_password{};
    chat_participant::unique_ptr_set m_participants{};
    const size_t m_max_recent_msgs {100};
    std::deque<std::string> m_recent_msgs{};
    std::string_view m_delim = "\r\n"sv;
public:
    explicit ChatRoom(std::string_view password);
    std::string_view password() const override;
    void join(IChatParticipant_ptr participant) override;
    void leave(IChatParticipant* participant) override ;
    void join_public(IChatParticipant* participant) override;
    void deliver_private(const std::string& msg, IControllableBroadcaster* participant) override;
    void deliver(const std::string& msg) override;
};


#endif