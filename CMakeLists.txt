cmake_minimum_required(VERSION 2.8)

project(Format)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/common)
include_directories(${PROJECT_SOURCE_DIR}/acodec)
include_directories(${PROJECT_SOURCE_DIR}/acodec)

aux_source_directory(${PROJECT_SOURCE_DIR}/common  COMMON_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/vcodec  VCODEC_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/acodec  ACODEC_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/flv     FLV_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/example EXAMPLE_SRC)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    message("enable Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb3")    
endif()

option(ENABLE_ALL "enable flv build" ON)
if(ENABLE_ALL)
add_subdirectory(flv)
endif()

option(ENABLE_EXAMPLE "enable example build" OFF)
if(ENABLE_EXAMPLE)
add_subdirectory(example)
endif()


add_custom_target(debug
        COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Build Debug"
    )

add_custom_target(release
    COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Build Release"
)

add_custom_target(example
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug -DENABLE_EXAMPLE=ON -DENABLE_ALL=OFF ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Build Example"
)



























