#ifndef SERVER_SETTINGS_H
#define SERVER_SETTINGS_H

#include <QDialog>
#include <memory>
#include <QString>
#include <qstring.h>
#include <QValidator>
#include <QColorDialog>
#include "connection_mode.h"

constexpr uint16_t MIN_PORT = 0;
constexpr uint16_t MAX_PORT = 65535;



namespace Ui {
class ServerSettings;
}

class ServerSettings : public QDialog
{
    Q_OBJECT

public:
    explicit ServerSettings(ConnectionMode mode, uint16_t port, QString password, uint32_t max_char, bool should_broadcast, QString host, QWidget *parent = nullptr);
    ~ServerSettings();
    QString password() const;
    uint16_t port() const;
    QString host() const;
    ConnectionMode connection_mode() const;
    bool should_broadcast_name() const;
    uint32_t max_char() const;
private slots:
    void on_tcpRadio_toggled(bool checked);
    void on_sslRadio_toggled(bool checked);


    void on_broadcastNameOn_toggled(bool checked);

    void on_broadcastNameOff_toggled(bool checked);

private:
    ConnectionMode m_connect_mode{};
    bool m_should_broadcast_name{};
    std::unique_ptr<Ui::ServerSettings> m_managed_ui{};
    Ui::ServerSettings* ui;
    std::unique_ptr<QValidator> m_port_validator{std::make_unique<QIntValidator>(MIN_PORT, MAX_PORT)};
};

#endif // SERVER_SETTINGS_H
