#include "mainwindow.h"
// #include "ssl_client_model.h"
// #include "tcp_client_model.h"
// #include "abstract_tcp_presenter.h"
// #include "app_presenter.h"
// #include "tcp_presenter.h"
// #include "ssl_presenter.h"
#include <QObject>
#include <QApplication>
#include <QStringView>
// #include "abstract_tcp_model.h"


// constexpr uint16_t    PORT             = 6970;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // AbstractTcpModel_ptr tcp_client     = std::make_unique<TcpClientModel>(PORT);
    // AbstractTcpPresenter_ptr presenter  = std::make_unique<TcpPresenter>(std::move(tcp_client));

    MainWindow main_window{};

    main_window.show();
    return a.exec();
}
