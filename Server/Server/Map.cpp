#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Map.h"

#include <map>
#include "yaml-cpp/yaml.h"


std::map<UINT, Map*> Maps;

void LoadMapConfig()
{
	try
	{
		YAML::Node Config = YAML::LoadFile("Map.yml");
		for (size_t i = 0; i < Config.size(); i++)
		{
			YAML::Node map = Config[i];
			Map* pMap = new Map;
			Maps[map["ID"].as<UINT>()] = pMap;
			pMap->Head = map["Head"].as<UINT>();
			pMap->End = map["End"].as<UINT>();
			pMap->Loop = map["Loop"].as<bool>();
			pMap->Round = map["Round"].as<UINT>();
		}
	}
	catch (const std::exception&)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		printf("加载地图文件出错\n");
		exit(0);
	}
}

Map* GetMap(UINT ID)
{
	return Maps[ID];
}
