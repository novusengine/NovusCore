#include "PreparedStatement.h"

#include <cassert>
#include <iostream>

PreparedStatement::PreparedStatement(std::string statement)
{
	_statement = statement;
}

PreparedStatement& PreparedStatement::Bind(std::string value)
{
	size_t replacePos = _statement.find("{s}");

	if (value.find("'") != 0)
		value = "'" + value;

	if (value.find_last_of("'") != value.length() - 1)
		value += "'";

	if (replacePos != std::string::npos)
	{
		_statement.replace(replacePos, 3, value);
	}
	else
	{
		std::cout << "Error: Could not find token {s} in statement: " << _statement << std::endl;
		assert(false);
	}
	
	return *this;
}

PreparedStatement& PreparedStatement::Bind(unsigned int value)
{
    size_t replacePos = _statement.find("{u}");

    if (replacePos != std::string::npos)
    {
        _statement.replace(replacePos, 3, std::to_string(value));
    }
    else
    {
        std::cout << "Error: Could not find token {u} in statement: " << _statement << std::endl;
        assert(false);
    }

    return *this;
}
PreparedStatement& PreparedStatement::Bind(uint8_t value)
{
    size_t replacePos = _statement.find("{u}");

    if (replacePos != std::string::npos)
    {
        _statement.replace(replacePos, 3, std::to_string(value));
    }
    else
    {
        std::cout << "Error: Could not find token {u} in statement: " << _statement << std::endl;
        assert(false);
    }

    return *this;
}
PreparedStatement& PreparedStatement::Bind(uint64_t value)
{
    size_t replacePos = _statement.find("{u}");

    if (replacePos != std::string::npos)
    {
        _statement.replace(replacePos, 3, std::to_string(value));
    }
    else
    {
        std::cout << "Error: Could not find token {u} in statement: " << _statement << std::endl;
        assert(false);
    }

    return *this;
}

PreparedStatement& PreparedStatement::Bind(int value)
{
	size_t replacePos = _statement.find("{i}");

	if (replacePos != std::string::npos)
	{
		_statement.replace(replacePos, 3, std::to_string(value));
	}
	else
	{
		std::cout << "Error: Could not find token {i} in statement: " << _statement << std::endl;
		assert(false);
	}

	return *this;
}

PreparedStatement& PreparedStatement::Bind(float value)
{
	size_t replacePos = _statement.find("{f}");

	if (replacePos != std::string::npos)
	{
		_statement.replace(replacePos, 3, std::to_string(value));
	}
	else
	{
		std::cout << "Error: Could not find token {f} in statement: " << _statement << std::endl;
		assert(false);
	}

	return *this;
}

PreparedStatement& PreparedStatement::Bind(double value)
{
	size_t replacePos = _statement.find("{d}");

	if (replacePos != std::string::npos)
	{
		_statement.replace(replacePos, 3, std::to_string(value));
	}
	else
	{
		std::cout << "Error: Could not find token {d} in statement: " << _statement << std::endl;
		assert(false);
	}

	return *this;
}

bool PreparedStatement::Verify()
{
	size_t pos = _statement.find("{s}");
	if (pos == std::string::npos)
		pos = _statement.find("{i}");
	if (pos == std::string::npos)
		pos = _statement.find("{u}");
	if (pos == std::string::npos)
		pos = _statement.find("{f}");
	if (pos == std::string::npos)
		pos = _statement.find("{d}");

	return pos == std::string::npos;
}

std::string PreparedStatement::Get()
{
	if (!Verify())
	{
		std::cout << "Error: Found unreplaced tokens in statement: " << _statement << std::endl;
		assert(false);
	}

	return _statement;
}