# MIT License

# Copyright (c) 2018 NovusCore

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

project(worldserver VERSION 1.0.0 DESCRIPTION "Worldserver for NovusCore")
add_executable(worldserver)

set(WORLDSERVER_MAIN
    "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/NodeSocketHandler.h"
)
set(WORLDSERVER_WORLDSOCKET
"${CMAKE_CURRENT_LIST_DIR}/Socket/NodeSocket.h"
"${CMAKE_CURRENT_LIST_DIR}/Socket/NodeSocket.cpp"
)

set(WORLDSERVER_DEPENDENCIES
    "../common/Dependencies/amy"
    "../common/Dependencies/json"
)

source_group(" " FILES ${WORLDSERVER_MAIN})
source_group(Socket FILES ${WORLDSERVER_WORLDSOCKET})

target_sources(worldserver
    PRIVATE
    ${WORLDSERVER_MAIN}
    ${WORLDSERVER_WORLDSOCKET}
)


# Set VERSION & FOLDER Property
set_target_properties(worldserver PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(worldserver PROPERTIES FOLDER "server")

target_link_libraries(worldserver common)
target_include_directories(worldserver PRIVATE ${WORLDSERVER_DEPENDENCIES})
include_directories(worldserver "${common_SOURCE_DIR}")
install(TARGETS worldserver DESTINATION bin)