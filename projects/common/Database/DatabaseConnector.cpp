#include <iostream>
#include "DatabaseConnector.h"
#include <amy/placeholders.hpp>

std::string DatabaseConnector::_host = "INVALID";
std::string DatabaseConnector::_username = "root";
std::string DatabaseConnector::_password = "";
SharedPool<DatabaseConnector> DatabaseConnector::_connectorPools[];
ConcurrentQueue<AsyncSQLJob> DatabaseConnector::_asyncJobQueue(1024);

// Initialization
void DatabaseConnector::Setup(std::string host, std::string username, std::string password)
{ 
	_host = host; 
	_username = username; 
	_password = password;

	// Create Async SQL thread
	std::thread asyncThread(AsyncSQLThreadMain);
	asyncThread.detach();
}

// Static Connector creation
bool DatabaseConnector::Create(DATABASE_TYPE type, std::unique_ptr<DatabaseConnector>& out)
{
	if (_host == "INVALID")
	{
		std::cerr << "ERROR: Failed to connect to MySQL Server, please initialize with DatabaseConnector::Setup!\n";
		return false;
	}

	out.reset(new DatabaseConnector());
	out->_Connect(type);

	return true;
}

bool DatabaseConnector::Borrow(DATABASE_TYPE type, std::shared_ptr<DatabaseConnector>& out)
{
	if (_host == "INVALID")
	{
		std::cerr << "ERROR: Failed to connect to MySQL Server, please initialize with DatabaseConnector::Setup!\n";
		assert(false);
		return false;
	}

	// If we are out of connectors, create one!
	if (_connectorPools[type].empty())
	{
		DatabaseConnector* newConnector = new DatabaseConnector();
		newConnector->_Connect(type);
		_connectorPools[type].add(std::unique_ptr<DatabaseConnector>(newConnector));
	}

	out = _connectorPools[type].acquire();

	return true;
}

// Static Asyncs
void DatabaseConnector::QueryAsync(DATABASE_TYPE type, std::string sql, std::function<void(amy::result_set& result, DatabaseConnector& connector)> const& func)
{
	_asyncJobQueue.emplace(type, sql, func);
}

void DatabaseConnector::ExecuteAsync(DATABASE_TYPE type, std::string sql)
{
	_asyncJobQueue.emplace(type, sql, nullptr);
}

void DatabaseConnector::AsyncSQLThreadMain()
{
	std::cout << "[SQL] Async SQL Thread has been created\n";
	AsyncSQLJob job;
	amy::result_set results;
	size_t tries = 0;

	std::unique_ptr<DatabaseConnector> connectors[DATABASE_TYPE::COUNT];
	for (size_t i = 0; i < DATABASE_TYPE::COUNT; i++)
	{
		if (!DatabaseConnector::Create((DATABASE_TYPE)i, connectors[i]))
		{
			std::cout << "Connecting to database failed!\n";
			assert(false);
		}
	}
	
	// This thread will just spin and try to execute any AsyncSQLJobs in the queue.
	// When the queue is empty, it will first retry immediately 10 times, after that it will retry 10 times a second for 9 seconds, and after that it will decrease to 2 checks a second
	// When the thread finds something in the queue, this behavior is reset.
	// This behavior should give us a good mix between short wait times and not consuming 100% of a CPU core.
	while (true)
	{
		if (_asyncJobQueue.try_pop(job))
		{
			tries = 0;
			
			bool isQuery = job.func != nullptr;

			if (isQuery)
			{
				connectors[job.type]->Query(job.sql, results);
				job.func(results, *(connectors[job.type]));
			}
			else
			{
				connectors[job.type]->Execute(job.sql);
			}
		}
		else
		{
			if (tries > 100)
			{
				std::this_thread::sleep_for(std::chrono::microseconds(500)); // Sleep for 0.5 second, 2 checks a second
				continue; // This continue makes sure that we don't increase tries more than we need to, preventing any possible overflows
			}
			if (tries > 10)
				std::this_thread::sleep_for(std::chrono::microseconds(100)); // Sleep for 0.1 second, 10 checks a second

			tries++;
			continue;
		}
	}
}

// Non static functions
DatabaseConnector::DatabaseConnector()
{

}

DatabaseConnector::~DatabaseConnector()
{

}

bool DatabaseConnector::_Connect(DATABASE_TYPE type)
{
	_type = type;

	std::string db = "INVALID";

	switch (_type)
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
	try
	{
		_connector->connect(endpoint, amy::auth_info(_username, _password), db, amy::default_flags);
		return true;
	}
	catch(amy::system_error error)
	{
		std::cout << "ERROR: "<< error.what() << "\n";
		assert(false);
		return false;
	}
}

bool DatabaseConnector::Execute(std::string sql)
{
	std::error_code error;
	_connector->query(sql, error);

	if (error)
	{
		std::cout << error.message() << "\n";
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
		std::cout << error.message() << "\n";
		assert(false);
		return false;
	}

	results = _connector->store_result();
	return true;
}
