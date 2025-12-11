#ifndef SSL_CHAT_SERVER
#define SSL_CHAT_SERVER

#include "interface_chat_room.hpp"
#include "abstract_chat_server.hpp"
#include "ssl_chat_session.hpp"
 

class SslChatServer : public AbstractChatServer
{
    asio::ssl::context m_ssl_context;  /// <==== SSL is here
public:
    explicit SslChatServer(asio::io_context& io, unsigned short port, asio::ssl::context ssl_context)
        : AbstractChatServer{io,port}, m_ssl_context{std::move(ssl_context)}  
    {
    }
 
    awaitable<void> serve_forever(std::shared_ptr<IChatRoom> chat_room) override
    {
        while (true)
        {
            auto client_socket = co_await m_acceptor.async_accept(asio::use_awaitable);
            SslSocket ssl_client_socket{std::move(client_socket), m_ssl_context};   /// <==== SSL is here
            co_await ssl_client_socket.async_handshake(asio::ssl::stream_base::server, asio::use_awaitable);

            auto participant = std::make_unique<SslChatSession>(std::move(ssl_client_socket), chat_room);
            chat_room->join(std::move(participant));
        }
    }
};

#endif