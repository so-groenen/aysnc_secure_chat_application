#ifndef TCP_CHAT_SERVER
#define TCP_CHAT_SERVER

#include "interface_chat_room.hpp"
#include "chat_session.hpp"

#include <asio/io_context.hpp>
#include <asio/detached.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/co_spawn.hpp>
#include <asio/awaitable.hpp>
#include <asio/use_awaitable.hpp>

 
class TcpChatServer
{
    asio::io_context& m_io;
    asio::ip::tcp::acceptor m_acceptor;
    unsigned short m_port{};
public:
    explicit TcpChatServer(asio::io_context& io, unsigned short port)
        : m_io{io}, m_acceptor{tcp::acceptor(io, {tcp::v4(), port})}, m_port{port}
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
            std::unique_ptr<IChatParticipant> participant = std::make_unique<ChatSession>(std::move(client_socket), chat_room);
            
            chat_room->join(std::move(participant));
        }
    }
};

#endif