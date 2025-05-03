#include "Room.h"
#include "Map.h"
#include "sqlite/sqlite3.h"
#include "Database.h"
#include <stdio.h>
#include <time.h>
#include <map>
#include <mutex>
extern sqlite3* PlayerDB;

HANDLE TimerQueue = CreateTimerQueue();
void WINAPI OnDanceTimer(RoomNode* Room);

std::map<long, RoomNode*> Rooms;
std::mutex Mutex;
long SeqRoomID = 0;
RoomNode* NewRoom()
{
	RoomNode* Room = new RoomNode;
	Room->ID = InterlockedIncrement(&SeqRoomID);
	memset(Room->Name, 0, sizeof(Room->Name));
	Room->RoomOwnerID = 0;
	Room->ServerID = 0;
	Room->BaseGameMode = 0;
	Room->SubGameMode = 0;
	Room->SeatNum = 0;
	Room->Status = 0;
	Room->CurrentPlayerNum = 0;
	Room->Flag = 0;
	memset(Room->Password, 0, sizeof(Room->Password));
	for (size_t i = 0; i < 6; i++)
	{
		Room->Player[i] = NULL;
	}
	Room->Timer = NULL;
	Room->Callback = NULL;

	Room->MapID = 0;
	Room->STDynamicInfo.Season = -1;
	Room->STDynamicInfo.Timeslot = -1;
	Room->STDynamicInfo.Weather = -1;

	Room->SceneID = 0;
	Room->MusicID = 0;


	Mutex.lock();
	Rooms[Room->ID] = Room;
	Mutex.unlock();
	return Room;
}
RoomNode* GetRoom(long RoomID)
{
	RoomNode* Room = NULL;
	Mutex.lock();
	auto iter = Rooms.find(RoomID);
	if (iter != Rooms.end())
	{
		Room = iter->second;
	}
	else
	{
		//printf("无效的房间ID:%d\n", RoomID);
	}
	Mutex.unlock();
	return Room;
}
void PrintRoom()
{
	std::map<long, RoomNode*>::iterator iter = Rooms.begin();
	while (iter != Rooms.end())
	{
		RoomNode* Room = iter->second;
		if (Room)
		{
			printf("RoomID:%d, ", Room->ID);
			printf("BaseGameMode:%d, ", Room->BaseGameMode);
			printf("SubGameMode:%d, ", Room->SubGameMode);
			printf("ServerID:%d, ", Room->ServerID);
			printf("RoomOwnerID:%d, ", Room->RoomOwnerID);
			printf("Name:%s\n", Room->Name);
			printf("Flag:%d, ", Room->Flag);
			printf("房间密码:%s\n", Room->Password);
			for (char i = 0; i < 6; i++)
			{
				ClientNode* RoomClient = Room->Player[i];
				if (RoomClient)
				{
					printf("\tPlayerID:%d, SeatID:%d\n", RoomClient->ConnID, i);
				}
			}

			
		}
		iter++;
	}
}

VOID WINAPI OnTimer(PVOID p, BOOLEAN b)
{
	RoomNode* Room = (RoomNode*)p;
	if (DeleteTimerQueueTimer(TimerQueue, Room->Timer, NULL))
	{
		Room->Timer = NULL;
	}
	Room->Timer = NULL;
	Room->Callback(Room);
}

void CreateRoomTimer(RoomNode* Room, int Timeout, RoomOnTimer Callback)
{
	Room->Callback = Callback;
	CreateTimerQueueTimer(&Room->Timer, TimerQueue, OnTimer, Room, Timeout, 0, WT_EXECUTEINTIMERTHREAD);
}


void RequestGetRoomList(ClientNode* Client, BYTE* Body, size_t BodyLen)
{

	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	USHORT StartRoomIdx = Read16(Body);
	USHORT GetRoomNum = Read16(Body);
	USHORT BaseGameMode = Read16(Body);
	USHORT SubGameMode = Read16(Body);
	BYTE FBLev = Read8(Body);
	BYTE FBSeason = Read8(Body);
	BYTE RoomFlag = Read8(Body);

	ResponseGetRoomList(Client, BaseGameMode, SubGameMode);
}
void ResponseGetRoomList(ClientNode* Client, USHORT BaseGameMode, USHORT SubGameMode)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, 0); //ResultID

	Write16(p, (WORD)Rooms.size()); //CurTotalRoomNum
	Write16(p, 0); //StartIdx//房间位置

	BYTE* pRoomNum = p;
	Write16(p, 0); //RoomNum
	{
		int RoomNum = 0;
		
		Mutex.lock();
		std::map<long, RoomNode*>::iterator iter;
		for (iter = Rooms.begin(); iter != Rooms.end(); iter++)
		{
			RoomNode* Room = iter->second;
			RoomNum++;
			//PrintRoom();
			{ //Rooms
				BYTE* pRooms = p;
				Write16(pRooms, 0); //len
				Write16(pRooms, Room->ID); //RoomID
				Write16(pRooms, Room->BaseGameMode); //BaseGameMode
				Write16(pRooms, Room->SubGameMode); //SubGameMode
				Write8(pRooms, Room->CurrentPlayerNum); //CurrentPlayerNum
				Write8(pRooms, Room->SeatNum); //TotalSeatNum 总座位数
				Write8(pRooms, Room->Status); //Status//0为禁止进入房间 1位正常
				Write32(pRooms, Room->MapID); //MapID
#ifdef ZingSpeed
				Write8(pRooms, (BYTE)Room->Flag); //Flag
#else
				Write16(pRooms, Room->Flag); //Flag
#endif

				memcpy(pRooms, Room->Name, MaxRoomName); //RoomName
				pRooms += MaxRoomName;

				Write8(pRooms, 0); //HasFBInfo
				/*
m_stRooms[].m_astFBInfo[].m_bAvailableLev
m_stRooms[].m_astFBInfo[].m_bLev
m_stRooms[].m_astFBInfo[].m_bSubLev
m_stRooms[].m_astFBInfo[].m_iBossID
m_stRooms[].m_astFBInfo[].m_bFBSeason
				*/

#ifndef ZingSpeed
				/*
m_stRooms[%d].m_ucSpeFlag
m_stRooms[%d].m_uiRoomNo
m_stRooms[%d].m_ushSvrId
m_stRooms[%d].m_shSceneID
m_stRooms[%d].m_bCurrentOBNum
m_stRooms[%d].m_bTotalOBNum
m_stRooms[%d].m_cSpecialActivityStatus
m_stRooms[%d].m_ucAllowAutoStart
m_stRooms[%d].m_iGender
m_stRooms[%d].m_iLocaleCode
m_stRooms[%d].m_ucTalkRoomBGID
m_stRooms[%d].m_uchSpecialActivityIDNum
m_stRooms[%d].m_uchSpecialActivityInfoNum
m_stRooms[%d].m_astSpecialActivityInfo[%d].m_uchProfessionLicenseInfoNum
m_stRooms[%d].m_astSpecialActivityInfo[%d].m_stProfessionLicenseInfo[%d].m_uchBigLevel
m_stRooms[%d].m_astSpecialActivityInfo[%d].m_stProfessionLicenseInfo[%d].m_uchSmallLevel
m_stRooms[%d].m_uchValueNum
m_stRooms[%d].m_aiEnterRoomValue[%d]
m_stRooms[%d].m_uchParaNum
m_stRooms[%d].m_aiParaList[%d]
m_stRooms[%d].m_uiGSvrRoomNo
m_stRooms[%d].m_szOwnerName[]
				*/
#endif

				len = pRooms - p;
				Set16(p, (WORD)len);
				p += len;
			}

		}
		Mutex.unlock();

		Set16(pRoomNum, RoomNum);
	}


	Write8(p, 0); //ReasonLen

	Write16(p, BaseGameMode); //BaseGameMode
	Write16(p, SubGameMode); //SubGameMode
	Write8(p, 0); //FBAvailableLev
	Write8(p, 0); //FBLev
	Write8(p, 0); //FBSeason

	Write16(p, 0); //ChannelStatus
	Write8(p, 1); //RoomFlag


	len = p - buf;
	SendToClient(Client, 102, buf, len, Client->ServerID, FE_GAMESVRD, Client->ConnID, Response);
}

