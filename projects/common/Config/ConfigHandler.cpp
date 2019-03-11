#include "ConfigHandler.h"
#include "../Utils/DebugHandler.h"

std::string ConfigHandler::_configFileName;
json ConfigHandler::_configFile;

bool ConfigHandler::Load(std::string configFileName)
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
        NC_LOG_FATAL("Could not find '" + configFileName + "' in directory. Press a key to exit.");
        return false;
    }
    
    if (_configFile.size() == 0)
    {
        NC_LOG_FATAL("Failed to initialize config file, found 0 configurations.");
        return false;
    }

    NC_LOG_SUCCESS("Loaded config file: '" + configFileName);
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