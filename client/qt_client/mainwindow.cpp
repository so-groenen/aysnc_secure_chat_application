#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui{std::make_unique<Ui::MainWindow>()}
{
    ui->setupUi(this);
    set_Btn_to_connect();
    // ui->connectBtn->setDisabled(true);
}

MainWindow::~MainWindow()
{

}

void MainWindow::on_connectBtn_clicked()
{
    if(!m_is_connected)
    {
        QString hostname  {ui->HostEdit->text()};

        ui->ResultEdit->setText("Awaiting response");
        emit trying_to_connect(hostname);
    }
    else
    {
        emit disconnecting();
        ui->ResultEdit->setText("");
        m_is_connected = false;
        set_Btn_to_connect();
    }
}

void MainWindow::handle_connect_response(bool is_connected, const QString& msg)
{
    if(is_connected)
    {
        ui->ResultEdit->setText(msg); 
        ui->MessagesList->addItem("<Try connecting>");
        set_Btn_to_disconnect();
    }
    else
    {
        ui->ResultEdit->setText(msg);
        set_Btn_to_connect();
    }

    m_is_connected = is_connected;
}

void MainWindow::handle_msg_reception(const QString &msg)
{
    if(m_is_connected)
    {
        const int last {ui->MessagesList->count()};
        ui->MessagesList->addItem(m_other + ": " + msg);
        ui->MessagesList->item(last)->setForeground(Qt::blue);
    }
}

void MainWindow::handle_disconnect_from_host()
{
    ui->ResultEdit->setText("Connection lost");
    ui->MessagesList->addItem("<Connection Lost>");

    set_Btn_to_connect();
    m_is_connected = false;
}

void MainWindow::handle_message_sent_status(const QString &msg, bool is_sent) // NEEDS TO BE CONNECTED TO SLOT
{
    const int last {ui->MessagesList->count()};

    if (is_sent)
    {
        ui->MessagesList->addItem(m_name + ": " + msg);
        ui->MessagesList->item(last)->setForeground(Qt::red);
    }
    else
    {
        ui->MessagesList->addItem(" ! " + m_name + ": " + msg + "<sending fail>");
        ui->MessagesList->item(last)->setForeground(Qt::gray);
    }
}


void MainWindow::on_SendBtn_clicked()
{
    if(m_is_connected)
    {
        const QString msg {ui->MessageEdit->text()};
        emit sending_message(msg);

        const int last {ui->MessagesList->count()};

        ui->MessagesList->addItem(m_name + ": " + msg);
        ui->MessagesList->item(last)->setForeground(Qt::red);
        ui->MessageEdit->clear();
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

