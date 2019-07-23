aux_source_directory(. DIR_BASE_SRCS)

add_library(sh_base ${DIR_BASE_SRCS})

set(HEADERS TimeStamp.h)
#install(FILES ${HEADERS} DESTINATION sh/base)

#FILE(GLOB HEADERS "*.h")
#add_custom_target(sh_base_headers SOURCES ${HEADERS})
