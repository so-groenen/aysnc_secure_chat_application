#include "chat_room.hpp"
#include <print>
 
ChatRoom::ChatRoom(std::string_view password) 
    : m_password{password}
{
} 
std::string_view ChatRoom::password() const
{
    return std::string_view(m_password);
}
void ChatRoom::join(IChatParticipant_ptr participant)
{
    std::println("New connection!");
//unsafe
//{
    auto raw_participant = participant.get();        
    m_participants.insert(std::move(participant)); // we want to start the session AFTER they have joined
    raw_participant->start();
//}
}
void ChatRoom::leave(IChatParticipant* participant) 
{
    auto it = m_participants.find(participant);
    if (it != m_participants.end())
    {
        m_participants.erase(it);
    }
}
void ChatRoom::join_public(IChatParticipant* participant)
{
    auto it = m_participants.find(participant);
    if (it != m_participants.end())
    {
        for (auto msg: m_recent_msgs)
        {   
            // auto msg_ = msg;
            for (const auto c : m_delim)
                msg.push_back(c);
 
            participant->deliver(msg); // populate "outbox"
        }
    }
}
void ChatRoom::deliver_private(const std::string& msg, IPublicBroadcaster* participant)
{
    auto chat_participant = dynamic_cast<IChatParticipant*>(participant); 
    auto it = m_participants.find(chat_participant);
    if (it != m_participants.end())
    {
        participant->deliver(msg);
    }
}
void ChatRoom::deliver(const std::string& msg)
{
    m_recent_msgs.push_back(msg);
    while (m_recent_msgs.size() > m_max_recent_msgs)
    {
        m_recent_msgs.pop_front();
    }

    for (auto& participant: m_participants)
    {
        participant->deliver(msg);
    }
}
