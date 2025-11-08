#include "mainwindow.h"
#include "tcp_client.h"
#include "app_presenter.h"
#include <QObject>
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    uint16_t port{6970};

    std::unique_ptr<ITcpClientModel> tcp_client = std::make_unique<TcpClient>(port);
    std::unique_ptr<ITcpView> tcp_view          = std::make_unique<MainWindow>();

    AppPresenter presenter{std::move(tcp_client), std::move(tcp_view)};
    presenter.show();
    return a.exec();
}
