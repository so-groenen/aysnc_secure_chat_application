#ifndef CONNECTION_MODE_H
#define CONNECTION_MODE_H

#include <QString>
#include <optional>

enum class ConnectionMode
{
    Tcp,
    Ssl,
};


inline QString connect_mode_to_qstring(ConnectionMode mode)
{
    switch (mode)
    {
    case ConnectionMode::Tcp:
        return "tcp";
    case ConnectionMode::Ssl:
        return "ssl";
    default:
        return "";
    }
}

inline auto try_get_connect_mode_from_view(QStringView view) -> std::optional<ConnectionMode>
{
    if(view.toString().toLower() == "tcp")
    {
        return ConnectionMode::Tcp;
    }
    else if(view.toString().toLower() == "ssl" || view.toString().toLower() == "tls")
    {
        return ConnectionMode::Ssl;
    }
    return std::nullopt;
}


#endif // CONNECTION_MODE_H
