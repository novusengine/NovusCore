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

project(relayserver VERSION 1.0.0 DESCRIPTION "relayserver for NovusCore")
add_executable(relayserver)

set(RELAYSERVER_MAIN
    "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RelaySocketHandler.h"
)
set(RELAYSERVER_RELAYSOCKET
    "${CMAKE_CURRENT_LIST_DIR}/Socket/RelaySocket.h"
    "${CMAKE_CURRENT_LIST_DIR}/Socket/RelaySocket.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Socket/RelayNodeConnection.h"
    "${CMAKE_CURRENT_LIST_DIR}/Socket/RelayNodeConnection.cpp"
)

set(RELAYSERVER_DEPENDENCIES
    "../common/Dependencies/amy"
    "../common/Dependencies/json"
)

source_group(" " FILES ${RELAYSERVER_MAIN})
source_group(Socket FILES ${RELAYSERVER_RELAYSOCKET})

target_sources(relayserver
    PRIVATE
    ${RELAYSERVER_MAIN}
    ${RELAYSERVER_RELAYSOCKET}
)


# Set VERSION & FOLDER Property
set_target_properties(relayserver PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(relayserver PROPERTIES FOLDER "server")

target_link_libraries(relayserver common)
target_include_directories(relayserver PRIVATE ${RELAYSERVER_DEPENDENCIES})
include_directories(relayserver "${common_SOURCE_DIR}")
install(TARGETS relayserver DESTINATION bin)