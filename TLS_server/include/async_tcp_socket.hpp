#ifndef ASYNC_TCP_SOCKET_HPP
#define ASYNC_TCP_SOCKET_HPP

// #include "abstract_chat_session.hpp"
#include "interface_async_socket.hpp"
#include <asio/ssl.hpp>
#include <asio/ip/tcp.hpp>
#include <print>

using TcpSocket = asio::ip::tcp::socket;

class AsyncTcpSocket : public IAsyncSocket  
{
    TcpSocket m_tcp_socket;
public:
    explicit AsyncTcpSocket(TcpSocket tcp_socket);
    awaitable<size_t> async_read_until(std::string& buffer, size_t buff_len, std::string_view delim) override;
    awaitable<size_t> async_write(const std::string& buffer) override;
    bool is_open() const override;
    void close_socket() override; 
    std::string get_ip() const override;
    asio::any_io_executor get_executor() override;
    ~AsyncTcpSocket()
    {
        std::println("TCP Socket closed successfully");
    }
};

#endif