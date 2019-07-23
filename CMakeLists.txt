cmake_minimum_required(VERSION 2.6)

project(sh_muduo)

# add the source in project root directory
aux_source_directory(. DIR_SRC)

# add header file include directories
#include_directories(./ ./sh/net ./sh/base)  # 可以不要后俩个，#include "sh/net/*.h" 这么写
include_directories(./)

# add block directories
add_subdirectory(sh/base)
add_subdirectory(sh/net)

# target
add_executable(sh_muduo ${DIR_SRC})
target_link_libraries(sh_muduo sh_base sh_net)
