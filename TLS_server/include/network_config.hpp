#include "json_loader.hpp"
#include <cctype>
#include <ranges>
#include <string_view>


using namespace std::ranges::views;


namespace network_config 
{
    constexpr std::string_view CONFIG_FILENAME{"chat_app_server_config.json"}; 
    
    enum class ConnectionType 
    {
        Tcp,
        Ssl,
    };
    inline std::string_view connection_to_sv(ConnectionType connection){if(connection==ConnectionType::Ssl)return "Ssl"; else return "tcp";};

    struct NetworkConfig
    {
        std::string path_to_certificates;
        std::string password;
        std::string root_subdir;
        ConnectionType connection_type;
        uint16_t port;
    };


    namespace idx
    {
        inline constexpr size_t PathToCertificates = 0;
        inline constexpr size_t Password           = 1;
        inline constexpr size_t ClientCAsubdir     = 2;
        inline constexpr size_t ConnectionType     = 3;
        inline constexpr size_t Port               = 4;
    }

    constexpr inline std::array<std::string_view, 5> KEYS
    {
        "\"path/to/certificates\"",
        "\"password\"",
        "\"client root CA subdir\"",
        "\"connection type\"",
        "\"port\"",
    };


    inline auto build_network_config(const fs::path& path_to_json) -> Result<NetworkConfig, std::string>
    {
        auto json_dict_res = json_loader::build_json_dict(path_to_json, KEYS);
        if(!json_dict_res)
        {
            return std::unexpected{json_dict_res.error()};
        }
        JsonDict& json_dict {json_dict_res.value()};

        std::stringstream ss{json_dict.at(KEYS[idx::Port])};
        uint16_t port = 0;
        if(!(ss >> port))
        {
            return std::unexpected{std::string{"Failed to parse port"}};
        }

        std::string_view connection_sv = json_dict.at(KEYS[idx::ConnectionType]);

        auto parsed_connection = connection_sv | transform([](char c){return std::tolower(c);});
        auto connection_str    = std::ranges::to<std::string>(parsed_connection);

        if(!connection_str.contains("tcp") && !connection_str.contains("ssl") && !connection_str.contains("tls"))
        {
            return std::unexpected{std::string{"Connection type must be: \"tcp\" or \"ssl\" (tls being an alias)"}};
        }
        
        if(connection_str.size() > 3)
        {
            return std::unexpected{std::string{"Connection type must be: \"tcp\" or \"ssl\" (tls being an alias)"}};
        }
        
        ConnectionType connection_type{ConnectionType::Tcp};

        if(connection_str.compare("tcp") == 0)
        {
            connection_type = ConnectionType::Tcp;
        }
        else
        {
            connection_type = ConnectionType::Ssl;
        }

        NetworkConfig config
        {
            .path_to_certificates = std::move(json_dict.at(KEYS[idx::PathToCertificates])),
            .password             = std::move(json_dict.at(KEYS[idx::Password])),
            .root_subdir          = std::move(json_dict.at(KEYS[idx::ClientCAsubdir])),
            .connection_type      = connection_type,
            .port                 = port,
        };
        return config;
    }
}