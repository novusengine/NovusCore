/*
    MIT License

    Copyright (c) 2018-2019 NovusCore

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
#pragma once
#include <NovusTypes.h>

enum DataCacheType
{
    ACCOUNT_CONFIG_CACHE = 0,
    CHARACTER_CONFIG_CACHE = 1,
    ACCOUNT_BINDINGS_CACHE = 2,
    CHARACTER_BINDINGS_CACHE = 3,
    ACCOUNT_MACROS_CACHE = 4,
    CHARACTER_MACROS_CACHE = 5,
    CHARACTER_LAYOUT_CACHE = 6,
    CHARACTER_CHAT_CACHE = 7
};
#define MAX_DATA_CACHE_TYPES 8
#define ACCOUNT_DATA_CACHE_MASK 0x15
#define CHARACTER_DATA_CACHE_MASK 0xEA