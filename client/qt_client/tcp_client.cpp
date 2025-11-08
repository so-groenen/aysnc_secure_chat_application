#include "tcpclient.h"

// using namespace  QAbstractSocket::SocketState;
using enum QAbstractSocket::SocketState;

TcpClient::TcpClient(uint32_t Port, QObject *parent)
    : QObject{parent}, m_port{Port}
{

    QObject::connect(this, TcpClient::ready_to_connect, this, &TcpClient::connecting_to_host);

    QObject::connect(&m_socket, &QTcpSocket::readyRead, this, &TcpClient::recieving_msg);
    QObject::connect(&m_socket, &QTcpSocket::disconnected, this, &TcpClient::is_disconnected);
    QObject::connect(&m_socket, &QTcpSocket::errorOccurred, this, &TcpClient::retrieve_error);
}

void TcpClient::set_up_connection(QString hostname)
{
    QHostInfo::lookupHost(hostname, this, &TcpClient::look_up_host_infos);
}

void TcpClient::look_up_host_infos(const QHostInfo &host)
{
    if (host.error() != QHostInfo::NoError) {
        qDebug() << "Lookup failed:" << host.errorString();

        emit connection_result(false, "HOST NOT FOUND"); //host.errorString(), maybe use "emit hostInfo_result(success, msg)"??

        return;
    }

    QString ipv4_str{"No ipv4"};
    const auto addresses = host.addresses();
    for (const QHostAddress &address : addresses)
    {
        qDebug() << "Found address:" << address.toString();
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            // q() << "IP4 found, adress set";
            m_server_address = address;
            ipv4_str         = address.toString();
        }
    }

    qDebug() << "IP4:" << ipv4_str ;

    // emit connection_result(true, ipv4_str);
    emit ready_to_connect();
}

// void slot: handle host info result: {if success, connecting to host, else emit connection_result(false, "HOST NOT FOUND"))

void TcpClient::connecting_to_host()
{
    m_socket.connectToHost(m_server_address, m_port);
    m_socket.waitForConnected();
    if (m_socket.waitForConnected())
    {
        m_socket.open(QIODevice::ReadWrite);
        qDebug() << "Socket opened succesfully";
        qDebug() << "Socket descriptor:" << static_cast<qint64>(m_socket.socketDescriptor());

        m_is_connected = true;

        // to be checked by NO need to emit using MVP approach, the Presenter part should handle it.
        emit connection_result(m_is_connected, m_server_address.toString());
    }
    else
    {
        qDebug() << "Socket opening failure";
        m_is_connected = false;
        emit connection_result(m_is_connected, m_socket.errorString());
    }
}



bool TcpClient::is_connected() const
{
    return (m_socket.state() == QAbstractSocket::SocketState::ConnectedState);
}


void TcpClient::recieving_msg()
{
    QByteArray recieved_data {m_socket.readAll()};
    QString    msg           {recieved_data};
    emit recieved_msg(msg);
}



void TcpClient::disconnect()
{
    if(is_connected())
    {
        m_socket.disconnectFromHost();
        qDebug() << "Socket closing";
    }
}

void TcpClient::is_disconnected()
{
    qDebug() << "Socket is closed";
}

void TcpClient::retrieve_error(QAbstractSocket::SocketError socketError)
{
    qDebug() << socketError;
    emit is_disconneced_from_Host(); //
}

void TcpClient::send_message(QString msg)
{
    if(is_connected())
    {
        qDebug() << "Sending:" << msg;
        msg.append("\n\r");
        qint64 bytesWritten {m_socket.write(msg.toStdString().c_str())};
        bool is_sent        {false};

        if(bytesWritten == -1)
        {
            qDebug() << "error sending" << msg;
            is_sent = false;
        }
        else
        {
            is_sent = true;
        }

        emit message_sent_status(msg, is_sent); // NEEDS TO BE CONNECTED TO SLOT
        // use signal to send back confirmation signal
        // & let window handle: add msg to Messages if true, else add <send failure>
        // signal: message_sent_status(Qstring msg, bool is_sent)
    }
}

