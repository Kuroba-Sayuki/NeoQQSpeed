#pragma once
#include "Message.h"


#ifdef ZingSpeed
constexpr int MaxRoomName = 66;
#else
constexpr int MaxRoomName = 22;
#endif


class RoomNode;
typedef void(WINAPI* RoomOnTimer)(RoomNode*);

class RoomNode
{
public:
	UINT ID;
	char Name[MaxRoomName];
	USHORT RoomOwnerID;
	USHORT ServerID;
	USHORT BaseGameMode;
	USHORT SubGameMode;
	BYTE SeatNum;
	BYTE Status;
	BYTE CurrentPlayerNum;
	USHORT Flag;
	char Password[16];
	ClientNode* Player[6];
	HANDLE Timer;
	RoomOnTimer Callback;
	//
	UINT MapID;
	class
	{
	public:
		char Season;
		char Timeslot;
		char Weather;
	} STDynamicInfo;

	//Dance
	int SceneID;
	int MusicID;

	//

};

RoomNode* GetRoom(long RoomID);
void LeaveRoom(ClientNode* Client, short LeaveType);
void PrintRoom();
RoomNode* NewRoom();
void CreateRoomTimer(RoomNode* Room, int Timeout, RoomOnTimer Callback);


void RequestGetRoomList(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseGetRoomList(ClientNode* Client, USHORT BaseGameMode, USHORT SubGameMode);

void RequestCreateRoom(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseCreateRoom(ClientNode* Client, RoomNode* Room);

void RequestEnterRoom(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseEnterRoom(ClientNode* Client, RoomNode* Room, char SeatID);
void NotifyEnterRoom(ClientNode* Client, RoomNode* Room, ClientNode* RoomClient, char SeatID);

void RequestLeaveRoom(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseLeaveRoom(ClientNode* Client, short LeaveType);
void NotifyLeaveRoom(ClientNode* Client, UINT uin, char SeatID, bool IsChangeRoomOwner, short NewRoomOwnerID, short LeaveType, int Flag);

void RequestStartGame(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseStartGame(ClientNode* Client, const char* Reason);

void RequestChangeMap(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseChangeMap(ClientNode* Client, RoomNode* Room);
void NotifyChangeMap(ClientNode* Client, UINT Uin, RoomNode* Room);

void RequestChangeSeatState(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseChangeSeatState(ClientNode* Client, BYTE SeatID, BYTE SeatState);

void RequestKickFromRoom(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseKickFromRoom(ClientNode* Client, UINT DstUin);

void RequestChangeReadyState(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseChangeReadyState(ClientNode* Client, bool ReadyState);

void RequestTalk(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseTalk(ClientNode* Client, BYTE Type, BYTE ClientUseFlag, UINT DestPlayerUin);
void NotifyTalk(ClientNode* Client, UINT SrcUin, BYTE Type, BYTE ClientUseFlag, UINT DestPlayerUin, short ContentLength, const char* Content);
//禁止进入房间
//void NotifyForbidFromRoom(ClientNode* Client, RoomNode* Room);
