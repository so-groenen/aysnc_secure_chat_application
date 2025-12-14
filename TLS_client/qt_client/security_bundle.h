#ifndef SECURITY_BUNDLE_H
#define SECURITY_BUNDLE_H
#include <QSslKey>
#include <QSslCertificate>
#include <QString>

struct SecurityBundle
{
    QSslKey private_key{};
    QString private_key_path{};

    QSslCertificate public_key{};
    QString public_key_path{};

    QSslCertificate root_CA{};
    QString root_CA_path{};

    SecurityBundle() = default;
};

#endif // SECURITY_BUNDLE_H
