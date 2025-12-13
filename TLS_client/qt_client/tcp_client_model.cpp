#include "tcp_client_model.h"
#include <expected>

using enum QAbstractSocket::SocketState;

TcpClientModel::TcpClientModel(uint16_t Port, QObject *parent)
    : QObject{parent}, m_port{Port}
{
    QObject::connect(&m_socket, &QTcpSocket::readyRead, this, &TcpClientModel::recieving_msg);
    QObject::connect(&m_socket, &QTcpSocket::disconnected, this, &TcpClientModel::is_disconnected);
    QObject::connect(&m_socket, &QTcpSocket::errorOccurred, this, &TcpClientModel::retrieve_error);
}


void TcpClientModel::set_port(uint16_t port)
{
    m_port = port;
}

void TcpClientModel::send_message(const QString& msg)
{
    if(is_connected())
    {
        qDebug() << "Sending:" << msg;
        QString message = msg + m_delim;

        qint64 bytes_written {m_socket.write(message.toStdString().c_str())};
        bool is_sent         {bytes_written != -1};

        if(!is_sent)
        {
            qDebug() << "error sending" << msg;
        }
        else
        {
            qDebug() << "successfully sent:" << msg;
        }

        auto failed_msg = (is_sent)? std::nullopt : std::optional<QString>(msg);
        m_presenter->handle_msg_send_failure(failed_msg);

    }
}

void TcpClientModel::disconnect()
{
    if(is_connected())
    {
        m_socket.disconnectFromHost();
        qDebug() << "Socket closing";
    }
}

void TcpClientModel::set_up_connection(const QString& hostname)
{
    QHostInfo::lookupHost(hostname, this, &TcpClientModel::look_up_host_infos);
}

void TcpClientModel::look_up_host_infos(const QHostInfo &host)
{
    if (host.error() != QHostInfo::NoError)
    {
        qDebug() << "Lookup failed:" << host.errorString();

        m_presenter->handle_connect_response(std::unexpected("HOST NOT FOUND"));
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


void TcpClientModel::connecting_to_host()
{
    m_socket.connectToHost(m_server_address, m_port);
    m_socket.waitForConnected();
    m_is_connected = m_socket.waitForConnected();

    if (m_is_connected)
    {
        m_socket.open(QIODevice::ReadWrite);
        qDebug() << "Socket opened succesfully";
        qDebug() << "Socket descriptor:" << static_cast<qint64>(m_socket.socketDescriptor());
        m_presenter->handle_connect_response(m_server_address.toString());
    }
    else
    {
        qDebug() << "Socket opening failure";
        // m_presenter->handle_connect_response(std::unexpected(m_socket.errorString()));
    }
}

void TcpClientModel::attach(ITcpEventHandler *presenter)
{
    m_presenter = presenter;
}

const QString &TcpClientModel::get_delimiter() const
{
    return m_delim;
}


bool TcpClientModel::is_connected() const
{
    return (m_socket.state() == QAbstractSocket::SocketState::ConnectedState);
}


void TcpClientModel::recieving_msg()
{
    QByteArray recieved_data {m_socket.readAll()};
    QString    messages{recieved_data};


    auto inbox = messages.split(m_delim);

    if(inbox.back().isEmpty())
    {
        inbox.pop_back();
    }


    for(const auto& msg : std::as_const(inbox))
    {
        m_presenter->handle_msg_reception(msg);
    }


}

void TcpClientModel::is_disconnected()
{
    qDebug() << "Socket is closed";
    // m_presenter->handle_disconnect_from_host();
}

void TcpClientModel::retrieve_error(QAbstractSocket::SocketError socket_error)
{
    qDebug() << "TcpClientModel::retrieve_error" << socket_error;
    if (socket_error == QAbstractSocket::RemoteHostClosedError)
    {
        m_presenter->handle_disconnect_from_host();
    }
    else if (socket_error == QAbstractSocket::ConnectionRefusedError)
    {
        m_presenter->handle_connect_response(std::unexpected{ConnectionError{"ConnectionRefusedError"}});
    }

    // ConnectionResult result = std::unexpected{m_socket.errorString()};
    // m_presenter->handle_connect_response(result);
}

