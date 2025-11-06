#ifndef IWINDOWCONTROLLER_H
#define IWINDOWCONTROLLER_H

#include <QObject>

// This interface reactos to MainWindow events
// - We are connected to MainWindow Signals
// - We have a reference to a MainWindow object and can draw on the GUI depending on the state.
// We follow "interface segregation principle": One controller interface for the MainWindow, one for the TCP
// Then we implement both of them in the Presenter class, which can "talk" to both of them.

class IWindowController
{
public:
    virtual ~IWindowController();
    virtual void handle_connect_Btn(QString hostname) = 0;
    virtual void handle_disconnect_Btn() = 0;
    virtual void handle_message_send(QString msg) = 0;


signals:

};

#endif // IWINDOWCONTROLLER_H
