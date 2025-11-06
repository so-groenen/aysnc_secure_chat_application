
// Loosely adapted from chat_server.cpp/time_out tutorial by christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

 
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <print>
#include <memory>
#include <set>
#include <unordered_set>
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

//----------------------------------------------------------------------

class IPublicBroadcaster
{
public:
    virtual ~IPublicBroadcaster() {}
    virtual void deliver(const std::string& msg) = 0;
};
typedef std::shared_ptr<IPublicBroadcaster> IBroadcaster_ptr;




//----------------------------------------------------------------------
class IControllableBroadcaster : public IPublicBroadcaster
{
public:
    virtual ~IControllableBroadcaster() {}
    virtual std::string_view ip() const = 0;
    virtual awaitable<size_t> async_read_password(std::string& buff, size_t pass_buff_len, std::string_view delim) = 0;
    // virtual awaitable<size_t> async_respond(std::string buff, size_t pass_buff_len, std::string_view delim) = 0;
};

namespace chat_participant
{
    class IChatParticipant : public IControllableBroadcaster
    {
    public:
        virtual ~IChatParticipant() {}
        virtual void start() = 0;
        // virtual std::string_view ip() const = 0;
    };
    typedef std::unique_ptr<IChatParticipant> IChatParticipant_ptr;

    struct ChatPartHash_
    {
        using is_transparent = void;

        auto operator()(IChatParticipant* p) const { return std::hash<IChatParticipant*>{}(p); }
        auto operator()(const IChatParticipant_ptr& p) const { return std::hash<IChatParticipant*>{}(p.get()); }
    };

    struct ChartPartEqual_
    {
        using is_transparent = void;
        template <typename LHS, typename RHS>
        auto operator()(const LHS& lhs, const RHS& rhs) const
        {
            return AsPtr(lhs) == AsPtr(rhs);
        }
    private:
        static const IChatParticipant* AsPtr(const IChatParticipant* p) { return p; }
        static const IChatParticipant* AsPtr(const IChatParticipant_ptr& p) { return p.get(); }
    };

    using unique_ptr_set = std::unordered_set<IChatParticipant_ptr, ChatPartHash_, ChartPartEqual_>;
}

using chat_participant::IChatParticipant;
using chat_participant::IChatParticipant_ptr;

class IPrivateBroadcaster
{
public:
    virtual ~IPrivateBroadcaster() {}
    virtual void deliver_private(const std::string& msg, IPublicBroadcaster* participant) = 0; //should be IPublicBroadcaster
    virtual std::string_view password() const = 0;
};


class IChatRoom : public IPublicBroadcaster, public IPrivateBroadcaster
{
public:
    virtual ~IChatRoom() {}
    virtual void join(IChatParticipant_ptr participant) = 0;
    virtual void join_public(IChatParticipant* participant) = 0;
    // virtual void deliver_private(const std::string& msg, IChatParticipant* participant) = 0;
    virtual void leave(IChatParticipant* participant) = 0;
};

class ChatRoom : public IChatRoom
{
    std::string m_password{"louvre"};
    chat_participant::unique_ptr_set m_participants{};
    const size_t m_max_recent_msgs {100};
    std::deque<std::string> m_recent_msgs{};
public:
    std::string_view password() const override
    {
        return std::string_view(m_password);
    }
    void join(IChatParticipant_ptr participant) override
    {
        std::println("new connection!");

        auto raw_participant = participant.get();       // a bit dangerous!
        m_participants.insert(std::move(participant));
        
        // for (auto msg: m_recent_msgs)
        // {
        //     raw_participant->deliver(msg); // populate "outbox"
        // }
        raw_participant->start();
    }

    void leave(IChatParticipant* participant) override // this will call participant->stop
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
                auto msg_ = msg + "\n";
                participant->deliver(msg_); // populate "outbox"
            }
        }
    }
    void deliver_private(const std::string& msg, IPublicBroadcaster* participant) override // this will call participant->stop
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
#include <asio/experimental/awaitable_operators.hpp>

using namespace asio::experimental::awaitable_operators;

class ChatSession;

