#ifndef SSL_CHAT_SESSION_HPP
#define SSL_CHAT_SESSION_HPP

// #include "interface_ssl_chat_participant.hpp"
// #include "interface_ssl_chat_room.hpp"
// #include "interface_private_broadcaster.hpp"

#include "interface_chat_participant.hpp"
#include "interface_chat_room.hpp"
#include "interface_private_broadcaster.hpp"
#include "interface_ssl_broadcaster.hpp"


#include "ssl_hand_shaker.hpp"

#include "asio/awaitable.hpp"
#include "asio/detached.hpp"
#include "asio/co_spawn.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/read_until.hpp"
#include "asio/read.hpp"
#include "asio/redirect_error.hpp"
// #include "asio/signal_set.hpp"
// #include "asio/basic_signal_set.hpp"
#include "asio/steady_timer.hpp"
#include "asio/use_awaitable.hpp"
#include "asio/write.hpp"
#include "asio/ssl.hpp"

using namespace std::string_view_literals;
using asio::ip::tcp;
using asio::awaitable;
using asio::redirect_error;
using SslSocket = asio::ssl::stream<tcp::socket>;

class SslChatSession : public IChatParticipant, public ISslBroadcaster //public ISecureSocketLayer
{
    std::string_view m_delim{"\r\n"sv};
    size_t m_read_buff_len{1024};
    SslSocket m_ssl_socket;
    asio::steady_timer m_timer;
    SslHandShaker m_hand_shaker;
    std::shared_ptr<IChatRoom> m_room;
    std::deque<std::string> m_write_msgs;
    std::string m_ip{};
    bool m_stopped{};

    public:
    SslChatSession(SslSocket socket, std::shared_ptr<IChatRoom> room);

    // Controllable (ie, password) interface
    awaitable<size_t> async_read_password(std::string& buff, size_t pass_buff_len, std::string_view delim) override;
    
    // Secure Socket Interface
    awaitable<void> async_perform_ssl_handshake() override;
    
    // Chat participant:
    std::string_view ip() const override ;
    void start() override;
    void deliver(const std::string& msg) override;

private:
    awaitable<void> reader();
    void log_error(std::string_view from, const std::exception& e) const ;
    awaitable<void> writer();
    void stop();
};

#endif