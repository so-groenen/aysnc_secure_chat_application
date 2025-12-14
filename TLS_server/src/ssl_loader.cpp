#include "ssl_loader.hpp"
// #include <iostream>
#include <print>
#include <climits>
#include <unistd.h>

 
static bool is_certificate(const fs::directory_entry& entry)
{
    return entry.path().extension().compare(".crt") == 0 || entry.path().extension().compare(".pem") == 0;
}
static bool is_private_key(const fs::directory_entry& entry)
{
    return (entry.path().extension().compare(".key") == 0);
}



auto build_security_bundle(const fs::path& certificate_dir, const fs::path& root_CA_subdir, bool should_log) -> std::expected<SecurityBundle, std::string>
{
    fs::path server_private_key_path{};
    fs::path server_public_key_path {};
    fs::path client_root_dir_path   {};

    if(!fs::exists(certificate_dir))
    {
        return std::unexpected{"Certificate folder not found"};
    }

    for(auto& entry : fs::directory_iterator{certificate_dir})
    {
        if(entry.is_directory())
        {
            if(entry.path().compare(root_CA_subdir) == 0)
            {
                client_root_dir_path = std::move(entry.path()); // root_CA_subdir is a subid
            }
        }
        if(entry.path().has_extension())
        {
            if(is_certificate(entry))
            {
                server_public_key_path = std::move(entry.path());
            }
            if(is_private_key(entry)) 
            {
                server_private_key_path = std::move(entry.path());
            }
        }
    }

    if(server_private_key_path.empty())
    {
        return std::unexpected{"Could not find private key"};
    }
    if(server_public_key_path.empty())
    {
        return std::unexpected{"Could not find public key"};
    }
    if(client_root_dir_path.empty())
    {
        return std::unexpected{"Could not find root CA directory"};
    }

    if(should_log)
    {
        std::println("Found private key: {}", server_private_key_path.filename().c_str());
        std::println("Found public key: {}", server_public_key_path.filename().c_str());
    }

    std::vector<fs::path> root_CA_paths{};
    for(auto& dir_entry : fs::directory_iterator{client_root_dir_path})
    {
        if(is_certificate(dir_entry))
        {
            if(should_log)
            {
                std::println("Found root CA: {}", dir_entry.path().filename().c_str());
            }
            root_CA_paths.emplace_back(dir_entry.path());
        }
    }

    if(root_CA_paths.empty())
    {
        return std::unexpected{"Could not find root CA certifs"};
    }
    
    SecurityBundle security_bundle = 
    {
        .private_key_path = std::move(server_private_key_path),
        .public_key_path  = std::move(server_public_key_path),
        .root_CA_paths    = std::move(root_CA_paths),
    };

    return security_bundle;
}


auto create_ssl_context(const SecurityBundle& bundle, asio::ssl::context_base::method ssl_version) -> std::expected<asio::ssl::context, std::string>
{   
    if(bundle.public_key_path.empty())
    {
        return std::unexpected{"Bundle does not contain public key"};
    }
    if(bundle.private_key_path.empty())
    {
        return std::unexpected{"Bundle does not contain private key"};
    }
    if(bundle.root_CA_paths.empty())
    {
        return std::unexpected{"Bundle does not contain root CAs"};
    }

    // SecurityBundle bundle = std::move(bundle_);
    assert(!bundle.public_key_path.empty());
    assert(!bundle.private_key_path.empty());
    assert(!bundle.root_CA_paths.empty());

    asio::ssl::context ssl_context{ssl_version};
    ssl_context.use_certificate_file(bundle.public_key_path, asio::ssl::context::pem);
    ssl_context.use_private_key_file(bundle.private_key_path, asio::ssl::context::pem);
     
    for(const auto& root_CA_path : bundle.root_CA_paths)
    {
        try
        {
            ssl_context.load_verify_file(root_CA_path);
        }
        catch(const std::exception& e)
        {
            return std::unexpected{std::format("Could not load file {}: {}", root_CA_path.c_str(), e.what())};
        }
    }
    return ssl_context;
}
auto create_sslv23_context(const SecurityBundle& bundle) -> std::expected<asio::ssl::context, std::string>
{
    return create_ssl_context(bundle, asio::ssl::context::sslv23);
}

 