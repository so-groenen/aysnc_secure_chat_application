#ifndef SSL_LOADER_HPP

#include <print>
#include <filesystem>
#include <iostream>
#include <asio.hpp>
#include <asio/ssl.hpp>
#include <expected>

namespace fs = std::filesystem;

 
struct SecurityBundle
{
    fs::path private_key_path{};
    fs::path public_key_path{};
    std::vector<fs::path> root_CA_paths{};
};


auto build_security_bundle(const fs::path& certificate_dir, 
                           std::string_view root_CA_subdir,
                           bool should_log = false) -> std::expected<SecurityBundle, std::string>;

auto create_ssl_context(SecurityBundle bundle, asio::ssl::context_base::method ssl_version) -> std::expected<asio::ssl::context, std::string>;
 

#define SSL_LOADER_HPP
#endif