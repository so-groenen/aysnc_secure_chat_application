#include "app_presenter.h"
#include <QMainWindow>
#include <QDebug>
AppPresenter::AppPresenter(std::unique_ptr<ITcpClientModel> model, std::unique_ptr<ITcpView> gui)
    : m_model{std::move(model)}, m_view{std::move(gui)}
{
    m_view->attach(this);
    m_model->attach(this);
    qDebug() << "pass";
}

void AppPresenter::set_port(uint16_t port)
{
    m_model->set_port(port);
}

void AppPresenter::send_message(QString msg)
{
    m_model->send_message(msg);
}

void AppPresenter::disconnect()
{
    m_model->disconnect();
}

void AppPresenter::set_up_connection(QString hostname)
{
    m_model->set_up_connection(hostname);
}

void AppPresenter::handle_connect_response(bool value, const QString &ipv4)
{
    m_view->handle_connect_response(value, ipv4);
}

void AppPresenter::handle_msg_reception(const QString &msg)
{
    m_view->handle_msg_reception(msg);
}

void AppPresenter::handle_disconnect_from_host()
{
    m_view->handle_disconnect_from_host();
}

void AppPresenter::handle_msg_sent_status(QString msg, bool is_sent)
{
    m_view->handle_msg_sent_status(msg, is_sent);
}

void AppPresenter::show() // ASSUME m_view derives from QMainWindow!!!! We inheriting virtual classes with QMainwindow don't compile//
{
    auto window = m_view.get();
    dynamic_cast<QMainWindow*>(window)->show();
}