class HandShaker
{
    // asio::steady_timer* m_timeout_timer{};
    // asio::any_io_executor& m_executor;
    std::shared_ptr<IPrivateBroadcaster> m_private_room{};
    IControllableBroadcaster* m_participant{}; 
    size_t m_timeout_secs{5};
    size_t m_write_response_await_ms{250};
    bool m_should_accept{};
    bool m_is_terminating{};
public:
    explicit HandShaker(std::shared_ptr<IPrivateBroadcaster> room, IControllableBroadcaster* participant)
        : m_private_room{room}, m_participant{participant} 
    {
    } 
private:
    void log_error(std::string_view from, const std::exception& e) const 
    {
        std::println("[{}(): ip: {}: <Terminated with>: {}]", from, m_participant->ip(), e.what());
    }
    awaitable<bool> is_password_correct()
    {
        std::string_view pass_delim = "\r\n"sv;
        size_t pass_buff_len        = 32;
        std::string pass_buff;
        try
        { 
            size_t n = co_await m_participant->async_read_password(pass_buff, pass_buff_len, pass_delim);
            std::string_view response{pass_buff};
            if (n <= pass_delim.length())
            {
                std::print("[from: {}; password]: {} => ", m_participant->ip(), response);
                co_return false;
            }    
            response = response.substr(0, n - pass_delim.length());

            std::print("[from: {}; password]: {} => ", m_participant->ip(), response);
            if (m_private_room->password().compare(response) != 0)
            {
                co_return false;
            }
            co_return true;
        }

        catch (const asio::system_error& e) 
        {
            if (e.code() == asio::error::operation_aborted) 
            {
                log_error("is_password_correct.operation_aborted"sv, e);
            }
            else 
            {
                log_error("is_password_correct"sv, e);
                throw;
            }
            co_return false;
        }
    }
    awaitable<void> client_first_response()
    {
        m_should_accept = co_await is_password_correct();
    }
    bool is_rejected_session() const
    {
        return !m_should_accept;
    }
    awaitable<void> perform_handshake(asio::steady_timer& timer)
    {
        co_await client_first_response();
        if(is_rejected_session() && !m_is_terminating) // wrong answer recieved
        {
            timer.cancel();
            co_await reject_connection(); // wait for writer to dispatch message & close if necessary
        }
        if(!is_rejected_session())
        {
            timer.cancel();
            co_await accept_connection();
        }
    }
    awaitable<void> reject_connection()
    {
        if(!m_is_terminating)
        {
            m_is_terminating = true;

            std::string server_response = "[reject]";
            std::println("sending: {}", server_response);
            m_private_room->deliver_private(server_response, m_participant);

            std::println("Terminating {} [waiting {}ms to flush writer]", m_participant->ip(), m_write_response_await_ms);
            asio::steady_timer t{(co_await asio::this_coro::executor), asio::chrono::microseconds(m_write_response_await_ms)};
            co_await t.async_wait(asio::use_awaitable);

            std::println("[from: {}: stoping...]", m_participant->ip());
            throw asio::system_error(asio::error::connection_refused);
        }
    }
    awaitable<void> accept_connection()
    {
        if(!m_is_terminating)
        {
            std::string server_response{"[accept]"};
            m_private_room->deliver_private(server_response, m_participant);
            std::println("{}", server_response);

            // std::println("Terminating {} [waiting {}ms to flush writer]", m_ip, timeout_ms);
            asio::steady_timer t{(co_await asio::this_coro::executor), asio::chrono::microseconds(m_write_response_await_ms)};
            co_await t.async_wait(asio::use_awaitable);
        }
    }
    awaitable<void> timeout_connection()
    {
        if(!m_is_terminating)
        {
            m_is_terminating = true;
            std::string server_response{"[Timeout]"};
            std::println("sending: {}", server_response);
            m_private_room->deliver_private(server_response, m_participant);

            std::println("Terminating {} [waiting {}ms to flush writer]", m_participant->ip(), m_write_response_await_ms);
            asio::steady_timer t{(co_await asio::this_coro::executor), asio::chrono::microseconds(m_write_response_await_ms)};
            co_await t.async_wait(asio::use_awaitable);

            std::println("[from: {}: stoping...]", m_participant->ip());
            throw asio::system_error(asio::error::timed_out); 
        }
    }
    