void RequestCreateRoom(ClientNode* Client, BYTE* Body, size_t BodyLen)//响应创建房间
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	USHORT BaseGameMode = Read16(Body);
	USHORT SubGameMode = Read16(Body);
#ifdef ZingSpeed
	char* RoomName = (char*)Body;
	Body += MaxRoomName;

	BYTE SeatNum = Read8(Body);
	BYTE Flag = Read8(Body);
#else
	USHORT Flag = Read16(Body);//4 皇族 8车队 12=4+8
	//	Flag = 8; //4 皇族 8车队 12=4+8   65533全满
	char* RoomName = (char*)Body;
	Body += MaxRoomName;

	BYTE SeatNum = Read8(Body);
#endif

	char Password[16];
	memcpy(Password, Body, 16);
	Body += 16;

	BYTE IsNewerOpen = Read8(Body);
	BYTE FBSeason = Read8(Body);
	BYTE Contex = Read8(Body);
	BYTE FBLev = Read8(Body);


	BYTE SpeFlag = Read8(Body);
	BYTE MoreInfoNum = Read8(Body);
	for (BYTE i = 0; i < MoreInfoNum; i++)
	{
		//m_astMoreInfos[].m_iTaskID
		Body += Get16(Body);
	}

	USHORT CreateRoomType = Read16(Body);
	USHORT ServerMatchType = Read16(Body);
	USHORT TeamMatchType = Read16(Body);
	BYTE ChatRoomType = Read8(Body);
	USHORT SceneID = Read16(Body);
	BYTE PresetOBNum = Read8(Body);
	BYTE AllowAutoStart = Read8(Body);
	BYTE TalkRoomBGID = Read8(Body);

	BYTE ValueNum = Read8(Body);
	for (BYTE i = 0; i < ValueNum; i++)
	{
		//m_aiEnterRoomValue[]
		Read32(Body);
	}

	RoomNode* Room = NewRoom();
	Room->BaseGameMode = BaseGameMode;
	Room->SubGameMode = SubGameMode;
	strcpy_s(Room->Name, RoomName);
	Room->ServerID = Client->ServerID;
	Room->RoomOwnerID = Client->ConnID;
	Client->RoomID = Room->ID;
	Room->SeatNum = SeatNum;
	Room->Status = 1;
	Room->CurrentPlayerNum = 1;
	Room->Flag = Flag;
	memcpy(Room->Password, Password, 16);
	Room->Player[0] = Client;



	ResponseCreateRoom(Client, Room);
}
void ResponseCreateRoom(ClientNode* Client, RoomNode* Room)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;
	USHORT Flag = Room->Flag;

	Write16(p, 0); //ResultID

	Write16(p, Room->ID); //RoomID
	Write16(p, Room->BaseGameMode); //BaseGameMode
	Write16(p, Room->SubGameMode); //SubGameMode
#ifdef ZingSpeed
	memcpy(p, Room->Name, MaxRoomName); //RoomName
	p += MaxRoomName;

	Write8(p, Room->SeatNum); //SeatNum
	Write8(p, (BYTE)Room->Flag); //Flag
#else
	Write16(p, Flag); //Flag

	memcpy(p, Room->Name, MaxRoomName); //RoomName
	p += MaxRoomName;

	Write8(p, Room->SeatNum); //SeatNum
#endif // ZingSpeed


	Write8(p, 0); //SeatID
	Write8(p, 0); //TeamID

	Write8(p, 0); //ReasonLen


	Write8(p, 0); //LoverRaceOrPro

	Write32(p, Room->MapID); //MapID
	Write8(p, 0); //DebutOrX5
	Write32(p, (DWORD)time(nullptr)); //ServerTime
	Write32(p, 0); //PreKnowMapID
	Write32(p, 0); //PreKnownMusicID

	Write8(p, 0); //HasFBInfo
	/*
m_astFBInfo[%d].m_bAvailableLev
m_astFBInfo[%d].m_bLev
m_astFBInfo[%d].m_bSubLev
m_astFBInfo[%d].m_iBossID
m_astFBInfo[%d].m_bFBSeason
	*/

	Write32(p, 0); //HasFBInfoGuildScoreInNextGame
	Write8(p, 0); //HasFBInfoContex
	Write16(p, 0); //GuildScoreAlreadyAward
	Write16(p, 0); //GuildScoreTotalAward
#ifndef ZingSpeed
	Write8(p, 0); //HasFBInfoSpeFlag
	Write32(p, 12345678); //RoomNo

	Write8(p, 0); //MoreInfoNum
	/*
m_astMoreInfos[%d].m_iTaskID
	*/


	Write16(p, 0); //RaceCardNums
	/*
m_astRaceCardAddValueInfo[%d].m_uiRaceCardID
m_astRaceCardAddValueInfo[%d].m_uiAddValue
	*/

	Write16(p, 0); //CreateRoomType
	Write16(p, 0); //ServerMatchType
	Write16(p, 0); //TeamMatchType
	Write8(p, 0); //ChatRoomType
	Write16(p, 0); //SceneID
	Write8(p, 0); //PresetOBNum
	Write8(p, 0); //AllowAutoStart
	Write8(p, 0); //TalkRoomBGID
	Write16(p, 0); //Role

	Write8(p, 0); //ParaNum
	//ParaList[]

	Write8(p, 0); //ValueNum
	//EnterRoomValue[]

	Write32(p, 0); //PlayerEquipKartID
	Write32(p, 0); //RankedMatchTestDriveKartID
	Write8(p, 0); //GVoiceRoomNameLen
	Write8(p, 0); //GVoiceReasonLen
	Write8(p, 0); //TDCExcluItemNum
	//TDCExclusiveItemID[]

	Write8(p, 0); //TestKartGameType
	Write32(p, 0); //GameTestKartID

	Write8(p, 0); //HasStarsFightingInfo
	/*
m_stStarsFightingInfo[%d].m_iStar
m_stStarsFightingInfo[%d].m_iStarVal
m_stStarsFightingInfo[%d].m_iStarsFightingStarLevel
	*/

	Write8(p, 0); //HasYuLeJiaNianHuaInfo
	/*
m_stYuLeJiaNianHuaInfo[%d].m_uiMode
m_stYuLeJiaNianHuaInfo[%d].m_uiScore
m_stYuLeJiaNianHuaInfo[%d].m_uchRankRangNum
m_stYuLeJiaNianHuaInfo[%d].m_auiRankRange[%d]
m_stYuLeJiaNianHuaInfo[%d].m_uchRankScoreNum
m_stYuLeJiaNianHuaInfo[%d].m_auiRankScore[%d]
m_stYuLeJiaNianHuaInfo[%d].m_uchInRecommend
m_stYuLeJiaNianHuaInfo[%d].m_uchInDoubleScore
	*/
