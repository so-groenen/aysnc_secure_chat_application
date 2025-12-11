# Async / TLS Chat Application Using Qt6 & Boost::asio 

For "vanilla" TCP Chat Application branche see [TCP branche](https://github.com/so-groenen/aysnc_chat_application/tree/TCP_async_chat_app)


TODO: 
* Remove async ssl handshake 
* remove "ISecureSocketLayer"
* keep handshake everything in SslServer
* maybe use macros in chat_Session: #ifdef USE_SSL: using SocketType = SslSocket; #else using SocketType = TcpSocket
* maybe also in chat server when including chat_session: in TCP_chat_server: #define USE_SSL
* IChatRoom lives as long as the program: keep it as a unique pointer? Or Reference?
* user ITcpPresenter (extends ITcpClient + ITcpEventHandler) --> ISslPresenter   (+ISecureSocketLayer)
* ITcpClientModel (extends ITcpClient + ITcpEventHandler)    --> ISslClientModel (+ISecureSocketLayer)
* TcpPresenter --> SslPresenter (extends) and keeps pointer to ISslSecureSocketLayer* = dynamic_cas<ISslClientModel>(m_ssl_model)
