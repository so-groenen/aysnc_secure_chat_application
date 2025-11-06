
// Loosely adapted from chat_server.cpp/time_out tutorial by christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "interface_chat_room.hpp"
#include "interface_chat_participant.hpp"
#include "interface_public_broadcaster.hpp"
#include "hand_shaker.hpp"

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <print>
#include <string>
#include <utility>
#include <system_error>
#include <asio/awaitable.hpp>
#include <asio/detached.hpp>
#include <asio/co_spawn.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/read_until.hpp>
#include <asio/read.hpp>
#include <asio/redirect_error.hpp>
#include <asio/signal_set.hpp>
#include <asio/basic_signal_set.hpp>
#include <asio/steady_timer.hpp>
#include <asio/use_awaitable.hpp>
#include <asio/write.hpp>



using namespace std::string_view_literals;
using asio::ip::tcp;
using asio::awaitable;
using asio::co_spawn;
using asio::redirect_error;
 
class ChatRoom : public IChatRoom
{
    std::string m_password{};
    chat_participant::unique_ptr_set m_participants{};
    const size_t m_max_recent_msgs {100};
    std::deque<std::string> m_recent_msgs{};
    std::string_view m_delim = "\r\n"sv;
public:
    explicit ChatRoom(std::string_view password = "LOUVRE") //https://edition.cnn.com/2025/11/06/europe/louvre-password-cctv-security-intl
        : m_password{password}
    {
    } 
    std::string_view password() const override
    {
        return std::string_view(m_password);
    }
    void join(IChatParticipant_ptr participant) override
    {
        std::println("New connection!");

        auto raw_participant = participant.get();       // a bit dangerous!
        m_participants.insert(std::move(participant));
        raw_participant->start();
    }
    void leave(IChatParticipant* participant) override 
    {
        auto it = m_participants.find(participant);
        if (it != m_participants.end())
        {
            m_participants.erase(it);
        }
    }
    void join_public(IChatParticipant* participant) override
    {
        auto it = m_participants.find(participant);
        if (it != m_participants.end())
        {
            for (const auto& msg: m_recent_msgs)
            {   
                auto msg_ = msg;
                for (const auto c : m_delim)
                    msg_.push_back(c);
                
                participant->deliver(msg_); // populate "outbox"
            }
        }
    }
    void deliver_private(const std::string& msg, IPublicBroadcaster* participant) override
    {
        auto chat_participant = dynamic_cast<IChatParticipant*>(participant); 
        auto it = m_participants.find(chat_participant);
        if (it != m_participants.end())
        {
            participant->deliver(msg);
        }
    }
    void deliver(const std::string& msg) override
    {
        m_recent_msgs.push_back(msg);
        while (m_recent_msgs.size() > m_max_recent_msgs)
        {
            m_recent_msgs.pop_front();
        }

        for (auto& participant: m_participants)
        {
            participant->deliver(msg);
        }
    }
};


class ChatSession : public IChatParticipant 
{
    std::string_view m_delim{"\r\n"sv};
    size_t m_read_buff_len{1024};
    tcp::socket m_client_socket;
    asio::steady_timer m_timer;
    HandShaker m_hand_shaker;
    std::shared_ptr<IChatRoom> m_room;
    std::deque<std::string> m_write_msgs;
    std::string m_ip{};
    bool m_stopped{};
public:
    ChatSession(tcp::socket socket, std::shared_ptr<IChatRoom> room)
        : m_client_socket(std::move(socket)), 
        m_hand_shaker{std::dynamic_pointer_cast<IPrivateBroadcaster>(room), dynamic_cast<IControllableBroadcaster*>(this)},
        m_timer(m_client_socket.get_executor()), m_room(room),
         m_ip{m_client_socket.remote_endpoint().address().to_string()}
    {
        m_timer.expires_at(std::chrono::steady_clock::time_point::max());
    }
    awaitable<size_t> async_read_password(std::string& buff, size_t pass_buff_len, std::string_view delim) override
    {
        size_t n = co_await asio::async_read_until(m_client_socket, asio::dynamic_buffer(buff, pass_buff_len), delim, asio::use_awaitable);
        co_return n;
    }

    std::string_view ip() const override 
    {
        return std::string_view{m_ip};
    }
 
    void start() override
    {        
        auto handle_connection_response = [this](bool connection_is_acctepd)
        {
            if (connection_is_acctepd)
            {
                co_spawn(m_client_socket.get_executor(), [this]
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
 
        co_spawn(m_client_socket.get_executor(),[this,handle_connection_response]
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
    void deliver(const std::string& msg) override
    {
        m_write_msgs.push_back(msg);
        m_timer.cancel_one();
    }
private:
    
    awaitable<void> reader()
    {
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
    void log_error(std::string_view from, const std::exception& e) const 
    {
        std::println("[{}(): ip: {}: <Terminated with>: {}]", from, m_ip, e.what());
    }
    awaitable<void> writer()
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
    void stop()
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
};

class TcpChatServer
{
    asio::io_context& m_io;
    asio::ip::tcp::acceptor m_acceptor;
    unsigned short m_port{};
public:
    explicit TcpChatServer(asio::io_context& io, unsigned short port)
        : m_io{io}, m_acceptor{tcp::acceptor(io, {tcp::v4(), port})}, m_port{port}
    {
    }
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
        co_spawn(m_io, [this, chat_room]
        {
            return serve_forever(chat_room);
        }, asio::detached);
    }
private:
    awaitable<void> serve_forever(std::shared_ptr<IChatRoom> chat_room)
    {
        while (true)
        {
            auto client_socket = co_await m_acceptor.async_accept(asio::use_awaitable);
            std::unique_ptr<IChatParticipant> participant = std::make_unique<ChatSession>(std::move(client_socket), chat_room);
            chat_room->join(std::move(participant));
        }
    }
};


//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
    try
    {
        asio::io_context io{};
        unsigned short port{6970};
        std::string_view password{"LOUVRE"};
        TcpChatServer chat_server{io, port};

        std::shared_ptr<IChatRoom> chat_room = std::make_shared<ChatRoom>(password);
        chat_server.start(chat_room);
        std::println("Start serving on {}:{}", chat_server.ip(), chat_server.port());

        io.run();
    }
    catch (std::exception& e)
    {
        std::println("Exception: {} ", e.what());
    }

    return 0;
}