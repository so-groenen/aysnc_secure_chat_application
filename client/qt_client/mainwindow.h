#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "my_text_edit.h"
#include <QMainWindow>
#include <memory>
#include "interface_presenter.h"
#include "interface_tcp_view.h"
#include <QTextEdit>
#include "server_settings.h"
#include "user_name_dialog.h"
#include "formatted_message_model.h"
#include "bubble_delegate.h"
#include "line_message_delegate.h"
#include "message_delegate_modes.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE



class MainWindow : public QMainWindow, public ITcpView
{
    Q_OBJECT

private:
    Ui::MainWindow* ui;
    ITcpClient* m_presenter{};
    bool m_is_connected{false};
    QString m_username{"Your name"};
    QString m_name{"me"};
    QString m_password{"louvre"};           
    uint16_t m_port{6970};                 
    QString m_hostname{""};                
    void set_Btn_to_connect();
    void set_Btn_to_disconnect();
    void dispatch_user_info_dialog();
    QColor m_my_color{255, 182, 193};
    bool m_clear_history_on_reconnect{true};
    DelegateMode m_delegate_mode{DelegateMode::BubbleDelegate};
    std::unique_ptr<FormattedMessageListModel>  m_message_model{std::make_unique<FormattedMessageListModel>()};
    std::unique_ptr<BubbleDelegate>             m_bubble_delegate{std::make_unique<BubbleDelegate>()};
    std::unique_ptr<LineMessageDelegate>        m_line_msg_delegate{std::make_unique<LineMessageDelegate>()};
public:
    ~MainWindow();
    MainWindow(QWidget *parent = nullptr);

    // TCP Event Handler
    void handle_connect_response(const ConnectionResult& connect_result) override;
    void handle_msg_reception(const MessageVariant &msg) override;
    void handle_msg_send_failure(const MessageSendFailure& failed_msg) override;
    void handle_disconnect_from_host() override;

    // View
    void attach(ITcpClient* presenter) override;
    const QString& get_password() const override;
    const QString& get_username() const override;
    QColor get_font_color() const override;
    void set_default_hostname(QStringView host) override;
    void set_password(QStringView password) override;
private:
    void set_delegate(DelegateMode mode);

private slots:
    void on_SendBtn_clicked();
    void on_connectBtn_clicked();
    void on_actionEdit_Server_triggered();
    void on_actionEdit_Username_triggered();
};
#endif // MAINWINDOW_H