#endif

	len = p - buf;
	SendToClient(Client, 103, buf, len, Client->ServerID, FE_GAMESVRD, Client->ConnID, Response);
}

void RequestEnterRoom(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	USHORT RoomID = Read16(Body);

	/*char Password[16] = (char*)Read16(Body);

	memcpy(Password, Body, 19);
	memset(Password, 0, 19);
	memcpy(Body + 2, Password, strlen(Password));
	Body += 19;
	printf("房间密码1:%d\n", Password);*/
	char pwd[16] = { 0 };
	memcpy(pwd, Body, 16);
	Body += 16; //Password

	BYTE InviteType = Read8(Body);
	UINT InviterUin = Read32(Body);
	std::string Reason = "";

	RoomNode* Room = GetRoom(RoomID);
	if (!Room)
	{
		Reason = "对不起,您要加入的房间已经不存在,请选择加入其他房间!";
		ResponseStartGame(Client, Reason.c_str());
		return;
	}
	if (Room->Flag == 1)
	{
		int cmppw = strcmp(pwd, Room->Password);
		if (cmppw != 0)
		{
			Reason = "对不起,房间密码输入错误!";
			ResponseStartGame(Client, Reason.c_str());
			return;
		}
	}
	if (!Room->Status)
	{
		Reason = "对不起,您要加入的房间正在游戏中!";
		ResponseStartGame(Client, Reason.c_str());
		return;
	}
	LeaveRoom(Client, 0);
	/*printf("房间密码2:%d\n", Room->Password);*/

	//if (Room->Password == Password)
	{


	}
	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient == NULL)
		{
			Client->RoomID = RoomID;
			Client->IsReady = true;
			ResponseEnterRoom(Client, Room, i);
			Room->Player[i] = Client;
			Room->CurrentPlayerNum++;

			//通知其它玩家
			for (char i2 = 0; i2 < 6; i2++)
			{
				ClientNode* RoomClient = Room->Player[i2];
				if (RoomClient && RoomClient != Client)
				{
					NotifyEnterRoom(RoomClient, Room, Client, i);
					NotifyEnterRoom(Client, Room, RoomClient, i2);
				}
			}
			break;
		}
	}

}
void ResponseEnterRoom(ClientNode* Client, RoomNode* Room, char SeatID)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, 0); //ResultID

	Write16(p, Room->ID); //RoomID
	Write16(p, Room->BaseGameMode); //BaseGameMode
	Write16(p, Room->SubGameMode); //SubGameMode
	//printf("房间密码2:%d\n", Room->Password);

	memcpy(p, Room->Name, MaxRoomName); //RoomName
	p += MaxRoomName;

	Write8(p, Room->SeatNum); //SeatNum
#ifdef ZingSpeed
	Write8(p, (BYTE)Room->Flag); //Flag
#else
	Write16(p, Room->Flag); //Flag
