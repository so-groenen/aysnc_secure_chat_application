#ifndef TCP_CHAT_SERVER
#define TCP_CHAT_SERVER

// #include "interface_ssl_chat_room.hpp"
#include "interface_chat_room.hpp"

#include "ssl_chat_session.hpp" /// ssl_chat_session

#include <asio/ssl.hpp>
#include <asio/io_context.hpp>
#include <asio/detached.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/co_spawn.hpp>
#include <asio/awaitable.hpp>
#include <asio/use_awaitable.hpp>

using SslSocket = asio::ssl::stream<tcp::socket>;
 
class SslChatServer
{
    asio::io_context& m_io;
    asio::ip::tcp::acceptor m_acceptor;
    asio::ssl::context m_ssl_context;  /// <====
    unsigned short m_port{};
public:
    explicit SslChatServer(asio::io_context& io, unsigned short port, asio::ssl::context ssl_context)
        : m_io{io}, m_acceptor{tcp::acceptor(io, {tcp::v4(), port})}, m_port{port}, m_ssl_context{std::move(ssl_context)}
    {
    }
    unsigned short port() const 
    {
        return m_port;
    }
    std::string ip() const 
    {
        return m_acceptor.local_endpoint().address().to_string();
    }
    void start(std::shared_ptr<IChatRoom> chat_room)  
    {
        asio::co_spawn(m_io, [this, chat_room]
        {
            return serve_forever(chat_room);
        }, asio::detached);
    }
private:
    awaitable<void> serve_forever(std::shared_ptr<IChatRoom> chat_room)
    {
        while (true)
        {
            auto client_socket = co_await m_acceptor.async_accept(asio::use_awaitable);
            SslSocket ssl_client_socket{std::move(client_socket), m_ssl_context}; /// <====
            // co_await ssl_client_socket.async_handshake(asio::ssl::stream_base::server, asio::use_awaitable);

            auto participant = std::make_unique<SslChatSession>(std::move(ssl_client_socket), chat_room);
            chat_room->join(std::move(participant));
        }
    }
};

#endif