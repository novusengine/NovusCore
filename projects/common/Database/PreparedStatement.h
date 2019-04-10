/*
# MIT License

# Copyright(c) 2018-2019 NovusCore

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
*/
#pragma once

#include <string>
#include "../NovusTypes.h"

// Valid type/tokens
// {s} - std::string
// {i} - i32
// {u} - u32
// {f} - f32
// {d} - f64

class PreparedStatement
{
public:
	PreparedStatement(std::string statement);

    PreparedStatement& Bind(std::string value);
    PreparedStatement& Bind(u8 value);
    PreparedStatement& Bind(u16 value);
    PreparedStatement& Bind(i16 value);
    PreparedStatement& Bind(u32 value);
	PreparedStatement& Bind(i32 value);
	PreparedStatement& Bind(f32 value);
	PreparedStatement& Bind(f64 value);
    PreparedStatement& Bind(u64 value);

	bool Verify();
	std::string Get();

private:
	std::string _statement;
};