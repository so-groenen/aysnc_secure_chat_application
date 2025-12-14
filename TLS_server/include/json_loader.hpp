#ifndef JSON_LOADER_HPP
#define JSON_LOADER_HPP

#include <cstdlib>
#include <span>
#include <string>
#include <string_view>
#include <filesystem>
#include <unordered_map>
#include <expected>


namespace fs = std::filesystem;

template<typename Tp, typename Er>
using Result = std::expected<Tp, Er>;
 

using JsonDict = std::unordered_map<std::string_view, std::string>;

namespace json_loader
{
    // template<size_t N>
    auto build_json_dict(const fs::path& json_path, std::span<const std::string_view> keys) -> Result<JsonDict, std::string>;
}

#endif