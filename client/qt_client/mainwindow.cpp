#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <cassert>

#include "server_settings.h"
#include "usernamedialog.h"

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

void MainWindow::handle_connect_response(bool is_connected, const QString &Ipv4)
{
    if(is_connected)
    {
        ui->ResultEdit->setText(Ipv4);
        ui->MessagesList->addItem("<CONNECTED>");
        set_Btn_to_disconnect();
    }
    else
    {
        ui->MessagesList->addItem("<COULD NOT CONNECT>");
        ui->ResultEdit->setText(Ipv4);
        set_Btn_to_connect();
    }

    m_is_connected = is_connected;
}



void MainWindow::handle_msg_reception(const MessageVariant &msg)
{
    if(m_is_connected)
    {
        const int last {ui->MessagesList->count()};
        assert(std::holds_alternative<Message>(msg) && "msg is a Message type");

        Message message = std::get<Message>(msg);

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

void MainWindow::handle_msg_sent_status(QString msg, bool is_sent)
{
    const int last {ui->MessagesList->count()};
    qDebug() << "Message sent status: " << is_sent;

    if(!is_sent)
    {
        ui->MessagesList->addItem("! Me: " + msg + "<sending fail>");
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




