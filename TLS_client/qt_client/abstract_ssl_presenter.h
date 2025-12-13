#ifndef ABSTRACT_SSL_PRESENTER_H
#define ABSTRACT_SSL_PRESENTER_H

#include "abstract_tcp_presenter.h"
#include "interface_ssl.h"

#include <QString>
#include <QMainWindow>

class AbstractSslPresenter : public AbstractTcpPresenter, public ISecureSocketLayer
{
public:
    virtual ~AbstractSslPresenter() = default;
};

using AbstractSslPresenter_ptr = std::unique_ptr<AbstractSslPresenter>;



#endif // ABSTRACT_SSL_PRESENTER_H
