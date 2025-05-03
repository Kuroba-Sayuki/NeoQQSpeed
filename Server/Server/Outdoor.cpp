#include "Outdoor.h"

#include <stdio.h>
#include <string>
#include <time.h>



constexpr int MAX_CHAIR_NUM = 200;
constexpr int MAX_NUM_IN_EACHCHAIR = 2;

struct MoveInfo
{
	UINT Uin;
	char WalkStatue;
	int Posion[3];
	int Ori[9];
	UINT AttachUin;
	int WalkExtState;
};


ChatRoomNode ChatRooms[ChatRoomNum];
void InitChatRoom()
{
	ChatRooms[0].ID = 0;
	ChatRooms[0].Name = "琳琅湾";
	ChatRooms[0].CurrPlayerNum = 0;
	ChatRooms[0].MaxPlayerNum = 99;
	ChatRooms[0].RoomType = 1;
	ChatRooms[0].SceneID = 109;
	ChatRooms[0].x = 0;
	ChatRooms[0].y = -50000;
	ChatRooms[0].z = 0;
	for (size_t i = 0; i < 99; i++)
	{
		ChatRooms[0].Player[i] = NULL;
	}

	ChatRooms[1].ID = 1;
	ChatRooms[1].Name = "琳琅镇";
	ChatRooms[1].CurrPlayerNum = 0;
	ChatRooms[1].MaxPlayerNum = 99;
	ChatRooms[1].RoomType = 2;
	ChatRooms[1].SceneID = 25;
	ChatRooms[1].x = 0;
	ChatRooms[1].y = 0;
	ChatRooms[1].z = 0;
	for (size_t i = 0; i < 99; i++)
	{
		ChatRooms[1].Player[i] = NULL;
	}

	ChatRooms[2].ID = 2;
	ChatRooms[2].Name = "单身派对";
	ChatRooms[2].CurrPlayerNum = 0;
	ChatRooms[2].MaxPlayerNum = 99;
	ChatRooms[2].RoomType = 3;
	ChatRooms[2].SceneID = 109;
	ChatRooms[2].x = 0;
	ChatRooms[2].y = -40000;
	ChatRooms[2].z = 0;
	for (size_t i = 0; i < 99; i++)
	{
		ChatRooms[2].Player[i] = NULL;
	}

	ChatRooms[3].ID = 3;
	ChatRooms[3].Name = "皇族休闲区";
	ChatRooms[3].CurrPlayerNum = 0;
	ChatRooms[3].MaxPlayerNum = 99;
	ChatRooms[3].RoomType = 4;
	ChatRooms[3].SceneID = 136;
	ChatRooms[3].x = 10000;
	ChatRooms[3].y = 0;
	ChatRooms[3].z = 0;
	for (size_t i = 0; i < 99; i++)
	{
		ChatRooms[3].Player[i] = NULL;
	}

	ChatRooms[4].ID = 4;
	ChatRooms[4].Name = "中国城";
	ChatRooms[4].CurrPlayerNum = 0;
	ChatRooms[4].MaxPlayerNum = 99;
	ChatRooms[4].RoomType = 3;
	ChatRooms[4].SceneID = 4;
	ChatRooms[4].x = 33071;
	ChatRooms[4].y = -45773;
	ChatRooms[4].z = 4368;
	for (size_t i = 0; i < 99; i++)
	{
		ChatRooms[4].Player[i] = NULL;
	}

}



void NotifyNPCInfo(ClientNode* Client);
void NotifyChairInfo(ClientNode* Client);
void ResponseEnterOutdoor(ClientNode* Client, short SceneID, int x, int y, int z);
void NotifyGuildRoomMoreInfo(ClientNode* Client);
void ResponseChangeChairState(ClientNode* Client, BYTE ChangeState, int ChairIndx, BYTE SeatIndx);
void NotifyEnterOutdoor(ClientNode* Client, ChatRoomNode* ChatRoom, ClientNode* RoomClient, char SeatID);

ChatRoomNode* GetChatRoom(short ChatRoomID)
{
	ChatRoomNode* ChatRoom = NULL;
	for (size_t i = 0; i < ChatRoomNum; i++)
	{
		if (ChatRooms[i].ID == ChatRoomID)
		{
			ChatRoom = &ChatRooms[i];
			break;
		}
	}
	return ChatRoom;
}

