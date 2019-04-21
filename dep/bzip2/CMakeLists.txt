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

if (UNIX OR APPLE)
    find_package(BZip2 REQUIRED)

    add_library(BZip2 INTERFACE)
    add_library(BZip2::BZip2 ALIAS BZip2)

    target_include_directories(BZip2 SYSTEM INTERFACE "${BZIP2_INCLUDE_DIRS}")
    target_link_libraries(BZip2 INTERFACE "${BZIP2_LIBRARIES}")
else()
    file(GLOB sources *.c)
    add_library(BZip2 STATIC ${sources})
    add_library(BZip2::BZip2 ALIAS BZip2)

    target_include_directories(BZip2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    set_target_properties(BZip2 PROPERTIES FOLDER "dep")
endif()
