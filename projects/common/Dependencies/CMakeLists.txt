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

project(common_deps VERSION 1.0.0)

## Amy

find_package(MySQL REQUIRED)

add_library(amy INTERFACE)
add_library(amy::amy ALIAS amy)

target_include_directories(amy INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/amy"
)

target_link_libraries(amy INTERFACE
    MySQL::MySQL
    asio::asio
)

## Json

add_library(json INTERFACE)
add_library(json::json ALIAS json)

target_include_directories(json INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/json"
)

## Robin hood hashing

add_library(robin-hood-hashing INTERFACE)
add_library(robin-hood-hashing::robin-hood-hashing ALIAS robin-hood-hashing)

target_include_directories(robin-hood-hashing INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/robin-hood-hashing"
)

## common_deps

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
add_library(common::dependencies ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} INTERFACE
    amy::amy
    json::json
    robin-hood-hashing::robin-hood-hashing
)
