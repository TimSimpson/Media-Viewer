cmake_minimum_required(VERSION 3.5)
project(MediaView CXX)

find_package(sdl2 2.0.7)
find_package(Lp3_Gfx)
find_package(Lp3_Input)

add_executable(windows ${CMAKE_CURRENT_SOURCE_DIR}/src/windows.cpp)
target_link_libraries(windows Boost::boost lp3::core lp3::gfx)
