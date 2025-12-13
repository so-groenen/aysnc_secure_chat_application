#include <print>
#include <memory>
#include <iostream>
#include <optional>

#define EXITING false

// MESSAGES
std::array<const char*, 5> MSGS = {"Hello world", "How are you", "Who is this", "Nothing here","42"};
size_t CURRENT_MSG_INDEX        = 0;



// Rust Style C++ definition
auto None = std::nullopt;
template<typename T>
using Option = std::optional<T>;




/////////////////////////////////////////////////////////
/////             BASIC  NETWORKS INTERFACES        /////
/////////////////////////////////////////////////////////

class ITcpNetwork
{
public:
    virtual ~ITcpNetwork() = default;
    virtual void make_connection() = 0;
};

class ISecureLayer
{
public:
    virtual ~ISecureLayer() = default;
    virtual void set_private_key(const char*) = 0;
};

class ITcpEventHandler
{
public:
    virtual ~ITcpEventHandler() = default;
    virtual void handle_message_reception(const char* msg) = 0;
};


////////////////////////////////////////////////////////////
/////             MODEL-VIEW-PRESENTER                /////
///////////////////////////////////////////////////////////

// GUI side
class ITcpView : public ITcpEventHandler
{
public:
    virtual ~ITcpView() = default;
};


//GUI + TCP Abstraction Layer: Handles input validation + msg passing
class AbstractTcpPresenter : public ITcpNetwork, public ITcpEventHandler
{
protected:
    ITcpView* m_view{};
public:
    virtual ~AbstractTcpPresenter() = default;
    void attch(ITcpView* view)
    {
        m_view = view;
    }
};

// The models:
class AbstractTcpModel : public ITcpNetwork 
{
protected:
    ITcpEventHandler* m_tcp_event_handler{};
public:
    ~AbstractTcpModel() = default;
    void attach(ITcpEventHandler* event_handler)
    {
        m_tcp_event_handler = event_handler;
    }
};

class AbstractSslModel : public AbstractTcpModel, public ISecureLayer
{
public:
    ~AbstractSslModel() = default;
};


////////////////////////////////////////////////////////
/////           CONCRETE NETWORKS MODELS           /////
////////////////////////////////////////////////////////

// Interacting with the Os 

class TcpClientModel : public AbstractTcpModel
{
public:
    TcpClientModel() = default;
    void make_connection() override
    {
        std::println("TcpNetwork: using TCP Sockets! Getting a message:");
        m_tcp_event_handler->handle_message_reception(MSGS[CURRENT_MSG_INDEX]);
        CURRENT_MSG_INDEX = (CURRENT_MSG_INDEX+1) % MSGS.size();
    }
};


class SslClientModel : public AbstractSslModel
{
public:
    SslClientModel() = default;
    void make_connection() override
    {
        std::println("SslNetwork: using Secure Sockets to make connection! Getting a message:");
        std::string secure_msg = "Secure " + std::string{MSGS[CURRENT_MSG_INDEX]}; 

        m_tcp_event_handler->handle_message_reception(secure_msg.c_str());
        CURRENT_MSG_INDEX = (CURRENT_MSG_INDEX+1) % MSGS.size();
    }
    void set_private_key(const char* key) override
    {
        std::println("SslNetwork: Loading private keys from \"{}\" into ssl sockets!", key);
    }
};



///////////////////////////////////////////////////////////////
/////       PRESENTER INTERACTS WITH ABSTRACT MODELS      /////
///////////////////////////////////////////////////////////////

class TcpPresenter : public AbstractTcpPresenter
{
protected:
    std::unique_ptr<AbstractTcpModel> m_tcp_model{};
public:

    TcpPresenter(std::unique_ptr<AbstractTcpModel> network = std::make_unique<TcpClientModel>())
         : m_tcp_model{std::move(network)}
    {
        m_tcp_model->attach(this);
    }
    void make_connection() override
    {
        std::println("TcpPresenter: Set up all the data (IP, Hostname) and call the network!");
        m_tcp_model->make_connection();
    }
    void handle_message_reception(const char* msg) override
    {
        std::println("TcpPresenter: formatting msg: FROM SERVER: \"{}\"", msg);
        m_view->handle_message_reception(msg);
    }
};

class SslPresenter : public TcpPresenter, public ISecureLayer
{
    ISecureLayer* m_secure{};
public:
    SslPresenter(std::unique_ptr<AbstractSslModel> secure_tcp_model = std::make_unique<SslClientModel>())
        : TcpPresenter(std::move(secure_tcp_model))                // presenter only takes TcpCLientModel part!
    {
        m_secure = dynamic_cast<ISecureLayer*>(m_tcp_model.get());      // secure only takes secure part! 
    }
    void set_private_key(const char* file) override
    {
        std::println("SslPresenter: Grab the private key from the GUI!");
        std::println("SslPresenter: found \"{}\": validated!", file);
        m_secure->set_private_key(file);
    }
};

///////////////////////////////
/////       THE GUI       /////
///////////////////////////////

enum class ConnectionMode
{
    TCP,
    SSL,
};



class MainWindow : public ITcpView
{
    std::unique_ptr<AbstractTcpPresenter> m_presenter{};
    Option<ISecureLayer*> m_key_handler = None;
    ConnectionMode m_mode{};
    bool m_is_running = true;
public:
    MainWindow(ConnectionMode mode)
        : m_mode{mode}
    {
        handle_connection_mode();
    }
    void handle_message_reception(const char* msg) override
    {
        std::println("MainWindow: GOT: \"{}\"", msg);
    }

    void handle_connection_mode() 
    {
        switch (m_mode)
        {
            case ConnectionMode::SSL:
                {
                    auto ssl_interface = std::make_unique<SslPresenter>();
                    m_key_handler      = ssl_interface.get();               // Interface segregation: ISecurity
                    m_presenter        = std::move(ssl_interface);          // Interface segregation: INetwork
                }
                break;
            case ConnectionMode::TCP:
                {
                    auto tcp_interface = std::make_unique<TcpPresenter>();
                    m_key_handler      = None;                              // Using Option = None like in Rust
                    m_presenter        = std::move(tcp_interface);
                }
                break;
        }
        m_presenter->attch(this);
    }
    bool handle_button()
    {
        std::string input_buffer{};
        while (true)
        {
            std::println("tcp or ssl? otherwise exit!");
            std::getline(std::cin, input_buffer);
            if(input_buffer.contains("tcp"))
            {
                m_mode = ConnectionMode::TCP;
                break;
            }
            else if(input_buffer.contains("ssl"))
            {
                m_mode = ConnectionMode::SSL;
                break;
            }
            else if(input_buffer.contains("exit"))
            {
                std::println("exiting");
                m_is_running = false;
                return m_is_running;
            }
        }
        return true;
    }
    void make_connection()
    {
        if(m_key_handler.has_value())
        {
            std::string input_buffer{};
            std::println("enter key path");
            std::getline(std::cin, input_buffer);
            m_key_handler.value()->set_private_key(input_buffer.c_str());
        }    
        m_presenter->make_connection();
    }
    void run()
    {
        while (m_is_running)
        {
            if(handle_button() == EXITING)
                break;

            handle_connection_mode();
            make_connection();
        }
    }
};

 

int main()
{
    MainWindow qt_app{ConnectionMode::TCP};
    qt_app.run();


    std::println("goodbye");
}
