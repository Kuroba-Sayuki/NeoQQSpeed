#pragma once

#include "yaml-cpp/yaml.h"

void LoadPetLevelConfig();
void GetPetInfo(ClientNode* Client);
UINT GetPetLevel(UINT Exp);
UINT GetPetStatus(UINT Level);
UINT GetPetMaxExp(UINT Level);
YAML::Node GetPetConfig(UINT PetID);

void RequestUpdatePetName(ClientNode* Client, BYTE* Body, size_t BodyLen); //178
void ResponseUpdatePetName(ClientNode* Client); //178