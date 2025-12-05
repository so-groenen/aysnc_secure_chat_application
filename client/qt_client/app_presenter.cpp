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

void AppPresenter::send_message(const QString& msg)
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

void AppPresenter::set_up_connection(const QString& hostname)
{
    m_model->set_up_connection(hostname);
}

void AppPresenter::handle_connect_response(const ConnectionResult &connect_result)
{
    m_view->handle_connect_response(connect_result);

    if(!connect_result)
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
    assert(std::holds_alternative<QString>(msg) && "msg is of QString type");
    QString messages = std::get<QString>(msg);

    auto inbox = messages.split(m_model->get_delimiter());
    // qDebug() << "messages: " << messages;
    // qDebug() << "Split:";
    // for(const auto& ms : std::as_const(inbox))
    // {
    //     qDebug() << ms;
    // }

    if(inbox.back().isEmpty())
    {
        inbox.pop_back();
    }

    if(m_handshake_mode == HandShakeMode::AwaitSessionId)
    {
        auto first_msg    = inbox.at(0);

        qDebug() << "AWAITING SESSION ID:" << first_msg;
        bool ok           = false;
        qint64 session_id = first_msg.toLongLong(&ok);

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
        inbox.pop_front();
        m_message_handler = std::make_unique<MessageHandler>(m_view->get_username(), m_view->get_font_color(), session_id);
        m_handshake_mode  = HandShakeMode::Ok;
    }

    if(!m_message_handler)
    {
        qDebug() << "Message handler not init!!";
        return;
    }

    for(const auto& messages : std::as_const(inbox))
    {
        qDebug() << "handle_msg_reception: " << messages;
        auto parsed_msg_res = m_message_handler->parse_to_receive(messages);
        if(!parsed_msg_res)
        {
            m_view->handle_msg_reception(parsed_msg_res.error()); //pass QString msg if parsing Json fails;
            continue;
        }
        m_view->handle_msg_reception(parsed_msg_res.value()); //pass Message
    }
}

void AppPresenter::handle_disconnect_from_host()
{
    m_view->handle_disconnect_from_host();
    m_message_handler.reset();
    m_handshake_mode = HandShakeMode::AwaitHandShake;
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
