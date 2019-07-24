aux_source_directory(. DIR_BASE_SRCS)

add_library(sh_base ${DIR_BASE_SRCS})

set(HEADERS TimeStamp.h)
#install(FILES ${HEADERS} DESTINATION sh/base)

target_link_libraries(sh_base pthread)

#FILE(GLOB HEADERS "*.h")
#add_custom_target(sh_base_headers SOURCES ${HEADERS})
