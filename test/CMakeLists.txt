add_executable(TcpClient TcpClient.cpp)
target_link_libraries(TcpClient sh_base sh_net)

add_executable(Singleton Singleton.cpp)
#target_link_libraries(Singleton pthread)
