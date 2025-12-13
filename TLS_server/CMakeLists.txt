cmake_minimum_required(VERSION 3.10.0)
project(async_chat_server VERSION 0.1.0 LANGUAGES C CXX)

add_executable(async_chat_server src/main.cpp
                                 src/ssl_loader.cpp
                                 src/chat_room.cpp
                                 src/abstract_chat_session.cpp)
 

target_compile_features    (async_chat_server PRIVATE cxx_std_23)
target_include_directories (async_chat_server PRIVATE include)
target_link_libraries      (async_chat_server PRIVATE ssl crypto)
target_compile_definitions (async_chat_server PRIVATE USE_SSL_SOCKET)

add_custom_command(
    TARGET async_chat_server
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/certificates 
    $<TARGET_FILE_DIR:async_chat_server>/certificates
)

add_custom_command(
    TARGET async_chat_server
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/password.txt 
    $<TARGET_FILE_DIR:async_chat_server>/password.txt
)

if(MINGW)
    target_link_libraries(async_chat_server PRIVATE ws2_32 wsock32 )
endif()