#endif

	Write8(p, SeatID); //SeatID
	Write8(p, 0); //TeamID
	Write32(p, Room->MapID); //MapID
	Write16(p, Room->RoomOwnerID); //RoomOwnerID
	
	//SeatStatus[6]
	for (size_t i = 0; i < 6; i++)
	{
		/*
		Seat_Closed		= 0,  //关闭
		Seat_Free		= 1, // 空位
		Seat_Used		= 2   //有人
		*/
		Write8(p, 2);
	}

	BYTE* pPlayerCount = p;
	Write8(p, 0); //PlayerCount
	int PlayerCount = 0;

	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		//这里不这样的话多人游戏的时候在舞蹈界面会多出个人
		if (RoomClient && Room->RoomOwnerID == RoomClient->ConnID)
		{
			PlayerCount++;

			WriteVisbleInfo(p, RoomClient->Uin, i, RoomClient->ConnID); //PlayerRoomVisibleInfo
		}
	}
	Set8(pPlayerCount, PlayerCount);

	Write8(p, 0); //ReasonLen
	Write8(p, 0); //PlayerCount2
	/*
m_astPlayerRelationInfo[].m_uiSrcUin
m_astPlayerRelationInfo[].m_iRelationFlag
m_astPlayerRelationInfo[].m_uiRelationUin
m_astPlayerRelationInfo[].m_szRelationNickname[]:
m_astPlayerRelationInfo[].m_u32EngageTime
m_astPlayerRelationInfo[].m_uiNextCanBookingTimeLimit
m_astPlayerRelationInfo[].m_uiBeginHoneyMoonTime
m_astPlayerRelationInfo[].m_uiEndHoneyMoonTime
m_astPlayerRelationInfo[].m_bEngageFlag
	*/

	Write16(p, 0); //KartNum

	/*
m_astKartRefitInfo[].m_uiUin
m_astKartRefitInfo[].m_iKartId
m_astKartRefitInfo[].m_iRefitCout
m_astKartRefitInfo[].m_shMaxFlags
m_astKartRefitInfo[].m_shWWeight
m_astKartRefitInfo[].m_shSpeedWeight
m_astKartRefitInfo[].m_shJetWeight
m_astKartRefitInfo[].m_shSJetWeight
m_astKartRefitInfo[].m_shAccuWeight
m_astKartRefitInfo[].m_iShapeRefitCount
m_astKartRefitInfo[].m_iKartHeadRefitItemID
m_astKartRefitInfo[].m_iKartTailRefitItemID
m_astKartRefitInfo[].m_iKartFlankRefitItemID
m_astKartRefitInfo[].m_iKartTireRefitItemID
	*/

	Write8(p, 0); //LoverRaceOrPro

	{ //STDynamicInfo
		BYTE* pSTDynamicInfo = p;
		Write16(pSTDynamicInfo, 0); //len

		Write8(pSTDynamicInfo, Room->STDynamicInfo.Season);
		Write8(pSTDynamicInfo, Room->STDynamicInfo.Timeslot);
		Write8(pSTDynamicInfo, Room->STDynamicInfo.Weather);

		len = pSTDynamicInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 0); //PreKnowMapID
	Write8(p, 0); //HasMoreInfo
	Write32(p, 0); //MapSubID

	Write8(p, 0); //HasFBInfo
	/*
m_astFBInfo[].m_bAvailableLev
m_astFBInfo[].m_bLev
m_astFBInfo[].m_bSubLev
m_astFBInfo[].m_iBossID
m_astFBInfo[].m_bFBSeason
	*/
	Write32(p, 0); //GuildScoreInNextGame
	Write16(p, 0); //GuildScoreAlreadyAward
	Write16(p, 0); //GuildScoreTotalAward


	len = p - buf;
	SendToClient(Client, 104, buf, len, Room->ID, FE_ROOM, Client->ConnID, Response);
}
void NotifyEnterRoom(ClientNode* Client, RoomNode* Room, ClientNode* RoomClient, char SeatID)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;


	Write16(p, Room->ID); //RoomID

	WriteVisbleInfo(p, RoomClient->Uin, SeatID, RoomClient->ConnID); //NewPlayerInfo

	{ //PlayerRelationInfo
		BYTE* pPlayerRelationInfo = p;
		Write16(pPlayerRelationInfo, 0); //len

		Write32(pPlayerRelationInfo, RoomClient->Uin); //SrcUin
		Write32(pPlayerRelationInfo, 0); //RelationFlag
		Write32(pPlayerRelationInfo, 0); //RelationUin

		char RelationNickName[MaxNickName] = "";
		memcpy(pPlayerRelationInfo, RelationNickName, MaxNickName);
		pPlayerRelationInfo += MaxNickName;

		Write32(pPlayerRelationInfo, 0); //EngageTime
		Write32(pPlayerRelationInfo, 0); //NextCanBookingTimeLimit
		Write32(pPlayerRelationInfo, 0); //BeginHoneyMoonTime
		Write32(pPlayerRelationInfo, 0); //EndHoneyMoonTime
		Write8(pPlayerRelationInfo, 0); //EngageFlag


		len = pPlayerRelationInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write16(p, 0); //KartNum
	/*
m_astKartRefitInfo[].m_uiUin
m_astKartRefitInfo[].m_iKartId
m_astKartRefitInfo[].m_iRefitCout
m_astKartRefitInfo[].m_shMaxFlags
m_astKartRefitInfo[].m_shWWeight
m_astKartRefitInfo[].m_shSpeedWeight
m_astKartRefitInfo[].m_shJetWeight
m_astKartRefitInfo[].m_shSJetWeight
m_astKartRefitInfo[].m_shAccuWeight
m_astKartRefitInfo[].m_iShapeRefitCount
m_astKartRefitInfo[].m_iKartHeadRefitItemID
m_astKartRefitInfo[].m_iKartTailRefitItemID
m_astKartRefitInfo[].m_iKartFlankRefitItemID
m_astKartRefitInfo[].m_iKartTireRefitItemID
	*/
#ifndef ZingSpeed
	Write8(p, 0); //SpeFlag
	Write8(p, 0); //MapNum
	
	
	//{ //ShuttleMapList
	//	BYTE* pShuttleMapList = p;
	//	Write16(pShuttleMapList, 0); //len

	//	Write32(pShuttleMapList, 0); //MapID
	//	Write16(pShuttleMapList, 0); //BeginIdx
	//	Write16(pShuttleMapList, 0); //EndIdx
	//	Write8(pShuttleMapList, 0); //Reverse
	//	Write16(pShuttleMapList, 0); //BeginCheckPointIdx

	//	len = pShuttleMapList - p;
	//	Set16(p, (WORD)len);
	//	p += len;
	//}
	
	Write32(p, 0); //RoomNo
	Write16(p, 0); //SvrId
	Write16(p, 0); //RaceCardNums
	
	
	//{ //RaceCardAddValueInfo
	//	BYTE* pRaceCardAddValueInfo = p;
	//	Write16(pRaceCardAddValueInfo, 0); //len

	//	Write32(pRaceCardAddValueInfo, 0); //RaceCardID
	//	Write32(pRaceCardAddValueInfo, 0); //AddValue

	//	len = pRaceCardAddValueInfo - p;
	//	Set16(p, (WORD)len);
	//	p += len;
	//}
	
	Write16(p, 0); //CreateRoomType
	Write16(p, 0); //ServerMatchType
	Write16(p, 0); //TeamMatchType
	Write16(p, 0); //MatchedRoomType
	Write8(p, 0); //InviteType
	Write8(p, 0); //HasWeRelayRoomInfo
	
	
	//{ //WeRelayRoomInfo
	//	BYTE* pWeRelayRoomInfo = p;
	//	Write16(pWeRelayRoomInfo, 0); //len

	//	Write16(pWeRelayRoomInfo, 0); //MapNum
	//	Write32(pWeRelayRoomInfo, 0); //MapID[]
	//	Write16(pWeRelayRoomInfo, 0); //CarTypeNum
	//	
	//	{ //CarRandom
	//		BYTE* pCarRandom = pWeRelayRoomInfo;
	//		Write16(pCarRandom, 0); //len

	//		Write32(pCarRandom, 0); //CarType
	//		Write32(pCarRandom, 0); //CarNum
	//		
	//		{ //RandomCarInfo
	//			BYTE* pRandomCarInfo = pCarRandom;
	//			Write16(pRandomCarInfo, 0); //len

	//			Write32(pRandomCarInfo, 0); //CarID
	//			Write16(pRandomCarInfo, 0); //FitLev
	//			Write8(pRandomCarInfo, 0); //HasRefitInfo
	//			
	//			BYTE* pKartNum = p;
	//			{
	//				const char* sql = NULL;
	//				sqlite3_stmt* stmt = NULL;
	//				int result;
	//				size_t i = 0;

	//				int OldRefitCout = 0;
	//				int OldMaxFlags = 0;
	//				int OldWWeight = 0;
	//				int OldSpeedWeight = 0;
	//				int OldJetWeight = 0;
	//				int OldSJetWeight = 0;
	//				int OldAccuWeight = 0;
	//				int OldShapeRefitCount = 0;
	//				int OldKartHeadRefitItemID = 0;
	//				int OldKartTailRefitItemID = 0;
	//				int OldKartFlankRefitItemID = 0;
	//				int OldKartTireRefitItemID = 0;
	//				//int OldShapeSuitID = 0;


	//				sql = "SELECT RefitCout,MaxFlags,WWeight,SpeedWeight,JetWeight,SJetWeight,AccuWeight,ShapeRefitCount,KartHeadRefitItemID,KartTailRefitItemID,KartFlankRefitItemID,KartTireRefitItemID  FROM KartRefit WHERE Uin=? AND KartID=?;";
	//				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	//				if (result == SQLITE_OK) {
	//					sqlite3_bind_int(stmt, 1, RoomClient->Uin);
	//					sqlite3_bind_int(stmt, 2, RoomClient->KartID);
	//					if (sqlite3_step(stmt) == SQLITE_ROW)
	//					{
	//						
	//						OldRefitCout = sqlite3_column_int(stmt, 0);
	//						OldMaxFlags = sqlite3_column_int(stmt, 1);
	//						OldWWeight = sqlite3_column_int(stmt, 2);
	//						OldSpeedWeight = sqlite3_column_int(stmt, 3);
	//						OldJetWeight = sqlite3_column_int(stmt, 4);
	//						OldSJetWeight = sqlite3_column_int(stmt, 5);
	//						OldAccuWeight = sqlite3_column_int(stmt, 6);
	//						OldShapeRefitCount = sqlite3_column_int(stmt, 7);
	//						OldKartHeadRefitItemID = sqlite3_column_int(stmt, 8);
	//						OldKartTailRefitItemID = sqlite3_column_int(stmt, 9);
	//						OldKartFlankRefitItemID = sqlite3_column_int(stmt, 10);
	//						OldKartTireRefitItemID = sqlite3_column_int(stmt, 11);
	//						//OldShapeSuitID = sqlite3_column_int(stmt, 12);
	//						
	//					}
	//				}

	//				sqlite3_finalize(stmt);
	//				stmt = NULL;



	//			{ //KartRefitInfo
	//				BYTE* pKartRefitInfo = pRandomCarInfo;
	//				Write16(pKartRefitInfo, 0); //len

	//				Write32(pKartRefitInfo, RoomClient->Uin); //Uin
	//	
	//				Write32(pKartRefitInfo, RoomClient->KartID); //KartId

	//				Write32(pKartRefitInfo, OldRefitCout); //RefitCout
	//				Write16(pKartRefitInfo, OldMaxFlags); //MaxFlags
	//				Write16(pKartRefitInfo, OldWWeight); //WWeight
	//				Write16(pKartRefitInfo, OldSpeedWeight); //SpeedWeight
	//				Write16(pKartRefitInfo, OldJetWeight); //JetWeight
	//				Write16(pKartRefitInfo, OldSJetWeight); //SJetWeight
	//				Write16(pKartRefitInfo, OldAccuWeight); //AccuWeight
	//				Write32(pKartRefitInfo, OldShapeRefitCount); //ShapeRefitCount
	//				Write32(pKartRefitInfo, OldKartHeadRefitItemID); //KartHeadRefitItemID
	//				Write32(pKartRefitInfo, OldKartTailRefitItemID); //KartTailRefitItemID
	//				Write32(pKartRefitInfo, OldKartFlankRefitItemID); //KartFlankRefitItemID
	//				Write32(pKartRefitInfo, OldKartTireRefitItemID); //KartTireRefitItemID

	//				Write32(pKartRefitInfo, 0); //SecondRefitCount
	//				Write16(pKartRefitInfo, 0); //Speed2Weight
	//				Write16(pKartRefitInfo, 0); //DriftVecWeight
	//				Write16(pKartRefitInfo, 0); //AdditionalZSpeedWeight
	//				Write16(pKartRefitInfo, 0); //AntiCollisionWeight
	//				Write16(pKartRefitInfo, 0); //LuckyValue
	//				Write16(pKartRefitInfo, 0); //RefitLuckyValueMaxWeight
	//				Write32(pKartRefitInfo, 0); //ShapeSuitID
	//				Write8(pKartRefitInfo, 0); //LegendSuitLevel
	//				Write32(pKartRefitInfo, 0); //LegendSuitLevelChoice
	//				Write32(pKartRefitInfo,0); //ShapeLegendSuitID

	//				len = pKartRefitInfo - pRandomCarInfo;
	//				Set16(pRandomCarInfo, (WORD)len);
	//				pRandomCarInfo += len;
	//			}
	//			Write8(pRandomCarInfo, 0); //HasStoneInfo
	//			
	//			{ //STKartStoneGrooveInfo
	//				BYTE* pSTKartStoneGrooveInfo = pRandomCarInfo;
	//				Write16(pSTKartStoneGrooveInfo, 0); //len

	//				Write32(pSTKartStoneGrooveInfo, 0); //KartID
	//				Write32(pSTKartStoneGrooveInfo, 0); //StoneGrooveNum
	//				
	//				{ //StoneGrooveInfo
	//					BYTE* pStoneGrooveInfo = pSTKartStoneGrooveInfo;
	//					Write16(pStoneGrooveInfo, 0); //len

	//					Write32(pStoneGrooveInfo, 0); //StoneUseOccaType
	//					Write32(pStoneGrooveInfo, 0); //SkillStoneID

	//					len = pStoneGrooveInfo - pSTKartStoneGrooveInfo;
	//					Set16(pSTKartStoneGrooveInfo, (WORD)len);
	//					pSTKartStoneGrooveInfo += len;
	//				}

	//				len = pSTKartStoneGrooveInfo - pRandomCarInfo;
	//				Set16(pRandomCarInfo, (WORD)len);
	//				pRandomCarInfo += len;
	//			}
	//			Write32(pRandomCarInfo, 0); //KartType

	//			len = pRandomCarInfo - pCarRandom;
	//			Set16(pCarRandom, (WORD)len);
	//			pCarRandom += len;
	//		}

	//		len = pCarRandom - pWeRelayRoomInfo;
	//		Set16(pWeRelayRoomInfo, (WORD)len);
	//		pWeRelayRoomInfo += len;
	//	}

	//	len = pWeRelayRoomInfo - p;
	//	Set16(p, (WORD)len);
	//	p += len;
	//}
	//
	Write8(p, 0); //RoomFlag
	Write8(p, 0); //MaxOBNum
	Write8(p, 0); //AllowAutoStart
	Write8(p, 0); //HasLMInfo
	
	
	//{ //LMInfo
	//	BYTE* pLMInfo = p;
	//	Write16(pLMInfo, 0); //len

	//	Write8(pLMInfo, 0); //GameType

	//	len = pLMInfo - p;
	//	Set16(p, (WORD)len);
	//	p += len;
	//}
	
	Write8(p, 0); //TalkRoomBGID
	Write16(p, 0); //Role
	Write8(p, 0); //ParaNum
	//Write32(p, 0); //ParaList[]
	{ //RoomInfoChange
		BYTE* pRoomInfoChange = p;
		Write16(pRoomInfoChange, 0); //len

		Write32(pRoomInfoChange, 0); //TeamJumpLevel
		Write32(pRoomInfoChange, 0); //RoomBGID

		len = pRoomInfoChange - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); //PlayerEquipKartID
	Write32(p, 0); //RankedMatchTestDriveKartID
	Write8(p, 0); //FirstlyTipMap
	Write8(p, 0); //GVoiceRoomNameLen
	Write8(p, 0); //GVoiceReasonLen
	Write8(p, 0); //TDCExcluItemNum
	Write32(p, 0); //TDCExclusiveItemID[]
	Write8(p, 0); //TestKartGameType
	Write32(p, 0); //GameTestKartID
	Write8(p, 0); //HasStarsFightingInfo
	
	
	//{ //StarsFightingInfo
	//	BYTE* pStarsFightingInfo = p;
	//	Write16(pStarsFightingInfo, 0); //len

	//	Write32(pStarsFightingInfo, 0); //Star
	//	Write32(pStarsFightingInfo, 0); //StarVal
	//	Write32(pStarsFightingInfo, 0); //StarsFightingStarLevel

	//	len = pStarsFightingInfo - p;
	//	Set16(p, (WORD)len);
	//	p += len;
	//}
	
	Write8(p, 0); //HasYuLeJiaNianHuaInfo
	
	
	//{ //YuLeJiaNianHuaInfo
	//	BYTE* pYuLeJiaNianHuaInfo = p;
	//	Write16(pYuLeJiaNianHuaInfo, 0); //len

	//	Write32(pYuLeJiaNianHuaInfo, 0); //Mode
	//	Write32(pYuLeJiaNianHuaInfo, 0); //Score
	//	Write8(pYuLeJiaNianHuaInfo, 0); //RankRangNum
	//	Write32(pYuLeJiaNianHuaInfo, 0); //RankRange[]
	//	Write8(pYuLeJiaNianHuaInfo, 0); //RankScoreNum
	//	Write32(pYuLeJiaNianHuaInfo, 0); //RankScore[]
	//	Write8(pYuLeJiaNianHuaInfo, 0); //InRecommend
	//	Write8(pYuLeJiaNianHuaInfo, 0); //InDoubleScore

	//	len = pYuLeJiaNianHuaInfo - p;
	//	Set16(p, (WORD)len);
	//	p += len;
	//}
	

#endif
	len = p - buf;
	SendToClient(Client, 500, buf, len, Room->ID, FE_ROOM, Client->ConnID, Notify);
}    

