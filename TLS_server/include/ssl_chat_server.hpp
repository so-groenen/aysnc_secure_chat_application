#ifndef SSL_CHAT_SERVER
#define SSL_CHAT_SERVER

#include "interface_async_socket.hpp"
#include "interface_chat_room.hpp"
#include "abstract_chat_server.hpp"
#include "chat_session.hpp"
#include "async_secure_socket.hpp"
// #include <algorithm>
#include <memory>
// #include "ssl_chat_session.hpp"
 

class SslChatServer : public AbstractChatServer
{
    asio::ssl::context m_ssl_context;  /// <==== SSL is here
public:
    explicit SslChatServer(asio::io_context& io, unsigned short port, asio::ssl::context ssl_context)
        : AbstractChatServer{io,port}, m_ssl_context{std::move(ssl_context)}  
    {
        std::println("Server created: Using SSL sockets. [SECURE]");
    }
    awaitable<void> serve_forever(std::shared_ptr<IChatRoom> chat_room) override
    {
        while (true)
        {
            auto client_socket = co_await m_acceptor.async_accept(asio::use_awaitable);
            SslSocket ssl_client_socket{std::move(client_socket), m_ssl_context};   /// <==== SSL is here           
            try
            {
                co_await ssl_client_socket.async_handshake(asio::ssl::stream_base::server, asio::use_awaitable);
                IAsyncSocket_shrd_ptr socket_interface = std::make_shared<AsyncSecureSocket>(std::move(ssl_client_socket));
                
                auto participant = std::make_unique<ChatSession>(std::move(socket_interface), chat_room);
                chat_room->join(std::move(participant));
            }
            catch(const std::exception& e)
            {
                std::println("Incoming connection: Could not perform handshake: {}", e.what());
            }
        }
    }
};

#endif