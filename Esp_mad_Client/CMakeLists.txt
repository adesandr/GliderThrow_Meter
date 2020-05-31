# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

#Project version
set(PROJECT_VER "1.0.2")

#Additional Directorie for comonents
set(EXTRA_COMPONENT_DIRS "${CMAKE_SOURCE_DIR}/../extra_components")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(esp-mad-client)