void RequestLeaveRoom(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	short LeaveType = 0;
#ifndef ZingSpeed
	LeaveType = Read16(Body);
	BYTE LeaveRoomReason = Read8(Body);
#endif

	LeaveRoom(Client, LeaveType);
}
void LeaveRoom(ClientNode* Client, short LeaveType)
{
	if (Client->RoomID == 0)
	{
		return;
	}
	RoomNode* Room = GetRoom(Client->RoomID);
	if (!Room)
	{
		return;
	}
	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient == Client)
		{
			ResponseLeaveRoom(Client, LeaveType);
			Room->Player[i] = NULL;
			Client->RoomID = 0;
			Room->CurrentPlayerNum--;

			BOOL IsRoomOwner = FALSE;
			short NewRoomOwnerID = Room->RoomOwnerID;
			if (NewRoomOwnerID == Client->ConnID)
			{
				IsRoomOwner = TRUE;
				for (char i2 = 0; i2 < 6; i2++)
				{
					RoomClient = Room->Player[i2];
					if (RoomClient)
					{
						NewRoomOwnerID = RoomClient->ConnID; //转让房主
						break;
					}
				}
			}
			Room->RoomOwnerID = NewRoomOwnerID;
			int Flag = 0;
			Flag = Room->Flag;
			//通知其它玩家
			int n = 0;
			for (char i2 = 0; i2 < 6; i2++)
			{
				RoomClient = Room->Player[i2];
				if (RoomClient)
				{
					NotifyLeaveRoom(RoomClient, Client->Uin, i, IsRoomOwner, NewRoomOwnerID, LeaveType, Flag);
					n++;
				}
			}

			if (n == 0)
			{
				if (Room->Timer)
				{
					if (DeleteTimerQueueTimer(TimerQueue, Room->Timer, INVALID_HANDLE_VALUE))
					{
						Room->Timer = NULL;
					}
				}
				Mutex.lock();
				Rooms.erase(Room->ID);
				Mutex.unlock();
				delete Room;
			}
			break;
		}

	}
}
void ResponseLeaveRoom(ClientNode* Client, short LeaveType)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, 0); //ResultID

	Write8(p, 0); //ReasonLen

