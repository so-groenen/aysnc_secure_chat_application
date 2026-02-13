#include "server_settings.h"
#include "ui_server_settings.h"
#include "QDebug"



ServerSettings::ServerSettings(ConnectionMode connect_mode, uint16_t port, QString password, uint32_t max_char, bool should_broadcast, QString host, QWidget *parent)
    : QDialog(parent), m_connect_mode{connect_mode},
    m_managed_ui{std::make_unique<Ui::ServerSettings>()}, m_should_broadcast_name{should_broadcast},
    ui{m_managed_ui.get()}
{
    ui->setupUi(this);
    ui->passwordEdit->setText(password);
    ui->hostnameEdit->setText(host);

    if(m_should_broadcast_name)
        ui->broadcastNameOn->toggle();
    else
        ui->broadcastNameOff->toggle();

    switch (connect_mode)
    {
        case ConnectionMode::Tcp:
            ui->tcpRadio->toggle();
            break;
        case ConnectionMode::Ssl:
            ui->sslRadio->toggle();
            break;
    }

    auto port_str = QString::number(port, 10);
    ui->portEdit->setValidator(m_port_validator.get());
    ui->portEdit->setText(port_str);
    ui->maxCharEdit->setText(QString::number(max_char));
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
    auto port = static_cast<uint16_t>(view.toInt(&success));
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

ConnectionMode ServerSettings::connection_mode() const
{
    return m_connect_mode;
}

bool ServerSettings::should_broadcast_name() const
{
    return m_should_broadcast_name;
}

uint32_t ServerSettings::max_char() const
{
    bool ok{};
    return ui->maxCharEdit->text().toUInt(&ok);
}



void ServerSettings::on_tcpRadio_toggled(bool checked)
{
    if(checked)
        m_connect_mode = ConnectionMode::Tcp;
}


void ServerSettings::on_sslRadio_toggled(bool checked)
{
    if(checked)
        m_connect_mode = ConnectionMode::Ssl;
}





void ServerSettings::on_broadcastNameOn_toggled(bool checked)
{
    if(checked)
        m_should_broadcast_name = true;
}


void ServerSettings::on_broadcastNameOff_toggled(bool checked)
{
    if(checked)
        m_should_broadcast_name = false;
}

