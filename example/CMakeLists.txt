project(example)

find_library(FLV libflv.a ${CMAKE_SOURCE_DIR}/build/flv/)

add_executable(audioFilewriter flv/AudioFlvFileWriter.cpp)
target_link_libraries(audioFilewriter ${FLV})

add_executable(flvdemuxer flv/FlvDemuxer.cpp)
target_link_libraries(flvdemuxer ${FLV})

add_executable(videoFilewriter flv/VideoFlvFileWriter.cpp)
target_link_libraries(videoFilewriter ${FLV})







