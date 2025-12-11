#ifndef TCP_CHAT_SERVER
#define TCP_CHAT_SERVER

#include "interface_chat_room.hpp"
#include "abstract_chat_server.hpp"
#include "tcp_chat_session.hpp"
 

class TcpChatServer : public AbstractChatServer
{
public:
    explicit TcpChatServer(asio::io_context& io, unsigned short port)
       : AbstractChatServer{io, port}
    {
    }
    awaitable<void> serve_forever(std::shared_ptr<IChatRoom> chat_room)
    {
        while (true)
        {
            auto client_socket = co_await m_acceptor.async_accept(asio::use_awaitable);
            std::unique_ptr<IChatParticipant> participant = std::make_unique<TcpChatSession>(std::move(client_socket), chat_room);
            
            chat_room->join(std::move(participant));
        }
    }
};

#endif