#ifndef ZingSpeed
	Write16(p, LeaveType); //LeaveType
	Write16(p, 0); //BaseGameMode
	Write16(p, 0); //SubGameMode
	Write8(p, 0); //IsLeaveGVoice
#endif

	len = p - buf;
	SendToClient(Client, 105, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Response);
}
void NotifyLeaveRoom(ClientNode* Client, UINT Uin, char SeatID, bool IsChangeRoomOwner, short NewRoomOwnerID, short LeaveType, int Flag)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, Client->RoomID); //RoomID
	Write32(p, Uin); //Uin
	Write8(p, SeatID); //SeatID
	Write8(p, IsChangeRoomOwner); //IsChangeRoomOwner
	Write16(p, NewRoomOwnerID); //NewRoomOwnerID

#ifdef ZingSpeed
	Write8(p, Flag); //Flag
#else
	Write16(p, Flag); //Flag


#endif
	
	Write8(p, 0); //HasFBInfo
	/*
m_astFBInfo[].m_bAvailableLev
m_astFBInfo[].m_bLev
m_astFBInfo[].m_bSubLev
m_astFBInfo[].m_iBossID
m_astFBInfo[].m_bFBSeason
	*/

	len = p - buf;
	SendToClient(Client, 501, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Notify);
}

void RequestStartGame(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	RoomNode* Room = GetRoom(Client->RoomID);
	if (!Room)
	{
		return;
	}

	Room->Status = 0;

	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient)
		{
			if (RoomClient->ConnID == Room->RoomOwnerID)
				continue;
			else
			{
				if (RoomClient->IsReady)
				{
					Room->Status = 1;
					ResponseStartGame(Client, "还有玩家没有准备,无法开始游戏!");
					return;
				}
				else
				{
					continue;
				}
			}
		}
	}

	if (Room->BaseGameMode == 4) //舞蹈
	{
		if (Room->SceneID == 0)
		{
			Room->SceneID = 17;
		}

		if (Room->MusicID < 10)
		{
			Room->MusicID = 303;
		}

		for (char i = 0; i < 6; i++)
		{
			ClientNode* RoomClient = Room->Player[i];
			if (RoomClient)
			{
				RoomClient->GameID = 2;
				RoomClient->IsReady = FALSE;

				RoomClient->Scores = 0;

				NotifyDanceGameBegin(RoomClient);
			}
		}
		CreateRoomTimer(Room, DanceMusicDuration(Room->MusicID), OnDanceTimer);
	}
	else
	{
		UINT MapID = Room->MapID;
		if (MapID < 100) //随机
		{
			if (MapID == 5)//边境随机
			{
				double a[58] = { 137,108,189,400,381,346,256,239,123,196,202,299,132,190,131,122,118,133,185,187,276,121,167,396,222,142,141,124,254,351,267,262,139,140,204,181,208,153,368,329,320,258,191,339,199,194,238,231,212,260,283,266,397,342,332,277,321,322 };
				int N = 58, n = 60, i, j; 
				srand((unsigned)time(NULL));
				for (i = 0; i < n; i++) {
					j = rand() % N; 
					MapID = a[j]; 
				};
			}
			else//全部随机

				if (MapID == 7)//自定义结算测试
				{
					int N;
					N = 335;
					MapID = N;
				}
				else
				{
					int ban0[] = { 106,107,109,110,115,116,117,114,125,126,127,128,155,156,157,172,209,210,223,236,241,242,247,248,249,252,271,272,285,286,287,288,289,291,292,293,306,335,336,337,344,362,375,383,388,401,402,403,405,407,408,409,410,411,412,413,414,417,419,425,426 };
					int arrlen = sizeof(ban0) / sizeof(ban0[0]);
				ran:
					srand((unsigned)time(NULL));
					MapID = rand() % 326 + 104;
					for (int i = 0; i < arrlen; i++)
					{
						if (ban0[i] == MapID)
						{
							goto ran;
						}
					}
				}
		
		}
		
		

		Map* pMap = GetMap(MapID);
		if (!pMap)
		{
			Room->Status = 1;
#ifndef ZingSpeed
			ResponseStartGame(Client, "/C03该地图未配置!只会保存不了记录,不影响游玩.");
			//ResponseStartGame(Client, "未配置此地图!稍后尝试兼容模式...");
			printf("请配置 MapID:%d\n", MapID);
			Sleep(2000);
#else
			ResponseStartGame(Client, u8"未配置此地图!");
			return;
#endif
		}
		for (char i = 0; i < 6; i++)
		{
			ClientNode* RoomClient = Room->Player[i];//开始游戏
			if (RoomClient)
			{
				////设置房间状态
				Room->Status = 0;
				RoomClient->GameID = 2;
				RoomClient->IsReady = FALSE;
				if (pMap)
				{
					RoomClient->MapCompatibilityMode = false;
					RoomClient->EndCheckPoint = pMap->End;
					RoomClient->TotalRound = pMap->Round;
					RoomClient->Loop = pMap->Loop;
				}
				else
				{
					RoomClient->MapCompatibilityMode = true;
					RoomClient->EndCheckPoint = 0;
					RoomClient->TotalRound = 1;
					RoomClient->Loop = false;
				}
				RoomClient->MapID = MapID;
				RoomClient->FinishTime = 0;
				RoomClient->Round = 0;
				RoomClient->PassedCheckPoint = 0;
				//PrintRoom();
				NotifyGameBegin(RoomClient, Room->Player);
				NotifyKartPhysParam(RoomClient, Room->Player);
				NotifyOtherKartStoneInfo(RoomClient);
			}
		}
	}
}

