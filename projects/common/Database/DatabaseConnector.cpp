#include "DatabaseConnector.h"
#include <iostream>
#include <amy/placeholders.hpp>

std::vector<std::string> DatabaseConnector::_hosts(DATABASE_TYPE::COUNT);
std::vector<std::string> DatabaseConnector::_usernames(DATABASE_TYPE::COUNT);
std::vector<std::string> DatabaseConnector::_passwords(DATABASE_TYPE::COUNT);
std::vector<std::string> DatabaseConnector::_names(DATABASE_TYPE::COUNT);
std::vector<u16>         DatabaseConnector::_ports(DATABASE_TYPE::COUNT);
bool                     DatabaseConnector::_initialized = false;
SharedPool<DatabaseConnector> DatabaseConnector::_connectorPools[];
ConcurrentQueue<AsyncSQLJob> DatabaseConnector::_asyncJobQueue(1024);

void DatabaseConnector::Setup(std::string hosts[], u16 ports[], std::string usernames[], std::string passwords[], std::string names[])
{ 

    for (i32 i = 0; i < DATABASE_TYPE::COUNT; i++)
    {
        _hosts[i]       = hosts[i];
        _usernames[i]   = usernames[i];
        _passwords[i]   = passwords[i];
        _names[i]       = names[i];
        _ports[i]       = ports[i];
    }
    _initialized = true;

	// Create Async SQL thread
	std::thread asyncThread(AsyncSQLThreadMain);
	asyncThread.detach();
}

// Static Connector creation
bool DatabaseConnector::Create(DATABASE_TYPE type, std::unique_ptr<DatabaseConnector>& out)
{
	if (!_initialized)
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
    if (!_initialized)
	{
		std::cerr << "ERROR: Failed to connect to MySQL Server, please initialize with DatabaseConnector::Setup!\n";
		//assert(false);
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

void DatabaseConnector::Borrow(DATABASE_TYPE type, std::function<void(std::shared_ptr<DatabaseConnector>& connector)> const& func)
{
    if (!_initialized)
    {
        std::cerr << "ERROR: Failed to connect to MySQL Server, please initialize with DatabaseConnector::Setup!\n";
        //assert(false);
        return;
    }

    // If we are out of connectors, create one!
    if (_connectorPools[type].empty())
    {
        DatabaseConnector* newConnector = new DatabaseConnector();
        newConnector->_Connect(type);
        _connectorPools[type].add(std::unique_ptr<DatabaseConnector>(newConnector));
    }

    func(std::shared_ptr<DatabaseConnector>(_connectorPools[type].acquire()));
    return;
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
	AsyncSQLJob job;
	amy::result_set results;
	size_t tries = 0;

	std::unique_ptr<DatabaseConnector> connectors[DATABASE_TYPE::COUNT];
	for (size_t i = 0; i < DATABASE_TYPE::COUNT; i++)
	{
		if (!DatabaseConnector::Create((DATABASE_TYPE)i, connectors[i]))
		{
			std::cout << "Connecting to database failed!\n";
			//assert(false);
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
	AMY_ASIO_NS::ip::tcp::endpoint endpoint(AMY_ASIO_NS::ip::address::from_string(_hosts[_type]), _ports[_type]);
	AMY_ASIO_NS::io_service io_service;
	_connector = new amy::connector(io_service);
	try
	{
		_connector->connect(endpoint, amy::auth_info(_usernames[_type], _passwords[_type]), _names[_type], amy::default_flags);
		return true;
	}
	catch(amy::system_error error)
	{
		std::cout << "ERROR: "<< error.what() << "\n";
		//assert(false);
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
		//assert(false);
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
		std::cout << _connector->error_message(error) << "\n";
		//assert(false);
		return false;
	}

	results = _connector->store_result();
	return true;
}
