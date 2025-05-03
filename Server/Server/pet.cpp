#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Message.h"
#include "Pet.h"

#include <map>
#include <sqlite/sqlite3.h>

std::map<UINT, UINT> Levels;

extern sqlite3* PlayerDB;

void LoadPetLevelConfig() {
	YAML::Node Config = YAML::LoadFile("Pet.yml");
	
	for (size_t i = 0; i < Config["Levels"].size(); i++)
	{
		YAML::Node Level = Config["Levels"][i];

		Levels[Level["Level"].as<UINT>()] = Level["Exp"].as<UINT>();
	}
}

void GetPetInfo(ClientNode* Client) {
	sqlite3_stmt* stmt = NULL;

	if (sqlite3_prepare_v2(PlayerDB, "SELECT Name,Experience,PL FROM Pet WHERE Uin=? AND PetId=?;", -1, &stmt, NULL) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, Client->Pet->ID);

		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			char PetName[23] = "";
			memcpy(PetName, (char*)sqlite3_column_text(stmt, 0), 23);
			sprintf_s(Client->Pet->Name, PetName);

			Client->Pet->Exp = sqlite3_column_int(stmt, 1);
			Client->Pet->PL = sqlite3_column_int(stmt, 2);
			Client->Pet->Level = GetPetLevel(Client->Pet->Exp);
			Client->Pet->MaxExp = GetPetMaxExp(Client->Pet->Level);
			Client->Pet->Status = GetPetStatus(Client->Pet->Level);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
}

UINT GetPetLevel(UINT Exp) {
	UINT Level = 0;
	for (size_t i = 1; i <= 30; i++)
	{
		if (Exp < Levels[i]) {
			break;
		}
		Level++;
	}
	return Level;
}

UINT GetPetStatus(UINT Level) {
	UINT Status = 1;
	if (Level >= 30) {
		Status = 3;
	}
	else if (Level >= 20)
	{
		Status = 2;
	}
	return Status;
}

UINT GetPetMaxExp(UINT Level) {
	if (Level < 30) {
		Level += 1;
	}
	return Levels[Level];
}

YAML::Node GetPetConfig(UINT PetID) {
	char FilePath[MAX_PATH];
	YAML::Node Config;

	sprintf_s(FilePath, ".\\Pet\\%d.yml", PetID);
	bool isFileExists;
	struct stat buffer;
	isFileExists = (stat(FilePath, &buffer) == 0);

	if (isFileExists) {
		Config = YAML::LoadFile(FilePath);
	}
	else {
#ifdef DEBUG
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		printf("缺少 Pet %d 的配置文件\n", PetID);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
#endif
	}

	return Config;
}

void RequestUpdatePetName(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);

	char* PetName = (char*)p;
	sqlite3_stmt* stmt = NULL;
	if (sqlite3_prepare_v2(PlayerDB, "UPDATE Pet SET Name=? WHERE Uin = ? AND PetId = ?;", -1, &stmt, NULL) == SQLITE_OK)
	{
		sqlite3_bind_text(stmt, 1, PetName, strlen(PetName), SQLITE_TRANSIENT);
		sqlite3_bind_int(stmt, 2, Client->Uin);
		sqlite3_bind_int(stmt, 3, Client->Pet->ID);
		sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	ResponseUpdatePetName(Client);
}

void ResponseUpdatePetName(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, 0); //Result

	len = p - buf;
	SendToClient(Client, 178, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}