    awaitable<void> countdown_timer(asio::steady_timer& timer)
    {
        std::println("[from: {}]: Waiting {}s", m_participant->ip(), m_timeout_secs);
        try
        {
            co_await timer.async_wait(asio::use_awaitable);
            std::println("[from: {}]: done Waiting {}s", m_participant->ip(), m_timeout_secs);
            
            if (is_rejected_session() && !m_is_terminating)
            {
                co_await timeout_connection();
            }
        }
        catch (const asio::system_error& e) 
        {
            if (e.code() == asio::error::operation_aborted) 
            {
                // log_error("time_out.cancel"sv, e);
            }
            else if (e.code() == asio::error::connection_refused) 
            {
                log_error("time_out.connection_refused"sv, e);
            }
            else if (e.code() == asio::error::timed_out) 
            {
                throw; //rethrow to handler
            }
            else
            {
                log_error("time_out/other", e);
                throw;
            }
        }
        catch(const std::exception& e)
        {
            log_error("time_out/???", e);
            throw;
        }
    }    
public:
    awaitable<bool> try_handshake()
    {
        asio::steady_timer timer{co_await asio::this_coro::executor, std::chrono::seconds(m_timeout_secs)};
        try
        {
            co_await (perform_handshake(timer) && countdown_timer(timer));  
        }
        catch (const asio::system_error& e) 
        {
            if (e.code() == asio::error::operation_aborted) 
            {
                log_error("try_hand_shake/operation_aborted"sv, e);
            }
            else if (e.code() == asio::error::connection_refused) 
            {
                log_error("try_hand_shake/connection_refused"sv, e);
            }
            else if (e.code() == asio::error::timed_out) 
            {
                log_error("try_hand_shake/timed_out"sv, e);
            }
            else
            {
                log_error("try_hand_shake/other", e);
            }
            co_return false;
        }
        co_return true;
    }
    template<typename AcceptanceToken, typename RejectionToken>
    awaitable<void> try_connect(AcceptanceToken&& accept_callback, RejectionToken&& reject_callback)
        requires std::invocable<AcceptanceToken> && std::invocable<RejectionToken>
    {
        asio::steady_timer timer{co_await asio::this_coro::executor, std::chrono::seconds(m_timeout_secs)};
        try
        {
            co_await (perform_handshake(timer) && countdown_timer(timer));  
        }
        catch (const asio::system_error& e) 
        {
            if (e.code() == asio::error::operation_aborted) 
            {
                log_error("try_connect/operation_aborted"sv, e);
            }
            else if (e.code() == asio::error::connection_refused) 
            {
                log_error("try_connect/connection_refused"sv, e);
            }
            else if (e.code() == asio::error::timed_out) 
            {
                log_error("try_connect/timed_out"sv, e);
            }
            else
            {
                log_error("try_connect/other", e);
            }
            reject_callback();
            co_return;
        }
        accept_callback();
        co_return;
    }
    template<typename CompletionToken>
    awaitable<void> try_connect(CompletionToken&& completion_token) requires std::invocable<CompletionToken,bool>
    {
        asio::steady_timer timer{co_await asio::this_coro::executor, std::chrono::seconds(m_timeout_secs)};
        try
        {
            co_await (perform_handshake(timer) && countdown_timer(timer));  
        }
        catch (const asio::system_error& e) 
        {
            if (e.code() == asio::error::operation_aborted) 
            {
                log_error("try_connect/operation_aborted"sv, e);
            }
            else if (e.code() == asio::error::connection_refused) 
            {
                log_error("try_connect/connection_refused"sv, e);
            }
            else if (e.code() == asio::error::timed_out) 
            {
                log_error("try_connect/timed_out"sv, e);
            }
            else
            {
                log_error("try_connect/other", e);
            }
            completion_token(false);
            co_return;
        }
        completion_token(true);
        co_return;
    }
};

class ChatSession : public IChatParticipant 
{
    tcp::socket m_client_socket;
    asio::steady_timer m_timer;
    HandShaker m_hand_shaker;
    std::shared_ptr<IChatRoom> m_room;
    // std::optional<bool> m_status{};
    std::deque<std::string> m_write_msgs;
    std::string m_ip{};
    bool m_stopped{};
    // bool m_is_terminating{};
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
    // awaitable<void> handle_handshake()
    // {
    //     bool is_success = co_await m_hand_shaker.try_handshake();
    //     if(!is_success)
    //     {
    //         std::println("chat_session terminate");
    //         stop();
    //         co_return;
    //     }   

