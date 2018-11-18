/*
# MIT License

# Copyright(c) 2018 NovusCore

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
#include <functional>

#include <amy/connector.hpp>

enum DATABASE_TYPE
{
	AUTH,
	WORLD
};

class DatabaseConnector
{
public:
	static void SetHost(std::string inHost) { host = inHost; }
	static bool Create(DATABASE_TYPE type, std::unique_ptr<DatabaseConnector>& out);

	bool Query(std::string sql, amy::result_set& results);
	bool Execute(std::string sql);

	~DatabaseConnector();
private:
	DatabaseConnector(); // Constructor is private because we don't want to allow newing these, use Create to aquire a smartpointer.

	amy::connector* _connector;
	static std::string host;
};