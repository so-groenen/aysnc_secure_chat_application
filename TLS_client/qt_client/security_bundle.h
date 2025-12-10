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
    // explicit SecurityBundle(const QSslKey& private_key_, const QSslCertificate& public_key_, const QSslCertificate& root_CA_)
    //     : private_key{private_key_}, public_key{public_key_}, root_CA{root_CA_}
    // {
    // }
};

#endif // SECURITY_BUNDLE_H
