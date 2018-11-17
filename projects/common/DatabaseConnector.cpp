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