cmake_minimum_required(VERSION 3.10)

# 프로젝트 이름 및 버전 설정
project(rtsp_server VERSION 1.0)

# C++ 표준 설정
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# GStreamer 패키지를 찾음
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0)
pkg_check_modules(GSTRTSP REQUIRED gstreamer-rtsp-server-1.0)

# 실행 파일 생성
add_executable(rtsp_server rtsp_server.cpp)

# GStreamer 및 GStreamer RTSP Server 라이브러리 포함
target_include_directories(rtsp_server PRIVATE ${GST_INCLUDE_DIRS} ${GSTRTSP_INCLUDE_DIRS})
target_link_libraries(rtsp_server ${GST_LIBRARIES} ${GSTRTSP_LIBRARIES})

# 컴파일러 경고를 포함한 추가 옵션 (선택 사항)
target_compile_options(rtsp_server PRIVATE -Wall -Wextra -Wpedantic)
