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