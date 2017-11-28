cmake_minimum_required(VERSION 3.5)
project(MediaView CXX)

find_package(sdl2 2.0.7)
find_package(Lp3_Gfx)
find_package(Lp3_Input)
find_package(Lp3_Sims)

add_library(media_viewer_lib
    ${CMAKE_CURRENT_SOURCE_DIR}/src/browser.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/browser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/controls.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/controls.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mplayer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mplayer.cpp
)
target_link_libraries(media_viewer_lib
                      Boost::boost
                      lp3::core lp3::gfx lp3::input lp3::sims)

add_executable(windows ${CMAKE_CURRENT_SOURCE_DIR}/src/windows.cpp)
target_link_libraries(windows media_viewer_lib Boost::boost sdl2::ttf lp3::core lp3::gfx)
