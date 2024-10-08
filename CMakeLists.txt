cmake_minimum_required(VERSION 3.10)

# 프로젝트 이름 및 버전 설정
project(RPI_mini_VMS_client VERSION 1.0)

# C++ 표준 설정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# GStreamer 패키지를 찾음
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0)
pkg_check_modules(GSTRTSP REQUIRED gstreamer-rtsp-server-1.0)

# jsoncpp 패키지를 찾음
find_package(jsoncpp REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${GST_INCLUDE_DIRS})
include_directories(${GSTRTSP_INCLUDE_DIRS})
include_directories(/usr/include/jsoncpp)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/src/rtsp_server.cpp
    ${CMAKE_SOURCE_DIR}/src/gst_util.cpp
)

# 실행 파일 생성
add_executable(RPI_mini_VMS_client ${SOURCES})

# GStreamer 및 GStreamer RTSP Server 라이브러리 포함
target_include_directories(RPI_mini_VMS_client PRIVATE ${GST_INCLUDE_DIRS} ${GSTRTSP_INCLUDE_DIRS})

# 라이브러리 연결
target_link_libraries(RPI_mini_VMS_client
    ${GST_LIBRARIES}
    ${GSTRTSP_LIBRARIES}
    jsoncpp
)
