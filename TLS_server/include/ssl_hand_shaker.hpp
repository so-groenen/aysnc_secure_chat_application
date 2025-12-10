#ifndef SSL_HAND_SHAKER_HPP
#define SSL_HAND_SHAKER_HPP

#include <iostream>
#include <print>
#include <random>
#include <format>

#include "asio/steady_timer.hpp"
#include "asio/use_awaitable.hpp"
#include "asio/experimental/awaitable_operators.hpp"
#include "interface_ssl_broadcaster.hpp"
#include "interface_private_broadcaster.hpp"

using namespace std::string_view_literals;
using namespace asio::experimental::awaitable_operators; // to be able to write "co_await (coro1() && coro2())""

// class ISslHandshaker 
// {
// public:
//     ~ISslHandshaker() = default;
//     virtual void attach(ISslBroadcaster* participant) = 0; // controllable (password protected) + SSL layer + broadcaster (exposes async read/write)
// };

class SslHandShaker
{
    std::string_view m_pass_delim {"\r\n"sv};
    size_t m_pass_buff_len{1024};
    std::string m_session_id{""}; //gets init in the constructor // SESSION ID
    std::string_view m_reject_resp{"[reject]"sv};
    std::string_view m_timeout_resp{"[timeout]"sv};

    std::shared_ptr<IPrivateBroadcaster> m_private_room{};
    ISslBroadcaster* m_participant{}; 
    size_t m_timeout_secs{5};
    size_t m_write_response_await_ms{250};
    bool m_should_accept{};
    bool m_is_terminating{};
public:
    explicit SslHandShaker(std::shared_ptr<IPrivateBroadcaster> room, ISslBroadcaster* participant)
        : m_private_room{room}, m_participant{participant} 
    {
        auto device  = std::random_device{};
        m_session_id = std::format("{}", device());
    } 
 

private:
    void log_error(std::string_view from, const std::exception& e) const 
    {
        std::println("[{}(): ip: {}: <Terminated with>: {}]", from, m_participant->ip(), e.what());
    }
    awaitable<bool> is_password_correct()
    {
        std::string pass_buff;
        try
        { 
            size_t n = co_await m_participant->async_read_password(pass_buff, m_pass_buff_len, m_pass_delim);
            std::string_view response{pass_buff};
            if (n <= m_pass_delim.length())
            {
                std::print("[from: {}; password]: \"{}\" [expect:\"{}\"] => ", m_participant->ip(), response, m_private_room->password());
                co_return false;
            }    
            response = response.substr(0, n - m_pass_delim.length());

            std::print("[from: {}; password]: \"{}\" [expect:\"{}\"] => ", m_participant->ip(), response, m_private_room->password());
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
    awaitable<void> perform_ssl_handshake( )
    {
        std::println("Starting SSL handshake");

        co_await m_participant->async_perform_ssl_handshake();

        std::println("Finished SSL handshake");
    }

    awaitable<void> perform_password_handshake(asio::steady_timer& timer)
    {
        co_await client_first_response();
        if(is_rejected_session() && !m_is_terminating)  
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

            std::string server_response{m_reject_resp};
            std::println("sending: {}", m_reject_resp);
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
            std::string server_response{m_session_id};
            m_private_room->deliver_private(server_response, m_participant);
            std::println("Session id: {}", m_session_id);

            asio::steady_timer timer{(co_await asio::this_coro::executor), asio::chrono::microseconds(m_write_response_await_ms)};
            co_await timer.async_wait(asio::use_awaitable);
        }
    }
    awaitable<void> timeout_connection()
    {
        if(!m_is_terminating)
        {
            m_is_terminating = true;
            std::string server_response{m_timeout_resp};
            std::println("sending: {}", m_timeout_resp);
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
                log_error("time_out.cancel"sv, e);
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
  
    template<typename CompletionToken>
    awaitable<void> try_connect(CompletionToken&& completion_token) requires std::invocable<CompletionToken,bool>
    {
        asio::steady_timer timer{co_await asio::this_coro::executor, std::chrono::seconds(m_timeout_secs)};
        try
        {
            co_await perform_ssl_handshake();
            co_await (perform_password_handshake(timer) && countdown_timer(timer));  
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
        asio::steady_timer flush_session_id_timer{(co_await asio::this_coro::executor), asio::chrono::milliseconds(m_write_response_await_ms)};
        co_await flush_session_id_timer.async_wait(asio::use_awaitable);
        
        completion_token(true);
        co_return;
    }
};



#endif