#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Message.h";

void InitDB();
void CloseDB();

BOOL UserDB_Register(const char* Username, const char* Password);
DWORD UserDB_GetUin(const char* Username);
BOOL UserDB_CmpPassword(DWORD Uin, const char* Password);

void PlayerDB_AddItem(DWORD Uin, DWORD ItemID, DWORD ItemNum, DWORD AvailPeriod, bool Status,int Type);
ItemInfo PlayerDB_AddItem2(DWORD Uin, DWORD ItemID, DWORD ItemNum, int AvailPeriod, bool Status);
DWORD PlayerDB_GetMapRecord(DWORD Uin, int MapID);
bool PlayerDB_SetMapRecord(DWORD Uin, int MapID, DWORD Record);
int PlayerDB_GetItemNum(DWORD Uin, DWORD ItemID);
void PlayerDB_UpdateItem(DWORD Uin, DWORD ItemID, DWORD ItemNum);
BOOL PlayerDB_DeleteItem(DWORD Uin, int ItemID);
void PlayerDB_AddStoneGroove(ClientNode* Client, int KartID, int StoneGrooveType);
int PlayerDB_RemoveSkillStone(ClientNode* Client, int KartID, int StoneGrooveID);
