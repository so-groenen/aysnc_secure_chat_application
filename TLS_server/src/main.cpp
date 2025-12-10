
// Loosely adapted from chat_server.cpp/time_out tutorial by christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

 
#include <print>
#include <fstream>
#include <system_error>
#include "asio/io_context.hpp"
#include "asio/basic_signal_set.hpp"

#include "chat_room.hpp"
// #include "tcp_chat_server.hpp"
#include "ssl_chat_server.hpp"
#include "ssl_loader.hpp"
 

std::optional<std::string> get_password(const std::string& path)
{
    std::ifstream password_file{path, std::ifstream::binary};
    if(!password_file.is_open())
    {
        return std::nullopt;
    }

    std::string password{};
    std::getline(password_file, password);
    return password;
}



int main()
{
    // Port 
    unsigned short port{6970};


    /// Setup SSL certificates ////
    fs::path certificate_dir       {"certificates"};
    std::string_view root_CA_subdir{"client_root_CAs"};
    bool should_log                {true};
    auto ssl_contest_result = build_security_bundle(certificate_dir, root_CA_subdir, should_log)
                                .and_then([](const auto& bundle){return create_ssl_context(bundle, asio::ssl::context::sslv23);});
    if(!ssl_contest_result)
    {
        std::println("Could not load certifcates Error: {}", ssl_contest_result.error());
        return EXIT_FAILURE;
    }
    asio::ssl::context ssl_context = std::move(ssl_contest_result.value());
    ssl_context.set_verify_mode(asio::ssl::verify_peer);
 

    /// Get password ///
    auto password_res = get_password("password.txt");
    if(!password_res.has_value())
    {
        std::println("Could not get password");
        return EXIT_FAILURE;
    }

    std::println("Using password: \"{}\"", password_res.value());

    try
    {
        asio::io_context io{};

        SslChatServer chat_server{io, port, std::move(ssl_context)};

        auto chat_room = std::make_shared<ChatRoom>(password_res.value());

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