#include "async_tcp_socket.hpp"
#include <asio/use_awaitable.hpp>
#include <asio/read_until.hpp>
#include <exception>
#include <utility>

AsyncTcpSocket::AsyncTcpSocket(TcpSocket tcp_socket)
    : m_tcp_socket{std::move(tcp_socket)}
{
}
awaitable<size_t> AsyncTcpSocket::async_read_until(std::string& buffer, size_t buff_len, std::string_view delim)
{
    try 
    {
        size_t n = co_await asio::async_read_until(m_tcp_socket, asio::dynamic_buffer(buffer, buff_len), delim, asio::use_awaitable);
        co_return n;
    } 
    catch (std::exception& e)
    {
        throw e;
    }
}
awaitable<size_t> AsyncTcpSocket::async_write(const std::string& buffer)
{
    try 
    {
        size_t bytes_written = co_await asio::async_write(m_tcp_socket, asio::buffer(buffer), asio::use_awaitable);
        co_return bytes_written;
    } 
    catch (std::exception& e)
    {
        throw e;
    }
}
bool AsyncTcpSocket::is_open() const
{
    return m_tcp_socket.is_open();
}
void AsyncTcpSocket::close_socket() 
{
    m_tcp_socket.close();
}
std::string AsyncTcpSocket::get_ip() const
{
    return m_tcp_socket.remote_endpoint().address().to_string();
}
asio::any_io_executor AsyncTcpSocket::get_executor()
{
    return m_tcp_socket.get_executor();
}
