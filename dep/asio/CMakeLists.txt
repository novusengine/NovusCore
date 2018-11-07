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

add_library(asio INTERFACE)

find_path(ASIO_DIR NAMES asio.hpp PATHS "${ASIO_INCLUDE_DIR}")
set_property(CACHE ASIO_DIR PROPERTY ADVANCED TRUE)
if (NOT ASIO_DIR)
    message(SEND_ERROR "ASIO header (asio.hpp) could not be found at ${ASIO_INCLUDE_DIR}.")
endif()

target_include_directories(asio SYSTEM INTERFACE "${ASIO_INCLUDE_DIR}")
target_link_libraries(asio INTERFACE "${CMAKE_THREAD_LIBS_INIT}")
target_compile_definitions(asio INTERFACE ASIO_STANDALONE INTERFACE ASIO_HAS_STD_CHRONO)
