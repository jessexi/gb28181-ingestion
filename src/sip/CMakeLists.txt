aux_source_directory(. DIR_LIB_SRCS)

link_libraries(avcodec avutil avformat swscale )

add_library(SIP_LIB ${DIR_LIB_SRCS})