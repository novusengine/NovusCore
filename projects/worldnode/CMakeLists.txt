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

project(worldnode VERSION 1.0.0 DESCRIPTION "Worldnode for NovusCore")

file(GLOB_RECURSE WORLDNODE_FILES "*.cpp" "*.h")
set(WORLDNODE_DEPENDENCIES
    "../common/Dependencies/amy"
    "../common/Dependencies/json"
    "../common/Dependencies/robin-hood-hashing"
    "${CMAKE_CURRENT_LIST_DIR}/Dependencies/Entt"
    "${ZLIB_INCLUDE_DIRS}"
    "${CMAKE_SOURCE_DIR}/dep/TracyProfiler"
    "${CMAKE_SOURCE_DIR}/dep/taskflow"
    "${CMAKE_SOURCE_DIR}/dep/asio"
    "${CMAKE_SOURCE_DIR}/dep/angelscript/include"
)

add_executable(worldnode ${WORLDNODE_FILES})
find_assign_files(${WORLDNODE_FILES})

set_property(TARGET worldnode PROPERTY CXX_STANDARD 17)

if (APPLE)
    target_link_libraries(worldnode c++fs)
elseif (UNIX)
    target_link_libraries(worldnode stdc++fs)
endif()

# Set VERSION & FOLDER Property
set_target_properties(worldnode PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(worldnode PROPERTIES FOLDER "server")

add_compile_definitions(NOMINMAX _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
if (ENABLE_TRACY_PROFILER)
add_compile_definitions(TRACY_ENABLE TRACY_ON_DEMAND)
endif()

target_link_libraries(worldnode common angelscript)
target_include_directories(worldnode PRIVATE ${WORLDNODE_DEPENDENCIES})
include_directories(worldnode "${common_SOURCE_DIR}")
install(TARGETS worldnode DESTINATION bin)
