#ifndef I_ASYNC_SOCKET_HPP
#define I_ASYNC_SOCKET_HPP
#include "asio/awaitable.hpp"
#include <cstddef>
#include <memory>
#include <asio/execution_context.hpp>




using asio::awaitable;


class IAsyncSocket  
{
public:
    virtual ~IAsyncSocket() = default;
    virtual awaitable<size_t> async_read_until(std::string& buffer, size_t buff_len, std::string_view delim) = 0;
    virtual awaitable<size_t> async_write(const std::string& buffer) = 0;
    virtual bool is_open() const = 0;
    virtual void close_socket() = 0; 
    virtual std::string get_ip() const = 0;
    virtual asio::any_io_executor  get_executor() = 0;
};
using IAsyncSocket_shrd_ptr = std::shared_ptr<IAsyncSocket>;
#endif