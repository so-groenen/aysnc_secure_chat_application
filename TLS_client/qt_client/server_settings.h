#ifndef SERVER_SETTINGS_H
#define SERVER_SETTINGS_H

#include <QDialog>
#include <memory>
#include <QString>
#include <qstring.h>
#include <QValidator>
#include <QColorDialog>


constexpr uint16_t MIN_PORT = 0;
constexpr uint16_t MAX_PORT = 65535;


namespace Ui {
class ServerSettings;
}

class ServerSettings : public QDialog
{
    Q_OBJECT

public:
    explicit ServerSettings(uint16_t port, QString password, QString host, QWidget *parent = nullptr);
    ~ServerSettings();
    QString password() const;
    uint16_t port() const;
    QString host() const;

private:
    std::unique_ptr<Ui::ServerSettings> m_managed_ui{};
    Ui::ServerSettings* ui;
    std::unique_ptr<QValidator> m_port_validator{std::make_unique<QIntValidator>(MIN_PORT, MAX_PORT)};
};

#endif // SERVER_SETTINGS_H
