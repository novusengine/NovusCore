# MIT License

# Copyright (c) 2018-2019 NovusCore

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# CMake >= 3.12
cmake_minimum_required(VERSION 3.12)

# add these options before declaring the project
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_CONFIGURATION_TYPES Debug Release RelWithDebInfo CACHE STRING "" FORCE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")
set(CMAKE_CXX_STANDARD 17)
project(NovusCore)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/build" CACHE PATH "default install path" FORCE )
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    if (NOT MSVC)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    endif()
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}")

add_compile_definitions(NC_$<CONFIG> NOMINMAX WIN32_LEAN_AND_MEAN)

if (MSVC)
    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang") #MSVC
        add_definitions("/MP")
        add_compile_options("/WX")
    else() #GCC/Clang on Windows
        add_compile_options("-Werror")
    endif()
else() #GCC/Clang
    #add_compile_options("-Werror")
endif()

include(cmake/ProjectOptions.cmake)
include(cmake/ShowProjectOptions.cmake)
include(cmake/FindFiles.cmake)

add_subdirectory(dep)
add_subdirectory(projects)