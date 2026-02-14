# Async / TLS Chat Application Using Qt6 & Boost::asio 

TCP (Transport Control Protocol) or TLS (Transport Layer Security, formally known as Secure Socket Layer)


## Qt6 TCP/TLS Client 

<p align="center">
<img src="demo/chat_app.jpg" alt="kick" width="200">
<img src="demo/network_settings.jpg" alt="kick" width="200">
</p>

App features:

* Choose between "more modern" speech bubble or "old school" IRC-style text
* Connection modes (TCP vs TLS) & network settings set at runtime
* Network Settings saved/loaded from config file (`chat_app_client_config.json`) file 
* Compatible with the [Raspberry Pi Pico 2W bare metal TCP chat server](https://github.com/so-groenen/pi_pico2_w_chat_server)

<br>

Behind the curtains:
* Uses Model-View-Presenter (MVP) architecture with C++20/23 features.
* Messages get parsed to json containing relevant fields (username, user session id, message, font color) and broadcaster by server.
* TCP Client with optional TLS layer on top. 



## Asynchronous Boost.asio TCP/TLS server

* Asyncronous server handlinng multiple clients concurrently 
* Uses c++20 coroutines (asio's *awaitable* / *co_await* syntax, reminiscent of Python's *async*/*await* syntax). 
* Server's network settings (TCP vs SSL, *path/to/certificates* etc) configured using json file (`chat_app_server_config.json`).