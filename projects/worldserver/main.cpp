#include <iostream>

#include "../common/Database/DatabaseConnector.h"

int main()
{
	DatabaseConnector::SetHost("127.0.0.1");

	std::unique_ptr<DatabaseConnector> connector;
	if (!DatabaseConnector::Create(DATABASE_TYPE::AUTHSERVER, connector))
	{
		std::cout << "Connecting to database failed!\n";
		return -1;
	}

	PreparedStatement sql("SELECT * FROM accounts WHERE id={i} AND name={s}");
	sql.Bind(1).Bind("Pursche");


	amy::result_set results;
	//if (!connector->Query("SELECT * FROM accounts", results)) // Non prepared statement
	if (!connector->Query(sql, results)) // Prepared statement
	{
		std::cout << "Querying the database failed!\n";
		return -1;
	}

	connector->QueryAsync("SELECT * FROM accounts WHERE id=1", [](std::error_code const& ec, amy::result_set rs, amy::connector& connector) {
		std::cout << "Got the data!";
	});

	std::cout
		<< "Affected rows: " << results.affected_rows()
		<< ", field count: " << results.field_count()
		<< ", result set size: " << results.size()
		<< std::endl;

	const auto& fields_info = results.fields_info();

	for (const auto& row : results) {
		std::cout
			<< fields_info[0].name() << ": " << row[0].as<amy::sql_bigint>() << ", "
			<< fields_info[1].name() << ": " << row[1].as<std::string>() << ", "
			<< fields_info[2].name() << ": " << row[2].as<std::string>() << ", "
			<< fields_info[3].name() << ": " << row[3].as<amy::sql_datetime>()
			<< std::endl;
	}

	return 0;
}