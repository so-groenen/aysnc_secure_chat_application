#ifndef ABSTRACT_CHAT_SERVER
#define ABSTRACT_CHAT_SERVER

#include "interface_chat_room.hpp"
 
#include <asio/ssl.hpp>
#include <asio/io_context.hpp>
#include <asio/detached.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/co_spawn.hpp>
#include <asio/awaitable.hpp>
#include <asio/use_awaitable.hpp>

using asio::ip::tcp;
 
class AbstractChatServer
{
    asio::io_context& m_io;
    unsigned short m_port{};
protected:
    asio::ip::tcp::acceptor m_acceptor;
public:
    AbstractChatServer(asio::io_context& io, unsigned short port)
        : m_io{io}, m_acceptor{tcp::acceptor(io, {tcp::v4(), port})}, m_port{port}
    {
    }
    virtual awaitable<void> serve_forever(std::shared_ptr<IChatRoom> chat_room) = 0;
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
};

#endif