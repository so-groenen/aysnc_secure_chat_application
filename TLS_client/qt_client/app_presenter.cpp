#include "app_presenter.h"
#include <QMainWindow>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QRandomGenerator>
#include <limits>
#include <concepts>
#include <cassert>
#include <ranges>

AppPresenter::AppPresenter(std::unique_ptr<ISslClientModel> model, std::unique_ptr<ISslView> gui)
    : m_model{std::move(model)}, m_view{std::move(gui)}, m_handshaker{m_model.get(), m_view.get()}
{
    m_view->attach(this);
    m_model->attach(this);
    qDebug() << "AppPresenter init!";
}

void AppPresenter::set_port(uint16_t port)
{
    m_model->set_port(port);
}

void AppPresenter::send_message(const QString& msg)
{
    if(!m_message_handler)
    {
        qDebug() << "Message handler not init!!";
        return;
    }

    if(m_view->get_font_color() != m_message_handler->get_font_color())
    {
        m_message_handler->set_font_color(m_view->get_font_color());
    }

    QString parsed_msg = m_message_handler->parse_to_send(msg);
    m_model->send_message(parsed_msg);
}

void AppPresenter::disconnect()
{
    m_model->disconnect();
    m_message_handler.reset();
    m_handshaker.reset();
}

void AppPresenter::set_up_connection(const QString& hostname)
{
    m_model->set_up_connection(hostname);
}

void AppPresenter::set_private_key(const QSslKey &private_key)
{
    qDebug() << "Presenter: private key is OK: " << !private_key.isNull();
    m_model->set_private_key(private_key);
}

void AppPresenter::set_public_key(const QSslCertificate &public_key)
{
    qDebug() << "Presenter: public key is OK: " << !public_key.isNull();
    m_model->set_public_key(public_key);
}

void AppPresenter::set_root_CA(const QSslCertificate &rootCA)
{
    qDebug() << "Presenter: rootCA is OK: " << !rootCA.isNull();
    m_model->set_root_CA(rootCA);
}

void AppPresenter::handle_connect_response(const ConnectionResult &connect_result)
{
    qDebug() <<  "AppPresenter::handle_connect_response";
    if(!connect_result)
    {
        m_view->handle_connect_response(connect_result);
        return;
    }
    m_handshaker.await_handshake(connect_result);
}


void AppPresenter::handle_msg_reception(const MessageVariant &msg)
{
    assert(std::holds_alternative<QString>(msg) && "msg is of QString type");
    QString message = std::get<QString>(msg);

    if(m_handshaker.is_awaiting_session_id() && m_handshaker.parse_session_id(message) == HandShaker::Result::Success)
    {
        m_message_handler = m_handshaker.make_message_handler();
        return;
    }


    if(!m_message_handler)
    {
        qDebug() << "Message handler not init!!";
        return;
    }


    qDebug() << "handle_msg_reception: " << message;
    auto parsed_msg_res = m_message_handler->parse_to_receive(message);
    if(!parsed_msg_res)
    {
        m_view->handle_msg_reception(parsed_msg_res.error());
        return;
    }
    m_view->handle_msg_reception(parsed_msg_res.value());
}

void AppPresenter::handle_disconnect_from_host()
{
    if(m_handshaker.has_session_id())
        m_view->handle_disconnect_from_host();

    m_message_handler.reset();
    m_handshaker.reset();
}

void AppPresenter::handle_msg_send_failure(const MessageSendFailure &failed_msg)
{
    m_view->handle_msg_send_failure(failed_msg);
}

void AppPresenter::show() // ASSUME m_view derives from QMainWindow!!!! We inheriting virtual classes with QMainwindow don't compile//
{
    auto window  = m_view.get();
    auto qwindow = dynamic_cast<QMainWindow*>(window);
    assert(qwindow != nullptr && "m_view must be derived from QMainWindow");
    qwindow->show();
}
