#ifndef ABSTRACT_SSL_MODEL_H
#define ABSTRACT_SSL_MODEL_H

#include <QString>
#include "abstract_tcp_model.h"
#include "interface_ssl.h"


// When an event occurs in Qt widget, we forward it to the presenter...
// the presenter needs the TCP client to expose the following methods:



class AbstractSslClientModel : public AbstractTcpClientModel, public ISecureSocketLayer
{
public:
    virtual ~AbstractSslClientModel() = default;
};

using AbstractSslModel_ptr = std::unique_ptr<AbstractSslClientModel>;



#endif // ABSTRACT_SSL_MODEL_H
