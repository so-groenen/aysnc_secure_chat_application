#include "async_secure_socket.hpp"
#include <asio/read_until.hpp>
#include <asio/any_io_executor.hpp>
#include <asio/use_awaitable.hpp>


AsyncSecureSocket::AsyncSecureSocket(SslSocket&& ssl_socket)
    : m_ssl_socket{std::move(ssl_socket)}
{
}
awaitable<size_t> AsyncSecureSocket::async_read_until(std::string& buffer, size_t buff_len, std::string_view delim)
{
    size_t n = co_await asio::async_read_until(m_ssl_socket, asio::dynamic_buffer(buffer, buff_len), delim, asio::use_awaitable);
    co_return n;
}
awaitable<size_t> AsyncSecureSocket::async_write(const std::string& buffer)
{
    size_t bytes_written = co_await asio::async_write(m_ssl_socket, asio::buffer(buffer), asio::use_awaitable);
    co_return bytes_written;
}
bool AsyncSecureSocket::is_open() const
{
    return m_ssl_socket.lowest_layer().is_open();
}
void AsyncSecureSocket::close_socket() 
{
    m_ssl_socket.lowest_layer().close();
}
std::string AsyncSecureSocket::get_ip() const
{
    return m_ssl_socket.lowest_layer().remote_endpoint().address().to_string();
}
asio::any_io_executor AsyncSecureSocket::get_executor()
{
    return m_ssl_socket.get_executor();
}
