cmake_minimum_required(VERSION 3.10.0)
project(async_chat_server VERSION 0.1.0 LANGUAGES C CXX)

add_executable(async_chat_server src/main.cpp)
target_compile_features(async_chat_server PRIVATE cxx_std_23)
target_link_libraries  (async_chat_server PRIVATE stdc++exp)
if(MINGW)
  target_link_libraries(async_chat_server PRIVATE ws2_32 wsock32 )
endif()