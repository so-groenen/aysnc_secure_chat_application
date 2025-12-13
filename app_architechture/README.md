# Model-Presenter-View Architechture for TCP/SSL Application

This is a small C++ file which showcases the whole architecture of the "Model-View-Presenter" software.<br>
Here, the Qt Mainwindow is the "View", the low level TCP/SecureSocketLayer clients are the "Models", and the Presenter, acts as a middle man,
responible for message formatting, error handling, and making additional connection checks (for example passing password to the network).
<br>  
One first defines the building block interfaces for the Network (ITcpClient, ISecureSocketLayer, ITcpEventHandler):<br>
 
- The TCP/SSL ("Model") network interfaces are both *ITcpClient*s, <br>
- The Qt GUI ("View") a *ITcpEventHandler*<br>
- and as such, the "Presenter" is both a *ITcpClient* (for the GUI) and a *ITcpEventHandler* (for the Networks)<br>

In the MVP pattern we just connect those parts:

- The Presenter owns the TcpClientModel and calls forwards & formats messages from the GUI to the Client.
- The TcpClientModel holds a pointer to the Presenter in order to notify it of TcpEvents (connection status, message reception etc) and thus exposes
 an "attach" method to the presenter.

- In turn, the View owns the Presenter, which acts as a "TcpClient". The presenter thus needs to hold a pointer to the View in order to notify it of TcpEvents,
while the View needs to call the presenter to format message to send them.

The *ISecureSocketLayer* interface is added "on top" of the existing *ITcpClientModel*: The *SecureSocketlPresenter* creates a *SslClientModel* and splits the interfaces into TCP part & *ISecureSocketLayer* part. <br> 
The connection protocol (TCP vs SSL) can thus easily be chosen at runtime! 
 