void ResponseStartGame(ClientNode* Client, const char* Reason)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, -1); //ResultID

	len = strlen(Reason);
	Write8(p, (BYTE)len); //ReasonLen
	memcpy(p, Reason, len);
	p += len;

	len = p - buf;
	SendToClient(Client, 110, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Response);
}


void RequestChangeMap(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	UINT NewMapID = Read32(Body);
	/*
m_iMapSubID
	*/
	char Season = -1;
	char Timeslot = -1;
	char Weather = -1;
	{
		BYTE* pSTDynamicInfo = Body;
		USHORT len = Read16(pSTDynamicInfo);

		Season = Read8(pSTDynamicInfo);
		Timeslot = Read8(pSTDynamicInfo);
		Weather = Read8(pSTDynamicInfo);

		Body += len;
	}

	RoomNode* Room = GetRoom(Client->RoomID);
	if (!Room)
	{
		return;
	}
	Room->MapID = NewMapID;
	Room->STDynamicInfo.Season = Season;
	Room->STDynamicInfo.Timeslot = Timeslot;
	Room->STDynamicInfo.Weather = Weather;
	printf("地图Mapid:%d\n", Room->MapID);

	ResponseChangeMap(Client, Room);

	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient && RoomClient!= Client)
		{
			NotifyChangeMap(RoomClient, Client->Uin, Room);
		}
	}


}
void ResponseChangeMap(ClientNode* Client, RoomNode* Room)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, 0); //ResultID
	Write32(p, Room->MapID); //NewMapID
	Write8(p, 0); //ReasonLen
	Write8(p, 0); //UnLockMapCondDescNum
	//m_astUnLockMapCondDesc[].m_bDescLen

	{ //STDynamicInfo
		BYTE* pSTDynamicInfo = p;
		Write16(pSTDynamicInfo, 0); //len

		Write8(pSTDynamicInfo, Room->STDynamicInfo.Season);
		Write8(pSTDynamicInfo, Room->STDynamicInfo.Timeslot);
		Write8(pSTDynamicInfo, Room->STDynamicInfo.Weather);

		len = pSTDynamicInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); //PreKnowMapID
	Write32(p, 0); //MapSubID
#ifndef ZingSpeed
	Write8(p, 0); //FirstlyTipMap
#endif

	len = p - buf;
	SendToClient(Client, 121, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Response);
}
void NotifyChangeMap(ClientNode* Client, UINT Uin, RoomNode* Room)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write32(p, Uin); //Uin
	Write32(p, Room->MapID); //NewMapID

	{ //STDynamicInfo
		BYTE* pSTDynamicInfo = p;
		Write16(pSTDynamicInfo, 0); //len

		Write8(pSTDynamicInfo, Room->STDynamicInfo.Season);
		Write8(pSTDynamicInfo, Room->STDynamicInfo.Timeslot);
		Write8(pSTDynamicInfo, Room->STDynamicInfo.Weather);

		len = pSTDynamicInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 0); //PreKnowMapID
	Write32(p, 0); //MapSubID
#ifndef ZingSpeed
	Write8(p, 0); //FirstlyTipMap
#endif

	len = p - buf;
	SendToClient(Client, 520, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Notify);
}


void RequestChangeSeatState(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	short RoomID = Read16(Body);
	BYTE SeatID = Read8(Body);
	BYTE SeatState = Read8(Body);

	ResponseChangeSeatState(Client, SeatID, SeatState);
}
void ResponseChangeSeatState(ClientNode* Client, BYTE SeatID, BYTE SeatState)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, 0); //ResultID

	Write8(p, SeatID);
	Write8(p, SeatState);

	Write8(p, 0); //ReasonLen

	len = p - buf;
	//SendToClient(Client, 107, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Response);
	RoomNode* Room = GetRoom(Client->RoomID);
	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient)
		{
			//Room->SeatNum = 1;设置房间人数
			SendToClient(RoomClient, 107, buf, len, RoomClient->RoomID, FE_ROOM, RoomClient->ConnID, Response);
		}
	}
}

void RequestKickFromRoom(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	UINT DstUin = Read32(Body);
	BYTE KickReason = Read8(Body);

	ResponseKickFromRoom(Client, DstUin);
}
void ResponseKickFromRoom(ClientNode* Client, UINT DstUin)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, 0); //ResultID

	Write32(p, DstUin);

	Write8(p, 0); //ReasonLen

	len = p - buf;
	SendToClient(Client, 108, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Response);
}


