#ifndef ASYNC_SSL_HPP
#define ASYNC_SSL_HPP


#include <asio/socket_base.hpp>
#include <cstddef>
#include <print>
#include <string>


// #include "abstract_chat_session.hpp"
#include "interface_async_socket.hpp"
#include <asio/ssl.hpp>
#include <asio/ip/tcp.hpp>
#include <string_view>


using TcpSocket = asio::ip::tcp::socket;
using SslSocket = asio::ssl::stream<TcpSocket>;

class AsyncSecureSocket : public IAsyncSocket  
{
    SslSocket m_ssl_socket;
public:
    explicit AsyncSecureSocket(SslSocket&& ssl_socket);
    awaitable<size_t> async_read_until(std::string& buffer, size_t buff_len, std::string_view delim) override;
    awaitable<size_t> async_write(const std::string& buffer) override;
    bool is_open() const override;
    void close_socket() override; 
    std::string get_ip() const override;
    asio::any_io_executor  get_executor() override;
    ~AsyncSecureSocket()
    {
        std::println("Ssl Socket closed successfully");
    }
};

#endif