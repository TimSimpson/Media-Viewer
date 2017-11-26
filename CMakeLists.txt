cmake_minimum_required(VERSION 3.5)
project(MediaView CXX)

find_package(sdl2 2.0.7)
find_package(Lp3_Gfx)
find_package(Lp3_Input)

add_library(media_viewer_lib
    ${CMAKE_CURRENT_SOURCE_DIR}/src/media.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/media.cpp
)
target_link_libraries(media_viewer_lib Boost::boost lp3::core lp3::gfx)

add_executable(windows ${CMAKE_CURRENT_SOURCE_DIR}/src/windows.cpp)
target_link_libraries(windows media_viewer_lib Boost::boost lp3::core lp3::gfx)
