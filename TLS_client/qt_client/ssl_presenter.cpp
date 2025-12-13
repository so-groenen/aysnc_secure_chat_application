#include "ssl_presenter.h"


SslPresenter::SslPresenter(AbstractSslModel_ptr model)
    : TcpPresenter{std::move(model)}, m_ssl{dynamic_cast<ISecureSocketLayer*>(m_model.get())}
{
    Q_ASSERT(m_ssl != nullptr && "ISecurity layer must be extractable from the AbstractSslModel!");
    m_model->attach(this);
    qDebug() << "SslPresenter init!";
}

void SslPresenter::set_private_key(const QSslKey &private_key)
{
    qDebug() << "Presenter: private key is OK: " << !private_key.isNull();
    m_ssl->set_private_key(private_key);
}

void SslPresenter::set_public_key(const QSslCertificate &public_key)
{
    qDebug() << "Presenter: public key is OK: " << !public_key.isNull();
    m_ssl->set_public_key(public_key);
}

void SslPresenter::set_root_CA(const QSslCertificate &rootCA)
{
    qDebug() << "Presenter: rootCA is OK: " << !rootCA.isNull();
    m_ssl->set_root_CA(rootCA);
}


