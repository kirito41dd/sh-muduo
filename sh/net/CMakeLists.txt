aux_source_directory(. DIR_NET_SRCS)

add_library(sh_net SHARED ${DIR_NET_SRCS})
target_link_libraries(sh_net sh_base)

#set(HEADERS *.h)
file(GLOB HEADERS "*.h")

# 安装文件到 ${CMAKE_INSTALL_PREFIX}/ 目录下
install(FILES ${HEADERS} DESTINATION include/sh/net)
install(TARGETS sh_net LIBRARY DESTINATION lib)

#FILE(GLOB HEADERS "*.h")
#add_custom_target(sh_net_headers SOURCES ${HEADERS})

add_subdirectory(./http)
