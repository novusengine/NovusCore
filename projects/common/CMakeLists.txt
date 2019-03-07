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

project(common VERSION 1.0.0 DESCRIPTION "Common is a static library for NovusCore")
add_library(common)

set(COMMON_MAIN
    "${CMAKE_CURRENT_LIST_DIR}/common.h"
    "${CMAKE_CURRENT_LIST_DIR}/common.cpp"
)
set(COMMON_CRYPTOGRAPHY
    "${CMAKE_CURRENT_LIST_DIR}/Cryptography/BigNumber.h"
    "${CMAKE_CURRENT_LIST_DIR}/Cryptography/BigNumber.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Cryptography/SHA1.h"
    "${CMAKE_CURRENT_LIST_DIR}/Cryptography/SHA1.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Cryptography/ArcFour.h"
    "${CMAKE_CURRENT_LIST_DIR}/Cryptography/ArcFour.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Cryptography/HMAC.h"
    "${CMAKE_CURRENT_LIST_DIR}/Cryptography/HMAC.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Cryptography/StreamCrypto.h"
    "${CMAKE_CURRENT_LIST_DIR}/Cryptography/StreamCrypto.cpp"
)
set(COMMON_DATABASE
    "${CMAKE_CURRENT_LIST_DIR}/Database/DatabaseConnector.h"
    "${CMAKE_CURRENT_LIST_DIR}/Database/DatabaseConnector.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Database/PreparedStatement.h"
    "${CMAKE_CURRENT_LIST_DIR}/Database/PreparedStatement.cpp"
)
set(COMMON_CONFIG
    "${CMAKE_CURRENT_LIST_DIR}/Config/ConfigHandler.h"
    "${CMAKE_CURRENT_LIST_DIR}/Config/ConfigHandler.cpp"
)
set(COMMON_NETWORKING
    "${CMAKE_CURRENT_LIST_DIR}/Networking/BaseSocket.h"
    "${CMAKE_CURRENT_LIST_DIR}/Networking/TcpServer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Networking/ByteBuffer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Networking/ByteBuffer.cpp"
)
set(COMMON_NETWORKING_OPCODE
    "${CMAKE_CURRENT_LIST_DIR}/Networking/Opcode/Opcode.h"
    "${CMAKE_CURRENT_LIST_DIR}/Networking/Opcode/Opcode.cpp"
)
set(COMMON_UTILS
    "${CMAKE_CURRENT_LIST_DIR}/Utils/SharedPool.h"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/ConcurrentQueue.h"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/DebugHandler.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/DebugHandler.h"
)

set(COMMON_DEPENDENCIES
    "${CMAKE_CURRENT_LIST_DIR}/Dependencies/amy"
    "${CMAKE_CURRENT_LIST_DIR}/Dependencies/json"
)

source_group(" " FILES ${COMMON_MAIN})
source_group(Cryptography FILES ${COMMON_CRYPTOGRAPHY})
source_group(Database FILES ${COMMON_DATABASE})
source_group(Config FILES ${COMMON_CONFIG})
source_group(Networking FILES ${COMMON_NETWORKING})
source_group(Networking\\Opcode FILES ${COMMON_NETWORKING_OPCODE})
source_group(Utils FILES ${COMMON_UTILS})

target_sources(common
    PRIVATE
    ${COMMON_MAIN}
    ${COMMON_CRYPTOGRAPHY}
    ${COMMON_DATABASE}
    ${COMMON_CONFIG}
    ${COMMON_NETWORKING}
    ${COMMON_NETWORKING_OPCODE}
    ${COMMON_UTILS}
)

target_include_directories(common PRIVATE ${COMMON_DEPENDENCIES})
target_link_libraries(common asio openssl mysql)

# Set VERSION & FOLDER Property
set_target_properties(common PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(common PROPERTIES FOLDER "server")
