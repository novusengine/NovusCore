#include "ConfigHandler.h"

std::string ConfigHandler::_configFileName;
json ConfigHandler::_configFile;
bool ConfigHandler::_isInitialized = false;

bool ConfigHandler::Setup(std::string configFileName)
{
    _configFileName = configFileName;

    std::ifstream configFile(_configFileName, std::ofstream::in);
    try
    {
        configFile >> _configFile;
        configFile.close();
    }
    catch (nlohmann::detail::exception e)
    {
        std::cerr << "ConfigHandler: Failed to initialize config file, " << configFileName << " could not be found!" << std::endl;
        return false;
    }
    
    if (_configFile.size() == 0)
    {
        std::cerr << "ConfigHandler: Failed to initialize config file, found 0 configurations!" << std::endl;
        return false;
    }

	_isInitialized = true;
	std::cout << "Successfully initialized config file: " << _configFileName << std::endl;
	return true;
}

json::value_type ConfigHandler::FindOptionInArray(std::string optionName, json::value_type value)
{
    for (auto& itr : value.items())
    {
        if (itr.key() == optionName)
            return itr.value();

        if (itr.value().is_structured())
        {
            return FindOptionInArray(optionName, itr.value());
        }
    }

    return json::value_type();
}