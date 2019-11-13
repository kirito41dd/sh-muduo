add_executable(TcpClient TcpClient.cpp)
target_link_libraries(TcpClient sh_base sh_net)

add_executable(Singleton Singleton.cpp)
target_link_libraries(Singleton pthread)

add_executable(Condition Condition.cpp)
target_link_libraries(Condition sh_base)

add_executable(HttpServer HttpServer.cpp)
target_link_libraries(HttpServer sh_net_http)