void RequestEnterOutdoor(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	short SceneID = Read16(Body);
	short ChatRoomID = Read16(Body);
	short NPCCfgVer = Read16(Body);

	LeaveOutdoor(Client);
	printf("SceneID:%d, ChatRoomID:%d\n",SceneID, ChatRoomID);

	ChatRoomNode* ChatRoom = GetChatRoom(ChatRoomID);
	if (!ChatRoom)
	{
		return;
	}
	for (char i = 0; i < 99; i++)
	{
		if (!ChatRoom->Player[i])
		{
			Client->ChatRoomID = ChatRoomID;
			ChatRoom->Player[i] = Client;
			ChatRoom->CurrPlayerNum++;

			SceneID = ChatRoom->SceneID;
			int x = ChatRoom->x;
			int y = ChatRoom->y;
			int z = ChatRoom->z;
			printf("SceneID:%d\n", SceneID);

			NotifyNPCInfo(Client);
			ResponseEnterOutdoor(Client, SceneID, x, y, z);
			NotifyGuildRoomMoreInfo(Client);
			NotifyChairInfo(Client);

			//通知其它玩家
			for (char i2 = 0; i2 < 6; i2++)
			{
				ClientNode* RoomClient = ChatRoom->Player[i2];
				if (RoomClient && RoomClient != Client)
				{
					NotifyEnterOutdoor(RoomClient, ChatRoom, Client, i);
				}
			}

			break;
		}
	}
}
void ResponseEnterOutdoor(ClientNode* Client, short SceneID, int x, int y, int z)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;


	Write16(p, Client->ChatRoomID); //ChatRoomID
	Write16(p, SceneID); //SceneID

	{ //HeroMoveInfo
		BYTE* pHeroMoveInfo = p;
		Write16(pHeroMoveInfo, 0); //len

		Write32(pHeroMoveInfo, 0); //Uin
		Write8(pHeroMoveInfo, 0); //WalkStatue

		Write32(pHeroMoveInfo, x); //Posion[3]
		Write32(pHeroMoveInfo, y);
		Write32(pHeroMoveInfo, z);

		Write32(pHeroMoveInfo, 1000); //Ori[9]
		Write32(pHeroMoveInfo, 0);
		Write32(pHeroMoveInfo, 0);
		Write32(pHeroMoveInfo, 0);
		Write32(pHeroMoveInfo, 1000);
		Write32(pHeroMoveInfo, 0);
		Write32(pHeroMoveInfo, 0);
		Write32(pHeroMoveInfo, 0);
		Write32(pHeroMoveInfo, 1000);

		Write32(pHeroMoveInfo, 0); //AttachUin
		Write32(pHeroMoveInfo, 0); //WalkExtState

		len = pHeroMoveInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write16(p, 0); //ResultID

	const char RoomName[] = "Name";
	len = strlen(RoomName);
	Write8(p, (BYTE)len); //ReasonLen
	memcpy(p, RoomName, len);
	p += len;


	Write16(p, 0); //HeroCount
	/*
m_astHeroInfo[].m_stHeroMoveInfo.m_uiUin
m_astHeroInfo[].m_stHeroMoveInfo.m_chsWalkStatue
m_astHeroInfo[].m_stHeroMoveInfo.m_aiPosion[3]
m_astHeroInfo[].m_stHeroMoveInfo.m_aiOri[9]
m_astHeroInfo[].m_stHeroMoveInfo.m_u32AttachUin
m_astHeroInfo[].m_stHeroMoveInfo.m_iWalkExtState
m_astHeroInfo[].m_iNextPoint
m_astHeroInfo[].m_shPathLen
m_astHeroInfo[].m_aiPath[]
m_astHeroInfo[].m_stHeroVisbleInfo.m_unUin
m_astHeroInfo[].m_stHeroVisbleInfo.m_unIdentity
m_astHeroInfo[].m_stHeroVisbleInfo.m_bSeatID
m_astHeroInfo[].m_stHeroVisbleInfo.m_bTeamID
m_astHeroInfo[].m_stHeroVisbleInfo.m_nPlayerID
m_astHeroInfo[].m_stHeroVisbleInfo.m_bStatus
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_szNickname[]:
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_bGender
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_bCountry
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_iLicense
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_unExperience
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_iSuperMoney
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_iMoney
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_unWinNum
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_uiSecondNum
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_uiThirdNum
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_unTotalNum
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_iCurHonor
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_uiTotalHonor
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_iTodayHonor
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_iRelaxTime
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_uiMonthDurationBefore
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_uiMonthDurationCur
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_uiCharm
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_uiDurationGame
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_uiDanceExp
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_iCoupons
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_uiAdmiration
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_iLuckMoney
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_uiTeamWorkExp
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_uiAchievePoint
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_uiRegTime
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_szSignature[]:
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_uiLuckyMatchPoint
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_uiLuckyMatchNum
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_ushLuckyMatchFirstNum
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_bLuckyMatchBestRand
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_uiLuckyMatchWinNum
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_uiFizzPointTotal
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_ushFizzPointDaily
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_ushFizzPointWeekly
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_ushFizzPointLastWeek
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_bFizzLotteryStatus
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_uiFizzLastUpdateTime
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_u32SNSLocaleCode
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_i32AuctionRightUnlockTime
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_ucReserveFlag
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_i32CurConsumeScore
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_i32HistoryConsumeScore
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_u8CrashModeSponsorID
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_ui32Popularity
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_uiLadderMatchAchievePoint
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_u32Cash
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_uiPlayerGPMIdentity
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_uiPointTotalScore
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_totalMapMedalNum
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_lastMapMedalNum
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_curMapMedalNum
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_curMapMedalSeasonId
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerDBBaseInfo.m_lastClearMapMedalTime
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerGuildInfo.m_szGuildName[]:
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerGuildInfo.m_ushDuty
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerGuildInfo.m_uiGuildID
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerGuildInfo.m_uiRight
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerGuildInfo.m_uiMedal
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerGuildInfo.m_uiContribute
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerGuildInfo.m_szDutyTitle[]:
m_astHeroInfo[].m_stHeroVisbleInfo.m_nEquipedItemNum
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerItemInfo[].m_iItemID
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerItemInfo[].m_iItemNum
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerItemInfo[].m_iAvailPeriod
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerItemInfo[].m_bStatus
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerItemInfo[].m_unObtainTime
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerItemInfo[].m_iOtherAttribute
m_astHeroInfo[].m_stHeroVisbleInfo.m_usPetNum
m_astHeroInfo[].m_stHeroVisbleInfo.m_astPetInfo[].m_uiUin
m_astHeroInfo[].m_stHeroVisbleInfo.m_astPetInfo[].m_iPetId
m_astHeroInfo[].m_stHeroVisbleInfo.m_astPetInfo[].m_szPetName[]:
m_astHeroInfo[].m_stHeroVisbleInfo.m_astPetInfo[].m_cIsProper
m_astHeroInfo[].m_stHeroVisbleInfo.m_astPetInfo[].m_usLevel
m_astHeroInfo[].m_stHeroVisbleInfo.m_astPetInfo[].m_ucStatus
m_astHeroInfo[].m_stHeroVisbleInfo.m_astPetInfo[].m_usPetSkillNum
m_astHeroInfo[].m_stHeroVisbleInfo.m_astPetInfo[].m_astPetSkillList[].m_usSkillID
m_astHeroInfo[].m_stHeroVisbleInfo.m_astPetInfo[].m_astPetSkillList[].m_ucStatus
m_astHeroInfo[].m_stHeroVisbleInfo.m_astPetInfo[].m_astPetSkillList[].m_cActive
m_astHeroInfo[].m_stHeroVisbleInfo.m_astPetInfo[].m_astPetSkillList[].m_iValue
m_astHeroInfo[].m_stHeroVisbleInfo.m_astPetInfo[].m_iStrengLevel
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerGuildMatchInfo.m_iSelfPoint
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerGuildMatchInfo.m_iWinNum
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerGuildMatchInfo.m_iTotalNum
m_astHeroInfo[].m_stHeroVisbleInfo.m_bIsInTopList
m_astHeroInfo[].m_stHeroVisbleInfo.m_bLoverRaceOrPro
m_astHeroInfo[].m_stHeroVisbleInfo.m_bTmpEffectNum
m_astHeroInfo[].m_stHeroVisbleInfo.m_aiTmpEffectItems[]
m_astHeroInfo[].m_stHeroVisbleInfo.m_bOBState
m_astHeroInfo[].m_stHeroVisbleInfo.m_bDebutOrX5
m_astHeroInfo[].m_stHeroVisbleInfo.m_bRandKeyFlag
m_astHeroInfo[].m_stHeroVisbleInfo.m_unVipFlag
m_astHeroInfo[].m_stHeroVisbleInfo.m_u8HaveAppellation
m_astHeroInfo[].m_stHeroVisbleInfo.m_astCurAppellation[].m_u8Type
m_astHeroInfo[].m_stHeroVisbleInfo.m_astCurAppellation[].m_u8Level
m_astHeroInfo[].m_stHeroVisbleInfo.m_astCurAppellation[].m_u8Status
m_astHeroInfo[].m_stHeroVisbleInfo.m_astCurAppellation[].m_u8Difficulty
m_astHeroInfo[].m_stHeroVisbleInfo.m_astCurAppellation[].m_i32MapId
m_astHeroInfo[].m_stHeroVisbleInfo.m_astCurAppellation[].m_u32Value
m_astHeroInfo[].m_stHeroVisbleInfo.m_stNobleInfo.m_u32NobleID
m_astHeroInfo[].m_stHeroVisbleInfo.m_stNobleInfo.m_u8NobleLevel
m_astHeroInfo[].m_stHeroVisbleInfo.m_stNobleInfo.m_u32NoblePoint
m_astHeroInfo[].m_stHeroVisbleInfo.m_stNobleInfo.m_u32NobleLeftDays
m_astHeroInfo[].m_stHeroVisbleInfo.m_bHasCarryWizard
m_astHeroInfo[].m_stHeroVisbleInfo.m_astCarryWizardInfo[].m_ushWizardID
m_astHeroInfo[].m_stHeroVisbleInfo.m_astCarryWizardInfo[].m_szNickname[]:
m_astHeroInfo[].m_stHeroVisbleInfo.m_astCarryWizardInfo[].m_ushWizardType
m_astHeroInfo[].m_stHeroVisbleInfo.m_astCarryWizardInfo[].m_bOrder
m_astHeroInfo[].m_stHeroVisbleInfo.m_astCarryWizardInfo[].m_bEvolutionBranch
m_astHeroInfo[].m_stHeroVisbleInfo.m_astCarryWizardInfo[].m_bIsHuanHua
m_astHeroInfo[].m_stHeroVisbleInfo.m_stGuildVipBaseInfo.m_u8GuildVipLevel
m_astHeroInfo[].m_stHeroVisbleInfo.m_stGuildVipBaseInfo.m_u32GuildVipPoint
m_astHeroInfo[].m_stHeroVisbleInfo.m_uiExFlag
m_astHeroInfo[].m_stHeroVisbleInfo.m_u8HaveLDMBaseInfo
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerLDMBaseInfo[].m_u8Grade
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerLDMBaseInfo[].m_u32Score
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerLDMBaseInfo[].m_u32MaxScore
m_astHeroInfo[].m_stHeroVisbleInfo.m_stPlayerLDMBaseInfo[].m_u32MaxGrade
m_astHeroInfo[].m_stHeroVisbleInfo.m_u8HasWl
m_astHeroInfo[].m_stHeroVisbleInfo.m_astPlayerWlVisibleInfo[].m_uiWonderLandID
m_astHeroInfo[].m_stHeroVisbleInfo.m_astPlayerWlVisibleInfo[].m_szWonderLandName[]:
m_astHeroInfo[].m_stHeroVisbleInfo.m_astPlayerWlVisibleInfo[].m_ushWonderLandDuty
m_astHeroInfo[].m_stHeroVisbleInfo.m_astPlayerWlVisibleInfo[].m_ucSubType
m_astHeroInfo[].m_stHeroVisbleInfo.m_astPlayerWlVisibleInfo[].m_bGuildGrade
m_astHeroInfo[].m_stHeroVisbleInfo.m_u8HasLoverVip
m_astHeroInfo[].m_stHeroVisbleInfo.m_astLoverVipInfo[].m_u8LoverVipLevel
m_astHeroInfo[].m_stHeroVisbleInfo.m_astLoverVipInfo[].m_u32LoverVipPoint
m_astHeroInfo[].m_stHeroVisbleInfo.m_astLoverVipInfo[].m_u8GrowRate
m_astHeroInfo[].m_stHeroVisbleInfo.m_bHasBattleModeSkillInfoList
m_astHeroInfo[].m_stHeroVisbleInfo.m_astBattleModeSkillInfoList[].m_bSkillNum
m_astHeroInfo[].m_stHeroVisbleInfo.m_astBattleModeSkillInfoList[].m_astSkillInfo[].m_shSkillId
m_astHeroInfo[].m_stHeroVisbleInfo.m_astBattleModeSkillInfoList[].m_astSkillInfo[].m_bSkillLevel
m_astHeroInfo[].m_shHeroEnterDelayTime
	*/
	
	Write16(p, 0); //RelationHeroCount
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

	Write16(p, 0); //HammerCount
	/*
m_astEquipedHammer[].m_iEquipedHammerID
	*/

	Write16(p, 0); //StealPigCount
	/*
m_astStealPig[].m_cStatus
m_astStealPig[].m_cPigPos
m_astStealPig[].m_iItemID
	*/
	Write16(p, 0); //MaxHeroNumInRoom


	len = p - buf;
	SendToClient(Client, 352, buf, len, Client->ChatRoomID, FE_OUTDOORSVRD, Client->ConnID, Response);
}

