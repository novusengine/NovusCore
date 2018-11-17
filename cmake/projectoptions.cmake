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

# ASIO Include Directory
if (NOT ASIO_INCLUDE_DIR)
    set (ASIO_INCLUDE_DIR "C:/local/asio-1.12.1/include" CACHE PATH "default install path" FORCE )
endif()
# MySQL Include Directory
if (NOT MYSQL_INCLUDE_DIR)
    set (MYSQL_INCLUDE_DIR "C:/Program Files/MySQL/MySQL Connector C 6.1/include" CACHE PATH "default install path" FORCE )
endif()
# MySQL Library Directory
if (NOT MYSQL_LIB_DIR)
    set (MYSQL_LIB_DIR "C:/Program Files/MySQL/MySQL Connector C 6.1/lib" CACHE PATH "default install path" FORCE )
endif()

# Folder Structure Options
option(WITH_FOLDER_STRUCTURE    "Build source tree"                            1)