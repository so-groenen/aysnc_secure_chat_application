#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <cassert>

#include "server_settings.h"
#include "user_name_dialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui{new Ui::MainWindow}
{
    ui->setupUi(this);
    set_Btn_to_connect();

    set_username();
}

void MainWindow::set_default_hostname(QStringView host)
{
    m_hostname = QString{host};
}

void MainWindow::set_password(QStringView password)
{
    m_password = QString{password};
}

void MainWindow::handle_connect_response(const ConnectionResult &connect_result)
{
    if(connect_result)
    {
        ui->ResultEdit->setText(connect_result.value()); // ip addresse
        ui->MessagesList->addItem("<CONNECTED>");
        set_Btn_to_disconnect();
    }
    else
    {
        ui->MessagesList->addItem("<COULD NOT CONNECT>");
        ui->ResultEdit->setText(connect_result.error()); // error msg
        set_Btn_to_connect();
    }

    m_is_connected = connect_result.has_value();
}



void MainWindow::handle_msg_reception(const MessageVariant &msg)
{
    if(m_is_connected)
    {
        const int last {ui->MessagesList->count()};

        if(std::holds_alternative<QString>(msg))
        {
            QString message = std::get<QString>(msg);

            qDebug() << "UNKNOWN SENDER: RAW UNPARSED MSG:";
            ui->MessagesList->addItem("<Unknown>: " + message);
            ui->MessagesList->item(last)->setForeground(Qt::black);
            return;
        }


        FormattedMessage message = std::get<FormattedMessage>(msg);
        if(message.is_current_user())
        {
            qDebug() << "CURRENT USER OK!";
            ui->MessagesList->addItem("Me: " + message.content());
            ui->MessagesList->item(last)->setForeground(Qt::white);
        }
        else
        {
            qDebug() << "NOT CURRENT USER";
            ui->MessagesList->addItem(message.username() + ": " + message.content());
            ui->MessagesList->item(last)->setForeground(Qt::red);
        }
    }
}

void MainWindow::handle_disconnect_from_host()
{
    ui->ResultEdit->setText("Connection lost");
    ui->MessagesList->addItem("<Connection Lost>");

    set_Btn_to_connect();
    m_is_connected = false;
}

void MainWindow::handle_msg_send_failure(const MessageSendFailure &failed_msg)
{
    qDebug() << "Message sent status: " << !failed_msg.has_value();

    if(failed_msg.has_value())
    {
        const int last {ui->MessagesList->count()};
        ui->MessagesList->addItem("! Me: " + failed_msg.value() + "<sending fail>");
        ui->MessagesList->item(last)->setForeground(Qt::gray);
    }
}


const QString &MainWindow::get_password() const
{
    return m_password;
}

const QString &MainWindow::get_username() const
{
    return m_username;
}


void MainWindow::set_username()
{
    UsernameDialog user_diaglog{m_username};
    user_diaglog.setModal(true);
    if(user_diaglog.exec() ==  QDialog::Accepted)
    {
        m_username = user_diaglog.get_username();
    }
}

void MainWindow::attach(ITcpClient *presenter)
{
    m_presenter = presenter;
}

void MainWindow::on_connectBtn_clicked()
{
    if(!m_is_connected)
    {
        ui->ResultEdit->setText("Awaiting response");
        m_presenter->set_up_connection(m_hostname);
    }
    else
    {
        m_presenter->disconnect();
        ui->MessagesList->addItem("<DICONNECTED>");
        ui->ResultEdit->setText("");
        m_is_connected = false;
        set_Btn_to_connect();
    }
}

void MainWindow::on_SendBtn_clicked()
{
    if(m_is_connected)
    {
        const QString msg {ui->MessageEdit->toPlainText()};
        m_presenter->send_message(msg);

    }
}

void MainWindow::set_Btn_to_connect()
{
    ui->connectBtn->setText("Connect");
}

void MainWindow::set_Btn_to_disconnect()
{
    ui->connectBtn->setText("Disconnect");
}

MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::on_actionEdit_Server_triggered()
{
    ServerSettings setting_diaglog{m_port, m_password, m_hostname};
    setting_diaglog.setModal(true);
    if(setting_diaglog.exec() ==  QDialog::Accepted)
    {
        m_password = setting_diaglog.password();
        m_hostname = setting_diaglog.host();
        m_port     = setting_diaglog.port();

        qDebug() << "got pass:" << setting_diaglog.password();
        qDebug() << "got port:" << setting_diaglog.port();
        qDebug() << "got host:" << setting_diaglog.host();

        m_presenter->set_port(m_port);
    }
}


void MainWindow::on_actionEdit_Username_triggered()
{
    set_username();
}




