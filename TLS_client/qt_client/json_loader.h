#ifndef JSON_LOADER_H
#define JSON_LOADER_H

#include <qdir.h>
#include <climits>
#ifdef __unix__
    #include <unistd.h>
#endif
#ifdef  __WIN32
    #include <windows.h>
#endif


#include <filesystem>
#include <QJsonObject>
#include <expected>

#include <filesystem>
namespace fs = std::filesystem;

inline fs::path get_application_dir_path()
{
    fs::path app_path{fs::current_path()};
#ifdef __unix__
    char path_to_proc[PATH_MAX + 1];                                     //unix doesn't add null termination
    ssize_t length = readlink("/proc/self/exe", path_to_proc, PATH_MAX); // ssize_t can be -1
    if(length != -1)
    {
        path_to_proc[length] = '\0';
        app_path = std::move(fs::path{path_to_proc}.parent_path());
    }
#endif
#ifdef _WIN32
    wchar_t path_to_proc[PATH_MAX];
    DWORD length = GetModuleFileNameW(NULL, path_to_proc, PATH_MAX);
    if(length != 0)
    {
        app_path = std::move(fs::path{path_to_proc}.parent_path());
    }
#endif
    return app_path;

}

auto load_json_from_file(const fs::path& path) -> std::expected<QJsonObject, QString>
{
    QFile json_config(path);
    if(!json_config.open(QFile::ReadOnly))
    {
        return std::unexpected{QString{"Could not open file"}};
    }
    QJsonParseError err{};
    auto json_doc = QJsonDocument::fromJson(json_config.readAll(), &err);
    if(err.error != QJsonParseError::NoError)
    {
        return std::unexpected{QString{"Could not pasre jsonDocument: " + err.errorString()}};
    }
    if(!json_doc.isObject())
    {
        return std::unexpected{QString{"JsonDocument is not a jsonObject"}};
    }
    return json_doc.object();
}

auto save_json_to_file(const fs::path& path, const QJsonObject& json_config) -> std::expected<bool, QString>
{
    QFile json_file(path);
    if(!json_file.open(QFile::WriteOnly))
    {
        return std::unexpected{QString{"Could not open file"}};
    }
    QJsonDocument json_doc{json_config};
    auto bytes = json_doc.toJson();
    json_file.write(bytes);
    return true;
}

#endif // JSON_LOADER_H
