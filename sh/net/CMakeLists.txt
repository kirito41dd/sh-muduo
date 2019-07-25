aux_source_directory(. DIR_NET_SRCS)
add_library(sh_net ${DIR_NET_SRCS})


set(HEADERS *.h)
#FILE(GLOB HEADERS "*.h")
#add_custom_target(sh_net_headers SOURCES ${HEADERS})
