aux_source_directory(. DIR_NET_SRCS)
add_library(sh_net ${DIR_NET_SRCS})



FILE(GLOB HEADERS "*.h")
add_custom_target(sh_net_headers SOURCES ${HEADERS})
