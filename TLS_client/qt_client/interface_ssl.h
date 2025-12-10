#ifndef INTERFACE_SSL_H
#define INTERFACE_SSL_H

#include <QSslKey>
#include <QSslCertificate>

class ISecureSocketLayer
{
public:
    virtual ~ISecureSocketLayer() = default;
    virtual void set_private_key(const QSslKey& private_key) = 0;
    virtual void set_public_key(const QSslCertificate& public_key) = 0;
    virtual void set_root_CA(const QSslCertificate& rootCA) = 0;
};


#endif // INTERFACE_SSL_H
