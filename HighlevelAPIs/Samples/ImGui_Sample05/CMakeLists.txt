# 최소 CMake 버전 설정
cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

# 프로젝트 이름 설정
project(MyProject)

# 소스 파일 지정
set(SOURCES
    sample05.cpp
)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# 실행 파일 생성
add_executable(MyExecutable ${SOURCES})

set(VIZAPI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../VizAPIs_Linux/dist)

add_definitions(-D__linux__)

target_include_directories(MyExecutable  PRIVATE
    ${VIZAPI_DIR}/include
)

# 필요한 라이브러리 링크
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
target_link_libraries(MyExecutable
    ${VIZAPI_DIR}/lib/libVizAPIs_Linuxd.so
 glfw
)
else()
target_link_libraries(MyExecutable
    ${VIZAPI_DIR}/lib/libVizAPIs_Linux.so
 glfw
)
endif()