    //     m_room->join_public(this);  
    //     co_spawn(m_client_socket.get_executor(),[this]
    //     {
    //         return reader();
    //     },
    //     asio::detached);
    // }
    // awaitable<std::optional<bool>> is_password_correct()
    // {
    //     std::string_view pass_delim = "\r\n"sv;
    //     size_t pass_buff_len        = 32;
    //     std::string read_msg;
    //     try
    //     { 
    //         std::size_t n = co_await asio::async_read_until(m_client_socket, asio::dynamic_buffer(read_msg, pass_buff_len), pass_delim, asio::use_awaitable);
    //         std::string_view response{read_msg};
    //         if (n <= pass_delim.length())
    //         {
    //             std::print("[from: {}; password]: {} => ", m_ip, response);
    //             co_return false;
    //         }    
    //         response = response.substr(0, n - pass_delim.length());

    //         std::print("[from: {}; password]: {} => ", m_ip, response);
    //         if (m_room->password().compare(response) != 0)
    //         {
    //             co_return false;
    //         }
    //         co_return true;
    //     }

    //     catch (const asio::system_error& e) 
    //     {
    //         if (e.code() == asio::error::operation_aborted) 
    //         {
    //             // log_error("is_password_correct.operation_aborted"sv, e);
    //         }
    //         else 
    //         {
    //             log_error("is_password_correct"sv, e);
    //             throw;
    //         }
    //         co_return false;
    //     }
    // }
    // awaitable<void> client_first_response()
    // {
    //     m_status = co_await is_password_correct();
    // }
    // bool is_rejected_session() const
    // {
    //     return (m_status.has_value() && m_status.value() == false) || !m_status.has_value();
    // }
    // awaitable<void> perform_handshake(asio::steady_timer& timer)
    // {
    //     co_await client_first_response();
    //     if(is_rejected_session() && !m_is_terminating) // wrong answer recieved
    //     {
    //         timer.cancel();
    //         co_await reject_connection(); // wait for writer to dispatch message & close if necessary
    //     }
    //     if(!is_rejected_session())
    //     {
    //         timer.cancel();
    //         co_await accept_connection();
    //     }
    // }
    // awaitable<void> reject_connection()
    // {
    //     if(!m_is_terminating)
    //     {
    //         m_is_terminating = true;

    //         std::string server_response{"[Reject]"};
    //         std::println("sending: {}", server_response);
    //         m_room->deliver_private(server_response, this);

    //         size_t timeout_ms{100};
    //         std::println("Terminating {} [waiting {}ms to flush writer]", m_ip, timeout_ms);
    //         asio::steady_timer t{m_client_socket.get_executor(), asio::chrono::microseconds(timeout_ms)};
    //         co_await t.async_wait(asio::use_awaitable);

    //         std::println("[from: {}: stoping...]", m_ip);
    //         throw asio::system_error(asio::error::connection_refused);
    //     }
    // }
    // awaitable<void> accept_connection()
    // {
    //     if(!m_is_terminating)
    //     {
    //         std::string server_response{"[Accept]"};
    //         m_room->deliver_private(server_response, this);
    //         std::println("{}", server_response);

    //         size_t timeout_ms{250};
    //         // std::println("Terminating {} [waiting {}ms to flush writer]", m_ip, timeout_ms);
    //         asio::steady_timer t{m_client_socket.get_executor(), asio::chrono::microseconds(timeout_ms)};
    //         co_await t.async_wait(asio::use_awaitable);
    //     }
    // }
    // awaitable<void> timeout_connection()
    // {
    //     if(!m_is_terminating)
    //     {
    //         m_is_terminating = true;

    //         std::string server_response{"[Timeout]"};
    //         std::println("sending: {}", server_response);
    //         m_room->deliver_private(server_response, this);

    //         size_t timeout_ms{250};
    //         std::println("Terminating {} [waiting {}ms to flush writer]", m_ip, timeout_ms);
    //         asio::steady_timer t{m_client_socket.get_executor(), asio::chrono::microseconds(timeout_ms)};
    //         co_await t.async_wait(asio::use_awaitable);

