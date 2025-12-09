#include "mainwindow.h"
#include "tcp_client.h"
#include "app_presenter.h"
#include <QObject>
#include <QApplication>
#include <QStringView>

constexpr QStringView DEFAULT_HOSTNAME = u"so-VivoBook-ASUSLaptop-X530FN-S530FN";
constexpr QStringView PASSWORD         = u"louvre";
constexpr uint16_t    PORT             = 6970;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    std::unique_ptr<ITcpClientModel> tcp_client = std::make_unique<TcpClient>(PORT);
    std::unique_ptr<ITcpView> tcp_view          = std::make_unique<MainWindow>();
    tcp_view->set_default_hostname(DEFAULT_HOSTNAME);
    tcp_view->set_password(PASSWORD);

    AppPresenter presenter{std::move(tcp_client), std::move(tcp_view)};
    presenter.show();
    return a.exec();
}
