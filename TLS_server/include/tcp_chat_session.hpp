#ifndef TCP_CHAT_SESSION_HPP
#define TCP_CHAT_SESSION_HPP


#ifndef USE_TCP_SOCKET
    #error "USE_TCP_SOCKET must be defined at compilation time"
#endif

#include "abstract_chat_session.hpp"
using TcpChatSession = AbstractChatSession;

#endif