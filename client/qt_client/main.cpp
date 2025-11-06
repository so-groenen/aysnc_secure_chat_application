#include "mainwindow.h"
#include "tcpclient.h"
#include <QObject>
#include <QApplication>

void connect_client_to_window(const TcpClient* client, const MainWindow* w)
{
    QObject::connect(w,      SIGNAL(trying_to_connect(QString)),
                     client, SLOT(set_up_connection(QString)));
    QObject::connect(client, SIGNAL(connection_result(bool,QString)),
                     w,      SLOT(handle_connect_response(bool,QString)));
    QObject::connect(w,      SIGNAL(disconnecting()),
                     client, SLOT(disconnect()));
    QObject::connect(client, SIGNAL(is_disconneced_from_Host()),
                     w,      SLOT(handle_disconnect_from_host()));
    QObject::connect(client, SIGNAL(recieved_msg(QString)),
                     w,      SLOT(handle_msg_reception(QString)));
    QObject::connect(w,      SIGNAL(sending_message(QString)),
                     client, SLOT(send_message(QString)));
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    uint32_t Port{6970};
    TcpClient client{Port};
    MainWindow window{};
    connect_client_to_window(&client, &window);

    window.show();
    return window.exec();
}
