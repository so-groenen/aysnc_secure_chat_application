#ifndef SSL_CHAT_SESSION_HPP
#define SSL_CHAT_SESSION_HPP


#ifndef USE_SSL_SOCKET
    #error "USE_SSL_SOCKET must be defined at compilation time"
#endif

#include "abstract_chat_session.hpp"
using SslChatSession = AbstractChatSession;
 
#endif