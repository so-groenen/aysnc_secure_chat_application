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


AppPresenter::AppPresenter(std::unique_ptr<ITcpClientModel> model, std::unique_ptr<ITcpView> gui)
    : m_model{std::move(model)}, m_view{std::move(gui)}
{
    m_view->attach(this);
    m_model->attach(this);
    qDebug() << "AppPresenter init!";
}

void AppPresenter::set_port(uint16_t port)
{
    m_model->set_port(port);
}

void AppPresenter::send_message(QString msg)
{
    if(!m_message_handler)
    {
        qDebug() << "Message handler not init!!";
        return;
    }
    QString parsed_msg = m_message_handler->parse_to_send(msg);
    m_model->send_message(parsed_msg);
}

void AppPresenter::disconnect()
{
    m_model->disconnect();
    m_message_handler.reset();
    m_handshake_mode = HandShakeMode::AwaitHandShake;
}

void AppPresenter::set_up_connection(QString hostname)
{
    m_model->set_up_connection(hostname);
}

void AppPresenter::handle_connect_response(bool is_connected, const QString &ipv4)
{
    m_view->handle_connect_response(is_connected, ipv4);

    if(!is_connected)
        return;

    if(m_handshake_mode == HandShakeMode::AwaitHandShake)
    {
        auto password = m_view->get_password();
        m_model->send_message(password);

        m_handshake_mode = HandShakeMode::AwaitSessionId;
    }
}

void AppPresenter::handle_msg_reception(const MessageVariant &msg)
{
    assert(std::holds_alternative<QString>(msg) && "msg is a QString type");
    QString message = std::get<QString>(msg);

    if(m_handshake_mode == HandShakeMode::AwaitSessionId)
    {
        bool ok           = false;
        qint64 session_id = message.toLongLong(&ok);
        if(!ok)
        {
            qDebug() << "COULD NOT PARSE SESSION ID! USING RNG TO GENERATE";
            QRandomGenerator generator = QRandomGenerator{};
            session_id = generator.bounded(0, std::numeric_limits<qint64>::max());
        }
        else
        {
            qDebug() << "GOT SESSION_ID: " << session_id;
        }

        m_message_handler = std::make_unique<MessageHandler>(m_view->get_username(), session_id);
        m_handshake_mode  = HandShakeMode::Ok;
        return;
    }

    if(!m_message_handler)
    {
        qDebug() << "Message handler not init!!";
        return;
    }

    qDebug() << "handle_msg_reception: " << message;
    Message parsed_msg = m_message_handler->parse_to_receive(message);
    m_view->handle_msg_reception(parsed_msg);
}

void AppPresenter::handle_disconnect_from_host()
{
    m_view->handle_disconnect_from_host();
    m_message_handler.reset();
    m_handshake_mode = HandShakeMode::AwaitHandShake;
}

void AppPresenter::handle_msg_sent_status(QString msg, bool is_sent)
{
    m_view->handle_msg_sent_status(msg, is_sent);
}

void AppPresenter::show() // ASSUME m_view derives from QMainWindow!!!! We inheriting virtual classes with QMainwindow don't compile//
{
    auto window  = m_view.get();
    auto qwindow = dynamic_cast<QMainWindow*>(window);
    assert(qwindow != nullptr && "m_view must be derived from QMainWindow");
    qwindow->show();
}
