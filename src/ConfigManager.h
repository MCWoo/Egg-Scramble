//
//  ConfigLoader.hpp
//  egg scramble
//
//  Created by Phoebe on 4/18/16.
//  Copyright © 2016 sunny side up. All rights reserved.
//

#ifndef ConfigLoader_h
#define ConfigLoader_h

#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <map>


class ConfigManager {
public:
    std::map<std::string, std::string> cfg_map;
    
    void LoadConfigs(std::string file_name)
	{
        std::string line;
        
        std::ifstream infile(file_name);
        
        while (getline(infile, line))
        {
            std::istringstream iss(line);
            std::string field, equal, value;
            if (!(iss >> field >> equal >> value)) { break; } // error
			cfg_map.insert(std::pair<std::string, std::string>(field, value));
        }
		infile.close();
    }

	std::string GetConfigValue(std::string key)
	{
		if (cfg_map.find(key) != cfg_map.end())
			return cfg_map.find(key)->second;
		else
			return "";
	}

	static void instantiate()
	{
		if (cfg == nullptr)
			cfg = new ConfigManager();
	}

	static ConfigManager* instance() { return cfg; }

private:
	static ConfigManager * cfg;
	ConfigManager(void);
	~ConfigManager(void);
};

#endif /* ConfigLoader_h */
