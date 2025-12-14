// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <print>
#include "abstract_chat_server.hpp"
#include "network_config.hpp"
#include "asio/io_context.hpp"
#include "chat_room.hpp"
#include "app_dir_path.hpp"
#include <optional>
#include "ssl_chat_server.hpp"
#include "ssl_loader.hpp"
#include "tcp_chat_server.hpp"

template<typename T>
using Option = std::optional<T>;


using network_config::NetworkConfig;
using network_config::ConnectionType;

int main()
{
    std::println("Welcome to the Async Chat Server");
    std::println("");

    auto application_dir = app_dir_path::get_application_dir_path();
    auto config_res      = network_config::build_network_config(application_dir / network_config::CONFIG_FILENAME);
    if (!config_res)
    {
        std::println("Could not load network config from {}: {}", network_config::CONFIG_FILENAME, config_res.error());
        return EXIT_FAILURE;
    }
    const NetworkConfig& config          = config_res.value();
    const ConnectionType connection_type = config.connection_type;

    std::println("Network config successfully loaded:");
    std::println("* Using connection type: \"{}\"", network_config::connection_to_sv(config.connection_type));
    std::println("* Using password: \"{}\"", config.password);
    std::println("* Using port: \"{}\"",     config.port);


    Option<asio::ssl::context> ssl_context{};
    if(config.connection_type == ConnectionType::Ssl)
    {
        auto certificates_dir   =  application_dir  / config.path_to_certificates;
        auto root_CA_subdir     =  certificates_dir / config.root_subdir;

        std::println("* Looking for public/private keys in: \"{}\"", certificates_dir.filename().c_str());
        std::println("* Looking for client root CAs in: \"{}\"", root_CA_subdir.filename().c_str());
        std::println("");
        auto ssl_context_result = build_security_bundle(certificates_dir, root_CA_subdir)
                                    .and_then(create_sslv23_context);
        if(!ssl_context_result)
        {
            std::println("Could not load certifcates Error: {}", ssl_context_result.error());
            return EXIT_FAILURE;
        }
        ssl_context = std::move(ssl_context_result.value());
        ssl_context.value().set_verify_mode(asio::ssl::verify_peer);
    }

    
    std::println("");
    try
    {
        asio::io_context io{};
        AbstractChatServer_ptr chat_server{};
        switch (config.connection_type)
        {
            case ConnectionType::Tcp:
                chat_server = std::make_unique<TcpChatServer>(io, config.port);
                break;
            case ConnectionType::Ssl:
                chat_server = std::make_unique<SslChatServer>(io, config.port, std::move(ssl_context.value()));
                break;
        }
        auto chat_room = std::make_shared<ChatRoom>(config.password);

        chat_server->start(chat_room);
        std::println("Start serving on {}:{}.", chat_server->ip(), chat_server->port());
        std::println("Chat room log:");
        io.run();
    }
    catch (std::exception& e)
    {
        std::println("Exception: {} ", e.what());
    }
    return 0;
}