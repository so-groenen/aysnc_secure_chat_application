#include "mainwindow.h"
#include "tcp_client.h"
#include "app_presenter.h"
#include <QObject>
#include <QApplication>
#include <QStringView>

constexpr QStringView DEFAULT_HOSTNAME = u"default_hostname";
constexpr QStringView PASSWORD         = u"louvre";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    uint16_t port{6970};

    std::unique_ptr<ITcpClientModel> tcp_client = std::make_unique<TcpClient>(port);
    std::unique_ptr<MainWindow> tcp_view        = std::make_unique<MainWindow>();
    tcp_view->set_default_hostname(DEFAULT_HOSTNAME);
    tcp_view->set_password(PASSWORD);

    AppPresenter presenter{std::move(tcp_client), std::move(tcp_view)};
    presenter.show();
    return a.exec();
}