    //         std::println("[from: {}: stoping...]", m_ip);
    //         throw asio::system_error(asio::error::timed_out); 
    //     }
    // }
    
    // awaitable<void> time_out(asio::steady_timer& timer, size_t timeout_s)
    // {
    //     std::println("[from: {}]: Waiting {}s", m_ip, timeout_s);
    //     try
    //     {
    //         co_await timer.async_wait(asio::use_awaitable);
    //         std::println("[from: {}]: done Waiting {}s", m_ip, timeout_s);
            
    //         if (is_rejected_session() && !m_is_terminating)
    //         {
    //             co_await timeout_connection();
    //         }
    //     }
    //     catch (const asio::system_error& e) 
    //     {
    //         if (e.code() == asio::error::operation_aborted) 
    //         {
    //             // log_error("time_out.cancel"sv, e);
    //         }
    //         else if (e.code() == asio::error::connection_refused) 
    //         {
    //             log_error("time_out.connection_refused"sv, e);
    //         }
    //         else if (e.code() == asio::error::timed_out) 
    //         {
    //             throw;
    //         }
    //         else
    //         {
    //             log_error("time_out/other", e);
    //             throw;
    //         }
    //     }
    //     catch(const std::exception& e)
    //     {
    //         log_error("time_out/???", e);
    //         throw;
    //     }
    // }    
    // // awaitable<void> try_handshake()
    // {
    //     size_t timeout_s{5};
    //     asio::steady_timer timer{m_client_socket.get_executor(), asio::chrono::seconds(timeout_s)};
    //     try
    //     {
    //         co_await (perform_handshake(timer) && time_out(timer, timeout_s));  
    //     }
    //     catch (const asio::system_error& e) 
    //     {
    //         if (e.code() == asio::error::operation_aborted) 
    //         {
    //             log_error("try_hand_shake/operation_aborted"sv, e);
    //         }
    //         else if (e.code() == asio::error::connection_refused) 
    //         {
    //             log_error("try_hand_shake/connection_refused"sv, e);
    //         }
    //         else if (e.code() == asio::error::timed_out) 
    //         {
    //             log_error("try_hand_shake/timed_out"sv, e);
    //         }
    //         else
    //         {
    //             log_error("try_hand_shake/other", e);
    //         }
    //         stop();
    //         co_return;
    //     }
    //     catch(const std::exception& e)
    //     {
    //         log_error("try_hand_shake"sv, e);
    //         stop();
    //         co_return;
    //     }
        // std::string server_response{"[Accept]: yep yep yep!"};
        // std::println("sending: {}", server_response);

        // size_t timeout_ms{250};
        // asio::steady_timer t{m_client_socket.get_executor(), asio::chrono::microseconds(timeout_ms)};
        // m_room->deliver_private(server_response, this);
        // co_await t.async_wait(asio::use_awaitable);

        // m_room->join_public(this);  
        // co_spawn(m_client_socket.get_executor(),[this]
        // {
        //     return reader();
        // },
        // asio::detached);

        // co_spawn(m_client_socket.get_executor(), [this]
        // {
        //     return writer();
        // },
        // asio::detached);

    // }


    awaitable<void> reader()
    {
        std::string_view delim = "\r\n"sv;
        std::string read_msg;

        // try
        // {
        //     co_await (perform_handshake() && time_out());  
        // }
        // catch(const std::exception& e)
        // {
        //     log_error("hand_shake"sv, e);
        //     stop();
        //     co_return;
        // }
        
        
        try
        { 
            while(m_client_socket.is_open())
            {
                std::size_t n = co_await asio::async_read_until(m_client_socket, asio::dynamic_buffer(read_msg, 1024), delim, asio::use_awaitable);
                std::string_view response{read_msg};
                
                response = response.substr(0, response.find_last_of(delim)-1); // assumes it exits etc..
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
    // c++ coroutine + async server
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
        TcpChatServer chat_server{io, port};
        std::shared_ptr<IChatRoom> chat_room = std::make_shared<ChatRoom>();

        chat_server.start(chat_room);
        std::println("Start serving on {}:{}", chat_server.ip(), chat_server.port());

        io.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}