void NotifyChangeReadyState(ClientNode* Client, UINT Uin, bool State)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write32(p, Uin);
	Write8(p, State);

	len = p - buf;
	SendToClient(Client, 504, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Notify);
}
void RequestChangeReadyState(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	bool ReadyState = Read8(Body);
	ResponseChangeReadyState(Client, ReadyState);
	RoomNode* Room = GetRoom(Client->RoomID);
	if (!Room)
	{
		return;
	}
	Client->IsReady = ReadyState;
	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient && RoomClient != Client)
		{
			NotifyChangeReadyState(RoomClient, Client->Uin, ReadyState);
		}
	}
}
void ResponseChangeReadyState(ClientNode* Client, bool ReadyState)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, 0); //ResultID

	Write8(p, ReadyState);

	Write8(p, 0); //ReasonLen


	len = p - buf;
	SendToClient(Client, 109, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Response);
}


void RequestTalk(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	BYTE Type = Read8(Body);
	BYTE ClientUseFlag = Read8(Body);
	UINT DestPlayerUin = Read32(Body);

	USHORT ContentLength = Read16(Body);
	char* Content = (char*)Body;
	Body += ContentLength;

	BYTE BugletType = Read8(Body);
	BYTE ReserveFlag = Read8(Body);
	short DstNickNameLength = Read16(Body);

	ResponseTalk(Client, Type, ClientUseFlag, DestPlayerUin);

	printf("Type:%d, ClientUseFlag:%d. DestPlayerUin:%d\n", Type, ClientUseFlag, DestPlayerUin);

	RoomNode* Room = GetRoom(Client->RoomID);
	if (!Room)
	{
		return;
	}
	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient)
		{
#ifdef ZingSpeed
			if (RoomClient == Client)
			{
				continue;
			}
#endif
			NotifyTalk(RoomClient, Client->Uin, Type, ClientUseFlag, DestPlayerUin, ContentLength, Content);
		}
	}
}
void ResponseTalk(ClientNode* Client, BYTE Type, BYTE ClientUseFlag, UINT DestPlayerUin)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, 0); //ResultID

	Write8(p, Type);
	Write8(p, ClientUseFlag);
	Write32(p, DestPlayerUin);

	Write8(p, 0); //ReasonLen
	Write8(p, 0); //BugletType
	Write8(p, 0); //ReserveFlag
	Write8(p, 0); //ReserveData
#ifndef ZingSpeed
	Write8(p, 0); //TalkSpecialType
	Write32(p, 0); //Seq
#endif

	len = p - buf;
	SendToClient(Client, 111, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Response);
}
//房间说话打字
void NotifyTalk(ClientNode* Client, UINT SrcUin, BYTE Type, BYTE ClientUseFlag, UINT DestPlayerUin, short ContentLength, const char* Content)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;
	std::string NickName = "";
	Write32(p, SrcUin);


	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	sql = "SELECT NickName,Gender,Country,License,Experience,SuperMoney,Money,WinNum,SecondNum,ThirdNum,TotalNum,CurHonor,TotalHonor,TodayHonor,RelaxTime,MonthDurationBefor,MonthDurationCur,Charm,DurationGame,DanceExp,Coupons,Admiration,LuckMoney,TeamWorkExp,AchievePoint,RegTime,Signature  FROM BaseInfo  WHERE Uin=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, SrcUin);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			//NickName = Utf8ToGbk((char*)sqlite3_column_text(stmt, 0));//读取名称
			NickName = (char*)sqlite3_column_text(stmt, 0);//读取名称


		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	

	memset(p, 0, MaxNickName); //SrcNickName[]
	memcpy(p, NickName.c_str(), strlen(NickName.c_str()));
	p += MaxNickName;
	//Write32(p, 10002); //SrcNickName[]

	Write8(p, 1); //Gender

	Write8(p, Type);
	Write8(p, ClientUseFlag);
	Write32(p, DestPlayerUin);

	Write16(p, ContentLength);


	memcpy(p, Content, ContentLength);
	
	p += ContentLength;
	//printf("说了什么:%d\n", p);

	Write8(p, 0); //BugletType
	Write16(p, 0); //Identify
	Write16(p, 0x0107); //VipFlag
	Write8(p, 0); //CanReport


	{ //TalkerInfo
		BYTE* pTalkerInfo = p;
		Write16(pTalkerInfo, 0); //len

		Write32(pTalkerInfo, 0); //Exp

		{ //NobleInfo
			BYTE* pNobleInfo = pTalkerInfo;
			Write16(pNobleInfo, 0); //len

			Write32(pNobleInfo, SrcUin); //NobleID
			Write8(pNobleInfo, 6); //NobleLevel
			Write32(pNobleInfo, 1); //NoblePoint
			Write32(pNobleInfo, 30); //NobleLeftDays

			len = pNobleInfo - pTalkerInfo;
			Set16(pTalkerInfo, (WORD)len);
			pTalkerInfo += len;
		}
#ifndef ZingSpeed
		{ //GuildVipBaseInfo
			BYTE* pGuildVipBaseInfo = pTalkerInfo;
			Write16(pGuildVipBaseInfo, 0); //len

			Write8(pGuildVipBaseInfo, 6); //GuildVipLevel
			Write32(pGuildVipBaseInfo, 1); //GuildVipPoint

			len = pGuildVipBaseInfo - pTalkerInfo;
			Set16(pTalkerInfo, (WORD)len);
			pTalkerInfo += len;
		}
		Write32(pTalkerInfo, 0); //GuildId

		Write8(pTalkerInfo, 0); //HasLoverVip
		/*
		for (size_t i = 0; i < n; i++)
		{ //LoverVipInfo
			BYTE* pLoverVipInfo = pTalkerInfo;
			Write16(pLoverVipInfo, 0); //len

			Write8(pLoverVipInfo, 0); //LoverVipLevel
			Write32(pLoverVipInfo, 0); //LoverVipPoint
			Write8(pLoverVipInfo, 0); //GrowRate

			len = pLoverVipInfo - pTalkerInfo;
			Set16(pTalkerInfo, (WORD)len);
			pTalkerInfo += len;
		}
		*/
		Write8(pTalkerInfo, 0); //GardenLevel
		Write8(pTalkerInfo, 0); //ConsumeVipLevel
		{ //EmperorInfo
			BYTE* pEmperorInfo = pTalkerInfo;
			Write16(pEmperorInfo, 0); //len

			Write8(pEmperorInfo, 0); //EmperorLevel
			Write32(pEmperorInfo, 0); //EmperorPoint
			Write32(pEmperorInfo, 0); //EmperorLeftDays
			Write8(pEmperorInfo, 0); //EmperorGrowRate

			len = pEmperorInfo - pTalkerInfo;
			Set16(pTalkerInfo, (WORD)len);
			pTalkerInfo += len;
		}
		Write32(pTalkerInfo, 0); //HelperLev
#endif

		len = pTalkerInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}


	Write8(p, 0); //ReserveFlag
	Write32(p, 0); //ReserveData
#ifndef ZingSpeed
	memset(p, 0, 18); //Contentid[]
	p+=18;
#endif
	Write8(p, 0); //transBufType
	Write16(p, 0); //transBufLen
#ifndef ZingSpeed
	Write8(p, 0); //MsgSource
	Write16(p, 0); //DstNickNameLength
	Write32(p, 0); //Seq
#endif

	len = p - buf;
	SendToClient(Client, 505, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Notify);
}
