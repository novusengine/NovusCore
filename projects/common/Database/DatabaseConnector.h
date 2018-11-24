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
#include "../Utils/SharedPool.h"
#include "PreparedStatement.h"

enum DATABASE_TYPE
{
	AUTHSERVER,
	CHARSERVER,
	WORLDSERVER
};

class DatabaseConnector
{
public:
	static void SetHost(std::string host) { _host = host; }
    static void SetUser(std::string username, std::string password) { _username = username; _password = password; }
	static bool Create(DATABASE_TYPE type, std::unique_ptr<DatabaseConnector>& out);
	static bool Borrow(DATABASE_TYPE type, std::shared_ptr<DatabaseConnector>& out);

	bool Query(std::string sql, amy::result_set& results);
	bool Query(PreparedStatement statement, amy::result_set& results);
	bool QueryAsync(std::string sql, std::function<void(std::error_code const& ec, amy::result_set rs, amy::connector& connector)> const& func);

	bool Execute(std::string sql);
	bool Execute(PreparedStatement statement);

	~DatabaseConnector();
private:
	DatabaseConnector(); // Constructor is private because we don't want to allow newing these, use Create to aquire a smartpointer.
	void _Connect(DATABASE_TYPE inType);

	DATABASE_TYPE type;
	std::thread* _connectorThread;
	amy::connector* _connector;

	static std::string _host;
    static std::string _username;
    static std::string _password;

	static SharedPool<DatabaseConnector> connectorPool;
};