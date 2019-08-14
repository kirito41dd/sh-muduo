aux_source_directory(. DIR_BASE_SRCS)

add_library(sh_base SHARED ${DIR_BASE_SRCS}) # 动态库使用 SHARED
target_link_libraries(sh_base pthread)

#set(HEADERS *.h)
file(GLOB HEADERS "*.h")


# 安装文件到 ${CMAKE_INSTALL_PREFIX}/ 目录下
install(FILES ${HEADERS} DESTINATION include/sh/base)
install(TARGETS sh_base LIBRARY DESTINATION lib)

#FILE(GLOB HEADERS "*.h")
#add_custom_target(sh_base_headers SOURCES ${HEADERS})
