#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// #include "my_text_edit.h"
#include <QMainWindow>
#include <memory>
#include "server_settings.h"
// #include "interface_ssl_presenter.h"
#include "interface_tcp_view.h"
#include "abstract_tcp_presenter.h"
// #include "abstract_ssl_presenter.h"
#include <QTextEdit>
#include "formatted_message_model.h"
#include "bubble_delegate.h"
#include "line_message_delegate.h"
#include "message_delegate_modes.h"
// #include "certificates_dialog.h"
#include "security_bundle.h"
#include "interface_ssl.h"
#include <optional>
#include "abstract_tcp_presenter.h"
#include <filesystem>

namespace fs = std::filesystem;

// #include "app_presenter.h"
// #include "tcp_presenter.h"
// #include "ssl_presenter.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

template<typename T>
using Option     = std::optional<T>;
inline auto None = std::nullopt;

class MainWindow : public QMainWindow, public ITcpView
{
    Q_OBJECT

private:
    std::unique_ptr<Ui::MainWindow> m_managed_ui{};
    Ui::MainWindow* ui;
    AbstractTcpPresenter_ptr m_presenter{}; // should be only TcpClient then
    Option<ISecureSocketLayer*> m_ssl{};

    bool m_is_connected{false};
    bool m_should_broadcast_name{false};
    QString m_username{"Your name"};
    QString m_name{"me"};
    uint32_t m_max_char{128};
    QString m_password{""};
    fs::path m_certs_dir{};
    uint16_t m_port{6970};
    QString m_hostname{""};
    QColor m_font_color{255, 182, 193};
    bool m_clear_history_on_reconnect{true};
    ConnectionMode m_connection_mode{ConnectionMode::Tcp};
    DelegateMode m_delegate_mode{DelegateMode::BubbleDelegate};

    SecurityBundle m_keys_and_certificates{};
    std::unique_ptr<FormattedMessageListModel>  m_message_model{std::make_unique<FormattedMessageListModel>()};
    std::unique_ptr<BubbleDelegate>             m_bubble_delegate{std::make_unique<BubbleDelegate>()};
    std::unique_ptr<LineMessageDelegate>        m_line_msg_delegate{std::make_unique<LineMessageDelegate>()};

private:
    void set_delegate(DelegateMode mode);
    void check_txt_len();
    void save_config();
private:
    void set_Btn_to_connect();
    void set_Btn_to_disconnect();
    void set_connection_mode();
public:
    ~MainWindow();
    MainWindow(/*AbstractTcpPresenter_ptr presenter,*/QWidget *parent = nullptr);
    void dispatch_user_info_dialog();
    void dispatch_server_setting_dialog();

    // TCP Event Handler
    void handle_connect_response(const ConnectionResult& connect_result) override;
    void handle_msg_reception(const MessageVariant &msg) override;
    void handle_msg_send_failure(const MessageSendFailure& failed_msg) override;
    void handle_disconnect_from_host() override;

    // View
    // void attach(ISslClient* presenter) override;
    const QString& get_password() const override;
    const QString& get_username() const override;
    QColor get_font_color() const override;
    void set_default_hostname(QStringView host);
    void set_password(QStringView password);



private slots:
    void on_SendBtn_clicked();
    void on_connectBtn_clicked();
    void on_actionEdit_Server_triggered();
    void on_actionEdit_Username_triggered();
    void on_actionEdit_Certificates_triggered();
    // void on_actionEdit_Certificates_checkableChanged(bool checkable);
};
#endif // MAINWINDOW_H
