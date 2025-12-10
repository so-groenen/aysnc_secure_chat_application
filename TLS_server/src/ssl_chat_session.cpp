#include "ssl_chat_session.hpp"


SslChatSession::SslChatSession(SslSocket socket, std::shared_ptr<IChatRoom> room)
    : m_ssl_socket(std::move(socket)), 
    m_hand_shaker{std::dynamic_pointer_cast<IPrivateBroadcaster>(room), this}, 
    m_timer(m_ssl_socket.get_executor()), m_room(room),
        m_ip{m_ssl_socket.lowest_layer().remote_endpoint().address().to_string()}
{
    m_timer.expires_at(std::chrono::steady_clock::time_point::max());

}
awaitable<size_t> SslChatSession::async_read_password(std::string& buff, size_t pass_buff_len, std::string_view delim)
{
    size_t n = co_await asio::async_read_until(m_ssl_socket, asio::dynamic_buffer(buff, pass_buff_len), delim, asio::use_awaitable);
    co_return n;
}

awaitable<void> SslChatSession::async_perform_ssl_handshake()
{
    co_await m_ssl_socket.async_handshake(asio::ssl::stream_base::server, asio::use_awaitable);
}

std::string_view SslChatSession::ip() const 
{
    return std::string_view{m_ip};
}

void SslChatSession::start()
{        

    auto handle_connection_response = [this](bool connection_is_acctepd)
    {
        if (connection_is_acctepd)
        {
            m_room->join_public(this); 
            asio::co_spawn(m_ssl_socket.get_executor(), [this]
            {
                return reader();
            },
            asio::detached);
        }
        else
        {
            std::println("chat_session terminate");
            stop();
        }
    };

    asio::co_spawn(m_ssl_socket.get_executor(),[this, handle_connection_response]
    {
        return m_hand_shaker.try_connect(handle_connection_response);
    },
    asio::detached);


    asio::co_spawn(m_ssl_socket.get_executor(), [this]
    {
        return writer();
    },
    asio::detached);
}

void SslChatSession::deliver(const std::string& msg)
{
    m_write_msgs.push_back(msg);
    m_timer.cancel_one();
}

awaitable<void> SslChatSession::reader()
{
    //m_room->join_public(this);

    std::string read_msg;
    try
    { 
        while(m_ssl_socket.lowest_layer().is_open())
        {
            std::size_t n = co_await asio::async_read_until(m_ssl_socket, asio::dynamic_buffer(read_msg, m_read_buff_len), m_delim, asio::use_awaitable);
            std::string_view response{read_msg};
            
            response = response.substr(0, response.find_last_of(m_delim)-1); // assumes it exits etc..
            std::println("[from: {}]: {}", m_ip, response);
            std::string response_str{response};

            m_room->deliver(response_str);
            read_msg.erase(0, n);
        }
    }
    catch (const std::exception& e)
    {
        log_error("reader"sv, e);
        stop();
    }
}
void SslChatSession::log_error(std::string_view from, const std::exception& e) const 
{
    std::println("[{}(): ip: {}: <Terminated with>: {}]", from, m_ip, e.what());
}
awaitable<void> SslChatSession::writer()
{
    try
    {
        while (m_ssl_socket.lowest_layer().is_open())
        {
            if (m_write_msgs.empty())
            {
                std::error_code ec;
                co_await m_timer.async_wait(redirect_error(asio::use_awaitable, ec));
            }
            else
            {
                co_await asio::async_write(m_ssl_socket, asio::buffer(m_write_msgs.front()), asio::use_awaitable);
                m_write_msgs.pop_front();
            }
        }
    }
    catch (const std::exception& e)
    {
        log_error("writer"sv, e);
        stop();
    }
} 
void SslChatSession::stop()
{
    if (!m_stopped)
    {
        std::println("[from: {}: is stopped]", m_ip);
        m_room->leave(this);
        m_ssl_socket.lowest_layer().close();
        m_timer.cancel();
        m_stopped = true;
    }
}
