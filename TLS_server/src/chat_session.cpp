#include "chat_session.hpp"


ChatSession::ChatSession(tcp::socket socket, std::shared_ptr<IChatRoom> room)
    : m_client_socket(std::move(socket)), 
    m_hand_shaker{std::dynamic_pointer_cast<IPrivateBroadcaster>(room), this}, //dynamic_cast<IControllableBroadcaster*>(this)},
    m_timer(m_client_socket.get_executor()), m_room(room),
        m_ip{m_client_socket.remote_endpoint().address().to_string()}
{
    m_timer.expires_at(std::chrono::steady_clock::time_point::max());
}
awaitable<size_t> ChatSession::async_read_password(std::string& buff, size_t pass_buff_len, std::string_view delim)
{
    size_t n = co_await asio::async_read_until(m_client_socket, asio::dynamic_buffer(buff, pass_buff_len), delim, asio::use_awaitable);
    co_return n;
}

std::string_view ChatSession::ip() const 
{
    return std::string_view{m_ip};
}

void ChatSession::start()
{        
    auto handle_connection_response = [this](bool connection_is_acctepd)
    {
        if (connection_is_acctepd)
        {
            m_room->join_public(this); 
            asio::co_spawn(m_client_socket.get_executor(), [this]
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

    asio::co_spawn(m_client_socket.get_executor(),[this, handle_connection_response]
    {
        return m_hand_shaker.try_connect(handle_connection_response);
    },
    asio::detached);


    co_spawn(m_client_socket.get_executor(), [this]
    {
        return writer();
    },
    asio::detached);
}

void ChatSession::deliver(const std::string& msg)
{
    m_write_msgs.push_back(msg);
    m_timer.cancel_one();
}

awaitable<void> ChatSession::reader()
{
    //m_room->join_public(this);

    std::string read_msg;
    try
    { 
        while(m_client_socket.is_open())
        {
            std::size_t n = co_await asio::async_read_until(m_client_socket, asio::dynamic_buffer(read_msg, m_read_buff_len), m_delim, asio::use_awaitable);
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
void ChatSession::log_error(std::string_view from, const std::exception& e) const 
{
    std::println("[{}(): ip: {}: <Terminated with>: {}]", from, m_ip, e.what());
}
awaitable<void> ChatSession::writer()
{
    try
    {
        while (m_client_socket.is_open())
        {
            if (m_write_msgs.empty())
            {
                std::error_code ec;
                co_await m_timer.async_wait(redirect_error(asio::use_awaitable, ec));
            }
            else
            {
                co_await asio::async_write(m_client_socket, asio::buffer(m_write_msgs.front()), asio::use_awaitable);
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
void ChatSession::stop()
{
    if (!m_stopped)
    {
        std::println("[from: {}: is stopped]", m_ip);
        m_room->leave(this);
        m_client_socket.close();
        m_timer.cancel();
        m_stopped = true;
    }
}
