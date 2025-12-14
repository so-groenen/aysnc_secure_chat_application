#include "json_loader.hpp"
#include <fstream>
#include <vector>
#include <ranges>
#include <span>
using namespace std::ranges::views;
using namespace std::string_view_literals;


static constexpr std::string_view trim_json(std::string_view sv) 
{
    size_t first_non_delim {sv.find_first_not_of(" {")};
    if (first_non_delim == std::string_view::npos)
    {
        return "";
    }
    sv.remove_prefix(first_non_delim);

    size_t last_non_delim {sv.find_last_not_of(" },")};
    sv.remove_suffix(sv.size() - (last_non_delim + 1));
    return sv;
}
static constexpr std::string_view trim_space(std::string_view sv) 
{
    size_t first_non_quote {sv.find_first_not_of(" ")};
    if (std::string_view::npos == first_non_quote)
    {
        return "";
    }
    sv.remove_prefix(first_non_quote);

    size_t last_non_quote {sv.find_last_not_of(" ")};
    sv.remove_suffix(sv.size() - (last_non_quote + 1));
    return sv;
}
static constexpr bool is_field(std::string_view sv) // use after white space trim!!
{
    size_t first_quote {sv.find_first_of("\"")};
    if(first_quote != 0)
    {
        return false;
    }

    size_t last_quote {sv.find_last_of("\"")};
    if(last_quote != sv.size()-1)
    {
        return false;
    }

    return true;
}

static constexpr std::string_view trim_field(std::string_view sv) 
{
    size_t first_non_quote {sv.find_first_not_of(" \"")};
    if (std::string_view::npos == first_non_quote)
    {
        return "";
    }
    sv.remove_prefix(first_non_quote);

    size_t last_non_quote {sv.find_last_not_of(" \"")};
    sv.remove_suffix(sv.size() - (last_non_quote + 1));
    return sv;
}

using JsonDict = std::unordered_map<std::string_view, std::string>;

namespace json_loader
{
    auto build_json_dict(const fs::path& json_path, std::span<const std::string_view> keys) -> Result<JsonDict, std::string>
    {
        std::ifstream json{json_path};
        if(!json.is_open())
        {
            return std::unexpected{std::string{"Could not open json config file"}};
        }
        std::vector<std::string> lines{};
        std::string buffer{};
        while (std::getline(json, buffer))
        {
            lines.emplace_back(std::move(buffer));
        }

        JsonDict json_dict{};

        auto to_sv     = [](const auto& st){return std::string_view{st};};
        auto has_colon = [](const auto& sv){return sv.contains(":");};
 
        auto parsed    = lines | filter(has_colon) | transform(trim_json);
        
        for(const auto& line : parsed )  
        {
            for(const auto& key : keys)
            {
                if(line.contains(key) && !json_dict.contains(key))
                {
                    auto token = line | split(":"sv) | transform(to_sv) | drop(1) | transform(trim_space) | filter(is_field) | transform(trim_field);
                    auto value = token.front();
                    // std::println("field: {}", fr);
                    if(!value.empty())
                        json_dict.emplace(key, value);
                }
            }
        }

        // for(const auto& [k,v] : dict)
        // {
        //     std::println("({}):({})", k,v);
        // }
        if(json_dict.size() != keys.size())
        {
            return std::unexpected{std::string{"Json fields does not match requiered keys"}};
        }
        return json_dict;
    }
}


// int main()
// {
//     fs::path json_path{"test.json"};
//     std::ifstream file{json_path};
    
//     std::println("is open :{}", file.is_open());
//     auto config_res = build_network_config(json_path);
//     if(!config_res)
//     {
//         std::println("{}", config_res.error());
//         exit(EXIT_FAILURE);
//     }

//     const NetworkConfig& config = config_res.value();
//     std::println("Got: \npassword: {},\npath_to_cert: {},\nport: {},\nroot subdir: {}",
//         config.password,
//         config.path_to_certificates.c_str(),
//         config.port,
//         config.root_subdir);
// }
