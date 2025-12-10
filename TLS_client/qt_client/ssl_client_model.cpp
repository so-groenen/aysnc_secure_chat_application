#include "ssl_client_model.h"
#include <expected>

using enum QAbstractSocket::SocketState;
static constexpr bool USE_SYSTEM_CONFIG = false;


SslClientModel::SslClientModel(uint16_t Port, QObject *parent)
    : QObject{parent}, m_port{Port}
{
    if(USE_SYSTEM_CONFIG)
    {
        m_config = QSslConfiguration::defaultConfiguration();
    }

    m_config.setPeerVerifyMode(QSslSocket::VerifyPeer);
    m_config.setProtocol(QSsl::TlsV1_3OrLater);


    QObject::connect(&m_ssl_socket, &QSslSocket::readyRead, this, &SslClientModel::recieving_msg);
    QObject::connect(&m_ssl_socket, &QTcpSocket::errorOccurred, this, &SslClientModel::retrieve_error);
    QObject::connect(&m_ssl_socket, &QSslSocket::stateChanged, [](QAbstractSocket::SocketState state)
    {
        qDebug() << "SocketState:" << state;
    });
    QObject::connect(&m_ssl_socket, &QSslSocket::sslErrors, [this](const QList<QSslError> &errors)
    {
        for(const QSslError& err: std::as_const(errors))
        {
            qDebug() << "Got error:" << err.errorString();
            qDebug() << "from"       << err.certificate().issuerInfo(QSslCertificate::SubjectInfo::Organization);
            if(err.error() == QSslError::HostNameMismatch)
            {
                qDebug() << m_ssl_socket.peerName(); // for ex: 127.0.0.1 vs 127.0.1.1 errors
                // can be be mitigated by adding "aliase" for the name (SANs or Subject Alternative Names)
#ifdef QT_DEBUG
                QList<QSslError> self_sign_err{err};
                qDebug() << "DEBUG IGNORING HostNameMismatch";
                m_ssl_socket.ignoreSslErrors(self_sign_err);
#endif
            }
        }
    });

    QObject::connect(&m_ssl_socket, &QSslSocket::disconnected, []
    {
        qDebug() << "Ssl Socket is closed";
    });
}


void SslClientModel::set_port(uint16_t port)
{
    m_port = port;
}

void SslClientModel::send_message(const QString& msg)
{
    if(is_connected())
    {
        qDebug() << "Sending:" << msg;
        QString message = msg + m_delim;

        qint64 bytes_written {m_ssl_socket.write(message.toStdString().c_str())};
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

void SslClientModel::disconnect()
{
    if(is_connected())
    {
        m_ssl_socket.disconnectFromHost();
        qDebug() << "Socket closing";
    }
}

void SslClientModel::set_up_connection(const QString& hostname)
{
    QHostInfo::lookupHost(hostname, this, &SslClientModel::look_up_host_infos);
}

void SslClientModel::look_up_host_infos(const QHostInfo &host)
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


void SslClientModel::connecting_to_host()
{
    m_ssl_socket.setSslConfiguration(m_config);
    m_ssl_socket.connectToHost(m_server_address, m_port);
    // m_ssl_socket.connectToHostEncrypted(m_server_address, m_port);
    if(!m_ssl_socket.waitForConnected(3000))
    {
        qDebug() << "Ssl Socket: Try connecting to host failure";
        // m_is_connected = false;
        return;
    }
    m_ssl_socket.startClientEncryption();
    if(!m_ssl_socket.waitForEncrypted(1000))
    {
        qDebug() << "Ssl Socket: waitForEncrypted failure";
        // m_is_connected = false;
        return;
    }

    m_ssl_socket.open(QIODevice::ReadWrite);
    qDebug() << "SslSocket opened succesfully";
    qDebug() << "Ssl Socket descriptor:" << static_cast<qint64>(m_ssl_socket.socketDescriptor());
    m_presenter->handle_connect_response(m_server_address.toString());
    // m_is_connected = true;

    // m_socket.connectToHost(m_server_address, m_port);
    // m_is_connected = m_socket.waitForConnected();

    // if (m_is_connected)
    // {
    //     m_socket.open(QIODevice::ReadWrite);
    //     qDebug() << "Socket opened succesfully";
    //     qDebug() << "Socket descriptor:" << static_cast<qint64>(m_socket.socketDescriptor());
    //     m_presenter->handle_connect_response(m_server_address.toString());
    // }
    // else
    // {
    //     qDebug() << "Socket opening failure";
    //     // m_presenter->handle_connect_response(std::unexpected(m_socket.errorString()));
    // }
}

void SslClientModel::attach(ITcpEventHandler *presenter)
{
    m_presenter = presenter;
}

const QString &SslClientModel::get_delimiter() const
{
    return m_delim;
}


bool SslClientModel::is_connected() const
{
    // return (m_socket.state() == QAbstractSocket::SocketState::ConnectedState);
    return (m_ssl_socket.state() == QAbstractSocket::SocketState::ConnectedState);

}

void SslClientModel::set_private_key(const QSslKey &private_key)
{
    m_config.setPrivateKey(private_key);
    qDebug() << "SslClientModel: private key set!";
}

void SslClientModel::set_public_key(const QSslCertificate &public_key)
{
    m_config.setLocalCertificate(public_key);
    qDebug() << "SslClientModel: public key set!";
}

void SslClientModel::set_root_CA(const QSslCertificate &rootCA)
{
    QList<QSslCertificate> certificates_lists{rootCA};
    m_config.setCaCertificates(certificates_lists);
    // m_config.addCaCertificate(rootCA);
    qDebug() << "SslClientModel: rootCA set!";
}


void SslClientModel::recieving_msg()
{
    // QByteArray recieved_data {m_socket.readAll()};
    QByteArray recieved_data {m_ssl_socket.readAll()};

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

void SslClientModel::is_disconnected()
{
    qDebug() << "Socket is closed";
    // m_presenter->handle_disconnect_from_host();
}

void SslClientModel::retrieve_error(QAbstractSocket::SocketError socket_error)
{
    qDebug() << "SslClientModel::retrieve_error" << socket_error;
    ConnectionError err_str = m_ssl_socket.errorString();

    m_presenter->handle_connect_response(std::unexpected{err_str});
    // if (socket_error == QAbstractSocket::RemoteHostClosedError)
    // {
    //     m_presenter->handle_disconnect_from_host();
    // }
    // else if (socket_error == QAbstractSocket::ConnectionRefusedError)
    // {
    //     m_presenter->handle_connect_response(std::unexpected{ConnectionError{"ConnectionRefusedError"}});
    // }

    // ConnectionResult result = std::unexpected{m_socket.errorString()};
    m_presenter->handle_connect_response(err_str);
}


