#include <iostream>
#include "DatabaseConnector.h"
#include <amy/placeholders.hpp>

std::string DatabaseConnector::_host = "INVALID";
std::string DatabaseConnector::_username = "root";
std::string DatabaseConnector::_password = "";
SharedPool<DatabaseConnector> DatabaseConnector::connectorPools[];

bool DatabaseConnector::Create(DATABASE_TYPE type, std::unique_ptr<DatabaseConnector>& out)
{
	if (_host == "INVALID")
	{
		std::cerr << "ERROR: Failed to connect to MySQL Server, please set host with DatabaseConnector::SetHost!\n";
		return false;
	}

	out.reset(new DatabaseConnector());
	out->_Connect(type);

	std::cout << "Successfully connected to database on " << _host << std::endl;

	return true;
}

bool DatabaseConnector::Borrow(DATABASE_TYPE type, std::shared_ptr<DatabaseConnector>& out)
{
	if (_host == "INVALID")
	{
		std::cerr << "ERROR: Failed to connect to MySQL Server, please set host with DatabaseConnector::SetHost!\n";
		assert(false);
		return false;
	}

	// If we are out of connectors, create one!
	if (connectorPools[type].empty())
	{
		DatabaseConnector* newConnector = new DatabaseConnector();
		newConnector->_Connect(type);
		connectorPools[type].add(std::unique_ptr<DatabaseConnector>(newConnector));
	}

	out = connectorPools[type].acquire();

	return true;
}

DatabaseConnector::DatabaseConnector()
{

}

DatabaseConnector::~DatabaseConnector()
{

}

void DatabaseConnector::_Connect(DATABASE_TYPE inType)
{
	type = inType;

	std::string db = "INVALID";

	switch (type)
	{
	case DATABASE_TYPE::AUTHSERVER:
		db = "authserver";
		break;
	case DATABASE_TYPE::CHARSERVER:
		db = "charserver";
		break;
	case DATABASE_TYPE::WORLDSERVER:
		db = "worldserver";
		break;
	}

	AMY_ASIO_NS::ip::tcp::endpoint endpoint(AMY_ASIO_NS::ip::address::from_string(_host), 3306);
	AMY_ASIO_NS::io_service io_service;
	_connector = new amy::connector(io_service);
	_connector->connect(endpoint, amy::auth_info(_username, _password), db, amy::default_flags);
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

bool DatabaseConnector::Execute(PreparedStatement statement)
{
	return Execute(statement.Get());
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

bool DatabaseConnector::Query(PreparedStatement statement, amy::result_set& results)
{
	return Query(statement.Get(), results);
}

bool DatabaseConnector::QueryAsync(std::string sql, std::function<void(std::error_code const& ec, amy::result_set rs, amy::connector& connector)> const& func)
{
	// TODO

	return true;
}