void NotifyEnterOutdoor(ClientNode* Client, ChatRoomNode* ChatRoom, ClientNode* RoomClient, char SeatID)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, ChatRoom->ID); //ChatRoomID
	Write16(p, ChatRoom->SceneID); //SceneID
	Write32(p, (DWORD)time(nullptr)); //Time


	{ //HeroInfo
		
		BYTE* pHeroInfo = p;
		Write16(pHeroInfo, 0); //len

		{ //HeroMoveInfo
			BYTE* pHeroMoveInfo = pHeroInfo;
			Write16(pHeroMoveInfo, 0); //len

			Write32(pHeroMoveInfo, RoomClient->Uin); //Uin
			Write8(pHeroMoveInfo, 0); //WalkStatue

			Write32(pHeroMoveInfo, ChatRoom->x); //Posion[3]
			Write32(pHeroMoveInfo, ChatRoom->y);
			Write32(pHeroMoveInfo, ChatRoom->z);

			Write32(pHeroMoveInfo, 1000); //Ori[9]
			Write32(pHeroMoveInfo, 0);
			Write32(pHeroMoveInfo, 0);
			Write32(pHeroMoveInfo, 0);
			Write32(pHeroMoveInfo, 1000);
			Write32(pHeroMoveInfo, 0);
			Write32(pHeroMoveInfo, 0);
			Write32(pHeroMoveInfo, 0);
			Write32(pHeroMoveInfo, 1000);

			Write32(pHeroMoveInfo, 0); //AttachUin
			Write32(pHeroMoveInfo, 0); //WalkExtState

			len = pHeroMoveInfo - pHeroInfo;
			Set16(pHeroInfo, (WORD)len);
			pHeroInfo += len;
		}

		Write32(pHeroInfo, 0); //NextPoint
		Write16(pHeroInfo, 0); //PathLen
		//m_astHeroInfo[].m_aiPath[]

		WriteVisbleInfo(pHeroInfo, RoomClient->Uin, 0, RoomClient->ConnID); //HeroVisbleInfo


		Write16(pHeroInfo, 0); //HeroEnterDelayTime


		len = pHeroInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	


	{ //PlayerRelationInfo
		BYTE* pPlayerRelationInfo = p;
		Write16(pPlayerRelationInfo, 0); //len

		Write32(pPlayerRelationInfo, RoomClient->Uin); //SrcUin
		Write32(pPlayerRelationInfo, 0); //RelationFlag
		Write32(pPlayerRelationInfo, 0); //RelationUin

		memset(pPlayerRelationInfo, 0, MaxNickName); //RelationNickname[]
		strcpy_s((char*)pPlayerRelationInfo, MaxNickName, "test");
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

	{ //StealPig
		BYTE* pStealPig = p;
		Write16(pStealPig, 0); //len


		Write8(pStealPig, 0); //Status
		Write8(pStealPig, 0); //PigPos
		Write32(pStealPig, 0); //ItemID


		len = pStealPig - p;
		Set16(p, (WORD)len);
		p += len;
	}


	len = p - buf;
	SendToClient(Client, 852, buf, len, ChatRoom->ID, FE_OUTDOORSVRD, Client->ConnID, Notify);
}

void NotifyNPCInfo(ClientNode* Client)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, 4); //NPCCfgVer
	Write8(p, 0); //NPCNum


	len = p - buf;
	SendToClient(Client, 863, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void NotifyGuildRoomMoreInfo(ClientNode* Client)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write32(p, 0); //GuildID
	Write8(p, 0); //GuildRoomID

	BYTE* pHeroCount = p;
	Write16(p, 0); //HeroCount
	ChatRoomNode* ChatRoom = GetChatRoom(Client->ChatRoomID);
	if (!ChatRoom)
	{
		return;
	}
	short HeroCount = 0;
	for (char i = 0; i < 99; i++)
	{
		if (ChatRoom->Player[i] && ChatRoom->Player[i] != Client)
		{ //HeroInfo
			HeroCount++;

			BYTE* pHeroInfo = p;
			Write16(pHeroInfo, 0); //len

			{ //HeroMoveInfo
				BYTE* pHeroMoveInfo = pHeroInfo;
				Write16(pHeroMoveInfo, 0); //len

				Write32(pHeroMoveInfo, ChatRoom->Player[i]->Uin); //Uin
				Write8(pHeroMoveInfo, 0); //WalkStatue

				Write32(pHeroMoveInfo, ChatRoom->x); //Posion[3]
				Write32(pHeroMoveInfo, ChatRoom->y);
				Write32(pHeroMoveInfo, ChatRoom->z);

				Write32(pHeroMoveInfo, 1000); //Ori[9]
				Write32(pHeroMoveInfo, 0);
				Write32(pHeroMoveInfo, 0);
				Write32(pHeroMoveInfo, 0);
				Write32(pHeroMoveInfo, 1000);
				Write32(pHeroMoveInfo, 0);
				Write32(pHeroMoveInfo, 0);
				Write32(pHeroMoveInfo, 0);
				Write32(pHeroMoveInfo, 1000);

				Write32(pHeroMoveInfo, 0); //AttachUin
				Write32(pHeroMoveInfo, 0); //WalkExtState

				len = pHeroMoveInfo - pHeroInfo;
				Set16(pHeroInfo, (WORD)len);
				pHeroInfo += len;
			}

			Write32(pHeroInfo, 0); //NextPoint
			Write16(pHeroInfo, 0); //PathLen
			//m_astHeroInfo[].m_aiPath[]

			WriteVisbleInfo(pHeroInfo, ChatRoom->Player[i]->Uin, i, ChatRoom->Player[i]->ConnID); //HeroVisbleInfo
			
			Write16(pHeroInfo, 0); //HeroEnterDelayTime


			len = pHeroInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}
	Set16(pHeroCount, HeroCount);


	Write16(p, 0); //RelationHeroCount
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

	Write16(p, 0); //HammerCount
	/*
m_astEquipedHammer[].m_iEquipedHammerID
	*/

	Write16(p, 0); //StealPigCount
	/*
m_astStealPig[].m_cStatus
m_astStealPig[].m_cPigPos
m_astStealPig[].m_iItemID
	*/

	Write8(p, 0); //HaveNext

	Write8(p, 0); //HaveAppellation
	/*
m_astCurAppellation[].m_u8Type
m_astCurAppellation[].m_u8Level
m_astCurAppellation[].m_u8Status
m_astCurAppellation[].m_u8Difficulty
m_astCurAppellation[].m_i32MapId
m_astCurAppellation[].m_u32Value
	*/


	len = p - buf;
	SendToClient(Client, 11019, buf, len, Client->ChatRoomID, FE_OUTDOORSVRD, Client->ConnID, Notify);
}

void NotifyChairInfo(ClientNode* Client)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	for (size_t i = 0; i < MAX_CHAIR_NUM; i++)
	{ //ChairInfo
		BYTE* pChairInfo = p;
		Write16(pChairInfo, 0); //len

		for (size_t i = 0; i < MAX_NUM_IN_EACHCHAIR; i++)
		{ //UinInChair
			Write32(pChairInfo, 0);
		}

		len = pChairInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 864, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}


void RequestChangeChairState(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);


	BYTE ChangeState = Read8(Body);
	int ChairIndx = Read32(Body);
	BYTE SeatIndx = Read8(Body);

	ResponseChangeChairState(Client, ChangeState, ChairIndx, SeatIndx);
}
void ResponseChangeChairState(ClientNode* Client, BYTE ChangeState, int ChairIndx, BYTE SeatIndx)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;


	Write32(p, Client->Uin); //Uin
	Write16(p, 0); //Result

	Write8(p, ChangeState);
	Write32(p, ChairIndx);
	Write8(p, SeatIndx);


	len = p - buf;
	SendToClient(Client, 360, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void LeaveOutdoor(ClientNode* Client)
{
	ChatRoomNode* ChatRoom = GetChatRoom(Client->ChatRoomID);
	if (!ChatRoom)
	{
		return;
	}
	for (char i = 0; i < 99; i++)
	{
		if (ChatRoom->Player[i] == Client)
		{
			ResponseLeaveOutdoor(Client);
			ChatRoom->Player[i] = NULL;
			Client->ChatRoomID = -1;
			ChatRoom->CurrPlayerNum--;

			//通知其它玩家
			for (char i2 = 0; i2 < 99; i2++)
			{
				if (ChatRoom->Player[i2])
				{
					NotifyLeaveOutdoor(ChatRoom->Player[i2], Client->Uin);
				}
			}

			break;
		}
	}
}
void RequestLeaveOutdoor(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	LeaveOutdoor(Client);
}
void ResponseLeaveOutdoor(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write8(p, 0); //ReasonLen

	len = p - buf;
	SendToClient(Client, 353, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyLeaveOutdoor(ClientNode* Client, UINT Uin)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Uin); //Uin
	Write32(p, 0); //Time

	len = p - buf;
	SendToClient(Client, 853, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}


void NotifyHeroMove(ClientNode* Client, MoveInfo* HeroMoveInfo);
void RequestHeroMove(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	MoveInfo HeroMoveInfo;
	{ //HeroMoveInfo
		BYTE* pHeroMoveInfo = p;
		len = Read16(pHeroMoveInfo);

		HeroMoveInfo.Uin = Read32(pHeroMoveInfo);
		HeroMoveInfo.WalkStatue = Read8(pHeroMoveInfo);
		for (size_t i = 0; i < 3; i++)
		{
			HeroMoveInfo.Posion[i] = Read32(pHeroMoveInfo);
		}
		for (size_t i = 0; i < 9; i++)
		{
			HeroMoveInfo.Ori[i] = Read32(pHeroMoveInfo);
		}
		HeroMoveInfo.AttachUin = Read32(pHeroMoveInfo);
		HeroMoveInfo.WalkExtState = Read32(pHeroMoveInfo);

		p += len;
	}
	int NextPoint = Read32(p);
	short PathLen = Read16(p);
	//int[] Path[] = Read32(p);
	int CurCellID = Read32(p);


	ChatRoomNode* ChatRoom = GetChatRoom(Client->ChatRoomID);
	if (!ChatRoom)
	{
		return;
	}
	for (char i = 0; i < 99; i++)
	{
		if (ChatRoom->Player[i] && ChatRoom->Player[i] != Client)
		{
			NotifyHeroMove(ChatRoom->Player[i], &HeroMoveInfo);
		}
	}
}
void NotifyHeroMove(ClientNode* Client, MoveInfo* HeroMoveInfo)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	{ //HeroMoveInfo
		BYTE* pHeroMoveInfo = p;
		Write16(pHeroMoveInfo, 0); //len

		Write32(pHeroMoveInfo, HeroMoveInfo->Uin);
		Write8(pHeroMoveInfo, HeroMoveInfo->WalkStatue);
		for (size_t i = 0; i < 3; i++)
		{
			Write32(pHeroMoveInfo, HeroMoveInfo->Posion[i]);
		}
		for (size_t i = 0; i < 9; i++)
		{
			Write32(pHeroMoveInfo, HeroMoveInfo->Ori[i]);
		}
		Write32(pHeroMoveInfo, HeroMoveInfo->AttachUin);
		Write32(pHeroMoveInfo, HeroMoveInfo->WalkExtState);

		len = pHeroMoveInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); //NextPoint
	Write16(p, 0); //PathLen
	//Write32(p, 0); //Path[]
	Write32(p, 0); //Time

	len = p - buf;
	SendToClient(Client, 851, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

