#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <cassert>
#include "certificates_dialog.h"
#include "server_settings.h"
#include "ssl_client_model.h"
#include "ssl_presenter.h"
#include "tcp_client_model.h"
#include "tcp_presenter.h"
#include "user_name_dialog.h"
#include "json_loader.h"
#include <QMessageBox>

static constexpr QStringView HOSTNAME      = u"hostname";
static constexpr QStringView PASSWORD      = u"password";
static constexpr QStringView PORT          = u"port";
static constexpr QStringView PATH_TO_CERTS = u"path to certs";
static constexpr QStringView BROADCAST_NAME = u"broadcast name";



MainWindow::MainWindow(/*AbstractTcpPresenter_ptr presenter,*/QWidget *parent)
    : QMainWindow(parent),
    m_managed_ui{std::make_unique<Ui::MainWindow>()},
    ui{m_managed_ui.get()}
    //,m_presenter{std::move(presenter)}
{

    auto app_dir = get_application_dir_path();
    auto config  = load_json_from_file(app_dir / "chat_app_config.json");
    QJsonObject json_config{};

    if(!config.has_value())
    {
        json_config[PASSWORD]       = "louvre";
        json_config[HOSTNAME]       = "192.168.1.33";
        json_config[PORT]           = 4242;
        json_config[BROADCAST_NAME] = true;
        json_config[PATH_TO_CERTS]  = QString::fromStdString(fs::current_path().string());

        save_json_to_file(app_dir / "chat_app_config.json", json_config);
        qDebug() << "json config saved";
    }
    else
    {
        json_config = std::move(config.value());
        qDebug() << "json config loaded";
    }

    m_hostname              = json_config.value(HOSTNAME).toString();
    m_password              = json_config.value(PASSWORD).toString();
    m_should_broadcast_name = json_config.value(BROADCAST_NAME).toBool(false);
    m_port                  = static_cast<uint16_t>(json_config.value(PORT).toInt());
    m_certs_dir             = json_config.value(PATH_TO_CERTS).toString().toStdString();

    if(!fs::exists(m_certs_dir))
    {
        m_certs_dir = fs::current_path();
    }


    ////////////////////////////////////////
    ui->setupUi(this);
    ui->SendBtn->setEnabled(m_is_connected);
    ui->username->setText(m_username);
    ui->messageListView->setItemDelegate(m_bubble_delegate.get());
    ui->messageListView->setModel(m_message_model.get());
    ui->messageListView->setWordWrap(true);

    ui->messageListView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    QObject::connect(ui->MessageEdit, &MyTextEdit::pressEnterEvent, this, &MainWindow::on_SendBtn_clicked);
    QObject::connect(ui->MessageEdit, &MyTextEdit::textChanged, this, &MainWindow::check_txt_len);

    set_Btn_to_connect();

    //Dialogs:
    ////////////////////////////////////////
    dispatch_user_info_dialog();
    on_actionEdit_Server_triggered();
}
void MainWindow::set_default_hostname(QStringView host)
{
    m_hostname = QString{host};
}

void MainWindow::set_password(QStringView password)
{
    m_password = QString{password};
}

void MainWindow::set_delegate(DelegateMode mode)
{
    m_delegate_mode = mode;
    switch (m_delegate_mode)
    {
        case DelegateMode::BubbleDelegate:
            ui->messageListView->setItemDelegate(m_bubble_delegate.get());
            break;
        case DelegateMode::LineMessageDelegate:
            ui->messageListView->setItemDelegate(m_line_msg_delegate.get());
            break;
        default:
            break;
    }
}

void MainWindow::handle_connect_response(const ConnectionResult &connect_result)
{
    if(connect_result)
    {
        if(m_clear_history_on_reconnect)
            m_message_model->clear_all();

        QString result{"<CONNECTED>"};
        m_message_model->add_warning(result);

        ui->ResultEdit->setText(connect_result.value()); // ip addresse
        set_Btn_to_disconnect();
    }
    else
    {
        m_message_model->add_warning(connect_result.error());

        ui->ResultEdit->setText(connect_result.error()); // error msg
        set_Btn_to_connect();
    }
    ui->messageListView->scrollToBottom();
    m_is_connected = connect_result.has_value();
    ui->SendBtn->setEnabled(m_is_connected);
}



void MainWindow::handle_msg_reception(const MessageVariant &msg)
{
    if(!m_is_connected)
        return;

    if(std::holds_alternative<QString>(msg))
    {
        QString message = std::get<QString>(msg);

        qDebug() << "UNKNOWN SENDER: RAW UNPARSED MSG:";
        m_message_model->add_unformatted_message(message);
        return;
    }

    FormattedMessage message = std::get<FormattedMessage>(msg);
    if(message.is_current_user())
    {
        qDebug() << "CURRENT USER OK!";
    }
    else
    {
        qDebug() << "NOT CURRENT USER";
    }
    m_message_model->add_message(message);
    ui->messageListView->scrollToBottom();
}

void MainWindow::handle_disconnect_from_host()
{
    ui->ResultEdit->setText("Connection lost");
    QString result{"<DISCONNECTED>"};
    m_message_model->add_warning(result);

    set_Btn_to_connect();
    m_is_connected = false;
    ui->SendBtn->setEnabled(m_is_connected);
}

void MainWindow::handle_msg_send_failure(const MessageSendFailure &failed_msg)
{
    qDebug() << "Message sent status: " << !failed_msg.has_value();

    if(failed_msg.has_value())
    {
        QString warning{"!<Sending fail>"};
        m_message_model->add_warning(warning);
    }
}


