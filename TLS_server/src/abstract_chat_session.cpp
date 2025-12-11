#include "abstract_chat_session.hpp"


static bool is_open(const TcpSocket& tcp_socket)
{
    return tcp_socket.is_open();
}
static bool is_open(const SslSocket& ssl_socket)
{
    return ssl_socket.lowest_layer().is_open();
}
static void close_socket(TcpSocket& tcp_socket)
{
    return tcp_socket.close();
}
static void close_socket(SslSocket& ssl_socket)
{
    return ssl_socket.lowest_layer().close();
}
static std::string get_ip(const TcpSocket& tcp_socket)
{
    return tcp_socket.remote_endpoint().address().to_string();
}
static std::string get_ip(const SslSocket& ssl_socket)
{
    return ssl_socket.lowest_layer().remote_endpoint().address().to_string();
}
 



AbstractChatSession::AbstractChatSession(AbstractSocket socket, std::shared_ptr<IChatRoom> room)
    : m_client_socket(std::move(socket)), 
    m_hand_shaker{std::dynamic_pointer_cast<IPrivateBroadcaster>(room), this}, //dynamic_cast<IControllableBroadcaster*>(this)},
    m_timer(m_client_socket.get_executor()), m_room(room),
        m_ip{get_ip(m_client_socket)}
{
    m_timer.expires_at(std::chrono::steady_clock::time_point::max());
}
awaitable<size_t> AbstractChatSession::async_read_password(std::string& buff, size_t pass_buff_len, std::string_view delim)
{
    size_t n = co_await asio::async_read_until(m_client_socket, asio::dynamic_buffer(buff, pass_buff_len), delim, asio::use_awaitable);
    co_return n;
}

std::string_view AbstractChatSession::ip() const 
{
    return std::string_view{m_ip};
}

void AbstractChatSession::start()
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


    asio::co_spawn(m_client_socket.get_executor(), [this]
    {
        return writer();
    },
    asio::detached);
}

void AbstractChatSession::deliver(const std::string& msg)
{
    m_write_msgs.push_back(msg);
    m_timer.cancel_one();
}

awaitable<void> AbstractChatSession::reader()
{
    //m_room->join_public(this);

    std::string read_msg;
    try
    { 
        while(is_open(m_client_socket))
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
void AbstractChatSession::log_error(std::string_view from, const std::exception& e) const 
{
    std::println("[{}(): ip: {}: <Terminated with>: {}]", from, m_ip, e.what());
}
awaitable<void> AbstractChatSession::writer()
{
    try
    {
        while(is_open(m_client_socket))
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
void AbstractChatSession::stop()
{
    if (!m_stopped)
    {
        std::println("[from: {}: is stopped]", m_ip);
        m_room->leave(this);
        close_socket(m_client_socket);
        m_timer.cancel();
        m_stopped = true;
    }
}
