#ifndef SSL_PRESENTER_H
#define SSL_PRESENTER_H

#include "tcp_presenter.h"
#include "interface_ssl.h"
#include "abstract_ssl_model.h"

#include <QString>
#include <QMainWindow>

class SslPresenter : public TcpPresenter, public ISecureSocketLayer
{
    ISecureSocketLayer* m_ssl{};
public:
    SslPresenter(AbstractSslModel_ptr model);
    void set_private_key(const QSslKey& private_key) override;
    void set_public_key(const QSslCertificate& public_key) override;
    void set_root_CA(const QSslCertificate& rootCA) override;
};

using SslPresenter_ptr = std::unique_ptr<SslPresenter>;



#endif // SSL_PRESENTER_H
