# Async / TLS Chat Application Using Qt6 & Boost::asio 


## Qt6 TCP/TLS Client 

Features:
* Model View Presenter (MVP) architecture
* Uses c++20/23
* TCP Client with optional TLS layer 
* Connection modes & network settings set at runtime
* Settings saved/loadable from config file (`chat_app_client_config.json`) file 
* Also works with [Raspberry Pi Pico 2W embedded TCP server](https://github.com/so-groenen/pi_pico2_w_chat_server)


## Asynchronous Boost.Asio TCP/TLS server

* Async server to handle multiple clients concurrently 
* Uses c++20 coroutines (using Asio's "awaitable" / "co_await" syntax, similar to Python/Rust async/await syntax). 
* Server's network settings configured using json file (`chat_app_server_config.json`) 