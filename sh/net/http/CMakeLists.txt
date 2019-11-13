aux_source_directory(. DIR_NET_HTTP_SRCS)

add_library(sh_net_http SHARED ${DIR_NET_HTTP_SRCS})
target_link_libraries(sh_net_http sh_net)

#set(HEADERS *.h)
file(GLOB HEADERS "*.h")

# 安装文件到 ${CMAKE_INSTALL_PREFIX}/ 目录下
install(FILES ${HEADERS} DESTINATION include/sh/net/http)
install(TARGETS sh_net_http LIBRARY DESTINATION lib)
