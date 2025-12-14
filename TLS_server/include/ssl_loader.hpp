#ifndef SSL_LOADER_HPP


#include <asio.hpp>
#include <asio/ssl.hpp>
#include <expected>
#include <filesystem>

namespace fs = std::filesystem;

 
struct SecurityBundle
{
    fs::path private_key_path{};
    fs::path public_key_path{};
    std::vector<fs::path> root_CA_paths{};
};
fs::path get_application_path();
 
auto build_security_bundle(const fs::path& certificate_dir, 
                           const fs::path& root_CA_subdir,
                           bool should_log = true) -> std::expected<SecurityBundle, std::string>;

auto create_ssl_context(const SecurityBundle& bundle, asio::ssl::context_base::method ssl_version) -> std::expected<asio::ssl::context, std::string>;
auto create_sslv23_context(const SecurityBundle& bundle) -> std::expected<asio::ssl::context, std::string>;

#define SSL_LOADER_HPP
#endif