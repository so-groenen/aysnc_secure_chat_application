#include "server_settings.h"
#include "ui_server_settings.h"
#include "QDebug"



ServerSettings::ServerSettings(uint16_t port, QString password, QString host, QWidget *parent)
    : QDialog(parent)
    , ui(std::make_unique<Ui::ServerSettings>())
{
    ui->setupUi(this);
    ui->passwordEdit->setText(password);
    ui->hostnameEdit->setText(host);

    auto port_str = QString::number(port, 10);
    ui->portEdit->setValidator(m_port_validator.get());
    ui->portEdit->setText(port_str);


}

ServerSettings::~ServerSettings()
{
}

QString ServerSettings::password() const
{
    return ui->passwordEdit->text();
}

uint16_t ServerSettings::port() const
{
    QStringView view{ui->portEdit->text()};
    bool success    {false};
    uint16_t port = static_cast<uint16_t>(view.toInt(&success));
    if(!success)
    {
        qDebug() << "Failed converting port:" << view;
    }
    return port;
}

QString ServerSettings::host() const
{
    return ui->hostnameEdit->text();
}