const QString &MainWindow::get_password() const
{
    return m_password;
}

const QString &MainWindow::get_username() const
{
    return m_username;
}

QColor MainWindow::get_font_color() const
{
    return m_font_color;
}


void MainWindow::dispatch_user_info_dialog()
{
    UsernameDialog user_diaglog{m_username, m_font_color, m_delegate_mode};
    user_diaglog.setModal(true);
    if(user_diaglog.exec() ==  QDialog::Accepted)
    {
        m_username         = user_diaglog.get_username();
        m_font_color       = user_diaglog.selected_color();
        auto delegate_mode = user_diaglog.get_delegate_mode();

        ui->username->setText(m_username);
        ui->username->setColor(m_font_color);
        set_delegate(delegate_mode);
    }
}

void MainWindow::set_connection_mode()
{
    if(m_is_connected)
        on_connectBtn_clicked();


    switch (m_connection_mode)
    {
    case ConnectionMode::Tcp:
        {
            auto tcp_model = std::make_unique<TcpClientModel>(m_port);
            m_presenter    = std::make_unique<TcpPresenter>(std::move(tcp_model));
            m_presenter->should_broadcast_name(m_should_broadcast_name);
            m_ssl          = None;
            m_presenter->attach(this);
            for (const auto action : ui->menuedit->actions())
            {
                if (action->text() == "Edit Certificates")
                {
                    action->setEnabled(false);
                    break;
                }
            }
        }
        break;
    case ConnectionMode::Ssl:
        {
            auto ssl_model = std::make_unique<SslClientModel>(m_port);
            m_presenter    = std::make_unique<SslPresenter>(std::move(ssl_model));
            m_presenter->should_broadcast_name(m_should_broadcast_name);
            m_ssl          = dynamic_cast<ISecureSocketLayer*>(m_presenter.get());
            m_presenter->attach(this);
            for (const auto action : ui->menuedit->actions())
            {
                if (action->text() == "Edit Certificates")
                {
                    action->setEnabled(true);
                    break;
                }
            }
            on_actionEdit_Certificates_triggered();
        }
        break;
    }
}

void MainWindow::check_txt_len()
{
    if(ui->MessageEdit->toPlainText().length() > m_max_char)
    {
        auto text = ui->MessageEdit->toPlainText();
        text.chop(text.length() - m_max_char);
        ui->MessageEdit->setPlainText(text);

        auto cursor = ui->MessageEdit->textCursor();
        cursor.setPosition(ui->MessageEdit->document()->characterCount() - 1);
        ui->MessageEdit->setTextCursor(cursor);
    }

}

void MainWindow::on_connectBtn_clicked()
{
    if(!m_is_connected)
    {
        ui->ResultEdit->setText("Awaiting response");
        m_presenter->set_up_connection(m_hostname);
    }
    else
    {
        m_presenter->disconnect();

        ui->ResultEdit->setText("");
        m_is_connected = false;
        ui->SendBtn->setEnabled(m_is_connected);
        set_Btn_to_connect();
    }
}

void MainWindow::on_SendBtn_clicked()
{
    if(m_is_connected)
    {
        const QString msg {ui->MessageEdit->toPlainText()};
        m_presenter->send_message(msg);
        ui->MessageEdit->clear();
    }
}

void MainWindow::set_Btn_to_connect()
{
    ui->connectBtn->setText("Connect");
}

void MainWindow::set_Btn_to_disconnect()
{
    ui->connectBtn->setText("Disconnect");
}

MainWindow::~MainWindow()
{
}


void MainWindow::on_actionEdit_Server_triggered()
{
    ServerSettings setting_diaglog{m_connection_mode, m_port, m_password, m_max_char, m_should_broadcast_name, m_hostname};
    setting_diaglog.setModal(true);
    if(setting_diaglog.exec() ==  QDialog::Accepted)
    {
        m_password              = setting_diaglog.password();
        m_hostname              = setting_diaglog.host();
        m_port                  = setting_diaglog.port();
        m_max_char              = setting_diaglog.max_char();
        m_connection_mode       = setting_diaglog.connection_mode();
        m_should_broadcast_name = setting_diaglog.should_broadcast_name();
        qDebug() << "got pass:" << setting_diaglog.password();
        qDebug() << "got port:" << setting_diaglog.port();
        qDebug() << "got host:" << setting_diaglog.host();
        qDebug() << "got host:" << setting_diaglog.should_broadcast_name();

        set_connection_mode();
        m_presenter->set_port(m_port);
    }
}


void MainWindow::on_actionEdit_Username_triggered()
{
    dispatch_user_info_dialog();
}





void MainWindow::on_actionEdit_Certificates_triggered()
{
    if(!m_ssl.has_value())
    {
        qDebug() << "No Secure layer set!";
        return;
    }

    CertificatesDialog cert_diaglog{m_certs_dir, std::move(m_keys_and_certificates)};
    cert_diaglog.setModal(true);
    if(cert_diaglog.exec() ==  QDialog::Accepted)
    {
        m_keys_and_certificates = std::move(cert_diaglog.get_security_bundle());

        //m_ssl_interface
        m_ssl.value()->set_private_key(m_keys_and_certificates.private_key);
        m_ssl.value()->set_public_key(m_keys_and_certificates.public_key);
        m_ssl.value()->set_root_CA(m_keys_and_certificates.root_CA);
    }
}





