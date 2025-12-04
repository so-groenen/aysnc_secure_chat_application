#include "tcp_client.h"

using enum QAbstractSocket::SocketState;

TcpClient::TcpClient(uint16_t Port, QObject *parent)
    : QObject{parent}, m_port{Port}
{
    QObject::connect(&m_socket, &QTcpSocket::readyRead, this, &TcpClient::recieving_msg);
    QObject::connect(&m_socket, &QTcpSocket::disconnected, this, &TcpClient::is_disconnected);
    QObject::connect(&m_socket, &QTcpSocket::errorOccurred, this, &TcpClient::retrieve_error);
}



void TcpClient::set_port(uint16_t port)
{
    m_port = port;
}

void TcpClient::send_message(QString msg)
{
    if(is_connected())
    {
        qDebug() << "Sending:" << msg;
        msg.append(m_delim);

        qint64 bytesWritten {m_socket.write(msg.toStdString().c_str())};
        bool is_sent        {false};

        if(bytesWritten == -1)
        {
            qDebug() << "error sending" << msg;
            is_sent = false;
        }
        else
        {
            qDebug() << "successfully sent:" << msg;
            is_sent = true;
        }

        m_presenter->handle_msg_sent_status(msg, is_sent);
        // TODO: implement in view
    }
}

void TcpClient::disconnect()
{
    if(is_connected())
    {
        m_socket.disconnectFromHost();
        qDebug() << "Socket closing";
    }
}

void TcpClient::set_up_connection(QString hostname) //
{
    QHostInfo::lookupHost(hostname, this, &TcpClient::look_up_host_infos);
}

void TcpClient::look_up_host_infos(const QHostInfo &host)
{
    if (host.error() != QHostInfo::NoError) {
        qDebug() << "Lookup failed:" << host.errorString();

        m_presenter->handle_connect_response(false, "HOST NOT FOUND");
        return;
    }

    QString ipv4_str{"No ipv4"};
    const auto addresses = host.addresses();
    for (const QHostAddress &address : addresses)
    {
        qDebug() << "Found address:" << address.toString();
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            m_server_address = address;
            ipv4_str         = address.toString();
        }
    }

    qDebug() << "IP4:" << ipv4_str ;
    connecting_to_host();
}


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
        m_presenter->handle_connect_response(m_is_connected, m_server_address.toString());
    }
    else
    {
        qDebug() << "Socket opening failure";
        m_is_connected = false;
        m_presenter->handle_connect_response(m_is_connected, m_socket.errorString());
    }
}

void TcpClient::attach(ITcpEventHandler *presenter)
{
    m_presenter = presenter;
}


bool TcpClient::is_connected() const
{
    return (m_socket.state() == QAbstractSocket::SocketState::ConnectedState);
}


void TcpClient::recieving_msg()
{
    QByteArray recieved_data {m_socket.readAll()};
    QString    msg           {recieved_data};
    m_presenter->handle_msg_reception(msg);
}


void TcpClient::is_disconnected()
{
    qDebug() << "Socket is closed";
}

void TcpClient::retrieve_error(QAbstractSocket::SocketError socketError)
{
    qDebug() << socketError;
    if (socketError == QAbstractSocket::RemoteHostClosedError)
        m_presenter->handle_disconnect_from_host();
}


