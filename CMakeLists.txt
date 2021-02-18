# Based off https://github.com/lefticus/cpp_starter_project/blob/master/CMakeLists.txt
# Licence : The Unlicense
# Adapted to C and stripped down to the scale of this project

cmake_minimum_required(VERSION 3.16)

# Set the project name to your project name, my project isn't very descriptive
project(Net-Game C)
include(cmake/StandardProjectSettings.cmake)
include(cmake/PreventInSourceBuilds.cmake)


# Link this 'library' to set the c standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE c_std_11)

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# sanitizer options if supported by compiler
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

# enable doxygen
include(cmake/Doxygen.cmake)
enable_doxygen()

option(ENABLE_TESTING "Enable Test Builds" ON)

# Add the modules to find SDL2 libraries
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/sdl2-cmake-scripts")

if(ENABLE_TESTING)
  set(NET_HEADERS_DIR "${CMAKE_SOURCE_DIR}/src/include")
  enable_testing()
  message("Building Tests.")
  add_subdirectory(test)
endif()

add_subdirectory(src)
