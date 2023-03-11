cmake_minimum_required(VERSION 3.12)

if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif()

project(NovusCore VERSION 1.0.0)

include(CMake/Options.cmake)

add_subdirectory(Submodules)