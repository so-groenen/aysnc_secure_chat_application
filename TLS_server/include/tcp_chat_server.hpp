#ifndef TCP_CHAT_SERVER
#define TCP_CHAT_SERVER

#include "interface_async_socket.hpp"
#include "interface_chat_room.hpp"
#include "abstract_chat_server.hpp"
#include "chat_session.hpp"
#include "async_tcp_socket.hpp"
#include <memory>
 

class TcpChatServer : public AbstractChatServer
{
public:
    explicit TcpChatServer(asio::io_context& io, unsigned short port)
       : AbstractChatServer{io, port}
    {
        std::println("Server created: Using TCP sockets. [UNSECURE]");
    }
    awaitable<void> serve_forever(std::shared_ptr<IChatRoom> chat_room)
    {
        while (true)
        {
            auto client_socket = co_await m_acceptor.async_accept(asio::use_awaitable);
            IAsyncSocket_shrd_ptr socket_interface = std::make_shared<AsyncTcpSocket>(std::move(client_socket));
            // shared ptr to avoid issues when read() and write() can both close the socket from different "spawned" functions
            std::unique_ptr<IChatParticipant> participant = std::make_unique<ChatSession>(std::move(socket_interface), chat_room);            
            chat_room->join(std::move(participant));
        }
    }
};

#endif