#include "tcp_presenter.h"

TcpPresenter::TcpPresenter(AbstractTcpModel_ptr model)
    : m_model{std::move(model)}
{
    m_model->attach(this);
    qDebug() << "TcpPresenter init!";
}

void TcpPresenter::set_port(uint16_t port)
{
    m_model->set_port(port);
}

void TcpPresenter::send_message(const QString& msg)
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

void TcpPresenter::disconnect()
{
    m_model->disconnect();
    m_message_handler.reset();
    m_handshaker->reset();
}

void TcpPresenter::set_up_connection(const QString& hostname)
{
    m_model->set_up_connection(hostname);
}

void TcpPresenter::attach(ITcpView* tcp_view)
{
    Q_ASSERT(tcp_view != nullptr && "tcp_view must not be null!!");

    m_view       = tcp_view;
    m_handshaker = std::make_unique<HandShaker>(m_model.get(), tcp_view);
    m_handshaker->should_broadcast_name(m_should_broadcast_name);
}

void TcpPresenter::should_broadcast_name(bool val)
{
    m_should_broadcast_name = val;
}


void TcpPresenter::handle_connect_response(const ConnectionResult &connect_result)
{
    qDebug() <<  "TcpPresenter::handle_connect_response";
    if(!connect_result)
    {
        m_view->handle_connect_response(connect_result);
        return;
    }
    m_handshaker->await_handshake(connect_result);
}


void TcpPresenter::handle_msg_reception(const MessageVariant &msg)
{
    assert(std::holds_alternative<QString>(msg) && "msg is of QString type");
    QString message = std::get<QString>(msg);

    if(m_handshaker->is_awaiting_session_id() && m_handshaker->parse_session_id(message) == HandShaker::Result::Success)
    {
        m_message_handler = m_handshaker->make_message_handler();
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

void TcpPresenter::handle_disconnect_from_host()
{
    // if(m_handshaker->has_session_id())
        m_view->handle_disconnect_from_host();

    // m_message_handler.reset();
    // m_handshaker.reset();
}

void TcpPresenter::handle_msg_send_failure(const MessageSendFailure &failed_msg)
{
    m_view->handle_msg_send_failure(failed_msg);
}

void TcpPresenter::show() // ASSUME m_view derives from QMainWindow!!!! We inheriting virtual classes with QMainwindow don't compile//
{
    auto window  = m_view;
    auto qwindow = dynamic_cast<QMainWindow*>(window);
    assert(qwindow != nullptr && "m_view must be derived from QMainWindow");
    qwindow->show();
}
