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

#include <iostream>
#include "DatabaseConnector.h"


#include <amy/placeholders.hpp>

std::string DatabaseConnector::host = "INVALID";



bool DatabaseConnector::Create(DATABASE_TYPE type, std::unique_ptr<DatabaseConnector>& out)
{
	if (host == "INVALID")
	{
		std::cerr << "ERROR: Failed to connect to MySQL Server, please set host with DatabaseConnector::SetHost!\n";
		return false;
	}

	out.reset(new DatabaseConnector());

	std::string db = "INVALID";
	
	if (type == DATABASE_TYPE::AUTH)
		db = "authserver";
	else if (type == DATABASE_TYPE::WORLD)
		db = "worldserver";

	

	AMY_ASIO_NS::ip::tcp::endpoint endpoint(AMY_ASIO_NS::ip::address::from_string(host), 3306);
	AMY_ASIO_NS::io_service io_service;
	out->_connector = new amy::connector(io_service);
	out->_connector->connect(endpoint, amy::auth_info("root"), db, amy::default_flags);

	std::cout << "Successfully connected to database on " << host << std::endl;
	return true;
}

DatabaseConnector::DatabaseConnector()
{

}

DatabaseConnector::~DatabaseConnector()
{

}

bool DatabaseConnector::Execute(std::string sql)
{
	std::error_code error;
	_connector->query(sql, error);

	if (error)
	{
		std::cout << error.message();
		assert(false);
		return false;
	}

	return true;
}

bool DatabaseConnector::Query(std::string sql, amy::result_set& results)
{
	std::error_code error;
	_connector->query(sql, error);

	if (error)
	{
		std::cout << error.message();
		assert(false);
		return false;
	}

	results = _connector->store_result();
	return true;
}