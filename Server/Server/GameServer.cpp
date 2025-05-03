#include "Message.h"

#include <stdio.h>
#include <time.h>
#include <map>
#include "yaml-cpp/yaml.h"

#include "Database.h"
#include "Item.h"
#include "sqlite/sqlite3.h"
#include "Room.h"
#include <iostream>
using namespace std;
extern sqlite3* PlayerDB;

const char* UITitleDispInfo_ADText = "本版本已开源.";
//const char* UITitleDispInfo_ADText = "test";
void NotifyRaceOver(ClientNode* Client);


struct PlayerRaceInfo
{
	UINT Uin;
	UINT Round;
	UINT FinishTime;
};

typedef struct _HideTaskResult
{
	WORD size;
	long uin;
	int TaskId;
	char Result;

}HIDETASKRESULT, * PHIDETASKRESULT;

void WriteKartPhysParam(BYTE*& p, UINT Uin, UINT KartID, UINT PetID, UINT PetLevel)
{



	WORD len;
	BYTE* pKartPhysParam = p;
	Write16(pKartPhysParam, 0); //len


	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

	int RefitCout = 0;
	short MaxFlags = 0;
	short WWeight = 0;
	short SpeedWeight = 0;
	short JetWeight = 0;
	short SJetWeight = 0;
	short AccuWeight = 0;
	int ShapeRefitCount = 0;
	int KartHeadRefitItemID = 0;
	int KartTailRefitItemID = 0;
	int KartFlankRefitItemID = 0;
	int KartTireRefitItemID = 0;

	int CrashCompensatePower = 0;
	if (Uin > 10000) //Not NPC
	{
		sql = "SELECT RefitCout,MaxFlags,WWeight,SpeedWeight,JetWeight,SJetWeight,AccuWeight,ShapeRefitCount,KartHeadRefitItemID,KartTailRefitItemID,KartFlankRefitItemID,KartTireRefitItemID  FROM KartRefit WHERE Uin=? AND KartID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, Uin);
			sqlite3_bind_int(stmt, 2, KartID);
			if (sqlite3_step(stmt) == SQLITE_ROW)
			{
				RefitCout = sqlite3_column_int(stmt, 0);
				MaxFlags = sqlite3_column_int(stmt, 1);
				WWeight = sqlite3_column_int(stmt, 2);
				SpeedWeight = sqlite3_column_int(stmt, 3);
				JetWeight = sqlite3_column_int(stmt, 4);
				SJetWeight = sqlite3_column_int(stmt, 5);
				AccuWeight = sqlite3_column_int(stmt, 6);

				ShapeRefitCount = sqlite3_column_int(stmt, 7);
				KartHeadRefitItemID = sqlite3_column_int(stmt, 8);
				KartTailRefitItemID = sqlite3_column_int(stmt, 9);
				KartFlankRefitItemID = sqlite3_column_int(stmt, 10);
				KartTireRefitItemID = sqlite3_column_int(stmt, 11);
			}
		}
		else
		{
			printf("%s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
		stmt = NULL;

		if (PetID > 0) {
			YAML::Node PetSkills = GetPetConfig(PetID);

			for (size_t i = 0; i < PetSkills.size(); i++)
			{
				YAML::Node PetSkill = PetSkills[i];

				switch (PetSkill["SkillID"].as<UINT>())
				{
				case 2:
					CrashCompensatePower = PetSkill["Value"].as<float>() * PetLevel;
					break;
				}
			}
		}
	}
	

	char FilePath[MAX_PATH];
	sprintf_s(FilePath, ".\\Kart\\%d.yml", KartID);
	YAML::Node config = YAML::LoadFile(FilePath);

	Write32(pKartPhysParam, config["KartID"].as<UINT>());

	YAML::Node PhysInfo = config["PhysInfo"];
	Write8(pKartPhysParam, (BYTE)PhysInfo.size()); //PhysInfoCount
	for (size_t i = 0; i < PhysInfo.size(); i++)
	{
		{ //PhysInfo
			BYTE* pPhysInfo = pKartPhysParam;
			Write16(pPhysInfo, 0); //len

			int PID = PhysInfo[i]["PID"].as<int>();
			int AffectID = PhysInfo[i]["AffectID"].as<int>();
			int Param = PhysInfo[i]["Param"].as<int>();
			int LifeTime = PhysInfo[i]["LifeTime"].as<int>();
			//TODO: 应该限制最大值, 但懒得做限制, 因为不同赛车等级是不同的, 留给后人去折腾吧!
			switch (PID)
			{
			case 1: //燃料
				Param += 10 * JetWeight;
				LifeTime += 8 * JetWeight;
				break;
			case 2001: //点火
				Param += 10 * SJetWeight;
				LifeTime += 8 * SJetWeight;
				break;
			case 99999: //进气
				Param += 300 * AccuWeight;
				break;
			default:
				break;
			}

			Write32(pPhysInfo, PID);
			Write32(pPhysInfo, AffectID);
			Write32(pPhysInfo, Param);
			Write32(pPhysInfo, LifeTime);

			len = pPhysInfo - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
	}

	YAML::Node CompensateParam = config["CompensateParam"];
	Write8(pKartPhysParam, (BYTE)CompensateParam.size()); //CompensateParamCount
	for (size_t i = 0; i < CompensateParam.size(); i++)
	{
		{ //CompensateParam
			BYTE* pCompensateParam = pKartPhysParam;
			Write16(pCompensateParam, 0); //len

			Write32(pCompensateParam, CompensateParam[i]["Percent"].as<int>());
			Write32(pCompensateParam, CompensateParam[i]["Value"].as<int>());

			len = pCompensateParam - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
	}

	{ //CollisionBoxParam
		YAML::Node CollisionBoxParam = config["CollisionBoxParam"];
		BYTE* pCollisionBoxParam = pKartPhysParam;
		Write16(pCollisionBoxParam, 0); //len

		Write32(pCollisionBoxParam, CollisionBoxParam["BoxLength"].as<int>());
		Write32(pCollisionBoxParam, CollisionBoxParam["BoxWidth"].as<int>());
		Write32(pCollisionBoxParam, CollisionBoxParam["BoxHeight"].as<int>());
		Write32(pCollisionBoxParam, CollisionBoxParam["AdjustOffset"].as<int>());
		Write32(pCollisionBoxParam, CollisionBoxParam["AdjustPickGap"].as<int>());
		Write32(pCollisionBoxParam, CollisionBoxParam["AdjustRemoteOffset"].as<int>());
		Write32(pCollisionBoxParam, CollisionBoxParam["AdjustBevelOffset"].as<int>());
		Write32(pCollisionBoxParam, CollisionBoxParam["ControllerOffset"].as<int>());

		len = pCollisionBoxParam - pKartPhysParam;
		Set16(pKartPhysParam, (WORD)len);
		pKartPhysParam += len;
	}

	{ //CarToCarCollisionParam
		YAML::Node CarToCarCollisionParam = config["CarToCarCollisionParam"];
		BYTE* pCarToCarCollisionParam = pKartPhysParam;
		Write16(pCarToCarCollisionParam, 0); //len

		Write32(pCarToCarCollisionParam, CarToCarCollisionParam["CollisionRadius"].as<int>());
		Write32(pCarToCarCollisionParam, CarToCarCollisionParam["CollisionLength"].as<int>());
		Write32(pCarToCarCollisionParam, CarToCarCollisionParam["CollisionWidth"].as<int>());
		Write32(pCarToCarCollisionParam, CarToCarCollisionParam["CollisionHeight"].as<int>());
		Write32(pCarToCarCollisionParam, CarToCarCollisionParam["CollisionLostCtrlTime"].as<int>());

		len = pCarToCarCollisionParam - pKartPhysParam;
		Set16(pKartPhysParam, (WORD)len);
		pKartPhysParam += len;
	}

	Write32(pKartPhysParam, config["BaseTurnRate"].as<int>());
	Write32(pKartPhysParam, config["VolatileTurnRate"].as<int>());
	Write32(pKartPhysParam, config["MaxTurnSpeed"].as<int>());
	Write32(pKartPhysParam, config["MinTurnSpeed"].as<int>());
	Write32(pKartPhysParam, config["MaxAccuTime"].as<int>());
	Write32(pKartPhysParam, config["BaseAccuRate"].as<int>());
	Write32(pKartPhysParam, config["MaxAffectSpeed"].as<int>());
	Write32(pKartPhysParam, config["Gravity"].as<int>() + (10 * WWeight)); //TODO: 悬挂; 未限制最大值!
	Write32(pKartPhysParam, config["AdditionalLocalZSpeed"].as<int>());
	Write32(pKartPhysParam, config["StartVec"].as<int>());
	Write32(pKartPhysParam, config["EndVecFist"].as<int>());
	Write32(pKartPhysParam, config["EndVecSecon"].as<int>());
	Write32(pKartPhysParam, config["DirKeyForce"].as<int>());
	Write32(pKartPhysParam, config["DirKeyTwist"].as<int>());
	Write32(pKartPhysParam, config["BannerTwist"].as<int>());
	Write32(pKartPhysParam, config["BannerKeyTwist"].as<int>());
	Write32(pKartPhysParam, config["BannerVecForce"].as<int>());
	Write32(pKartPhysParam, config["BannerHeadForce"].as<int>());
	Write32(pKartPhysParam, config["SlidFricForce"].as<int>());
	Write32(pKartPhysParam, config["RollFricForce"].as<int>());
	Write32(pKartPhysParam, config["StartWec"].as<int>());
	Write32(pKartPhysParam, config["MaxWec"].as<int>());
	Write32(pKartPhysParam, config["SuaiJianTwist"].as<int>());
	Write32(pKartPhysParam, config["DirUpKeyForce"].as<int>());

	YAML::Node AccelStatus = config["AccelStatus"];
	Write8(pKartPhysParam, (BYTE)AccelStatus.size()); //AccelStatusCount
	for (size_t i = 0; i < AccelStatus.size(); i++)
	{
		{ //m_astKartPhysParam[].m_aiAccelStatus[]
			Write32(pKartPhysParam, AccelStatus[i].as<int>());
		}
	}

	YAML::Node ForwardAccel = config["ForwardAccel"];
	Write8(pKartPhysParam, (BYTE)ForwardAccel.size()); //ForwardAccelNum
	for (size_t i = 0; i < ForwardAccel.size(); i++)
	{
		{ //ForwardAccel
			BYTE* pForwardAccel = pKartPhysParam;
			Write16(pForwardAccel, 0); //len

			int Key = ForwardAccel[i]["Key"].as<int>();
			int Value = ForwardAccel[i]["Value"].as<int>();
			if (i >= 7 && i <= 9)
			{
				//TODO: 引擎; 未限制最大值!
				Value += 35 * SpeedWeight;
			}
			Write32(pForwardAccel, Key);
			Write32(pForwardAccel, Value);

			len = pForwardAccel - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
	}

	YAML::Node ForwardDecel = config["ForwardDecel"];
	Write8(pKartPhysParam, (BYTE)ForwardDecel.size()); //ForwardDecelNum
	for (size_t i = 0; i < ForwardDecel.size(); i++)
	{
		{ //ForwardDecel
			BYTE* pForwardDecel = pKartPhysParam;
			Write16(pForwardDecel, 0); //len

			Write32(pForwardDecel, ForwardDecel[i]["Key"].as<int>());
			Write32(pForwardDecel, ForwardDecel[i]["Value"].as<int>());

			len = pForwardDecel - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
	}

	YAML::Node BackwardAccel = config["ForwardDecel"];
	Write8(pKartPhysParam, (BYTE)BackwardAccel.size()); //BackwardAccelNum
	for (size_t i = 0; i < BackwardAccel.size(); i++)
	{
		{ //BackwardAccel
			BYTE* pBackwardAccel = pKartPhysParam;
			Write16(pBackwardAccel, 0); //len

			Write32(pBackwardAccel, BackwardAccel[i]["Key"].as<int>());
			Write32(pBackwardAccel, BackwardAccel[i]["Value"].as<int>());

			len = pBackwardAccel - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
	}

	YAML::Node BackwardDecel = config["BackwardDecel"];
	Write8(pKartPhysParam, (BYTE)BackwardDecel.size()); //BackwardDecelNum
	for (size_t i = 0; i < BackwardDecel.size(); i++)
	{
		{ //BackwardDecel
			BYTE* pBackwardDecel = pKartPhysParam;
			Write16(pBackwardDecel, 0); //len

			Write32(pBackwardDecel, BackwardDecel[i]["Key"].as<int>());
			Write32(pBackwardDecel, BackwardDecel[i]["Value"].as<int>());

			len = pBackwardDecel - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
	}

	Write32(pKartPhysParam, 0); //CrashCompensatePower
	Write32(pKartPhysParam, 0); //DefenseRate
	Write32(pKartPhysParam, Uin); //Uin
	Write32(pKartPhysParam, 0); //SpeedupCardGenRate
	Write32(pKartPhysParam, 0); //ExtraFuncFlag

	Write8(pKartPhysParam, 0); //HasSkatePara
	/*
m_astKartPhysParam[].m_hasSkatePara[].m_iTrackSpeed
m_astKartPhysParam[].m_hasSkatePara[].m_iSkateSpeedCoef_AirBigJet
m_astKartPhysParam[].m_hasSkatePara[].m_iSkateSpeedCoef_Jump
m_astKartPhysParam[].m_hasSkatePara[].m_iSkateSpeedCoef_AirJetPoint
m_astKartPhysParam[].m_hasSkatePara[].m_iSkateSpeedCoef_MaxForce
m_astKartPhysParam[].m_hasSkatePara[].m_iOntrackAccRef
m_astKartPhysParam[].m_hasSkatePara[].m_iTrackAccRef
m_astKartPhysParam[].m_hasSkatePara[].m_iDashSkill
		*/

	Write8(pKartPhysParam, 0); //HasBattleModeSkillInfoList
	/*
m_astKartPhysParam[].m_astBattleModeSkillInfoList[].m_bSkillNum
m_astKartPhysParam[].m_astBattleModeSkillInfoList[].m_astSkillInfo[].m_shSkillId
m_astKartPhysParam[].m_astBattleModeSkillInfoList[].m_astSkillInfo[].m_bSkillLevel
		*/

#ifndef ZingSpeed
	Write32(pKartPhysParam, 0); //EnergyConvert
	Write16(pKartPhysParam, 0); //SuperN2ORate
	Write16(pKartPhysParam, 0); //CollisionLoseRate
	Write8(pKartPhysParam, 0); //NotInterruptDrift

	Write8(pKartPhysParam, 0); //AddWeightNum

	Write8(pKartPhysParam, 0); //WeightNum

	Write8(pKartPhysParam, 0); //HasAntiCollisionCfg
	/*
m_astKartPhysParam[].m_astAntiCollistionCfg[].m_uchNum
m_astKartPhysParam[].m_astAntiCollistionCfg[].m_aiPosition[]
		*/

	Write8(pKartPhysParam, 0); //HasBoatParam
	/*
m_astKartPhysParam[].m_astBoatParam[].m_iDirKeyForceParamC
m_astKartPhysParam[].m_astBoatParam[].m_iDirKeyForceParamD
m_astKartPhysParam[].m_astBoatParam[].m_iDTFLimitAngle
m_astKartPhysParam[].m_astBoatParam[].m_iDTFRatio
m_astKartPhysParam[].m_astBoatParam[].m_iUpKeyVecForce
m_astKartPhysParam[].m_astBoatParam[].m_iUpKeyVecForceParamA
m_astKartPhysParam[].m_astBoatParam[].m_iUpKeyVecForceParamB
m_astKartPhysParam[].m_astBoatParam[].m_iLRKeyVecForce
m_astKartPhysParam[].m_astBoatParam[].m_iLRKeyVecForceParamA
m_astKartPhysParam[].m_astBoatParam[].m_iLRKeyVecForceParamB
m_astKartPhysParam[].m_astBoatParam[].m_iVecForce
m_astKartPhysParam[].m_astBoatParam[].m_iVecForceParamA
m_astKartPhysParam[].m_astBoatParam[].m_iVecForceParamB
m_astKartPhysParam[].m_astBoatParam[].m_iBoatShape[3]
m_astKartPhysParam[].m_astBoatParam[].m_iWecEffect
m_astKartPhysParam[].m_astBoatParam[].m_iBannerKeyTwist
m_astKartPhysParam[].m_astBoatParam[].m_iBannerKeyTwistParamA
m_astKartPhysParam[].m_astBoatParam[].m_iBannerKeyTwistParamB
m_astKartPhysParam[].m_astBoatParam[].m_iSuaiJianTwist
m_astKartPhysParam[].m_astBoatParam[].m_iBoatHeighRatio
m_astKartPhysParam[].m_astBoatParam[].m_iBannerTwist
m_astKartPhysParam[].m_astBoatParam[].m_iBannerTwistParamA
m_astKartPhysParam[].m_astBoatParam[].m_iDirUpKeyForce
m_astKartPhysParam[].m_astBoatParam[].m_iDirUpKeyForceParamA
m_astKartPhysParam[].m_astBoatParam[].m_iDirUpKeyForceParamB
m_astKartPhysParam[].m_astBoatParam[].m_iDirKeyForce
m_astKartPhysParam[].m_astBoatParam[].m_iDirKeyForceParamA
m_astKartPhysParam[].m_astBoatParam[].m_iDirKeyForceParamB
m_astKartPhysParam[].m_astBoatParam[].m_iAddNO2PerSec
m_astKartPhysParam[].m_astBoatParam[].m_iBoatAirValue
m_astKartPhysParam[].m_astBoatParam[].m_ifSlidFricForce
		*/

	Write8(pKartPhysParam, 0); //BoatForwardAccelNum
	/*
	m_astKartPhysParam[].m_astBoatForwardAccel[].m_iKey
	m_astKartPhysParam[].m_astBoatForwardAccel[].m_iValue
	*/

	Write32(pKartPhysParam, 0); //DirUpKeyForceParamA
	Write32(pKartPhysParam, 0); //DirUpKeyForceParamB
#endif

	len = pKartPhysParam - p;
	Set16(p, len);
	p += len;
}


void WINAPI OnBegin(RoomNode* Room)
{
	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient)
		{	
			NotifyRaceBegin(RoomClient);
		}
	}
}

void WINAPI OnCountDown3(RoomNode* Room)
{
	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient)
		{
			Room->Status = 1;
			NotifyGameOver(RoomClient, 0, 0, NULL);
		}
	}
}   
void WINAPI OnCountDown2(RoomNode* Room)
{
	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient)
		{
			Room->Status = 1;
			NotifyRaceOver(RoomClient);
		}
	}
	CreateRoomTimer(Room, 20000, OnCountDown3);
}
void WINAPI OnCountDown(RoomNode* Room)
{
	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient)
		{
			
			NotifyRaceShow(RoomClient);//这个
		}
	}
	CreateRoomTimer(Room, 5000, OnCountDown2);
}



void NotifyGameBegin(ClientNode* Client, ClientNode* Player[])
{
	ClientNode* Players[6];
	char CurrentPlayers = 0;
	if (Player)
	{
		for (char i = 0; i < 6; i++)
		{
			if (Player[i])
			{
				Players[CurrentPlayers] = Player[i];
				CurrentPlayers++;
			}
		}
	}
	else
	{
		Players[0] = Client;
		CurrentPlayers = 1;
	}

	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, 8000); //NormalLoadTime 正常装载时间
	Write32(p, Client->MapID); //MapID
	//MapHash[]
	memset(p, 0, 32);
	p += 32;

	Write32(p, Client->GameID); //GameID
	Write32(p, (DWORD)time(nullptr)); //Seed

	Write8(p, CurrentPlayers);
	//m_aunRaceTrackOrders[MAX_MUCH_SEATNUMINROOM]
	int* aRaceTrackOrders = (int*)p;
	for (char i = 0; i < MAX_MUCH_SEATNUMINROOM; i++)
	{
		Write32(p, 0);
	}
	//起跑线并排的玩家
	int index[] = { 2,3,1,4,0,5 };
	for (char i = 0; i < CurrentPlayers; i++)
	{
		Set32((BYTE*)&aRaceTrackOrders[index[i]], Players[i]->Uin);
	}


	Write16(p, Client->TotalRound); //TotalMapRound
	Write32(p, 0); //PropUpdateInterval
	Write32(p, PlayerDB_GetMapRecord(Client->Uin, Client->MapID)); //Record


	Write8(p, 0); //NianShouTypeNum
	/*
m_astNianShouAwardInfo[].m_cAwardInGameType
m_astNianShouAwardInfo[].m_iActIDForClient
m_astNianShouAwardInfo[].m_cPointArrayIdx
m_astNianShouAwardInfo[].m_cCurrAwardNum
m_astNianShouAwardInfo[].m_acAwardPos[]
	*/

	Write8(p, CurrentPlayers); //SyncCarNum
	for (char i = 0; i < CurrentPlayers; i++)
	{ //SyncCar
		BYTE* pSyncCar = p;
		Write16(pSyncCar, 0); //len

		Write32(pSyncCar, Players[i]->Uin); //PlayerUin
		Write32(pSyncCar, Players[i]->KartID); //CurCarID
#ifndef ZingSpeed
		Write8(pSyncCar, 0); //HasStoneInfo
		/*
m_astSyncCar[].m_stSTKartStoneGrooveInfo[].m_iKartID
m_astSyncCar[].m_stSTKartStoneGrooveInfo[].m_iStoneGrooveNum
m_astSyncCar[].m_stSTKartStoneGrooveInfo[].m_astStoneGrooveInfo[].m_iStoneUseOccaType
m_astSyncCar[].m_stSTKartStoneGrooveInfo[].m_astStoneGrooveInfo[].m_iSkillStoneID
		*/
#endif

		len = pSyncCar - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); //ReportDataFlag

	Write8(p, 0); //CheckDataNum
	/*
m_astMapCheckDiv[].m_shBeginIdx
m_astMapCheckDiv[].m_shEndIdx
	*/


	Write8(p, 0); //P2PMode
	Write8(p, 0); //TcpFrequence

	Write8(p, 50); //MultiInfoLen
	memset(p, 0, 50);
	p += 50;

	Write8(p, 0); //FeedBackEnabled

	Write8(p, 0); //SpeedSectionNum
	/*
m_au16SpeedSectionLowerBound[]
	*/

	Write8(p, 0); //NormalSpeedSectionNum
	/*
m_au16NormalSpeedSectionLowerBound[]
	*/

	Write8(p, 0); //MemCheckInfoNum
	/*
m_astMemCheckInfo[].m_u8MemIdxNum
m_astMemCheckInfo[].m_au32MemOffset[]
m_astMemCheckInfo[].m_u8DataType
m_astMemCheckInfo[].m_uPara
	*/

	Write8(p, 4); //ExtraInfoInterval
	Write16(p, -1); //OffsetThreshold
	Write32(p, 200); //SpeedRatioThreshold1
	Write32(p, 200); //SpeedRatioThreshold2
	Write32(p, 0); //HideTaskId
	Write8(p, 0); //HideTaskType
	Write32(p, 0); //HideTaskParam1

	Write8(p, 0); //ForceReportCPNum
	//m_au16ForceReportCP[]

	Write8(p, 0); //CliReserveFlag

	char EnableAntiDriftCheat = true; //true; false; //反卡漂
	Write8(p, EnableAntiDriftCheat); //EnableAntiDriftCheat
	if (EnableAntiDriftCheat)
	{ //AntiDriftCheatPara
		BYTE* pAntiDriftCheatPara = p;
		Write16(pAntiDriftCheatPara, 0); //len

		Write32(pAntiDriftCheatPara, 0); //MaxDriftHistoryTime
		Write32(pAntiDriftCheatPara, 0); //MinTimeInterval
		Write32(pAntiDriftCheatPara, 0); //MaxTimeInterval
		Write32(pAntiDriftCheatPara, 220); //NormalThreshold
		Write32(pAntiDriftCheatPara, 15); //JetThreshold
		Write32(pAntiDriftCheatPara, 0); //JetInterval
		Write32(pAntiDriftCheatPara, 0); //OneSideSlidingInterval

		len = pAntiDriftCheatPara - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); //HasCrashModePara
	/*
m_astCrashModePara[].m_u8SponsorSkillNum
m_astCrashModePara[].m_astSponsorSkill[].m_u8SkillID
m_astCrashModePara[].m_astSponsorSkill[].m_iPara1
m_astCrashModePara[].m_u8InitEggNum
m_astCrashModePara[].m_u8EggRefreshInterval
m_astCrashModePara[].m_u8EggRefreshNum
m_astCrashModePara[].m_u8InitN2ONum
m_astCrashModePara[].m_u8EggSourceIncNum
m_astCrashModePara[].m_u32RandSeed
m_astCrashModePara[].m_u8PropPointRatio
m_astCrashModePara[].m_u8EggPropRatio
m_astCrashModePara[].m_u8AddN2ONumOnEggLost
	*/

	Write32(p, 0); //FizzStarTaskId
	Write8(p, 0); //FizzStarTaskType
	Write32(p, 0); //FizzStarTaskParam1
	Write32(p, 0); //LDMRecordID
	Write32(p, 0); //GameSeq

#ifndef ZingSpeed
	Write8(p, 1); //PlayerNums
	{ //GameBeginPlayerInfo
		BYTE* pGameBeginPlayerInfo = p;
		Write16(pGameBeginPlayerInfo, 0); //len

		Write32(pGameBeginPlayerInfo, Client->Uin); //Uin
		Write32(pGameBeginPlayerInfo, 0); //ChumCircleID

		Write8(pGameBeginPlayerInfo, 0); //SkillNums
		/*
m_astGameBeginPlayerInfo[].m_astChumCircleSkillInfo[].m_uiSkillID
m_astGameBeginPlayerInfo[].m_astChumCircleSkillInfo[].m_uiSkillLv
		*/

		Write32(pGameBeginPlayerInfo, 0); //WorldEscapeTaskID

		{ //ProfessionLicenseInfo
			BYTE* pProfessionLicenseInfo = pGameBeginPlayerInfo;
			Write16(pProfessionLicenseInfo, 0); //len


			Write8(pProfessionLicenseInfo, 0); //BigLevel
			Write8(pProfessionLicenseInfo, 0); //SmallLevel


			len = pProfessionLicenseInfo - pGameBeginPlayerInfo;
			Set16(pGameBeginPlayerInfo, (WORD)len);
			pGameBeginPlayerInfo += len;
		}

		Write8(pGameBeginPlayerInfo, 0); //ParaNum
		/*
m_astGameBeginPlayerInfo[].m_stEffectGameBuffInfo[].m_ushBuffID
m_astGameBeginPlayerInfo[].m_stEffectGameBuffInfo[].m_uchFeatureID
m_astGameBeginPlayerInfo[].m_stEffectGameBuffInfo[].m_uchBuffLevel
m_astGameBeginPlayerInfo[].m_stEffectGameBuffInfo[].m_uchIsIcon
		*/

		Write32(pGameBeginPlayerInfo, 0); //StarsFightingStar
		Write8(pGameBeginPlayerInfo, 0); //ParaNum
		/*
m_astGameBeginPlayerInfo[].m_iParaList[]
		*/

		len = pGameBeginPlayerInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}


	Write8(p, 0); //PlayeCheerNums
	/*
m_astCheers[]
	*/

	Write8(p, 0); //MapNum
	/*
m_astShuttleMapList[].m_iMapID
m_astShuttleMapList[].m_ushBeginIdx
m_astShuttleMapList[].m_ushEndIdx
m_astShuttleMapList[].m_uchReverse
m_astShuttleMapList[].m_ushBeginCheckPointIdx
	*/


	Write32(p, 0); //SpecialMapId
	Write8(p, 0); //NPCNum
	/*
m_astNPCInfo[].m_iNpcID
m_astNPCInfo[].m_szNPCName[17]
m_astNPCInfo[].m_szGuildName[17]
m_astNPCInfo[].m_stNpcItemInfo.m_iRoleID
m_astNPCInfo[].m_stNpcItemInfo.m_iHairID
m_astNPCInfo[].m_stNpcItemInfo.m_iFaceID
m_astNPCInfo[].m_stNpcItemInfo.m_iGlassID
m_astNPCInfo[].m_stNpcItemInfo.m_iCoatID
m_astNPCInfo[].m_stNpcItemInfo.m_iGloveID
m_astNPCInfo[].m_stNpcItemInfo.m_iTrousersID
m_astNPCInfo[].m_stNpcItemInfo.m_iPersonarID
m_astNPCInfo[].m_stNpcItemInfo.m_iTattooID
m_astNPCInfo[].m_stNpcItemInfo.m_iWingID
m_astNPCInfo[].m_stNpcItemInfo.m_iDecoratorID
m_astNPCInfo[].m_stNpcItemInfo.m_iKartID
m_astNPCInfo[].m_stNpcItemInfo.m_iColorID
m_astNPCInfo[].m_stNpcItemInfo.m_iTrackID
m_astNPCInfo[].m_stNpcItemInfo.m_iEffectID
m_astNPCInfo[].m_stNpcItemInfo.m_iBrandID
m_astNPCInfo[].m_stNpcItemInfo.m_iTailID
m_astNPCInfo[].m_iGpSkillPeriod
m_astNPCInfo[].m_iBaseValue
m_astNPCInfo[].m_stDefaultSpeedPara.m_iMaxSpeed
m_astNPCInfo[].m_stDefaultSpeedPara.m_iMinSpeed
m_astNPCInfo[].m_stDefaultSpeedPara.m_iMaxPlusForce
m_astNPCInfo[].m_stDefaultSpeedPara.m_iMinPlusForce
m_astNPCInfo[].m_stDefaultSpeedPara.m_iMaxTurnSpeed
m_astNPCInfo[].m_stDefaultSpeedPara.m_iMinTurnSpeed
m_astNPCInfo[].m_stDefaultSpeedPara.m_iMaxDist
m_astNPCInfo[].m_stDefaultSpeedPara.m_iMinDist
m_astNPCInfo[].m_cNormalSpeedNum
m_astNPCInfo[].m_astNormalSpeedPara[].m_iMaxSpeed
m_astNPCInfo[].m_astNormalSpeedPara[].m_iMinSpeed
m_astNPCInfo[].m_astNormalSpeedPara[].m_iMaxPlusForce
m_astNPCInfo[].m_astNormalSpeedPara[].m_iMinPlusForce
m_astNPCInfo[].m_astNormalSpeedPara[].m_iMaxTurnSpeed
m_astNPCInfo[].m_astNormalSpeedPara[].m_iMinTurnSpeed
m_astNPCInfo[].m_astNormalSpeedPara[].m_iMaxDist
m_astNPCInfo[].m_astNormalSpeedPara[].m_iMinDist
m_astNPCInfo[].m_cGpNum
m_astNPCInfo[].m_astGpPara[].m_iGpID
m_astNPCInfo[].m_astGpPara[].m_iProbability
m_astNPCInfo[].m_astGpPara[].m_iMaxDist
m_astNPCInfo[].m_astGpPara[].m_iMinDist
m_astNPCInfo[].m_sNPCAILevel
m_astNPCInfo[].m_sTeamID
	*/

	Write8(p, 0); //MapNum
	/*
m_astMapInfo[].m_iMapID
m_astMapInfo[].m_bReportDataFlag
m_astMapInfo[].m_byCheckDataNum
m_astMapInfo[].m_astMapCheckDiv[].m_shBeginIdx
m_astMapInfo[].m_astMapCheckDiv[].m_shEndIdx
m_astMapInfo[].m_sTotalMapRound
	*/

	for (size_t i = 0; i < MAXNPCNUMINROOM; i++)
	{
		//m_aunNPCTrackOrders[]
		Write32(p, 0);
	}

	Write8(p, 0); //PlayerNum
	/*
m_astAvatarChangeInfo[].m_dwPlayerUin
m_astAvatarChangeInfo[].m_iHairID
m_astAvatarChangeInfo[].m_iCoatID
m_astAvatarChangeInfo[].m_iTrousersID
m_astAvatarChangeInfo[].m_iSuitID
m_astAvatarChangeInfo[].m_iKartID
m_astAvatarChangeInfo[].m_uchHasItemInfo
m_astAvatarChangeInfo[].m_stItemInfo[].m_iRoleID
m_astAvatarChangeInfo[].m_stItemInfo[].m_iHairID
m_astAvatarChangeInfo[].m_stItemInfo[].m_iFaceID
m_astAvatarChangeInfo[].m_stItemInfo[].m_iGlassID
m_astAvatarChangeInfo[].m_stItemInfo[].m_iCoatID
m_astAvatarChangeInfo[].m_stItemInfo[].m_iGloveID
m_astAvatarChangeInfo[].m_stItemInfo[].m_iTrousersID
m_astAvatarChangeInfo[].m_stItemInfo[].m_iPersonarID
m_astAvatarChangeInfo[].m_stItemInfo[].m_iTattooID
m_astAvatarChangeInfo[].m_stItemInfo[].m_iWingID
m_astAvatarChangeInfo[].m_stItemInfo[].m_iDecoratorID
m_astAvatarChangeInfo[].m_stItemInfo[].m_iKartID
m_astAvatarChangeInfo[].m_stItemInfo[].m_iColorID
m_astAvatarChangeInfo[].m_stItemInfo[].m_iTrackID
m_astAvatarChangeInfo[].m_stItemInfo[].m_iEffectID
m_astAvatarChangeInfo[].m_stItemInfo[].m_iBrandID
m_astAvatarChangeInfo[].m_stItemInfo[].m_iTailID
	*/


	Write8(p, 0); //HasTowerInfo
	/*
m_stTowerLevelTask[].m_uiTaskID
m_stTowerLevelTask[].m_cLevelDifficulty
m_stTowerLevelTask[].m_uiSpecialFlag
MAX_TOWER_JUDGE_STAR_CONDITION_NUM_PERMAP
m_stTowerLevelTask[].m_astJudgeStarCondition[].m_uiType
m_stTowerLevelTask[].m_astJudgeStarCondition[].m_uiConditionNum
m_stTowerLevelTask[].m_uiTrackElementNum
m_stTowerLevelTask[].m_astTrackElement[].m_cType
m_stTowerLevelTask[].m_astTrackElement[].m_cCheckpointNum
m_stTowerLevelTask[].m_astTrackElement[].m_acCheckpoint[]
m_stTowerLevelTask[].m_astTrackElement[].m_uiCarID
m_stTowerLevelTask[].m_astTrackElement[].m_usReverseTimeLimit
m_stTowerLevelTask[].m_astTrackElement[].m_cCoinFrequence
m_stTowerLevelTask[].m_astTrackElement[].m_cCoinNumLimit
m_stTowerLevelTask[].m_astTrackElement[].m_cShadowID
	*/

	Write8(p, 0); //HasWeRelayGameBeginInfo
	/*
m_stWeRelayGameBeginInfo[].m_uiPlayerNum
m_stWeRelayGameBeginInfo[].m_astWeRelayGameBeginPlayerInfo[].m_uiUin
m_stWeRelayGameBeginInfo[].m_astWeRelayGameBeginPlayerInfo[].m_uiCarID
m_stWeRelayGameBeginInfo[].m_astWeRelayGameBeginPlayerInfo[].m_uiSuperN2O
m_stWeRelayGameBeginInfo[].m_astWeRelayGameBeginPlayerInfo[].m_uiScore
m_stWeRelayGameBeginInfo[].m_uiMapID
m_stWeRelayGameBeginInfo[].m_sTotalMapRound
m_stWeRelayGameBeginInfo[].m_bReportDataFlag
m_stWeRelayGameBeginInfo[].m_byCheckDataNum
m_stWeRelayGameBeginInfo[].m_astMapCheckDiv[].m_shBeginIdx
m_stWeRelayGameBeginInfo[].m_astMapCheckDiv[].m_shEndIdx
m_stWeRelayGameBeginInfo[].m_uiSuperN2OCoolDownTime
m_stWeRelayGameBeginInfo[].m_uiDis2No1ToFinishRace
	*/

	Write8(p, 0); //ChangeCar
	Write32(p, 12345678); //GameSeqIDHigh
	Write32(p, 87654321); //GameSeqIDLow
	Write32(p, 0); //KubiBigCoinReplaceItem
	Write32(p, 0); //TimerChallengeJumpLevel
	Write8(p, 0); //ShadowRunDelay
	Write16(p, 0); //ShadowCatchUpContinuesTime

	Write8(p, 0); //ArrestPlayerNums
	/*
m_astArrestRoleInfo[].m_uiUin
m_astArrestRoleInfo[].m_bTeamID
	*/

	Write16(p, 0); //MonitorCheckPointBegin
	Write16(p, 0); //MonitorCheckPointEnd

	Write8(p, 0); //MonitorLapCnt
	Write8(p, 0); //GameType
	Write8(p, 0); //PointID
	Write16(p, 0); //BaseMode
	Write16(p, 0); //SubMode
	Write8(p, 0); //GameType
	Write16(p, 0); //ReportPosSpan

	Write16(p, 0); //PropID
	Write32(p, 0); //PropIndex
	Write16(p, 0); //PropNum


	Write8(p, 0); //BaseGameModeEx
	Write8(p, 0); //ParaNum
	//m_iParaList[]
	Write8(p, 0); //MapCheckpointFileIndex

	Write8(p, 0); //HasTimerChallenge2ndGameBeginInfo
	/*
m_astTimerChallenge2ndGameBeginInfo[].m_stJumpInfo.m_iPlayerNum
m_astTimerChallenge2ndGameBeginInfo[].m_stJumpInfo.m_astPlayerJumpInfo[].m_uiUin
m_astTimerChallenge2ndGameBeginInfo[].m_stJumpInfo.m_astPlayerJumpInfo[].m_iJumplevel
m_astTimerChallenge2ndGameBeginInfo[].m_stJumpInfo.m_astPlayerJumpInfo[].m_iWeekHighestTongGuanNum
m_astTimerChallenge2ndGameBeginInfo[].m_stJumpInfo.m_astPlayerJumpInfo[].m_iBonusJumplevel
m_astTimerChallenge2ndGameBeginInfo[].m_stJumpInfo.m_astPlayerJumpInfo[].m_iHistoryMaxTongGuanNum
m_astTimerChallenge2ndGameBeginInfo[].m_stJumpInfo.m_iJumpLevel
m_astTimerChallenge2ndGameBeginInfo[].m_uiBeginUin
m_astTimerChallenge2ndGameBeginInfo[].m_bReverse
	*/

	Write8(p, 0); //HasGameStageInfo
	/*
m_stGameStageInfo[].m_u8StageIndex
m_stGameStageInfo[].m_u8TotalStageNum
	*/

	Write8(p, 0); //CarCollectInfoNum
	/*
m_stCarCollectInfo[].m_ushPlayerNum
m_stCarCollectInfo[].m_stCarCollectInfo[].m_uiUin
m_stCarCollectInfo[].m_stCarCollectInfo[].m_iCarCollectCarID
m_stCarCollectInfo[].m_stCarCollectInfo[].m_iCarCollectTaskID
	*/


	Write16(p, 10); //ReportAntiCollisionDataTime
	Write32(p, 0); //Duration
	Write32(p, 0); //BeginCDTime
	Write32(p, 0); //PropInteval
	Write32(p, 0); //MoveFreq


	Write8(p, 0); //HaveGameLogicTask
	/*
m_astRakedMatchGamelogicTaskInfo[].m_uiGameLogicTaskType
m_astRakedMatchGamelogicTaskInfo[].m_uiTaskFinishCondValue
m_astRakedMatchGamelogicTaskInfo[].m_ucJudgeRule
m_astRakedMatchGamelogicTaskInfo[].m_uiTaskAddGradeScore
m_astRakedMatchGamelogicTaskInfo[].m_uchGradeScoreRangeNum
m_astRakedMatchGamelogicTaskInfo[].m_auiGradeScoreRange[]
m_astRakedMatchGamelogicTaskInfo[].m_uchTaskFinishCondValueNum
m_astRakedMatchGamelogicTaskInfo[].m_auiTaskFinishCondValueRange[]
m_astRakedMatchGamelogicTaskInfo[].m_uchTaskAddGradeScoreNum
m_astRakedMatchGamelogicTaskInfo[].m_auiTaskAddGradeScoreRange[]
m_astRakedMatchGamelogicTaskInfo[].m_uchFailedAddGradeScoreNum
m_astRakedMatchGamelogicTaskInfo[].m_auiFailedAddGradeScoreRange[]
m_astRakedMatchGamelogicTaskInfo[].m_ucFinishTask
m_astRakedMatchGamelogicTaskInfo[].m_uiExtraFinishTaskAddScore
	*/

	Write16(p, 0); //RankedMatchBegainTipInfoType

	Write8(p, 0); //BegainTipLen


	Write8(p, 0); //BuffBum
	/*
m_stEffectBuffInfo[].m_ushBuffID
m_stEffectBuffInfo[].m_uchFeatureID
m_stEffectBuffInfo[].m_uchBuffLevel
m_stEffectBuffInfo[].m_uchIsIcon
	*/

	Write8(p, 0); //MapChallengeInfoNum
	/*
m_stMapChallengeInfo[].m_uchStarInfoNum
m_stMapChallengeInfo[].m_astMapChallengeStarInfo[].m_uiTime
m_stMapChallengeInfo[].m_astMapChallengeStarInfo[].m_uchStatus
m_stMapChallengeInfo[].m_stAssessInfo.m_stMapChallenge_AssessInfo.m_bType
m_stMapChallengeInfo[].m_stAssessInfo.m_stMapChallenge_AssessInfo.m_ushCount
m_stMapChallengeInfo[].m_stAssessInfo.m_uchStatus
m_stMapChallengeInfo[].m_stAssessInfo.m_ushMsgLen
m_stMapChallengeInfo[].m_iMapID
*/

	Write8(p, 0); //IsChangeCar

	Write8(p, 0); //AnonymousMode
/*
m_astAnonymousModeAvatarChangeInfo[].m_dwPlayerUin
m_astAnonymousModeAvatarChangeInfo[].m_iHairID
m_astAnonymousModeAvatarChangeInfo[].m_iCoatID
m_astAnonymousModeAvatarChangeInfo[].m_iTrousersID
m_astAnonymousModeAvatarChangeInfo[].m_iSuitID
m_astAnonymousModeAvatarChangeInfo[].m_iKartID
m_astAnonymousModeAvatarChangeInfo[].m_uchHasItemInfo
m_astAnonymousModeAvatarChangeInfo[].m_stItemInfo[].m_iRoleID
m_astAnonymousModeAvatarChangeInfo[].m_stItemInfo[].m_iHairID
m_astAnonymousModeAvatarChangeInfo[].m_stItemInfo[].m_iFaceID
m_astAnonymousModeAvatarChangeInfo[].m_stItemInfo[].m_iGlassID
m_astAnonymousModeAvatarChangeInfo[].m_stItemInfo[].m_iCoatID
m_astAnonymousModeAvatarChangeInfo[].m_stItemInfo[].m_iGloveID
m_astAnonymousModeAvatarChangeInfo[].m_stItemInfo[].m_iTrousersID
m_astAnonymousModeAvatarChangeInfo[].m_stItemInfo[].m_iPersonarID
m_astAnonymousModeAvatarChangeInfo[].m_stItemInfo[].m_iTattooID
m_astAnonymousModeAvatarChangeInfo[].m_stItemInfo[].m_iWingID
m_astAnonymousModeAvatarChangeInfo[].m_stItemInfo[].m_iDecoratorID
m_astAnonymousModeAvatarChangeInfo[].m_stItemInfo[].m_iKartID
m_astAnonymousModeAvatarChangeInfo[].m_stItemInfo[].m_iColorID
m_astAnonymousModeAvatarChangeInfo[].m_stItemInfo[].m_iTrackID
m_astAnonymousModeAvatarChangeInfo[].m_stItemInfo[].m_iEffectID
m_astAnonymousModeAvatarChangeInfo[].m_stItemInfo[].m_iBrandID
m_astAnonymousModeAvatarChangeInfo[].m_stItemInfo[].m_iTailID
	*/

	Write8(p, 0); //IsTimeShuttleGen

	Write8(p, 0); //HalloweenDdventureInfoNum
	/*
m_stHalloweenDdventureInfo[].m_ushPlayerNum
m_stHalloweenDdventureInfo[].m_stCarCollectInfo[].m_uiUin
m_stHalloweenDdventureInfo[].m_stCarCollectInfo[].m_iCarCollectCarID
m_stHalloweenDdventureInfo[].m_stCarCollectInfo[].m_iCarCollectTaskID
	*/

	Write8(p, 0); //MaxPropEffectNum
	Write8(p, 0); //IsCrossNoCollision

	Write8(p, 0); //RecordCheckCondNum
	/*
m_astRecordCheckConds[].m_usCheckRelation
m_astRecordCheckConds[].m_ucConditionNum
m_astRecordCheckConds[].m_astCheckConditions[].m_usCheckType
m_astRecordCheckConds[].m_astCheckConditions[].m_ucCheckParaNum
m_astRecordCheckConds[].m_astCheckConditions[].m_aIntParas[]
	*/

	Write32(p, 0); //GameBeginSwitchFlag
	Write8(p, 0); //TriggerVeggieDogTask

	Write8(p, 0); //HasQSpeedCrystalInfo
	/*
m_stQSpeedCrystalInfo[].m_iQSpeedCrystalID
m_stQSpeedCrystalInfo[].m_iQSpeedCrystalLevel
m_stQSpeedCrystalInfo[].m_uchQSpeedCrystalBuffBum
m_stQSpeedCrystalInfo[].m_stQSpeedCrystalEffectBuffInfo[].m_ushBuffID
m_stQSpeedCrystalInfo[].m_stQSpeedCrystalEffectBuffInfo[].m_uchFeatureID
m_stQSpeedCrystalInfo[].m_stQSpeedCrystalEffectBuffInfo[].m_uchBuffLevel
m_stQSpeedCrystalInfo[].m_stQSpeedCrystalEffectBuffInfo[].m_uchIsIcon
	*/
#endif

	len = p - buf;
	SendToClient(Client, 509, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Notify);
}

void NotifyKartPhysParam(ClientNode* Client, ClientNode* Player[])
{
	ClientNode* Players[6];
	char CurrentPlayers = 0;
	if (Player)
	{
		for (char i = 0; i < 6; i++)
		{
			if (Player[i])
			{
				Players[CurrentPlayers] = Player[i];
				CurrentPlayers++;
			}
		}
	}
	else
	{
		Players[0] = Client;
		CurrentPlayers = 1;
	}
	

	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, CurrentPlayers); //KartNum
	for (char i = 0; i < CurrentPlayers; i++)
	{ //KartPhysParam
		try
		{
			WriteKartPhysParam(p, Players[i]->Uin, Players[i]->KartID, Players[i]->Pet->ID, Players[i]->Pet->Level);
		}
		catch (const std::exception&)
		{
			return;
		}
		
	}

	Write16(p, 0); //SuperN2ORate
	
	len = p - buf;
	SendToClient(Client, 551, buf, len, Client->GameID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestPrepareReady(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
#ifndef ZingSpeed
	int MapID = Read32(Body);
	char MapHash = Read32(Body);
	UCHAR SingleGameRestartFlag = Read8(Body);
#endif

	if (Client->RoomID)
	{
		Client->IsReady = TRUE;
		RoomNode* Room = GetRoom(Client->RoomID);
		if (!Room)
		{
			return;
		}
		BOOL All = TRUE;
		for (char i = 0; i < 6; i++)
		{
			ClientNode* RoomClient = Room->Player[i];
			if (RoomClient)
			{
				if (RoomClient->IsReady == FALSE)
				{
					All = FALSE;
					break;
				}
			}
		}
		
		if (All) CreateRoomTimer(Room, 9000, OnBegin);
	}
}

void NotifyRaceBegin(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, 9000); //CountDownTime
	Write16(p, 0); //DelayTime
#ifndef ZingSpeed
	Write8(p, 0); //StageIndex
	Write32(p, 0); //RaceCountDownTime//赛跑倒计时时间
	Write32(p, 0); //RaceCountDownDis//比赛倒计时
	Write8(p, 0); //UseNewCountDownTime//使用新的倒计时时间
	Write32(p, 0); //NewCountDownTime//新的倒计时时间
	Write32(p, 0); //ServerSecond//服务器秒
	Write32(p, 0); //ServerMicroSecond//
#endif

	len = p - buf;
	SendToClient(Client, 511, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Notify);
}

void NotifyPlayerFinishRace(ClientNode* Client, bool NewRecord, int FinTime)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write8(p, NewRecord); //NewRecord
	Write32(p, FinTime); //FinTime

	len = p - buf;
	SendToClient(Client, 522, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Notify);
}

void RequestReportCurrentInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	UINT CurrentState = Read32(Body);

	//m_aiCurrentPosition[3]
	Read32(Body);
	Read32(Body);
	Read32(Body);

	BOOL IsFinish = FALSE;
	BYTE PassedCheckPointNum = Read8(Body);
	for (size_t i = 0; i < PassedCheckPointNum; i++)
	{
		WORD PassedCheckPointID = Read16(Body);
		//printf("保存点:%d\t", PassedCheckPointID);
		if (Client->Loop)
		{
			if (Client->EndCheckPoint != 0 && PassedCheckPointID > Client->EndCheckPoint) //大于终点的则可能在走近道
			{
				//printf("continue\t");
				continue;
			}
			if (PassedCheckPointID == 0 && Client->PassedCheckPoint > 4)
			{
				Client->Round++;
				if (Client->Round >= Client->TotalRound)
				{
					IsFinish = TRUE;
				}
			}
			else if (Client->PassedCheckPoint == 0 && PassedCheckPointID > 4)
			{
				Client->Round--;
			}
		}
		else
		{
			if (PassedCheckPointID == Client->EndCheckPoint)
			{
				Client->Round++;
				if (Client->Round >= Client->TotalRound)
				{
					IsFinish = TRUE;
				}
			}
		}
		Client->PassedCheckPoint = PassedCheckPointID;
	}

	UINT LapTime = Read32(Body);

	BYTE AccVerifyDataNum = Read8(Body);
	for (size_t i = 0; i < AccVerifyDataNum; i++)
	{
		/*
m_astAccelVerifyData[].m_unType
m_astAccelVerifyData[].m_ushTotalCount
m_astAccelVerifyData[].m_unMaxDurTime
m_astAccelVerifyData[].m_ushOverlayCount
		*/
		Body += Get16(Body);
	}

	BYTE MsgSequence = Read8(Body);


	BYTE ExtraDataNum = Read8(Body);
	for (size_t i = 0; i < ExtraDataNum; i++)
	{
		/*
m_astExtraData[].stSkillStoneTriggerData.m_uTriggerTotalCount
m_astExtraData[].stSkillStoneTriggerData.m_uCheckData1
m_astExtraData[].stSkillStoneTriggerData.m_uCheckData2
m_astExtraData[].m_stClientReportSelfErr.m_iRemoveFailedCount
m_astExtraData[].m_stClientReportSelfErr.m_iNo2CheckErrCount
m_astExtraData[].m_stClientReportSelfErr.m_iFrictionCheckErrCount
m_astExtraData[].m_stClientReportSelfErr.m_iGPTargetCheckErrCount
m_astExtraData[].m_stClientReportSelfErr.m_iPowerCheckErrCount
m_astExtraData[].m_stClientReportSelfErr.m_iStoneCheckErrCount
m_astExtraData[].m_stClientReportSelfErr.m_iHashCheckErrCount
m_astExtraData[].m_stClientReportSelfErr.m_iTencentCarCheckErrCount
m_astExtraData[].m_stClientReportSelfErr.m_iUseIlleagleNo2Count
m_astExtraData[].m_stClientReportSelfErr.m_iListAffectGPsCheckErrCount
m_astExtraData[].m_stClientReportSelfErr.m_iGPTargetInvalidErrCount
m_astExtraData[].m_stClientReportSelfErr.m_aiErrData[ENMCRET_MAX]
m_astExtraData[].m_stClientReportSeflTag.m_iPowerCheckTag
m_astExtraData[].m_stClientReportSeflTag.m_iNo2CheckTag
m_astExtraData[].m_stClientReportSeflTag.m_iTencentCarCheckTag
m_astExtraData[].m_stClientReportSeflTag.m_iGPTargetInvalidCheckTag
m_astExtraData[].m_u32JetCombinationFlag
m_astExtraData[].m_unMaxSpeed
m_astExtraData[].m_u32CollisionNumOnLastCheckPoint
m_astExtraData[].m_uiTotalDrift
m_astExtraData[].m_bySpeedSectionNum
m_astExtraData[].m_auSpeedSectionAccTime[]
m_astExtraData[].m_byNormalSpeedSectionNum
m_astExtraData[].m_auNormalSpeedSectionAccTime[]
m_astExtraData[].m_u8MemCheckResultNum
m_astExtraData[].m_au32MemData[]
m_astExtraData[].m_auClientCheckErr[ECCET_MAX]
m_astExtraData[].m_auClientCheckErrTag[ECCETT_MAX]
m_astExtraData[].m_u8UpdateCount
m_astExtraData[].m_u8OffsetOverThresholdCount
m_astExtraData[].m_u32OffsetOverThresholdSum
m_astExtraData[].m_u16MaxOffsetSpeed
m_astExtraData[].m_u16AvgOffsetSpeed
m_astExtraData[].m_u16MaxRealSpeed
m_astExtraData[].m_u16AvgRealSpeed
m_astExtraData[].m_u32MaxSpeedRatio
m_astExtraData[].m_u16SpeedRatioOverThresholdCount
m_astExtraData[].m_u32MaxSpeedRatio2
m_astExtraData[].m_u16SpeedRatioOverThresholdCount2
m_astExtraData[].m_u32FBSkillRebornTime
m_astExtraData[].m_u32FBSkillChargeTime
m_astExtraData[].m_uiTotalPerpetualNight
m_astExtraData[].m_iRPCSFlag
m_astExtraData[].m_u32CollisionPlayerNum
m_astExtraData[].m_u32CollisionNumOnPlayer
m_astExtraData[].m_iCodeItegrity
m_astExtraData[].m_iSendLaPkg
m_astExtraData[].m_iSendDataToSvr
m_astExtraData[].m_aiAbnormalData[4]
m_astExtraData[].m_stLevelInfo.m_cLowFPSRatio
m_astExtraData[].m_stLevelInfo.m_cAveFPS
m_astExtraData[].m_stLevelInfo.m_iMemUsed
m_astExtraData[].m_stLevelInfo.m_iVMemUsed
m_astExtraData[].m_stLevelInfo.m_iGameSetting
m_astExtraData[].m_uiPositionErrorCount
m_astExtraData[].m_uiDropToDeathSurfaceNums
m_astExtraData[].m_uiCollideNumsInN2OStat
m_astExtraData[].m_uiBeyondPlayerNums
		*/
		Body += Get16(Body);
	}

	BYTE N2OChangeEventNum = Read8(Body);
	for (size_t i = 0; i < N2OChangeEventNum; i++)
	{
		/*
m_astN2OChangeEvent[].m_u16EventID
m_astN2OChangeEvent[].m_u32BeginLapTime
m_astN2OChangeEvent[].m_u16LastingTime
m_astN2OChangeEvent[].m_u8EventType
m_astN2OChangeEvent[].m_i16N2OChange
		*/
		Body += Get16(Body);
	}

	UINT Flag = Read32(Body);
	
#ifndef ZingSpeed
	BYTE HasCrashModeData = Read8(Body);
	if (HasCrashModeData)
	{
		//m_astCrashModeData[].m_u8CurEggNum
		Body += Get16(Body);
	}

	BYTE HasPointChallengeData = Read8(Body);
	if (HasPointChallengeData)
	{
		/*
m_astPointChallengeData[].m_uiEatCoin
m_astPointChallengeData[].m_uiBeforeTime
m_astPointChallengeData[].m_uiBeforeLong
m_astPointChallengeData[].m_uiCollectN2
m_astPointChallengeData[].m_uiShuangpen
m_astPointChallengeData[].m_uiKongpen
m_astPointChallengeData[].m_uiLuodipen
m_astPointChallengeData[].m_uiNPCRunAfterTime
m_astPointChallengeData[].m_uiLeiYinCrazyJet
		*/
		Body += Get16(Body);
	}

	UINT SkateCoinNum = Read32(Body);
	for (size_t i = 0; i < SkateCoinNum; i++)
	{
		/*
m_astSkateCoinData[].m_uiCoinID
m_astSkateCoinData[].m_uiPlayerPosX
m_astSkateCoinData[].m_uiPlayerPosY
m_astSkateCoinData[].m_uiPlayerPosZ
m_u32SkateComboAwardCoinNum
m_astSkateComboAwardCoinData[].m_ushComboNum
m_astSkateComboAwardCoinData[].m_ushCoinNum
		*/
		Body += Get16(Body);
	}


	UINT SkateComboAwardCoinNum = Read32(Body);
	for (size_t i = 0; i < SkateComboAwardCoinNum; i++)
	{
		/*
m_astSkateComboAwardCoinData[].m_ushComboNum
m_astSkateComboAwardCoinData[].m_ushCoinNum
		*/
		Body += Get16(Body);
	}

	UINT SkateDoubleJumpCount = Read32(Body);
	UINT CurrMapID = Read32(Body);


	BYTE HasGameDetailRecord = Read8(Body);
	if (HasGameDetailRecord)
	{
		/*
m_astGameDetailRecord[].m_uiUin
m_astGameDetailRecord[].m_szNickName[17]
m_astGameDetailRecord[].m_iKartId
m_astGameDetailRecord[].m_unFinTime
m_astGameDetailRecord[].m_uiHighestSpeed
m_astGameDetailRecord[].m_uiAverageSpeed
m_astGameDetailRecord[].m_uiCollisionCount
m_astGameDetailRecord[].m_uiN2OGatherCount
m_astGameDetailRecord[].m_uiJetTime
m_astGameDetailRecord[].m_uiChaoJiQiBu
m_astGameDetailRecord[].m_uiZhuangQiangDianPen
m_astGameDetailRecord[].m_uiDriftCount
m_astGameDetailRecord[].m_uiSmallJetCount
m_astGameDetailRecord[].m_uiDoubleJetCount
m_astGameDetailRecord[].m_uiFlyJetCount
m_astGameDetailRecord[].m_uiFallJetCount
m_astGameDetailRecord[].m_uiDuanWeiQiBu
m_astGameDetailRecord[].m_uiDuanWeiDoubleJetCount
m_astGameDetailRecord[].m_uiLinkJetCount
m_astGameDetailRecord[].m_uiWCJetCount
m_astGameDetailRecord[].m_uiCeShenDriftCount
m_astGameDetailRecord[].m_uiShuaiWeiDriftCount
m_astGameDetailRecord[].m_uiCWWJetCount
m_astGameDetailRecord[].m_uiXiaoGuaCount
m_astGameDetailRecord[].m_teamID
m_astGameDetailRecord[].m_uiFastCornersNum
m_astGameDetailRecord[].m_ushDiffCornerNum
m_astGameDetailRecord[].m_uiFastCornerDetailInfo[]
m_astGameDetailRecord[].m_usWSingleJetCount
m_astGameDetailRecord[].m_usWCWJetCount
m_astGameDetailRecord[].m_usCWWJetCount
m_astGameDetailRecord[].m_ushCeShenJetCount
m_astGameDetailRecord[].m_ushShuaiWeiJetCount
m_astGameDetailRecord[].m_shRankedMatchGrade
m_astGameDetailRecord[].m_ushDuanWeiPiaoYi
m_astGameDetailRecord[].m_ushDuanWeiJet
m_astGameDetailRecord[].m_ushOutOfTurnQuickly
m_astGameDetailRecord[].m_ushLaunch
m_astGameDetailRecord[].m_ushHitWaterDrift
m_astGameDetailRecord[].m_ushOutWaterJet
m_astGameDetailRecord[].m_ushN2DriftContinue
m_astGameDetailRecord[].m_ushWaveFlyJet
m_astGameDetailRecord[].m_ushWorldID
m_astGameDetailRecord[].m_uchNpc
m_astGameDetailRecord[].m_uiGameSeq
m_astGameDetailRecord[].m_uiTime
		*/
		Body += Get16(Body);
	}


	BYTE CurrentInfoFlag = Read8(Body);
	UINT TotalAccelerateFuel = Read32(Body);
	UINT DstNPCID = Read32(Body);
	UINT DistanceToEnd = Read32(Body);
	UINT GansterProcess = Read32(Body);
	UINT TotalGangsterPKCount = Read32(Body);


	BYTE HasTowerChallengeData = Read8(Body);
	if (HasTowerChallengeData)
	{
		/*
m_astTowerChallengeData[].m_uiEatCoin
m_astTowerChallengeData[].m_uiCollisionCount
m_astTowerChallengeData[].m_uiDriftCount
m_astTowerChallengeData[].m_cKartStatus
m_astTowerChallengeData[].m_cTrackElementNum
m_astTowerChallengeData[].m_astTriggerInfo[].m_cType
m_astTowerChallengeData[].m_astTriggerInfo[].m_cInitCheckPoint
m_astTowerChallengeData[].m_astTriggerInfo[].m_cPlayerTriggerCheckPoint
		*/
		Body += Get16(Body);
	}

	BYTE CurrentSelfRank = Read8(Body);
	BYTE CurrentTeammateRank = Read8(Body);


	BYTE HasP2PStatusData = Read8(Body);
	if (HasP2PStatusData)
	{
		/*
m_astP2PStatusData[].m_uiUin
m_astP2PStatusData[].m_ucServerUDPLoginStaus
m_astP2PStatusData[].m_ucPlayerNum
m_astP2PStatusData[].m_astP2PStatus2Player[].m_uiUin
m_astP2PStatusData[].m_astP2PStatus2Player[].m_ucP2PConnectState
m_astP2PStatusData[].m_astP2PStatus2Player[].m_ucP2PCanPass
m_astP2PStatusData[].m_astP2PStatus2Player[].m_uiP2PNetSpeed
m_astP2PStatusData[].m_astP2PStatus2Player[].m_uiNetSpeed
m_astP2PStatusData[].m_astP2PStatus2Player[].m_uiNotPassStateCount
m_astP2PStatusData[].m_astP2PStatus2Player[].m_uiSendDataCount[ENMSDM_MAX]
m_astP2PStatusData[].m_astP2PStatus2Player[].m_uiRecvDataCount[ENMSDM_MAX]
m_astP2PStatusData[].m_astP2PStatus2Player[].m_uiServerRecvDataCount[ENMSDM_MAX]
m_astP2PStatusData[].m_astP2PStatus2Player[].m_ushRecvPkgCount
m_astP2PStatusData[].m_astP2PStatus2Player[].m_astRecvPkgInfo[].m_uiPkgID
m_astP2PStatusData[].m_astP2PStatus2Player[].m_astRecvPkgInfo[].m_uiRecvTime
m_astP2PStatusData[].m_astP2PStatus2Player[].m_astRecvPkgInfo[].m_uiDistanceDiff
m_astP2PStatusData[].m_astP2PStatus2Player[].m_uchDetailCount
m_astP2PStatusData[].m_astP2PStatus2Player[].m_astDetailInfo[].m_ucType
m_astP2PStatusData[].m_astP2PStatus2Player[].m_astDetailInfo[].m_uiMsgID
m_astP2PStatusData[].m_astP2PStatus2Player[].m_astDetailInfo[].m_uiCount
m_astP2PStatusData[].m_ushSendPkgCount
m_astP2PStatusData[].m_astSendPkgInfo[].m_uiPkgID
m_astP2PStatusData[].m_astSendPkgInfo[].m_uiSendTime
m_astP2PStatusData[].m_astSendPkgInfo[].m_uchUDPStatus
m_astP2PStatusData[].m_astSendPkgInfo[].m_uchSendDataMethod
m_astP2PStatusData[].m_uiRaceBeginTime
m_astP2PStatusData[].m_uiLagCnt
m_astP2PStatusData[].m_uiLagCntGlobal
m_astP2PStatusData[].m_uiTickInLevel
		*/
		Body += Get16(Body);
	}

	UINT DistanceToFirstRacer = Read32(Body);
	UINT TimerChallengeRecoverNum = Read32(Body);
	UINT CoinNum = Read32(Body);

	BYTE ClientPlayerNum = Read8(Body);
	for (size_t i = 0; i < ClientPlayerNum; i++)
	{
		/*
m_astClientGameInfo[].m_uiUin
m_astClientGameInfo[].m_uchDistanceNum
m_astClientGameInfo[].m_astDistanceToEndInfo[].m_uiTimeFromRaceBegin
m_astClientGameInfo[].m_astDistanceToEndInfo[].m_uiDistanceToEnd
m_astClientGameInfo[].m_uchCollisionNum
m_astClientGameInfo[].m_astClientCollisionInfo[].m_uiTimeFromRaceBegin
		*/
		Body += Get16(Body);
	}

	BYTE RankedMatchSpeedKingBuffer = Read8(Body);
	UINT DistanceTotal = Read32(Body);
	BYTE IsHitRecordContion = Read8(Body);
	USHORT HangSpan = Read16(Body);
	BYTE CurRound = Read8(Body);
	BYTE IncRound = Read8(Body);


	if (Client->MapCompatibilityMode)
	{
		IsFinish = FALSE;
		if (DistanceToEnd == 0)
		{
			IsFinish = TRUE;
		}
	}
#endif
#ifdef DEBUG
	printf("Round:%d\n", Client->Round);
#endif
	if (IsFinish)
	{
		if (Client->RoomID != 0 && Client->FinishTime == 0)
		{
			Client->FinishTime = LapTime * 10;
			bool NewRecord = PlayerDB_SetMapRecord(Client->Uin, Client->MapID, Client->FinishTime);
			//NotifyPlayerFinishRace(Client, NewRecord, Client->FinishTime);

			RoomNode* Room = GetRoom(Client->RoomID);
			if (!Room)
			{
				return;
			}
			int PlayerNum = 0;
			for (char i = 0; i < 6; i++)
			{
				ClientNode* RoomClient = Room->Player[i];
				if (RoomClient)
				{
					PlayerNum++;
					NotifyCountDown(RoomClient, Client->Uin, NewRecord, Client->FinishTime);
					////给完成的用户增加物品
					int CarGiftNum = PlayerDB_GetItemNum(RoomClient->Uin, 27570);
					//添加物品操作
					//if (CarGiftNum == 0)
					//{
					//	/*PlayerDB_AddItem(Client->Uin, 27570, 1, 0, false, 0);*/
					//	PlayerDB_UpdateItem(Client->Uin, 27570, 1);
					//}
					//else
					//{
					CarGiftNum = CarGiftNum + 1;
					PlayerDB_UpdateItem(RoomClient->Uin, 27570, CarGiftNum);
					//}
					ItemInfo CarGift[1];
					CarGift[0].ItemID = 27570;
					CarGift[0].Status = false;
					CarGift[0].ItemNum = CarGiftNum;
					CarGift[0].AvailPeriod = 0;
					CarGift[0].ItemType = 0;
					NotifyClientAddItem(RoomClient, 1, CarGift);
					//ItemStatus NewStatus[1];
					/*NewStatus[0].ItemID = CarGift[0].ItemID;
					NewStatus[0].NewStatus = CarGift[0].Status;
					ResponseChangeItemStatus(RoomClient, 1, &NewStatus[0]);*/
					int ClothesGiftNum = PlayerDB_GetItemNum(RoomClient->Uin, 69010);
					//if (ClothesGiftNum == 0)
					//{
					//	/*PlayerDB_AddItem(Client->Uin, 69010, 1, 0, false, 0);*/
					//	PlayerDB_UpdateItem(Client->Uin, 69010, 1);
					//}
					/*else
					{*/
					ClothesGiftNum = ClothesGiftNum + 1;
					PlayerDB_UpdateItem(RoomClient->Uin, 69010, ClothesGiftNum);
					//}
					ItemInfo ClothesGift[1];
					ClothesGift[0].ItemID = 69010;
					ClothesGift[0].Status = false;
					ClothesGift[0].ItemNum = ClothesGiftNum;
					ClothesGift[0].AvailPeriod = 0;
					ClothesGift[0].ItemType = 0;
					NotifyClientAddItem(RoomClient, 1, ClothesGift);
					/*NewStatus[1].ItemID = ClothesGift[0].ItemID;
					NewStatus[1].NewStatus = ClothesGift[0].Status;
					ResponseChangeItemStatus(RoomClient, 1, &NewStatus[1]);*/

					int PetGiftNum = PlayerDB_GetItemNum(RoomClient->Uin, 28286);
					//添加物品操作
					PetGiftNum = PetGiftNum + 1;
					PlayerDB_UpdateItem(RoomClient->Uin, 28286, PetGiftNum);
					//}
					ItemInfo PetGift[1];
					PetGift[0].ItemID = 28286;
					PetGift[0].Status = false;
					PetGift[0].ItemNum = PetGiftNum;
					PetGift[0].AvailPeriod = 0;
					PetGift[0].ItemType = 0;
					NotifyClientAddItem(RoomClient, 1, PetGift);

					int SetGiftNum = PlayerDB_GetItemNum(RoomClient->Uin, 22642);
					//添加物品操作
					SetGiftNum = SetGiftNum + 1;
					PlayerDB_UpdateItem(RoomClient->Uin, 22642, SetGiftNum);
					//}
					ItemInfo SetGift[1];
					SetGift[0].ItemID = 22642;
					SetGift[0].Status = false;
					SetGift[0].ItemNum = SetGiftNum;
					SetGift[0].AvailPeriod = 0;
					SetGift[0].ItemType = 0;
					NotifyClientAddItem(RoomClient, 1, SetGift);

					//RoomNode* Room = GetRoom(Client->RoomID);
		            //设置房间状态
					/*Room->BaseGameMode = Room->BaseGameMode;
					Room->SubGameMode = Room->SubGameMode;
					strcpy_s(Room->Name, Room->Name);
					Room->ServerID = Client->ServerID;
					Room->RoomOwnerID = Client->ConnID;
					Client->RoomID = Room->ID;
					Room->SeatNum = Room->SeatNum;
					Room->Status = 1;
					Room->CurrentPlayerNum = 1;
					Room->Flag = Room->Flag;
					memcpy(Room->Password, Room->Password, 16);
					Room->Player[0] = Client;*/


				}
			}
			if (!Room->Timer)
			{
				if (PlayerNum > 1)
				{
					CreateRoomTimer(Room, 10000, OnCountDown);
				}
				else
				{
					if (NewRecord)
					{
						CreateRoomTimer(Room, 3000, OnCountDown);
					}
					else
					{
						CreateRoomTimer(Room, 1, OnCountDown);
					}
				}

			}
		}
	}
}

void NotifyCountDown(ClientNode* Client, UINT WinnerUin, bool WinnerNewRecord, UINT FinTime)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write32(p, 10000); //CountDownTime
	Write32(p, WinnerUin); //WinnerUin
	Write8(p, WinnerNewRecord); //WinnerNewRecord
	Write32(p, FinTime); //FinTime

	len = p - buf;
	SendToClient(Client, 512, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Notify);
}

void NotifyRaceShow(ClientNode* Client)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write32(p, 0); //RaceShowTime
	Write8(p, 0); //HasFBInfo
	/*
m_stFBInfo[].m_bFBResult
m_stFBInfo[].m_uiKiller
	*/

#ifndef ZingSpeed
	Write8(p, 0); //WinTeamID
	Write32(p, 0); //EventID

	Write32(p, 0); //ParaNum
	//m_auiPara[]

#endif

	len = p - buf;
	SendToClient(Client, 515, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Notify);
}

int* GetScoreSort(RoomNode* Room);
int* GetScoreSort(RoomNode* Room) {

	int index = 0;
	int* Scoresort = new int[6];

	for (size_t i = 0; i < Room->CurrentPlayerNum; i++) {
		ClientNode* Player = Room->Player[i];
		if (Player) {
			Scoresort[index] = i;
			index++;
		}
	}

	for (int i = 0; i < index; ++i)
	{
		bool isSwap = false;
		for (int j = index - 1; j > i; --j)
		{

			if (Room->Player[Scoresort[j]]->FinishTime > Room->Player[Scoresort[j - 1]]->FinishTime) {
				std::swap(Scoresort[j], Scoresort[j - 1]);
				isSwap = true;
			}
		}

		if (isSwap == false) {
			break;
		}
	}

	return Scoresort;
}

void NotifyRaceOver(ClientNode* Client)//结算
{
	
	RoomNode* room = GetRoom(Client->RoomID);
	UINT PlayerNum = 0;
	PlayerRaceInfo ArrPlayerRaceInfo[6] = {};
	for (int i = 0; i < 6; i++)
	{
		PlayerRaceInfo tmp;
		tmp.Uin = -1;
		tmp.FinishTime = -1;
		ArrPlayerRaceInfo[i] = tmp;

		if (room->Player[i])
		{
			//完成时间如果是0还是会作为结算成绩进行排名
			//改为-1就是未完成了
			if (room->Player[i]->FinishTime == 0)room->Player[i]->FinishTime = -1;

			ArrPlayerRaceInfo[i].Uin = room->Player[i]->Uin;
			ArrPlayerRaceInfo[i].Round = room->Player[i]->Round;
			ArrPlayerRaceInfo[i].FinishTime = room->Player[i]->FinishTime;

			PlayerNum++;
		}
	}
	auto cmp_PlayerRaceInfo = [](PlayerRaceInfo& a, PlayerRaceInfo& b) -> bool
	{     return  a.FinishTime < b.FinishTime; };

	//名次排序
	sort(ArrPlayerRaceInfo, ArrPlayerRaceInfo + 6, cmp_PlayerRaceInfo);

	BYTE buf[8192] = {};
	BYTE* p = buf;
	size_t len;

	
	Write8(p, PlayerNum); //CurrentPlayerNum
	//取玩家i数组内值  根据值取玩家 返回
	for (int i = 0; i < PlayerNum; i++)
	{

	 //RaceScore
		BYTE* pRaceScore = p;
		Write16(pRaceScore, 0); //len
		Write32(pRaceScore, ArrPlayerRaceInfo[i].Uin); //Uin
		Write32(pRaceScore, ArrPlayerRaceInfo[i].FinishTime); //FinTime
		Write32(pRaceScore, 0); //TP
		Write32(pRaceScore, 300 + 300 / (i + 1)); //MoneyInc 酷币
		Write32(pRaceScore, 600); //TotalMoney
		Write32(pRaceScore, 600 + 600 / (i + 1)); //ExpInc 加的经验
		Write32(pRaceScore, 44); //TotalExp 加完现在的经验

		Write8(pRaceScore, 0); //AwardNum
		//m_iSpecialAward[]

		Write32(pRaceScore, 0); //TeamWorkExpInc
		Write32(pRaceScore, 0); //PropPoint
		Write32(pRaceScore, 0); //PropPointAddExp
		Write32(pRaceScore, 0); //LuckyMatchPointInc
		Write32(pRaceScore, 0); //LuckyMatchPointTotal
		Write32(pRaceScore, 0); //LuckyMatchScoreInc
		Write32(pRaceScore, 0); //LuckyMatchScoreTotal
		Write32(pRaceScore, 0); //LuckMoneyInc
		Write32(pRaceScore, 0); //LuckMoneyTotal
		Write32(pRaceScore, 0); //GuildScoreInc

		Write8(pRaceScore, 0); //CrazyPropAchieveNum
		//m_aiCrazyPropAchieve[]

		Write32(pRaceScore, 0); //IncWlMatchScore
		Write32(pRaceScore, 0); //IncWlDegree
		Write8(pRaceScore, 0); //IncItemNumByWl
		/*
m_astRaceScore[].m_astIncItemInfoByWl[].m_shGenderItemNum
m_astRaceScore[].m_astIncItemInfoByWl[].m_aiItemID[EGT_MAX]
m_astRaceScore[].m_astIncItemInfoByWl[].m_shItemNum
m_astRaceScore[].m_astIncItemInfoByWl[].m_iAvailPeriod
m_astRaceScore[].m_astIncItemInfoByWl[].m_usBroadcastType
m_astRaceScore[].m_astIncItemInfoByWl[].m_usBroadcastFlag
m_astRaceScore[].m_astIncItemInfoByWl[].m_ucShowPriority
m_astRaceScore[].m_astIncItemInfoByWl[].m_uchItemType
m_astRaceScore[].m_astIncItemInfoByWl[].m_uchIgnoreItem
m_astRaceScore[].m_astIncItemInfoByWl[].m_uiLimitPara
m_astRaceScore[].m_astIncItemInfoByWl[].m_ucSpecialAwardCtrlType
m_astRaceScore[].m_astIncItemInfoByWl[].m_ucAwardCtrlType
		*/

		Write32(pRaceScore, 0); //WlMutiplyCard
		Write32(pRaceScore, 0); //SkateCoinInc
		Write32(pRaceScore, 0); //SkateCoinTotal
		Write32(pRaceScore, 0); //SkateCoinHistoryTotal
		Write32(pRaceScore, 0); //TotalCoupons


		Write8(pRaceScore, 0); //ChallengeCheer
		/*
m_astRaceScore[].m_stChallengeCheer[].m_ushGuildCheerValueInc
m_astRaceScore[].m_stChallengeCheer[].m_ushCheerValueInc
m_astRaceScore[].m_stChallengeCheer[].m_uiTotalCheerValue
m_astRaceScore[].m_stChallengeCheer[].m_ushCouponsInc
m_astRaceScore[].m_stChallengeCheer[].m_iGuildScoreInc
m_astRaceScore[].m_stChallengeCheer[].m_iSuperMoneyInc
		*/

		Write32(pRaceScore, 0); //LoveValue
		Write32(pRaceScore, 0); //SkateCoinEmperorBonus
		Write32(pRaceScore, 0); //DetailRecordID

		Write8(pRaceScore, 0); //HasGangsterResult
		/*
m_astRaceScore[].m_astGangsterResult[].m_cAbnormalFalg
m_astRaceScore[].m_astGangsterResult[].m_uiGangsterBlood
m_astRaceScore[].m_astGangsterResult[].m_uiGangsterTotalScore
m_astRaceScore[].m_astGangsterResult[].m_uiGangsterIndentity
m_astRaceScore[].m_astGangsterResult[].m_uiFragmentInc
m_astRaceScore[].m_astGangsterResult[].m_uiKillTimes
m_astRaceScore[].m_astGangsterResult[].m_uiDeadTimes
m_astRaceScore[].m_astGangsterResult[].m_ucIsMultiple
m_astRaceScore[].m_astGangsterResult[].m_ucIsLimited
m_astRaceScore[].m_astGangsterResult[].m_iScoreInc
m_astRaceScore[].m_astGangsterResult[].m_iTotalScore
m_astRaceScore[].m_astGangsterResult[].m_iLoseStreakCount
		*/

		{ //EquippedActiveKartInfo
			BYTE* pEquippedActiveKartInfo = pRaceScore;
			Write16(pEquippedActiveKartInfo, 0); //len

			Write8(pEquippedActiveKartInfo, 0); //HaveActiveInfo
			Write32(pEquippedActiveKartInfo, 0); //KartID
			Write32(pEquippedActiveKartInfo, 0); //ActiveLevel

			len = pEquippedActiveKartInfo - pRaceScore;
			Set16(pRaceScore, (WORD)len);
			pRaceScore += len;
		}


		Write8(pRaceScore, 0); //HasWeRelayRaceOverInfo
		/*
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_stSumStageInfo.m_u32Score
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_stSumStageInfo.m_u32ScoreInc
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_stSumStageInfo.m_u32Supermoney
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_stSumStageInfo.m_u32Coupons
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_stSumStageInfo.m_u32SuperN2O
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiWeRelayScoreInc
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiWeRelayScoreNow
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiWeRelayScoreLevelNow
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiWeRelayScoreLevelLimit
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiSupermoneyLimit
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiSupermoneyNow
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiCouponsLimit
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiCouponsNow
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiLuckAwardMul
		*/

		Write8(pRaceScore, 0); //HaveSkatePropRaceInfo
		/*
	m_astRaceScore[].m_stSkatePropRaceInfo[].m_uiScoreAdd
	m_astRaceScore[].m_stSkatePropRaceInfo[].m_uiScoreTotal
		*/

		Write8(pRaceScore, 0); //IsNewBox


		Write8(pRaceScore, 0); //HaveArrestScoreInfo
		/*
	m_astRaceScore[].m_stArrestScoreInfo[].m_uiScoreInGame
	m_astRaceScore[].m_stArrestScoreInfo[].m_uiScoreByGold
	m_astRaceScore[].m_stArrestScoreInfo[].m_uiGoldMoney
	m_astRaceScore[].m_stArrestScoreInfo[].m_uiArrestNum
	m_astRaceScore[].m_stArrestScoreInfo[].m_uchResultType
	m_astRaceScore[].m_stArrestScoreInfo[].m_bTeamID
		*/

		Write8(pRaceScore, 0); //HasRankedMatchInfo
		/*
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchLastGradeInfo.m_uiGradeLevel
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchLastGradeInfo.m_uiChildLevel
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchLastGradeInfo.m_uiGradeScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchLastGradeInfo.m_uiMaxGradeScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchLastGradeInfo.m_uiTotalScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchGradeInfo.m_uiGradeLevel
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchGradeInfo.m_uiChildLevel
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchGradeInfo.m_uiGradeScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchGradeInfo.m_uiMaxGradeScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchGradeInfo.m_uiTotalScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchPromoInfo.m_ucPromotionMatch
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchPromoInfo.m_uiPromotionRoundNums
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchPromoInfo.m_uiPromotionSuccessRoundNums
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchPromoInfo.m_uiPromotionAlreadySuccessRoundNums
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchPromoInfo.m_uiPromotionRecordTag
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchPromoInfo.m_uiCurrentPromotionRoundNums
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchPromoInfo.m_uiPromoFailedMinusScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchPromoInfo.m_uiPromoSuccAddScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchPromoInfo.m_ucPromoDirectly
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchPromoInfo.m_ucGradeNoNeedPromo
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchPromoInfo.m_bRankedMemoryNoNeedPromo
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchPromoInfo.m_uchPromotionExtraScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchPromoInfo.m_ucPromtRaceFailedDefendFlag
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_iRankAddScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_iFinishAddScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_iKillAllAddScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_iGradeProtectAddScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_iGradeAddScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_uiProtectAddScoreGradeLimit
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_iDailyExtraAddScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_iAwardRankCoinNums
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_iWeeklyAwardRankCoinNums
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_iWeeklyAwardRankCoinLimit
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_iAwardRankCoinNumsByRank
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_iAwardRankCoinNumsByFinish
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_iBanMapExtraAddScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_iTeamRankAddScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_iAwardRankCoinNumsByTeamRank
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_iAwardRankCoinNumsByTask
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_ucUseDoubleScoreItem
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_ucUserNotDecreaseItem
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_ucFinishSeasonChallengeTaskOrNot
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_uiFinishedSeasonChallengeTaskIndex
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_uiSeasonChallengeTaskProtectAddScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_uchNotDecreaseReason
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_ucHaveSpeedKingInfo
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchSpeedKingInfos[].m_ucWinOrNot
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchSpeedKingInfos[].m_UiAwardSpeedKingEnergyItemNums
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchSpeedKingInfos[].m_uiWeeklyAwardSpeedKingEnergyItemNums
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchSpeedKingInfos[].m_uiWeeklyAwardSpeedKingEnergyItemLimitNums
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_ucHaveExtraComputeInfo
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchExtracComputeInfos[].m_uiExtraComputeType
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchExtracComputeInfos[].m_uiAwardItemNums
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchExtracComputeInfos[].m_uiWeekylyAwardItemNums
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchExtracComputeInfos[].m_uiWeeklyAwardItemLimitNums
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchExtracComputeInfos[].m_ucHaveGameLogicTask
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchExtracComputeInfos[].m_astRakedMatchGamelogicTaskInfo[].m_uiGameLogicTaskType
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchExtracComputeInfos[].m_astRakedMatchGamelogicTaskInfo[].m_uiTaskFinishCondValue
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchExtracComputeInfos[].m_astRakedMatchGamelogicTaskInfo[].m_ucJudgeRule
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchExtracComputeInfos[].m_astRakedMatchGamelogicTaskInfo[].m_uiTaskAddGradeScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchExtracComputeInfos[].m_astRakedMatchGamelogicTaskInfo[].m_uchGradeScoreRangeNum
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchExtracComputeInfos[].m_astRakedMatchGamelogicTaskInfo[].m_auiGradeScoreRange[]
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchExtracComputeInfos[].m_astRakedMatchGamelogicTaskInfo[].m_uchTaskFinishCondValueNum
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchExtracComputeInfos[].m_astRakedMatchGamelogicTaskInfo[].m_auiTaskFinishCondValueRange[]
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchExtracComputeInfos[].m_astRakedMatchGamelogicTaskInfo[].m_uchTaskAddGradeScoreNum
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchExtracComputeInfos[].m_astRakedMatchGamelogicTaskInfo[].m_auiTaskAddGradeScoreRange[]
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchExtracComputeInfos[].m_astRakedMatchGamelogicTaskInfo[].m_uchFailedAddGradeScoreNum
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchExtracComputeInfos[].m_astRakedMatchGamelogicTaskInfo[].m_auiFailedAddGradeScoreRange[]
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchExtracComputeInfos[].m_astRakedMatchGamelogicTaskInfo[].m_ucFinishTask
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchExtracComputeInfos[].m_astRakedMatchGamelogicTaskInfo[].m_uiExtraFinishTaskAddScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchExtracComputeInfos[].m_ucWin
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchExtracComputeInfos[].m_ucNormalPlayerOrNot
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_astRankedMatchExtracComputeInfos[].m_ucLeadRolePlayerNums
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_uiSeasonNewMapAddScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.uchSeasonChallengeBaseScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.uchSeasonChallengeContiWinExtrScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.uchSeasonChallengeRegretDefeatScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.uchSeasonChallengeRespectableOpponentScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_uchSpeedGodChallengeAddScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_uchSpeedGodChallengeAddCoin
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_uchSpeedFlashAddFactoryCoin
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_uchPinnacleBattleAddScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_uchPromotionExtraScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_uiSavedExtraScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_uiDailyExtraTaskID
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_uchSuitActiveAddScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_uchSpeedGodChallengeType
	m_astRaceScore[].m_stRankedMatchScore[].m_stRankedMatchComputeInfo.m_uchSuitActiveEffectID
	m_astRaceScore[].m_stRankedMatchScore[].m_uchHavePromotionExtraScoreInfo
	m_astRaceScore[].m_stRankedMatchScore[].m_stPromotionExtraScoreInfo[].m_ucPromotionMatch
	m_astRaceScore[].m_stRankedMatchScore[].m_stPromotionExtraScoreInfo[].m_uiPromotionRoundNums
	m_astRaceScore[].m_stRankedMatchScore[].m_stPromotionExtraScoreInfo[].m_uiPromotionSuccessRoundNums
	m_astRaceScore[].m_stRankedMatchScore[].m_stPromotionExtraScoreInfo[].m_uiPromotionAlreadySuccessRoundNums
	m_astRaceScore[].m_stRankedMatchScore[].m_stPromotionExtraScoreInfo[].m_uiPromotionRecordTag
	m_astRaceScore[].m_stRankedMatchScore[].m_stPromotionExtraScoreInfo[].m_uiCurrentPromotionRoundNums
	m_astRaceScore[].m_stRankedMatchScore[].m_stPromotionExtraScoreInfo[].m_uiPromoFailedMinusScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stPromotionExtraScoreInfo[].m_uiPromoSuccAddScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stPromotionExtraScoreInfo[].m_ucPromoDirectly
	m_astRaceScore[].m_stRankedMatchScore[].m_stPromotionExtraScoreInfo[].m_ucGradeNoNeedPromo
	m_astRaceScore[].m_stRankedMatchScore[].m_stPromotionExtraScoreInfo[].m_bRankedMemoryNoNeedPromo
	m_astRaceScore[].m_stRankedMatchScore[].m_stPromotionExtraScoreInfo[].m_uchPromotionExtraScore
	m_astRaceScore[].m_stRankedMatchScore[].m_stPromotionExtraScoreInfo[].m_ucPromtRaceFailedDefendFlag
		*/

		Write8(pRaceScore, 0); //HaveCrazyChaseScoreInfo
		/*
	m_astRaceScore[].m_astCrazyChaseScoreInfo[].m_uiUin
	m_astRaceScore[].m_astCrazyChaseScoreInfo[].m_shRole
	m_astRaceScore[].m_astCrazyChaseScoreInfo[].m_uiScore
	m_astRaceScore[].m_astCrazyChaseScoreInfo[].m_uchScoreNum
	m_astRaceScore[].m_astCrazyChaseScoreInfo[].m_astScoreInfo[].m_uchScoreType
	m_astRaceScore[].m_astCrazyChaseScoreInfo[].m_astScoreInfo[].m_iValue
	m_astRaceScore[].m_astCrazyChaseScoreInfo[].m_astScoreInfo[].m_iScore
		*/
		Write8(pRaceScore, 0); //TeamID

		Write8(pRaceScore, 0); //HasRankedMatchArenaInfo
		/*
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_ucWin
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_ucRank
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_ucTaskScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_iAwardRankCoinNums
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchLastGradeInfo.m_uiGradeLevel
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchLastGradeInfo.m_uiChildLevel
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchLastGradeInfo.m_uiGradeScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchLastGradeInfo.m_uiMaxGradeScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchLastGradeInfo.m_uiTotalScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchGradeInfo.m_uiGradeLevel
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchGradeInfo.m_uiChildLevel
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchGradeInfo.m_uiGradeScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchGradeInfo.m_uiMaxGradeScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchGradeInfo.m_uiTotalScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_ucPromotionMatch
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uiPromotionRoundNums
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uiPromotionSuccessRoundNums
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uiPromotionAlreadySuccessRoundNums
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uiPromotionRecordTag
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uiCurrentPromotionRoundNums
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uiPromoFailedMinusScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uiPromoSuccAddScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_ucPromoDirectly
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_ucGradeNoNeedPromo
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_bRankedMemoryNoNeedPromo
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uchPromotionExtraScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_ucPromtRaceFailedDefendFlag
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_ucWinNum
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_sCurrentScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_ucFinishScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_usAwardRankScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_ucAwardRankCoinNumsByRank
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_ucAwardRankCoinNumsByFinish
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_iWeeklyAwardRankCoinNums
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_iWeeklyAwardRankCoinLimit
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_iAwardRankCoinNumsByTask
		*/


		Write32(pRaceScore, 0); //DistanceToEnd
		Write8(pRaceScore, 0); //ShortDistancWinPoint
		Write8(pRaceScore, 0); //Status


		Write8(pRaceScore, 0); //HaveRankedMatchSpeedKingInfo
		/*
	m_astRaceScore[].m_stRankedMatchSpeedKingScoreInfo[].m_uiAwardSpeedKingEnergyNums
	m_astRaceScore[].m_stRankedMatchSpeedKingScoreInfo[].m_ucWin
		*/


		Write8(pRaceScore, 0); //ProfessionLicenseAwardNum
		/*
		m_astRaceScore[].m_stProfessionLicenseAward[].m_uiPointInc
		m_astRaceScore[].m_stProfessionLicenseAward[].m_uiPointLevelCur
		m_astRaceScore[].m_stProfessionLicenseAward[].m_uiPointLevelMax
		m_astRaceScore[].m_stProfessionLicenseAward[].m_ushIncResultID
		m_astRaceScore[].m_stProfessionLicenseAward[].m_uchShowQiPao
		m_astRaceScore[].m_stProfessionLicenseAward[].m_uiPointWeekTotal
		m_astRaceScore[].m_stProfessionLicenseAward[].m_uiPointWeekCur
		m_astRaceScore[].m_stProfessionLicenseAward[].m_stProfessionLicenseInfo.m_uchBigLevel
		m_astRaceScore[].m_stProfessionLicenseAward[].m_stProfessionLicenseInfo.m_uchSmallLevel
		m_astRaceScore[].m_stProfessionLicenseAward[].m_uchSwitchShowExtraAddPoint
		m_astRaceScore[].m_stProfessionLicenseAward[].m_ushBeginOffsetSeconds
		*/

		{ //RaceResult
			BYTE* pRaceResult = pRaceScore;
			Write16(pRaceResult, 0); //len

			Write8(pRaceResult, 0); //PLExpPowerValid
			Write8(pRaceResult, 0); //PLHonorPowerValid
			Write8(pRaceResult, 0); //ExpMultiNum
			Write8(pRaceResult, 0); //HonorMultiNum

			len = pRaceResult - pRaceScore;
			Set16(pRaceScore, (WORD)len);
			pRaceScore += len;
		}

		Write8(pRaceScore, 0); //HaveRankedMatchExtraInfo
		/*
		m_astRaceScore[].m_stRankedMatchExtraScoreInfo[].m_uiAwardItemNums
		m_astRaceScore[].m_stRankedMatchExtraScoreInfo[].m_ucWin
		*/

		Write8(pRaceScore, 0); //HasYuLeJiaNianHuaComputeData
		/*
		m_astRaceScore[].m_stYuLeJiaNianHuaComputeData[].m_iAwardScore
		m_astRaceScore[].m_stYuLeJiaNianHuaComputeData[].m_uchRecommend
		m_astRaceScore[].m_stYuLeJiaNianHuaComputeData[].m_uchScoreDouble
		m_astRaceScore[].m_stYuLeJiaNianHuaComputeData[].m_uchGuaJi
		*/

		len = pRaceScore - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); //WinTeamID
	Write32(p, 0); //WinType
	Write32(p, 0); //AwardTime
	Write8(p, 0); //LuckyMatchResult
	Write8(p, 0); //LuckyMatchType


	Write8(p, PlayerNum); //PlayerNumber
	for (int i = 0; i < PlayerNum; i++)
	{ //HideTaskResult

		HIDETASKRESULT hidTaskResult = { 0 };
		hidTaskResult.size = sizeof(HIDETASKRESULT);
		hidTaskResult.uin = room->Player[i]->Uin;

		BYTE* pHideTaskResult = p;
		//Write16(pHideTaskResult, 0); //len

		//Write32(pHideTaskResult, Client->Uin); //Uin
		//Write32(pHideTaskResult, 0); //TaskId
		//Write8(pHideTaskResult, 0); //HideTaskFinishResult

		//len = pHideTaskResult - p;
		//Set16(p, (WORD)len);
		//p += len;
		memcpy(pHideTaskResult, &hidTaskResult, hidTaskResult.size);
	}

	Write8(p, 0); //HasLadderMatchResult
	/*
m_stLDMRaceResult[].m_u8PlayerNum
m_stLDMRaceResult[].m_stResult[].m_u32Uin
m_stLDMRaceResult[].m_stResult[].m_u32Score
m_stLDMRaceResult[].m_stResult[].m_i32ScoreInc
m_stLDMRaceResult[].m_stResult[].m_u32NextGradeScore
m_stLDMRaceResult[].m_stResult[].m_u8Grade
m_stLDMRaceResult[].m_stResult[].m_u8BeforeGrade
m_stLDMRaceResult[].m_stResult[].m_stRank.m_u8RankType
m_stLDMRaceResult[].m_stResult[].m_stRank.m_u8RankValue
m_stLDMRaceResult[].m_stResult[].m_stEstimateRank.m_u8RankType
m_stLDMRaceResult[].m_stResult[].m_stEstimateRank.m_u8RankValue
	*/

	Write8(p, 0); //NeedFreshTutor
	Write8(p, 0); //hasMedalGameInfo
	//m_medalGameInfo[].m_curMedal


	Write8(p, PlayerNum); //KickOffPlayerNumber
	/*
m_stKickOffPlayer[].m_u32Uin
m_stKickOffPlayer[].m_szNickName[17]
m_stKickOffPlayer[].m_bGender
m_stKickOffPlayer[].m_bTeamID
m_stKickOffPlayer[].m_bCountry
	*/

	Write8(p, 0); //EliminatedRacerNum
	/*
m_astEliminatedRacer[].m_unUin
m_astEliminatedRacer[].m_szRacerName[17]
	*/

	Write8(p, 0); //TeamNum
	/*
m_astTeamScore[].m_ucTeamID
m_astTeamScore[].m_iTeamScore
m_astTeamScore[].m_ucTeamMinRank
m_astTeamScore[].m_ucCooperNum
m_astTeamScore[].m_ucIncCooper
m_astTeamScore[].m_ucCurCooper
	*/

	Write8(p, 0); //NPCRacerNum
	/*
m_astArrestNPCResultInfo[].m_iNpcID
m_astArrestNPCResultInfo[].m_szNPCName[17]
m_astArrestNPCResultInfo[].m_uchResultType
	*/

	Write8(p, 0); //hasGameFrameInfo
	/*
m_astGameFrameInfo[].m_uiUin
m_astGameFrameInfo[].m_iMapID
m_astGameFrameInfo[].m_ushFrameNum
m_astGameFrameInfo[].m_astFrameInfo[].m_ushCheckPointID
m_astGameFrameInfo[].m_astFrameInfo[].m_uiNum
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_unTimeElaps
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fInPosX[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fInPosY[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fInPosZ[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fStepX[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fStepY[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fStepZ[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fCarMat[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fOutPosX[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fOutPosY[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fOutPosZ[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fAccTime[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fFrameTime[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_ucSetPos
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_ucCollision
	*/

	Write8(p, 0); //hasDesperateEscapeTrophyInfo
	/*
m_astDesperateEscapeTrophyInfo[].m_uchNum
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_uiUin
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_uiSrcUin
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_uchTrophyType
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_uiID
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_uiRefitCount
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_uchStatus
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_uchHasRefit
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_uiUin
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iKartId
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iRefitCout
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shMaxFlags
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shWWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shSpeedWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shJetWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shSJetWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shAccuWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iShapeRefitCount
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iKartHeadRefitItemID
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iKartTailRefitItemID
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iKartFlankRefitItemID
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iKartTireRefitItemID
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iSecondRefitCount
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shSpeed2Weight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shDriftVecWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shAdditionalZSpeedWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shAntiCollisionWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shLuckyValue
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shRefitLuckyValueMaxWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iShapeSuitID
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_ucLegendSuitLevel
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iLegendSuitLevelChoice
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iShapeLegendSuitID
	*/

	Write8(p, 0); //HasGameStageInfo
	/*
m_stGameStageInfo[].m_u8StageIndex
m_stGameStageInfo[].m_u8TotalStageNum
	*/


	len = p - buf;
	SendToClient(Client, 513, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Notify);
}




void NotifyGameOver(ClientNode* Client, USHORT LeaveGameType, BYTE ParaNum, int* ParaList)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, 0); //ResultID

#ifndef ZingSpeed
	Write8(p, 0); //StageIndex
	Write8(p, 0); //ReturnHall
	Write8(p, 0); //WaitEnterRoom
	Write16(p, LeaveGameType); //LeaveGameType
	Write8(p, ParaNum); //ParaNum
	for (size_t i = 0; i < ParaNum; i++)
	{
		//m_aiParaList[]
		Write32(p, ParaList[i]);
	}
#endif

	len = p - buf;

	SendToClient(Client, 514, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Notify);
}

void RequestTransferByTCP(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	char Ver = Read8(Body);
	int Seq = Read32(Body);
	char DstNum = Read8(Body);

	struct DstInfo
	{
		USHORT PlayerID;
		UINT Uin;
	};
	DstInfo aDstInfo[6];
	for (char i = 0; i < DstNum; i++)
	{
		BYTE* pDstInfo = Body;
		USHORT len = Read16(pDstInfo);

		aDstInfo[i].PlayerID = Read16(pDstInfo);
		aDstInfo[i].Uin = Read32(pDstInfo);

		Body += len;
	}
	USHORT BuffLen = Read16(Body);


	RoomNode* Room = GetRoom(Client->RoomID);
	if (!Room)
	{
		return;
	}
	for (char i = 0; i < DstNum; i++)
	{
		for (char i2 = 0; i2 < 6; i2++)
		{
			ClientNode* RoomClient = Room->Player[i2];
			if (RoomClient)
			{
				if (RoomClient->Uin == aDstInfo[i].Uin)
				{
					if (RoomClient->IsReady)
					{
						NotifyTranferByTCP(RoomClient, Client->Uin, Client->ConnID, Seq, Body, BuffLen);
					}
					break;
				}
			}
		}
	}
}
void NotifyTranferByTCP(ClientNode* Client, UINT SrcUin, USHORT SrcPlayerID, int Seq, BYTE* Buff, int Bufflen)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write32(p, SrcUin);
	Write16(p, SrcPlayerID);
	Write8(p, 0); //Ver
	Write32(p, Seq);
	Write16(p, Bufflen);
	memcpy(p, Buff, Bufflen);
	p += Bufflen;


	len = p - buf;
	SendToClient(Client, 560, buf, len, SrcPlayerID, FE_PLAYER, Client->ConnID, Notify);
}


void NotifyAddPropBySkillStone(ClientNode* Client, int StoneSkillType, short PropID, int PropIndex)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write32(p, StoneSkillType);
	Write16(p, PropID);
	Write32(p, PropIndex);

	len = p - buf;
	SendToClient(Client, 908, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Notify);
}

void RequestUseProp2(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	short PropID = Read16(Body);
	UINT DstUin = Read32(Body);

	//m_aiCurrentPosition[3]
	Body += 3 * 4;

	int PropIndex = Read32(Body);
	short NewID = Read16(Body);
	char IsClearProp = Read8(Body);
	BYTE DstType = Read8(Body);
	int MyUseItemID = Read32(Body);
	int DstUseItemID = Read32(Body);
	BYTE IsConvertMode = Read8(Body);
	int StoneSkillType = Read32(Body);
	UINT LapTime = Read32(Body);
	BYTE DstUinNum = Read8(Body);
	//m_au32DstUin[]
	BYTE PropUseMode = Read8(Body);
	BYTE QueryUinNum = Read8(Body);
	//m_au32QueryUin[]
#ifndef ZingSpeed
	UCHAR CurrentSelfRank = Read8(Body);
	UCHAR CurrentTeammateRank = Read8(Body);
	UCHAR Position = Read8(Body);
	char PropSecType = Read8(Body);
	UCHAR PropPosIdx = Read8(Body);
	UINT NPCUin = Read32(Body);
	UCHAR UsePropProtoMode = Read8(Body);
	short SubPropID = Read16(Body);
	UCHAR OtherPropPosNum = Read8(Body);
	/*
	for (size_t i = 0; i < OtherPropPosNum; i++)
	{ //OtherPropPos
		BYTE* pOtherPropPos = p;
		len = Read16(pOtherPropPos);

		UINT Idx = Read32(pOtherPropPos);
		int[] Postion[] = Read32(pOtherPropPos);

		p += len;
	}
	*/
#endif

#ifdef DEBUG
	printf("PropID:%d, DstUin:%d, PropIndex:%d, StoneSkillType:%d\n", PropID, DstUin, PropIndex, StoneSkillType);
#endif

	int Prob = 380;
	if ((rand() % 1000) + Prob > 1000)
	{
		//发动 重生
		NotifyAddPropBySkillStone(Client, PropID, 1, PropIndex);
	}

}

void NotifySkillStoneKartInfo(ClientNode* Client)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write32(p, Client->Uin); //Uin

	BYTE* pStoneKartNum = p;
	Write32(p, 0); //StoneKartNum
	{
		int StoneKartNum = 0;
		const char* sql = NULL;
		sqlite3_stmt* stmt = NULL;
		int result;
		sql = "SELECT KartID,ID  FROM KartStoneGroove WHERE Uin = ?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, Client->Uin);
			while (sqlite3_step(stmt) == SQLITE_ROW)
			{ //KartStoneGrooveInfo
				StoneKartNum++;

				BYTE* pKartStoneGrooveInfo = p;
				Write16(pKartStoneGrooveInfo, 0); //len

				int KartID = sqlite3_column_int(stmt, 0);
				int ID = sqlite3_column_int(stmt, 1);
				Write32(pKartStoneGrooveInfo, KartID);

				BYTE* pStoneGrooveNum = pKartStoneGrooveInfo;
				Write32(pKartStoneGrooveInfo, 0); //StoneGrooveNum
				{
					int StoneGrooveNum = 0;
					const char* sql = NULL;
					sqlite3_stmt* stmt = NULL;
					int result;
					sql = "SELECT StoneUseOccaType,SkillStoneID  FROM KartStone WHERE ID = ?;";
					result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
					if (result == SQLITE_OK) {
						sqlite3_bind_int(stmt, 1, ID);
						while (sqlite3_step(stmt) == SQLITE_ROW)
						{ //StoneGrooveInfo
							StoneGrooveNum++;

							BYTE* pStoneGrooveInfo = pKartStoneGrooveInfo;
							Write16(pStoneGrooveInfo, 0); //len

							Write32(pStoneGrooveInfo, sqlite3_column_int(stmt, 0)); //StoneUseOccaType
							Write32(pStoneGrooveInfo, sqlite3_column_int(stmt, 1)); //SkillStoneID

							len = pStoneGrooveInfo - pKartStoneGrooveInfo;
							Set16(pKartStoneGrooveInfo, (WORD)len);
							pKartStoneGrooveInfo += len;
						}
					}
					else
					{
						printf("%s\n", sqlite3_errmsg(PlayerDB));
					}
					sqlite3_finalize(stmt);
					stmt = NULL;
					Set32(pStoneGrooveNum, StoneGrooveNum);
				}
				len = pKartStoneGrooveInfo - p;
				Set16(p, (WORD)len);
				p += len;
			}
		}
		else
		{
			printf("%s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
		Set32(pStoneKartNum, StoneKartNum);
	}

	len = p - buf;
	SendToClient(Client, 228, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void NotifyOtherKartStoneInfo(ClientNode* Client)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

	int ID = -1;
	sql = "SELECT ID  FROM KartStoneGroove  WHERE Uin=? AND KartID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, Client->KartID);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			ID = sqlite3_column_int(stmt, 0);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	if (ID == -1)
	{
		return;
	}

	Write32(p, 1); //OtherStoneKartNum
	{ //KartStoneGrooveInfo
		BYTE* paKartStoneGrooveInfo = p;
		Write16(paKartStoneGrooveInfo, 0); //len

		Write32(paKartStoneGrooveInfo, Client->Uin); //Uin
		{ //KartStoneGrooveInfo
			BYTE* pKartStoneGrooveInfo = paKartStoneGrooveInfo;
			Write16(pKartStoneGrooveInfo, 0); //len

			Write32(pKartStoneGrooveInfo, Client->KartID); //KartID
			BYTE* pStoneGrooveNum = pKartStoneGrooveInfo;
			Write32(pKartStoneGrooveInfo, 0); //StoneGrooveNum
			{
				int StoneGrooveNum = 0;
				const char* sql = NULL;
				sqlite3_stmt* stmt = NULL;
				int result;
				sql = "SELECT StoneUseOccaType,SkillStoneID  FROM KartStone WHERE ID = ?;";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK) {
					sqlite3_bind_int(stmt, 1, ID);
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{ //StoneGrooveInfo
						StoneGrooveNum++;

						BYTE* pStoneGrooveInfo = pKartStoneGrooveInfo;
						Write16(pStoneGrooveInfo, 0); //len

						Write32(pStoneGrooveInfo, sqlite3_column_int(stmt, 0)); //StoneUseOccaType
						Write32(pStoneGrooveInfo, sqlite3_column_int(stmt, 1)); //SkillStoneID

						len = pStoneGrooveInfo - pKartStoneGrooveInfo;
						Set16(pKartStoneGrooveInfo, (WORD)len);
						pKartStoneGrooveInfo += len;
					}
				}
				else
				{
					printf("%s\n", sqlite3_errmsg(PlayerDB));
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				Set32(pStoneGrooveNum, StoneGrooveNum);
			}

			len = pKartStoneGrooveInfo - paKartStoneGrooveInfo;
			Set16(paKartStoneGrooveInfo, (WORD)len);
			paKartStoneGrooveInfo += len;
		}

		len = paKartStoneGrooveInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 907, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestGetProp(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	short PropPositionNO = Read16(p);
	bool CurrentRank = Read8(p);
	//int CurrentPosition[3] = Read32(p);
	//int PreviousInterval = Read32(p);
	//char GetPropType = Read8(p);
	//bool TeammateRank = Read8(p);
	//char PointArrayIdx = Read8(p);
	//UINT LapTime = Read32(p);
#ifndef ZingSpeed
	//char GetPropSecType = Read8(p);
	//UCHAR PropPosIdx = Read8(p);
	//UINT NPCUin = Read32(p);
	//UCHAR ParaNum = Read8(p);
	//UINT ParaList[] = Read32(p);
#endif

	ResponseGetProp(Client);
}
void ResponseGetProp(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	static WORD PropIDs[] = { 1,2,3,4,5,6,7,8,9,10,15,16,18,21,25,27,41,43,44,45,46,47,49,50,51,53 };
	static int Index = 0;
	Index++;
	if (Index >= sizeof(PropIDs) / sizeof(WORD))
	{
		Index = 0;
	}
	printf("%d\n", Index);

	Write16(p, 0); //ResultID
	Write32(p, Client->Uin); //Uin
	Write16(p, PropIDs[Index]); //PropID
	Write32(p, 0); //PropIndex
	Write8(p, 0); //GetPropType
	Write32(p, 0); //AwardItemID
	Write8(p, 0); //ReasonLen
	Write16(p, 0); //PropPositionNO
	Write32(p, 0); //ActIDForClient
#ifndef ZingSpeed
	Write8(p, 0); //GetPropSecType
	Write8(p, 0); //PropPosIdx
	Write32(p, 0); //NPCUin
	Write8(p, 0); //ParaNum
	Write32(p, 0); //ParaList[]
#endif


	len = p - buf;
	SendToClient(Client, 124, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}


void NotifyMsgBox(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write8(p, 0); //MsgType
	{ //MsgBox
		BYTE* pMsgBox = p;
		Write16(pMsgBox, 0); //len

		{ //OnlineTaskMsg
			BYTE* pOnlineTaskMsg = pMsgBox;
			Write16(pOnlineTaskMsg, 0); //len

			Write8(pOnlineTaskMsg, 0); //MsgType
			Write32(pOnlineTaskMsg, 0); //OpenDate
			Write32(pOnlineTaskMsg, 0); //OpenTime
			Write32(pOnlineTaskMsg, 0); //CloseDate
			Write32(pOnlineTaskMsg, 0); //CloseTime
			Write8(pOnlineTaskMsg, 0); //IsToday
			Write8(pOnlineTaskMsg, 0); //URLLen
			Write8(pOnlineTaskMsg, 0); //WeekLimit

			len = pOnlineTaskMsg - pMsgBox;
			Set16(pMsgBox, (WORD)len);
			pMsgBox += len;
		}
		{ //VipPages
			BYTE* pVipPages = pMsgBox;
			Write16(pVipPages, 0); //len

			Write8(pVipPages, 0); //URLLen1
			Write8(pVipPages, 0); //URLLen2
			Write8(pVipPages, 0); //URLLen3

			len = pVipPages - pMsgBox;
			Set16(pMsgBox, (WORD)len);
			pMsgBox += len;
		}
		{ //ActivitiesAd
			BYTE* pActivitiesAd = pMsgBox;
			Write16(pActivitiesAd, 0); //len

			Write8(pActivitiesAd, 0); //URLLen

			len = pActivitiesAd - pMsgBox;
			Set16(pMsgBox, (WORD)len);
			pMsgBox += len;
		}
		{ //MatchSchedule
			BYTE* pMatchSchedule = pMsgBox;
			Write16(pMatchSchedule, 0); //len

			Write8(pMatchSchedule, 0); //URLLen

			len = pMatchSchedule - pMsgBox;
			Set16(pMsgBox, (WORD)len);
			pMsgBox += len;
		}
		{ //LoadingAd
			BYTE* pLoadingAd = pMsgBox;
			Write16(pLoadingAd, 0); //len

			Write8(pLoadingAd, 0); //Ver
			Write8(pLoadingAd, 0); //URLLen

			len = pLoadingAd - pMsgBox;
			Set16(pMsgBox, (WORD)len);
			pMsgBox += len;
		}
		{ //TaskAdvAd
			BYTE* pTaskAdvAd = pMsgBox;
			Write16(pTaskAdvAd, 0); //len

			Write8(pTaskAdvAd, 0); //URLLen

			len = pTaskAdvAd - pMsgBox;
			Set16(pMsgBox, (WORD)len);
			pMsgBox += len;
		}
		{ //LoginAd
			BYTE* pLoginAd = pMsgBox;
			Write16(pLoginAd, 0); //len

			Write8(pLoginAd, 0); //URLLen

			len = pLoginAd - pMsgBox;
			Set16(pMsgBox, (WORD)len);
			pMsgBox += len;
		}
		{ //T3Ad
			BYTE* pT3Ad = pMsgBox;
			Write16(pT3Ad, 0); //len

			Write8(pT3Ad, 0); //URLLen1
			Write8(pT3Ad, 0); //URLLen2
			Write8(pT3Ad, 0); //URLLen3
			Write8(pT3Ad, 0); //URLLen4

			len = pT3Ad - pMsgBox;
			Set16(pMsgBox, (WORD)len);
			pMsgBox += len;
		}

		len = pMsgBox - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 596, buf, len, Client->ServerID, FE_GAMESVRD, Client->ConnID, Notify);
}

void NotifySvrConfig(ClientNode* Client)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write32(p, 5000); //GuildScoreThreshold

	{ //LuckyMatchCfg
		BYTE* pLuckyMatchCfg = p;
		Write16(pLuckyMatchCfg, 0); //len

		const char Time[50] = "";
		memcpy(pLuckyMatchCfg, Time, 50);
		pLuckyMatchCfg += 50;

		const char Award[50] = "";
		memcpy(pLuckyMatchCfg, Award, 50);
		pLuckyMatchCfg += 50;

		const char StartCondition[50] = "";
		memcpy(pLuckyMatchCfg, StartCondition, 50);
		pLuckyMatchCfg += 50;

		Write32(pLuckyMatchCfg, 0); //LuckyMoney

		const char MatchTitle[50] = "";
		memcpy(pLuckyMatchCfg, MatchTitle, 50);
		pLuckyMatchCfg += 50;


		len = pLuckyMatchCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ //MsgStoneSysCfg
		BYTE* pMsgStoneSysCfg = p;
		Write16(pMsgStoneSysCfg, 0); //len

		Write8(pMsgStoneSysCfg, 1); //StoneSkillTakeEffect

		{ //SkillStoneCfg
			BYTE* pSkillStoneCfg = pMsgStoneSysCfg;
			Write16(pSkillStoneCfg, 0); //len


			BYTE* pSkillStoneNum = pSkillStoneCfg;
			Write32(pSkillStoneCfg, 0); //SkillStoneNum
			try
			{
				YAML::Node Config = YAML::LoadFile("SingleSkillStoneCfg.yml");
				size_t Num = Config.size();
				for (size_t i = 0; i < Num; i++)
				{
					YAML::Node SingleSkillStoneCfg = Config[i];
					{ //SingleSkillStoneCfg
						BYTE* pSingleSkillStoneCfg = pSkillStoneCfg;
						Write16(pSingleSkillStoneCfg, 0); //len

						Write32(pSingleSkillStoneCfg, SingleSkillStoneCfg["StoneID"].as<int>());
						Write32(pSingleSkillStoneCfg, SingleSkillStoneCfg["ActiveCondVal1"].as<int>());
						Write32(pSingleSkillStoneCfg, SingleSkillStoneCfg["UseCountUpperlimit"].as<int>());
						Write32(pSingleSkillStoneCfg, SingleSkillStoneCfg["ActiveSuccessProb"].as<int>());
						Write32(pSingleSkillStoneCfg, SingleSkillStoneCfg["CoolTime"].as<int>());
						Write32(pSingleSkillStoneCfg, SingleSkillStoneCfg["GenResultVal1"].as<int>());

						len = pSingleSkillStoneCfg - pSkillStoneCfg;
						Set16(pSkillStoneCfg, (WORD)len);
						pSkillStoneCfg += len;
					}
				}
				Set32(pSkillStoneNum, Num);
			}
			catch (const std::exception&)
			{
				return;
			}


			len = pSkillStoneCfg - pMsgStoneSysCfg;
			Set16(pMsgStoneSysCfg, (WORD)len);
			pMsgStoneSysCfg += len;
		}

		Write32(pMsgStoneSysCfg, 0); //Kart2StoneGrooveNum
		/*
m_stMsgStoneSysCfg.m_astKart2StoneGrooveCfg[].m_iKartID
m_stMsgStoneSysCfg.m_astKart2StoneGrooveCfg[].m_iInitStoneGrooveNum
m_stMsgStoneSysCfg.m_astKart2StoneGrooveCfg[].m_aiStoneUseOccasionType[MAX_KART_STONE_GROOVE_NUM]
		*/


		len = pMsgStoneSysCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}


	{ //UITitleDispInfo
		BYTE* pUITitleDispInfo = p;
		Write16(pUITitleDispInfo, 0); //len

		struct
		{
			bool GameName : 1;
			bool Version : 1;
			bool WorldName : 1;
			bool Ad : 1;
			bool NickName : 1;
			bool Unknown1 : 1;
			bool Unknown2 : 1;
			bool Unknown3 : 1;
		} DisplayBitMap{ 0,0,1,1,1,0,0,0 };
		memcpy(pUITitleDispInfo, &DisplayBitMap, sizeof(DisplayBitMap));
		pUITitleDispInfo += sizeof(DisplayBitMap);

		{ //ADText
			BYTE* pADText = pUITitleDispInfo;
			Write16(pADText, 0); //len

			len = strlen(UITitleDispInfo_ADText);
			Write8(pADText, (BYTE)len); //TextLen
			memcpy(pADText, UITitleDispInfo_ADText, len);
			pADText += len;

			len = pADText - pUITitleDispInfo;
			Set16(pUITitleDispInfo, (WORD)len);
			pUITitleDispInfo += len;
		}

		len = pUITitleDispInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ //LoaderTipInfo
		BYTE* pLoaderTipInfo = p;
		Write16(pLoaderTipInfo, 0); //len

		Write32(pLoaderTipInfo, 0); //Version
		Write8(pLoaderTipInfo, 0); //TipsNum
		//m_stLoaderTipInfo.m_astLoaderTipText[].m_u8TextLen


		len = pLoaderTipInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ //MailSysAD
		BYTE* pMailSysAD = p;
		Write16(pMailSysAD, 0); //len

		const char* ADText = "欢迎登录Stars服,祝您游玩愉快.";
		len = strlen(ADText);
		Write16(pMailSysAD, (WORD)len); //TextLen
		memcpy(pMailSysAD, ADText, len);
		pMailSysAD += len;


		len = pMailSysAD - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ //CheatReportCfg
		BYTE* pCheatReportCfg = p;
		Write16(pCheatReportCfg, 0); //len

		Write8(pCheatReportCfg, 1); //ReplayDayNum
		Write8(pCheatReportCfg, 1); //IsReportCheatEnable

		len = pCheatReportCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); //MaintainableKartNum
	//m_aiMaintainableKartID[]


	Write32(p, 0); //CommonBitSwitch
	Write8(p, 0); //QQKart2ButtonLen

	{ //GuildEnlistCfg
		BYTE* pGuildEnlistCfg = p;
		Write16(pGuildEnlistCfg, 0); //len

		Write8(pGuildEnlistCfg, 0); //MinUnloginDay
		Write8(pGuildEnlistCfg, 0); //CdDay
		Write8(pGuildEnlistCfg, 0); //MaxAwardTimeOneDay

		len = pGuildEnlistCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, true); //CloseQT
	Write32(p, 0); //QTCommonRoomID
	Write8(p, 0); //WlFetionDayLimit
	Write8(p, 0); //ShowShopIcon
	Write32(p, 0); //TiroRecommendMapNum
	//m_aiTiroRecommendMapIdList[]
	Write16(p, 0); //BoxMutiOpenLimit
	Write8(p, 0); //HallButtonShow
	Write8(p, 0); //HallButtonShowUrlLen
	Write32(p, 0); //EndlessModeBaseScorePerLevel
#ifndef ZingSpeed
	Write32(p, 0); //GetShopNonSaleInfoInterval
	{ //ShuttleRandMap
		BYTE* pShuttleRandMap = p;
		Write16(pShuttleRandMap, 0); //len

		Write16(pShuttleRandMap, 0); //BeginNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //BeginMapList
			BYTE* pBeginMapList = pShuttleRandMap;
			Write16(pBeginMapList, 0); //len

			Write32(pBeginMapList, 0); //MapID
			Write16(pBeginMapList, 0); //BeginIdx
			Write16(pBeginMapList, 0); //EndIdx
			Write8(pBeginMapList, 0); //Reverse
			Write16(pBeginMapList, 0); //BeginCheckPointIdx

			len = pBeginMapList - pShuttleRandMap;
			Set16(pShuttleRandMap, (WORD)len);
			pShuttleRandMap += len;
		}
		*/
		Write16(pShuttleRandMap, 0); //MidNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //MidMapList
			BYTE* pMidMapList = pShuttleRandMap;
			Write16(pMidMapList, 0); //len

			Write32(pMidMapList, 0); //MapID
			Write16(pMidMapList, 0); //BeginIdx
			Write16(pMidMapList, 0); //EndIdx
			Write8(pMidMapList, 0); //Reverse
			Write16(pMidMapList, 0); //BeginCheckPointIdx

			len = pMidMapList - pShuttleRandMap;
			Set16(pShuttleRandMap, (WORD)len);
			pShuttleRandMap += len;
		}
		*/
		Write16(pShuttleRandMap, 0); //EndNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //EndMapList
			BYTE* pEndMapList = pShuttleRandMap;
			Write16(pEndMapList, 0); //len

			Write32(pEndMapList, 0); //MapID
			Write16(pEndMapList, 0); //BeginIdx
			Write16(pEndMapList, 0); //EndIdx
			Write8(pEndMapList, 0); //Reverse
			Write16(pEndMapList, 0); //BeginCheckPointIdx

			len = pEndMapList - pShuttleRandMap;
			Set16(pShuttleRandMap, (WORD)len);
			pShuttleRandMap += len;
		}
		*/
		len = pShuttleRandMap - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); //HasWeeklyRecommendModeClientInfo
	/*
	for (size_t i = 0; i < n; i++)
	{ //WeeklyRecommendModeClientInfo
		BYTE* pWeeklyRecommendModeClientInfo = p;
		Write16(pWeeklyRecommendModeClientInfo, 0); //len

		Write32(pWeeklyRecommendModeClientInfo, 0); //OpenDate
		Write32(pWeeklyRecommendModeClientInfo, 0); //CloseDate
		Write32(pWeeklyRecommendModeClientInfo, 0); //OpenTime
		Write32(pWeeklyRecommendModeClientInfo, 0); //CloseTime
		Write8(pWeeklyRecommendModeClientInfo, 0); //WeekLimitNum
		Write8(pWeeklyRecommendModeClientInfo, 0); //WeekLimits[]
		Write32(pWeeklyRecommendModeClientInfo, 0); //DesktopSpecialShowFlag
		Write16(pWeeklyRecommendModeClientInfo, 0); //BaseMode
		Write16(pWeeklyRecommendModeClientInfo, 0); //SubMode
		Write32(pWeeklyRecommendModeClientInfo, 0); //SpecialFlag
		Write(pWeeklyRecommendModeClientInfo, 0); //AwardTriggerDesc[]
		Write8(pWeeklyRecommendModeClientInfo, 0); //AwardNum
		Write32(pWeeklyRecommendModeClientInfo, 0); //AwardItemID[]

		len = pWeeklyRecommendModeClientInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write32(p, 0); //ReportWeeklyRecommendModeTriggerInfoInterval
	Write32(p, 0); //MaxEnergy
	Write32(p, 0); //N2ORatio
	Write32(p, 0); //PlayerNumRatio[]
	Write32(p, 0); //IsOpenBuf
	Write8(p, 0); //CouponTreasureGameButtonShow
	Write8(p, 0); //NewYearFinancingShow
	Write32(p, 0); //GuildTeamSignatureReportNumsLimit
	Write32(p, 0); //GetRelationItemMaxTimeout
	Write32(p, 0); //ClientWaitGameBeginTimeout

#endif

	len = p - buf;
	SendToClient(Client, 800, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifySvrConfig2(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write8(p, 0); //HaveTopMapRecord
	/*
	{ //TopMapRecordConfig
		BYTE* pTopMapRecordConfig = p;
		Write16(pTopMapRecordConfig, 0); //len

		Write32(pTopMapRecordConfig, 0); //Version
		Write32(pTopMapRecordConfig, 0); //RecordDisplayLimit
		Write8(pTopMapRecordConfig, 0); //RecordNumber
		for (size_t i = 0; i < n; i++)
		{ //Record
			BYTE* pRecord = pTopMapRecordConfig;
			Write16(pRecord, 0); //len

			Write32(pRecord, 0); //MapId
			Write32(pRecord, 0); //Record
			Write32(pRecord, 0); //Uin
			Write8(pRecord, 0); //WorldId
			Write8(pRecord, 0); //NameLength
			Write8(pRecord, 0); //UrlLength

			len = pRecord - pTopMapRecordConfig;
			Set16(pTopMapRecordConfig, (WORD)len);
			pTopMapRecordConfig += len;
		}

		len = pTopMapRecordConfig - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write8(p, 0); //HaveKartRoomPushInfo
	/*
	{ //KartRoomPushInfo
		BYTE* pKartRoomPushInfo = p;
		Write16(pKartRoomPushInfo, 0); //len

		Write32(pKartRoomPushInfo, 0); //Version
		Write16(pKartRoomPushInfo, 0); //PushItemNumber
		for (size_t i = 0; i < n; i++)
		{ //PushItem
			BYTE* pPushItem = pKartRoomPushInfo;
			Write16(pPushItem, 0); //len

			Write32(pPushItem, 0); //KartId
			Write8(pPushItem, 0); //TextLength

			len = pPushItem - pKartRoomPushInfo;
			Set16(pKartRoomPushInfo, (WORD)len);
			pKartRoomPushInfo += len;
		}

		len = pKartRoomPushInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	{ //ClickStreamCfg
		BYTE* pClickStreamCfg = p;
		Write16(pClickStreamCfg, 0); //len

		Write8(pClickStreamCfg, 2); //ReportMode

		Write8(pClickStreamCfg, 3); //ContextNum
		Write8(pClickStreamCfg, 1);
		Write8(pClickStreamCfg, 2);
		Write8(pClickStreamCfg, 3);

		Write32(pClickStreamCfg, 100); //MaxNum

		len = pClickStreamCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 1); //IsGuildPKOpen
	Write8(p, 1); //IsEightYearsOpen
	Write8(p, 0); //HaveSkatePropGameLogicCfg
	/*
	{ //SkatePropGameLogicCfg
		BYTE* pSkatePropGameLogicCfg = p;
		Write16(pSkatePropGameLogicCfg, 0); //len

		Write32(pSkatePropGameLogicCfg, 0); //PropAddN2ONum
		Write32(pSkatePropGameLogicCfg, 0); //N2OSpeed
		Write32(pSkatePropGameLogicCfg, 0); //ZanAddScoreNum
		Write32(pSkatePropGameLogicCfg, 0); //GetSpecialPropAddCoinNum
		Write32(pSkatePropGameLogicCfg, 0); //SkatePropHitCD
		Write32(pSkatePropGameLogicCfg, 0); //SkatePropHit_CoinSubNum
		Write32(pSkatePropGameLogicCfg, 0); //MaxOneGameLogicGetScoreNum

		len = pSkatePropGameLogicCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	Write16(p, 0); //SkatePropShoeNum
	//Write32(p, 0); //SkatePropShoeList[]

	Write16(p, 0); //SkatePropShoeNumForMultiGame
	//Write32(p, 0); //SkatePropShoeListForMultiGame[]

	Write32(p, 1); //AddAngleTime

	const int EMBM_MAX = 47;
	for (size_t i = 0; i < EMBM_MAX; i++)
	{ //ClassicMapInfo
		BYTE* pClassicMapInfo = p;
		Write16(pClassicMapInfo, 0); //len

		Write32(pClassicMapInfo, 0); //MapNums
		//Write32(pClassicMapInfo, 0); //MapIDS[]

		len = pClassicMapInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	
	Write8(p, 0); //OpenPopAds
	Write8(p, 0); //IsCloseSaveKeyTransInfo
	Write8(p, 0); //IsCloseSuperMoneySecondPsw
	Write32(p, 0); //NewMapNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //NewMapTipsCfg
		BYTE* pNewMapTipsCfg = p;
		Write16(pNewMapTipsCfg, 0); //len

		Write32(pNewMapTipsCfg, 0); //Mapid
		Write16(pNewMapTipsCfg, 0); //WordLen

		len = pNewMapTipsCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	for(size_t i = 0; i < 16; i++)
	{
		Write8(p, 0); //ClientSwitch[]
	}

	Write8(p, 0); //OpenPwdRedEnvelop
	Write8(p, 0); //IsUseTCLS
	Write8(p, 0); //IsPropTeamMatchShow

	Write32(p, 0); //CfgNums
	/*
	for (size_t i = 0; i < 1; i++)
	{ //CommonItemTopListInfos
		BYTE* pCommonItemTopListInfos = p;
		Write16(pCommonItemTopListInfos, 0); //len

		Write32(pCommonItemTopListInfos, 75827); //ItemID

		Write16(pCommonItemTopListInfos, 1); //ExtendInfoNums
		Write32(pCommonItemTopListInfos, 0); //ExtendInfoForItemGene[]

		Write16(pCommonItemTopListInfos, 1); //UpdateTopListNums
		Write32(pCommonItemTopListInfos, 0); //TopListID[]

		Write32(pCommonItemTopListInfos, 0); //ActiveDate
		Write32(pCommonItemTopListInfos, 0); //InActiveDate
		Write32(pCommonItemTopListInfos, 0); //ActiveTime
		Write32(pCommonItemTopListInfos, 0); //InActiveTime
		Write8(pCommonItemTopListInfos, 1); //UpdateTopList
		Write32(pCommonItemTopListInfos, 0); //ExtendInfoForLastUpdateTime
		Write32(pCommonItemTopListInfos, 0); //DailyTopListExtendInfoID
		Write32(pCommonItemTopListInfos, 0); //UpdateDate

		len = pCommonItemTopListInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write8(p, 0); //IsShowBoardAllareaBubble
	Write8(p, 0); //RemindClearSuperMoneyLeftDays
	Write8(p, 0); //RemindClearSuperMoneyLeftHours
	Write32(p, 0); //RemindClearSuperMoneyThreshold
	Write16(p, 0); //RemindClearSuperMoneyInterval
	Write32(p, 0); //HPJMaxAccFuel
	Write32(p, 0); //HPJWildStatusAccelParam
	Write8(p, 0); //SwitchNum
	Write8(p, 1); //UseSign3
	Write8(p, 0); //OpenRankedMatchSpeedKing
	Write8(p, 0); //OpenLimitMapCollect
	Write8(p, 0); //HasMoreInfo

	len = p - buf;
	SendToClient(Client, 822, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifySvrConfig3(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	try
	{
		WriteKartPhysParam(p, 0, 0, 0, 0); //NPCKartPhysPara
	}
	catch (const std::exception&)
	{
		return;
	}
	
	const int MAX_NPC_AI_LEVEL_NUM = 12;
	for (size_t i = 0; i < MAX_NPC_AI_LEVEL_NUM; i++)
	{ //NPCKartAIPhysPara
		BYTE* pNPCKartAIPhysPara = p;
		Write16(pNPCKartAIPhysPara, 0); //len

		Write32(pNPCKartAIPhysPara, 0); //NpcN2OIntervalTime
		Write32(pNPCKartAIPhysPara, 0); //NpcTriggerN2ORate
		Write32(pNPCKartAIPhysPara, 0); //NpcRestCheckTime
		Write32(pNPCKartAIPhysPara, 0); //NpcFixVelocity
		Write32(pNPCKartAIPhysPara, 0); //NpcFixVelocityRate
		Write32(pNPCKartAIPhysPara, 0); //NpcRunDisHulanThreshold
		Write32(pNPCKartAIPhysPara, 0); //NpcSJetRate
		Write32(pNPCKartAIPhysPara, 0); //NpcTurnAjustDisHulanRate

		Write8(pNPCKartAIPhysPara, 0); //ForwardAccelNum
		/*
		for (size_t i = 0; i < 0; i++)
		{ //ForwardAccel
			BYTE* pForwardAccel = pNPCKartAIPhysPara;
			Write16(pForwardAccel, 0); //len

			Write32(pForwardAccel, 0); //Key
			Write32(pForwardAccel, 0); //Value

			len = pForwardAccel - pNPCKartAIPhysPara;
			Set16(pNPCKartAIPhysPara, (WORD)len);
			pNPCKartAIPhysPara += len;
		}
		*/

		Write8(pNPCKartAIPhysPara, 0); //BackwardAccelNum
		/*
		for (size_t i = 0; i < 0; i++)
		{ //BackwardAccel
			BYTE* pBackwardAccel = pNPCKartAIPhysPara;
			Write16(pBackwardAccel, 0); //len

			Write32(pBackwardAccel, 0); //Key
			Write32(pBackwardAccel, 0); //Value

			len = pBackwardAccel - pNPCKartAIPhysPara;
			Set16(pNPCKartAIPhysPara, (WORD)len);
			pNPCKartAIPhysPara += len;
		}
		*/

		Write32(pNPCKartAIPhysPara, 0); //StraightLenThreshold
		Write32(pNPCKartAIPhysPara, 0); //NpcDriftRate

		Write8(pNPCKartAIPhysPara, 0); //CompensateParamCount
		/*
		for (size_t i = 0; i < 0; i++)
		{ //CompensateParam
			BYTE* pCompensateParam = pNPCKartAIPhysPara;
			Write16(pCompensateParam, 0); //len

			Write32(pCompensateParam, 0); //Percent
			Write32(pCompensateParam, 0); //Value

			len = pCompensateParam - pNPCKartAIPhysPara;
			Set16(pNPCKartAIPhysPara, (WORD)len);
			pNPCKartAIPhysPara += len;
		}
		*/

		len = pNPCKartAIPhysPara - p;
		Set16(p, (WORD)len);
		p += len;
	}

	try
	{
		WriteKartPhysParam(p, 0, 0, 0, 0); //NPCKartPhysPara_NpcPlayer
	}
	catch (const std::exception&)
	{
		return;
	}
	

	Write8(p, 12); //LevelNum
	for (size_t i = 0; i < 12; i++)
	{ //NPCKartAIPhysPara_NpcPlayer
		BYTE* pNPCKartAIPhysPara_NpcPlayer = p;
		Write16(pNPCKartAIPhysPara_NpcPlayer, 0); //len

		{ //Param
			BYTE* pParam = pNPCKartAIPhysPara_NpcPlayer;
			Write16(pParam, 0); //len

			Write32(pParam, 0); //NpcN2OIntervalTime
			Write32(pParam, 0); //NpcTriggerN2ORate
			Write32(pParam, 0); //NpcRestCheckTime
			Write32(pParam, 0); //NpcFixVelocity
			Write32(pParam, 0); //NpcFixVelocityRate
			Write32(pParam, 0); //NpcRunDisHulanThreshold
			Write32(pParam, 0); //NpcSJetRate
			Write32(pParam, 0); //NpcTurnAjustDisHulanRate

			Write8(pParam, 0); //ForwardAccelNum
			/*
			for (size_t i = 0; i < 0; i++)
			{ //ForwardAccel
				BYTE* pForwardAccel = pParam;
				Write16(pForwardAccel, 0); //len

				Write32(pForwardAccel, 0); //Key
				Write32(pForwardAccel, 0); //Value

				len = pForwardAccel - pParam;
				Set16(pParam, (WORD)len);
				pParam += len;
			}
			*/

			Write8(pParam, 0); //BackwardAccelNum
			/*
			for (size_t i = 0; i < 1; i++)
			{ //BackwardAccel
				BYTE* pBackwardAccel = pParam;
				Write16(pBackwardAccel, 0); //len

				Write32(pBackwardAccel, 0); //Key
				Write32(pBackwardAccel, 0); //Value

				len = pBackwardAccel - pParam;
				Set16(pParam, (WORD)len);
				pParam += len;
			}
			*/
			Write32(pParam, 0); //StraightLenThreshold
			Write32(pParam, 0); //NpcDriftRate

			Write8(pParam, 0); //CompensateParamCount
			/*
			for (size_t i = 0; i < 1; i++)
			{ //CompensateParam
				BYTE* pCompensateParam = pParam;
				Write16(pCompensateParam, 0); //len

				Write32(pCompensateParam, 0); //Percent
				Write32(pCompensateParam, 0); //Value

				len = pCompensateParam - pParam;
				Set16(pParam, (WORD)len);
				pParam += len;
			}
			*/

			len = pParam - pNPCKartAIPhysPara_NpcPlayer;
			Set16(pNPCKartAIPhysPara_NpcPlayer, (WORD)len);
			pNPCKartAIPhysPara_NpcPlayer += len;
		}
		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); //N2OStartTime
		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); //NpcN2OMaxCount
		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); //Dis2FirstPlay
		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); //LostControlTime
		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); //LostInterval
		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); //LostControlCount

		len = pNPCKartAIPhysPara_NpcPlayer - p;
		Set16(p, (WORD)len);
		p += len;
	}

	try
	{
		WriteKartPhysParam(p, 0, 0, 0, 0); //NPCKartPhysPara_RankedMatch
	}
	catch (const std::exception&)
	{
		return;
	}

	Write8(p, 12); //NPCLevelNum_RankedMatch
	for (size_t i = 0; i < 12; i++)
	{ //NPCKartAIPhysPara_RankedMatch
		BYTE* pNPCKartAIPhysPara_RankedMatch = p;
		Write16(pNPCKartAIPhysPara_RankedMatch, 0); //len

		{ //Param
			BYTE* pParam = pNPCKartAIPhysPara_RankedMatch;
			Write16(pParam, 0); //len

			Write32(pParam, 0); //NpcN2OIntervalTime
			Write32(pParam, 0); //NpcTriggerN2ORate
			Write32(pParam, 0); //NpcRestCheckTime
			Write32(pParam, 0); //NpcFixVelocity
			Write32(pParam, 0); //NpcFixVelocityRate
			Write32(pParam, 0); //NpcRunDisHulanThreshold
			Write32(pParam, 0); //NpcSJetRate
			Write32(pParam, 0); //NpcTurnAjustDisHulanRate

			Write8(pParam, 0); //ForwardAccelNum
			/*
			for (size_t i = 0; i < 0; i++)
			{ //ForwardAccel
				BYTE* pForwardAccel = pParam;
				Write16(pForwardAccel, 0); //len

				Write32(pForwardAccel, 0); //Key
				Write32(pForwardAccel, 0); //Value

				len = pForwardAccel - pParam;
				Set16(pParam, (WORD)len);
				pParam += len;
			}
			*/
			Write8(pParam, 0); //BackwardAccelNum
			/*
			for (size_t i = 0; i < 0; i++)
			{ //BackwardAccel
				BYTE* pBackwardAccel = pParam;
				Write16(pBackwardAccel, 0); //len

				Write32(pBackwardAccel, 0); //Key
				Write32(pBackwardAccel, 0); //Value

				len = pBackwardAccel - pParam;
				Set16(pParam, (WORD)len);
				pParam += len;
			}
			*/
			Write32(pParam, 0); //StraightLenThreshold
			Write32(pParam, 0); //NpcDriftRate

			Write8(pParam, 0); //CompensateParamCount
			/*
			for (size_t i = 0; i < 0; i++)
			{ //CompensateParam
				BYTE* pCompensateParam = pParam;
				Write16(pCompensateParam, 0); //len

				Write32(pCompensateParam, 0); //Percent
				Write32(pCompensateParam, 0); //Value

				len = pCompensateParam - pParam;
				Set16(pParam, (WORD)len);
				pParam += len;
			}
			*/
			len = pParam - pNPCKartAIPhysPara_RankedMatch;
			Set16(pNPCKartAIPhysPara_RankedMatch, (WORD)len);
			pNPCKartAIPhysPara_RankedMatch += len;
		}
		Write32(pNPCKartAIPhysPara_RankedMatch, 0); //N2OStartTime
		Write32(pNPCKartAIPhysPara_RankedMatch, 0); //NpcN2OMaxCount
		Write32(pNPCKartAIPhysPara_RankedMatch, 0); //Dis2FirstPlay
		Write32(pNPCKartAIPhysPara_RankedMatch, 0); //LostControlTime
		Write32(pNPCKartAIPhysPara_RankedMatch, 0); //LostInterval
		Write32(pNPCKartAIPhysPara_RankedMatch, 0); //LostControlCount

		len = pNPCKartAIPhysPara_RankedMatch - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 0x0003BC27); //SwitchFlag1
	Write16(p, 60); //MaxQuickLerpThrehold
	Write16(p, 10); //QuickLerpStepCnts
	Write16(p, 12); //LerpSynccpFrequence

	Write16(p, 3); //ClientItemOpenTypeNum
	{ //ClientItemOpenCfg
		BYTE* pClientItemOpenCfg = p;
		Write16(pClientItemOpenCfg, 0); //len

		Write16(pClientItemOpenCfg, 5); //OpenType
		Write8(pClientItemOpenCfg, 5); //ItemNum
		Write32(pClientItemOpenCfg, 0); //ItemList[]

		len = pClientItemOpenCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ //ClientItemOpenCfg
		BYTE* pClientItemOpenCfg = p;
		Write16(pClientItemOpenCfg, 0); //len

		Write16(pClientItemOpenCfg, 10); //OpenType
		Write8(pClientItemOpenCfg, 10); //ItemNum
		Write32(pClientItemOpenCfg, 0); //ItemList[]

		len = pClientItemOpenCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ //ClientItemOpenCfg
		BYTE* pClientItemOpenCfg = p;
		Write16(pClientItemOpenCfg, 0); //len

		Write16(pClientItemOpenCfg, 50); //OpenType
		Write8(pClientItemOpenCfg, 50); //ItemNum
		Write32(pClientItemOpenCfg, 0); //ItemList[]

		len = pClientItemOpenCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	/*
	for (size_t i = 0; i < n; i++)
	{ //ClientItemOpenCfg
		BYTE* pClientItemOpenCfg = p;
		Write16(pClientItemOpenCfg, 0); //len

		Write16(pClientItemOpenCfg, 0); //OpenType
		Write8(pClientItemOpenCfg, 0); //ItemNum
		Write32(pClientItemOpenCfg, 0); //ItemList[]

		len = pClientItemOpenCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	Write8(p, 1); //OtherNum
	for (size_t i = 0; i < 1; i++)
	{ //CollisionPowerOtherInfo
		BYTE* pCollisionPowerOtherInfo = p;
		Write16(pCollisionPowerOtherInfo, 0); //len

		Write32(pCollisionPowerOtherInfo, 0); //Power
		Write32(pCollisionPowerOtherInfo, 100); //Ratio
		Write32(pCollisionPowerOtherInfo, 0); //Add

		len = pCollisionPowerOtherInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 400); //CollisionMinPower
	Write32(p, 2900); //ItemExtendLimit
	Write32(p, 50); //ChattingTalkLevelLimit
	Write32(p, 0); //ChattingSpecialOp

	Write8(p, 0); //ForbidMapNum
	//Write32(p, 0); //ForbidMap[]

	Write32(p, 20000); //MaxBuySpeedNum

	Write8(p, 0); //ClientItemIconNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //ClientIconCfg
		BYTE* pClientIconCfg = p;
		Write16(pClientIconCfg, 0); //len

		Write32(pClientIconCfg, 0); //ItemID
		Write8(pClientIconCfg, 0); //IconPathLen

		len = pClientIconCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	Write8(p, 0); //TDCBlackCarNum
	//Write32(p, 0); //TDCBlackCar[]

	Write8(p, 0); //TDCBlackCarTypeNum
	//Write32(p, 0); //TDCBlackCarType[]

	Write8(p, 0); //TDCBlackCarType2ndNum
	//Write32(p, 0); //TDCBlackCarType2nd[]


	{ //ClientFrameRateCfg
		BYTE* pClientFrameRateCfg = p;
		Write16(pClientFrameRateCfg, 0); //len

		Write32(pClientFrameRateCfg, 0); //MinFrameRate
		Write32(pClientFrameRateCfg, 60000); //MaxFrameRate
		Write32(pClientFrameRateCfg, 10); //FrameStep
		Write32(pClientFrameRateCfg, 0); //CurrentFrameRate

		len = pClientFrameRateCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 16); //ReportFrameRateValue
	Write16(p, 10); //FrameOpSpan
	Write8(p, 0); //VideoSwitch

	Write8(p, 0); //KartRefitCardTypeNum
	/*
	for (size_t i = 0; i < 1; i++)
	{ //KartRefitCardCfg
		BYTE* pKartRefitCardCfg = p;
		Write16(pKartRefitCardCfg, 0); //len

		Write32(pKartRefitCardCfg, 0); //RefitCardID
		Write8(pKartRefitCardCfg, 1); //EffectNum
		for (size_t i = 0; i < 1; i++)
		{ //RefitCardEffect
			BYTE* pRefitCardEffect = pKartRefitCardCfg;
			Write16(pRefitCardEffect, 0); //len

			Write8(pRefitCardEffect, 0); //RefitID
			Write8(pRefitCardEffect, 0); //ModifyValue

			len = pRefitCardEffect - pKartRefitCardCfg;
			Set16(pKartRefitCardCfg, (WORD)len);
			pKartRefitCardCfg += len;
		}
		Write8(pKartRefitCardCfg, 0); //KartNum
		//Write32(pKartRefitCardCfg, 0); //KartList[]

		len = pKartRefitCardCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	len = p - buf;
	SendToClient(Client, 25066, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void NotifySpeed2Cfg(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //MapEffectMaskNum
	/*
	for (size_t i = 0; i < 0; i++)
	{ //MapEffectMask
		BYTE* pMapEffectMask = p;
		Write16(pMapEffectMask, 0); //len

		Write16(pMapEffectMask, 0); //MapID
		Write32(pMapEffectMask, 0); //EffectMask

		len = pMapEffectMask - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write8(p, 0); //Speed2ShopLimit
	Write32(p, 127); //OpenFlag

	len = p - buf;
	SendToClient(Client, 11358, buf, len, Client->ServerID, FE_GAMESVRD, Client->ConnID, Notify);
}

void NotifyTopUIItemInfo(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	//ID
	{
		//19	变身派对
		//43	荣耀排位赛
		//62	赛道随心造
		//63	世界挑战赛
		//67	双人冲顶赛
		//81	狂野追逐
		//90	暗夜大逃亡
		//96	超能竞速赛
		//103	疾风前传
		//109	双人冲锋战
		//120	龙晶大闯关
		//127	极速对决
		//134	车王新剧情
		//143	互助接力赛
		//153	年兽大乱斗
		//155	雷诺嘉年华
		//162	QS极速幻境
		//171	绝命车王
		//183	转世续缘
		//190	跳跳派对
		//197	时空捕手
		//208	顺子大作战
		//227	冰雪飞驰
		//252	问天之约
		//257	全明星大乱斗
		//285	极速同行
		//362以后未做测试 不知道是什么
	}
	//UINT DisplayUIItemID[] = { 19,43,62,63,67,81,90,96,103,109,120,127,134,143,153,155,162,171,183,190,197,208,227,252,257,285 };
	UINT DisplayUIItemID[] = { 43,120 };
	UINT ShowUINum = sizeof(DisplayUIItemID) / sizeof(UINT);
	Write16(p, ShowUINum); //Num
	for (size_t i = 0; i < ShowUINum; i++)
	{ //TopUIItemInfo
		BYTE* pTopUIItemInfo = p;
		Write16(pTopUIItemInfo, 0); //len

		Write8(pTopUIItemInfo, 0); //Type
		Write16(pTopUIItemInfo, DisplayUIItemID[i]); //ID

		Write16(pTopUIItemInfo, 0); //Tag
		Write16(pTopUIItemInfo, 0); //NameLen
		Write16(pTopUIItemInfo, 0); //PrompLen

		len = pTopUIItemInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ //RecommendModeInfo
		BYTE* pRecommendModeInfo = p;
		Write16(pRecommendModeInfo, 0); //len

		Write16(pRecommendModeInfo, 0); //ModeNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //ModeInfo
			BYTE* pModeInfo = pRecommendModeInfo;
			Write16(pModeInfo, 0); //len

			Write16(pModeInfo, 0); //ModeID
			Write16(pModeInfo, 0); //Status
			Write32(pModeInfo, 0); //Privige
			Write16(pModeInfo, 0); //PrompLen

			len = pModeInfo - pRecommendModeInfo;
			Set16(pRecommendModeInfo, (WORD)len);
			pRecommendModeInfo += len;
		}
		*/
		len = pRecommendModeInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); //Count
	/*
	for (size_t i = 0; i < n; i++)
	{ //EntryList
		BYTE* pEntryList = p;
		Write16(pEntryList, 0); //len

		Write8(pEntryList, 0); //Type
		Write8(pEntryList, 0); //Status
		Write16(pEntryList, 0); //PromptLen
		Write8(pEntryList, 0); //Tag
		Write8(pEntryList, 0); //Icon
		Write8(pEntryList, 0); //Kind
		Write8(pEntryList, 0); //Privilige
		Write8(pEntryList, 0); //IsTodayOpen
		Write8(pEntryList, 0); //SetType
		Write8(pEntryList, 0); //EmbedType
		Write8(pEntryList, 0); //LoginWindow

		len = pEntryList - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write8(p, 0); //SetNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //EntrySet
		BYTE* pEntrySet = p;
		Write16(pEntrySet, 0); //len

		Write8(pEntrySet, 0); //Count
		for (size_t i = 0; i < n; i++)
		{ //EntryList
			BYTE* pEntryList = pEntrySet;
			Write16(pEntryList, 0); //len

			Write8(pEntryList, 0); //Type
			Write8(pEntryList, 0); //Status
			Write16(pEntryList, 0); //PromptLen
			Write8(pEntryList, 0); //Tag
			Write8(pEntryList, 0); //Icon
			Write8(pEntryList, 0); //Kind
			Write8(pEntryList, 0); //Privilige
			Write8(pEntryList, 0); //IsTodayOpen
			Write8(pEntryList, 0); //SetType
			Write8(pEntryList, 0); //EmbedType
			Write8(pEntryList, 0); //LoginWindow

			len = pEntryList - pEntrySet;
			Set16(pEntrySet, (WORD)len);
			pEntrySet += len;
		}

		len = pEntrySet - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write8(p, 0); //MouseOverNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //MouseOverTipsCfg
		BYTE* pMouseOverTipsCfg = p;
		Write16(pMouseOverTipsCfg, 0); //len

		Write16(pMouseOverTipsCfg, 0); //ID
		Write16(pMouseOverTipsCfg, 0); //PrompLen

		len = pMouseOverTipsCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	len = p - buf;
	SendToClient(Client, 25116, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void NotifyRedPointInfo(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write8(p, 0); //RedPointNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //RedPointInfo
		BYTE* pRedPointInfo = p;
		Write16(pRedPointInfo, 0); //len

		Write8(pRedPointInfo, 0); //Type
		Write8(pRedPointInfo, 0); //Status

		len = pRedPointInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write8(p, 1); //All
	Write8(p, 0); //IsNeedPlayVideoGuid

	len = p - buf;
	SendToClient(Client, 25114, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestGetActivityCenterInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseGetActivityCenterInfo(Client);
}
void ResponseGetActivityCenterInfo(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write32(p, 0); //TotalActivity
	Write32(p, 0); //CurrentWeekActivity
	Write32(p, 0); //LastWeekActivity
	Write32(p, 0); //CurrentDayActivity
	Write32(p, 0); //ActivityAppellcation
	Write32(p, 0); //ActivityLevel
	Write32(p, 0); //NextLevelActivity
	{ //DailyActivityBox
		BYTE* pDailyActivityBox = p;
		Write16(pDailyActivityBox, 0); //len

		Write32(pDailyActivityBox, 0); //Day
		Write32(pDailyActivityBox, 0); //BoxNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //ActivityBox
			BYTE* pActivityBox = pDailyActivityBox;
			Write16(pActivityBox, 0); //len

			Write32(pActivityBox, 0); //BoxLevel
			Write32(pActivityBox, 0); //BoxType
			Write32(pActivityBox, 0); //ActivityType
			Write32(pActivityBox, 0); //NeedActivityNum
			Write32(pActivityBox, 0); //BoxStatus
			Write32(pActivityBox, 0); //BoxID
			for (size_t i = 0; i < n; i++)
			{ //Award
				BYTE* pAward = pActivityBox;
				Write16(pAward, 0); //len

				Write32(pAward, 0); //Exp
				Write32(pAward, 0); //Money
				Write32(pAward, 0); //SuperMoney
				Write16(pAward, 0); //ItemNum
				for (size_t i = 0; i < n; i++)
				{ //ItemInfo
					BYTE* pItemInfo = pAward;
					Write16(pItemInfo, 0); //len

					Write32(pItemInfo, 0); //ItemID
					Write32(pItemInfo, 0); //ItemNum
					Write32(pItemInfo, 0); //AvailPeriod
					Write8(pItemInfo, 0); //Status
					WriteUn(pItemInfo, 0); //ObtainTime
					Write32(pItemInfo, 0); //OtherAttribute
					Write16(pItemInfo, 0); //ItemType

					len = pItemInfo - pAward;
					Set16(pAward, (WORD)len);
					pAward += len;
				}
				Write32(pAward, 0); //Coupons
				Write32(pAward, 0); //GuildPoint
				Write32(pAward, 0); //LuckMoney
				Write8(pAward, 0); //ExtendInfoNum
				for (size_t i = 0; i < n; i++)
				{ //ExtendInfoAward
					BYTE* pExtendInfoAward = pAward;
					Write16(pExtendInfoAward, 0); //len

					Write16(pExtendInfoAward, 0); //Key
					Write32(pExtendInfoAward, 0); //AddValue
					Write32(pExtendInfoAward, 0); //TotalValue
					Write8(pExtendInfoAward, 0); //ShowPriority
					Write16(pExtendInfoAward, 0); //BuyNeedScore
					Write8(pExtendInfoAward, 0); //OneMatchMaxNum

					len = pExtendInfoAward - pAward;
					Set16(pAward, (WORD)len);
					pAward += len;
				}
				Write32(pAward, 0); //SpeedCoin

				len = pAward - pActivityBox;
				Set16(pActivityBox, (WORD)len);
				pActivityBox += len;
			}
			{ //PreviewAward
				BYTE* pPreviewAward = pActivityBox;
				Write16(pPreviewAward, 0); //len

				Write32(pPreviewAward, 0); //SuperMoney
				Write32(pPreviewAward, 0); //Coupons
				Write16(pPreviewAward, 0); //ItemNum
				Write32(pPreviewAward, 0); //ItemID[]

				len = pPreviewAward - pActivityBox;
				Set16(pActivityBox, (WORD)len);
				pActivityBox += len;
			}
			Write32(pActivityBox, 0); //PresetBoxID
			Write8(pActivityBox, 0); //HasActiveAward

			len = pActivityBox - pDailyActivityBox;
			Set16(pDailyActivityBox, (WORD)len);
			pDailyActivityBox += len;
		}
		*/
		len = pDailyActivityBox - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ //ActivityTaskCfg
		BYTE* pActivityTaskCfg = p;
		Write16(pActivityTaskCfg, 0); //len

		Write32(pActivityTaskCfg, 0); //TabNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //ActivityTaskTab
			BYTE* pActivityTaskTab = pActivityTaskCfg;
			Write16(pActivityTaskTab, 0); //len

			Write32(pActivityTaskTab, 0); //TabOrder
			Write32(pActivityTaskTab, 0); //TabType
			Write32(pActivityTaskTab, 0); //Show
			Write32(pActivityTaskTab, 0); //TaskNum
			for (size_t i = 0; i < n; i++)
			{ //ActivityTask
				BYTE* pActivityTask = pActivityTaskTab;
				Write16(pActivityTask, 0); //len

				Write32(pActivityTask, 0); //TaskID
				Write32(pActivityTask, 0); //ActivityAward
				Write16(pActivityTask, 0); //ChildTaskNum
				Write32(pActivityTask, 0); //ChildTaskID[]

				len = pActivityTask - pActivityTaskTab;
				Set16(pActivityTaskTab, (WORD)len);
				pActivityTaskTab += len;
			}

			len = pActivityTaskTab - pActivityTaskCfg;
			Set16(pActivityTaskCfg, (WORD)len);
			pActivityTaskCfg += len;
		}
		*/
		len = pActivityTaskCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); //BCNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //BC
		BYTE* pBC = p;
		Write16(pBC, 0); //len

		Write16(pBC, 0); //ContentLen

		len = pBC - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write32(p, 0); //CurrentWeekActivityUsed
	{ //MultipleTimeCfg
		BYTE* pMultipleTimeCfg = p;
		Write16(pMultipleTimeCfg, 0); //len

		Write32(pMultipleTimeCfg, 0); //BeginTime
		Write32(pMultipleTimeCfg, 0); //EndTime
		Write16(pMultipleTimeCfg, 0); //MultiValue

		len = pMultipleTimeCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); //CurrentWeekActivityForLotto
	Write8(p, 0); //LottoItemNum
	//Write32(p, 0); //LottoItemID[]

	len = p - buf;
	SendToClient(Client, 24029, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestC2GSign3Operate(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UCHAR OperateType = Read8(Body);

	ResponseC2GSign3Operate(Client, OperateType);
}
void ResponseC2GSign3Operate(ClientNode* Client, UCHAR OperateType)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write16(p, 0); //ResultID
	Write8(p, 0); //ReasonLen
	Write8(p, OperateType); //OperateType
	Write8(p, 0); //HasSign3Info
	/*
	for (size_t i = 0; i < n; i++)
	{ //Sign3Info
		BYTE* pSign3Info = p;
		Write16(pSign3Info, 0); //len

		Write32(pSign3Info, 0); //BeginDate
		Write32(pSign3Info, 0); //EndDate
		Write8(pSign3Info, 0); //SignNum
		Write8(pSign3Info, 0); //CanSign
		Write8(pSign3Info, 0); //AwardNum
		for (size_t i = 0; i < n; i++)
		{ //Sign2Award
			BYTE* pSign2Award = pSign3Info;
			Write16(pSign2Award, 0); //len

			{ //Award
				BYTE* pAward = pSign2Award;
				Write16(pAward, 0); //len

				Write32(pAward, 0); //Exp
				Write32(pAward, 0); //Money
				Write32(pAward, 0); //SuperMoney
				Write16(pAward, 0); //ItemNum
				for (size_t i = 0; i < n; i++)
				{ //ItemInfo
					BYTE* pItemInfo = pAward;
					Write16(pItemInfo, 0); //len

					Write32(pItemInfo, 0); //ItemID
					Write32(pItemInfo, 0); //ItemNum
					Write32(pItemInfo, 0); //AvailPeriod
					Write8(pItemInfo, 0); //Status
					WriteUn(pItemInfo, 0); //ObtainTime
					Write32(pItemInfo, 0); //OtherAttribute
					Write16(pItemInfo, 0); //ItemType

					len = pItemInfo - pAward;
					Set16(pAward, (WORD)len);
					pAward += len;
				}
				Write32(pAward, 0); //Coupons
				Write32(pAward, 0); //GuildPoint
				Write32(pAward, 0); //LuckMoney
				Write8(pAward, 0); //ExtendInfoNum
				for (size_t i = 0; i < n; i++)
				{ //ExtendInfoAward
					BYTE* pExtendInfoAward = pAward;
					Write16(pExtendInfoAward, 0); //len

					Write16(pExtendInfoAward, 0); //Key
					Write32(pExtendInfoAward, 0); //AddValue
					Write32(pExtendInfoAward, 0); //TotalValue
					Write8(pExtendInfoAward, 0); //ShowPriority
					Write16(pExtendInfoAward, 0); //BuyNeedScore
					Write8(pExtendInfoAward, 0); //OneMatchMaxNum

					len = pExtendInfoAward - pAward;
					Set16(pAward, (WORD)len);
					pAward += len;
				}
				Write32(pAward, 0); //SpeedCoin

				len = pAward - pSign2Award;
				Set16(pSign2Award, (WORD)len);
				pSign2Award += len;
			}
			Write16(pSign2Award, 0); //AwardTimes
			Write8(pSign2Award, 0); //HighGrade
			Write8(pSign2Award, 0); //FuncDes

			len = pSign2Award - pSign3Info;
			Set16(pSign3Info, (WORD)len);
			pSign3Info += len;
		}
		Write32(pSign3Info, 0); //SpecailAwardBeginDate
		Write32(pSign3Info, 0); //SpecailAwardEndDate
		{ //SpecialAward
			BYTE* pSpecialAward = pSign3Info;
			Write16(pSpecialAward, 0); //len

			Write32(pSpecialAward, 0); //Exp
			Write32(pSpecialAward, 0); //Money
			Write32(pSpecialAward, 0); //SuperMoney
			Write16(pSpecialAward, 0); //ItemNum
			for (size_t i = 0; i < n; i++)
			{ //ItemInfo
				BYTE* pItemInfo = pSpecialAward;
				Write16(pItemInfo, 0); //len

				Write32(pItemInfo, 0); //ItemID
				Write32(pItemInfo, 0); //ItemNum
				Write32(pItemInfo, 0); //AvailPeriod
				Write8(pItemInfo, 0); //Status
				WriteUn(pItemInfo, 0); //ObtainTime
				Write32(pItemInfo, 0); //OtherAttribute
				Write16(pItemInfo, 0); //ItemType

				len = pItemInfo - pSpecialAward;
				Set16(pSpecialAward, (WORD)len);
				pSpecialAward += len;
			}
			Write32(pSpecialAward, 0); //Coupons
			Write32(pSpecialAward, 0); //GuildPoint
			Write32(pSpecialAward, 0); //LuckMoney
			Write8(pSpecialAward, 0); //ExtendInfoNum
			for (size_t i = 0; i < n; i++)
			{ //ExtendInfoAward
				BYTE* pExtendInfoAward = pSpecialAward;
				Write16(pExtendInfoAward, 0); //len

				Write16(pExtendInfoAward, 0); //Key
				Write32(pExtendInfoAward, 0); //AddValue
				Write32(pExtendInfoAward, 0); //TotalValue
				Write8(pExtendInfoAward, 0); //ShowPriority
				Write16(pExtendInfoAward, 0); //BuyNeedScore
				Write8(pExtendInfoAward, 0); //OneMatchMaxNum

				len = pExtendInfoAward - pSpecialAward;
				Set16(pSpecialAward, (WORD)len);
				pSpecialAward += len;
			}
			Write32(pSpecialAward, 0); //SpeedCoin

			len = pSpecialAward - pSign3Info;
			Set16(pSign3Info, (WORD)len);
			pSign3Info += len;
		}
		Write8(pSign3Info, 0); //SpecialAwardState
		Write8(pSign3Info, 0); //IsSpecailAwardRemind
		Write8(pSign3Info, 0); //TipsLen
		Write32(pSign3Info, 0); //SpecialAwardID
		Write8(pSign3Info, 0); //LabelNum
		Write8(pSign3Info, 0); //GotLabelNum
		Write8(pSign3Info, 0); //FinalAwardState
		{ //FinalAward
			BYTE* pFinalAward = pSign3Info;
			Write16(pFinalAward, 0); //len

			Write32(pFinalAward, 0); //Exp
			Write32(pFinalAward, 0); //Money
			Write32(pFinalAward, 0); //SuperMoney
			Write16(pFinalAward, 0); //ItemNum
			for (size_t i = 0; i < n; i++)
			{ //ItemInfo
				BYTE* pItemInfo = pFinalAward;
				Write16(pItemInfo, 0); //len

				Write32(pItemInfo, 0); //ItemID
				Write32(pItemInfo, 0); //ItemNum
				Write32(pItemInfo, 0); //AvailPeriod
				Write8(pItemInfo, 0); //Status
				WriteUn(pItemInfo, 0); //ObtainTime
				Write32(pItemInfo, 0); //OtherAttribute
				Write16(pItemInfo, 0); //ItemType

				len = pItemInfo - pFinalAward;
				Set16(pFinalAward, (WORD)len);
				pFinalAward += len;
			}
			Write32(pFinalAward, 0); //Coupons
			Write32(pFinalAward, 0); //GuildPoint
			Write32(pFinalAward, 0); //LuckMoney
			Write8(pFinalAward, 0); //ExtendInfoNum
			for (size_t i = 0; i < n; i++)
			{ //ExtendInfoAward
				BYTE* pExtendInfoAward = pFinalAward;
				Write16(pExtendInfoAward, 0); //len

				Write16(pExtendInfoAward, 0); //Key
				Write32(pExtendInfoAward, 0); //AddValue
				Write32(pExtendInfoAward, 0); //TotalValue
				Write8(pExtendInfoAward, 0); //ShowPriority
				Write16(pExtendInfoAward, 0); //BuyNeedScore
				Write8(pExtendInfoAward, 0); //OneMatchMaxNum

				len = pExtendInfoAward - pFinalAward;
				Set16(pFinalAward, (WORD)len);
				pFinalAward += len;
			}
			Write32(pFinalAward, 0); //SpeedCoin

			len = pFinalAward - pSign3Info;
			Set16(pSign3Info, (WORD)len);
			pSign3Info += len;
		}
		Write32(pSign3Info, 0); //SeasonBeginDate
		Write32(pSign3Info, 0); //SeasonEndDate

		len = pSign3Info - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write8(p, 0); //SignNum
	Write8(p, 0); //HasAward
	/*
	for (size_t i = 0; i < n; i++)
	{ //Award
		BYTE* pAward = p;
		Write16(pAward, 0); //len

		Write32(pAward, 0); //Exp
		Write32(pAward, 0); //Money
		Write32(pAward, 0); //SuperMoney
		Write16(pAward, 0); //ItemNum
		for (size_t i = 0; i < n; i++)
		{ //ItemInfo
			BYTE* pItemInfo = pAward;
			Write16(pItemInfo, 0); //len

			Write32(pItemInfo, 0); //ItemID
			Write32(pItemInfo, 0); //ItemNum
			Write32(pItemInfo, 0); //AvailPeriod
			Write8(pItemInfo, 0); //Status
			WriteUn(pItemInfo, 0); //ObtainTime
			Write32(pItemInfo, 0); //OtherAttribute
			Write16(pItemInfo, 0); //ItemType

			len = pItemInfo - pAward;
			Set16(pAward, (WORD)len);
			pAward += len;
		}
		Write32(pAward, 0); //Coupons
		Write32(pAward, 0); //GuildPoint
		Write32(pAward, 0); //LuckMoney
		Write8(pAward, 0); //ExtendInfoNum
		for (size_t i = 0; i < n; i++)
		{ //ExtendInfoAward
			BYTE* pExtendInfoAward = pAward;
			Write16(pExtendInfoAward, 0); //len

			Write16(pExtendInfoAward, 0); //Key
			Write32(pExtendInfoAward, 0); //AddValue
			Write32(pExtendInfoAward, 0); //TotalValue
			Write8(pExtendInfoAward, 0); //ShowPriority
			Write16(pExtendInfoAward, 0); //BuyNeedScore
			Write8(pExtendInfoAward, 0); //OneMatchMaxNum

			len = pExtendInfoAward - pAward;
			Set16(pAward, (WORD)len);
			pAward += len;
		}
		Write32(pAward, 0); //SpeedCoin

		len = pAward - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write8(p, 0); //LabelNum

	len = p - buf;
	SendToClient(Client, 1401, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestHyperSpaceJumboGetCfg(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UCHAR CfgType = Read8(Body);

	ResponseHyperSpaceJumboGetCfg(Client, CfgType);
}

void ResponseHyperSpaceJumboGetCfg(ClientNode* Client, UCHAR CfgType)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write16(p, 0); //ResultID

	Write32(p, 0); //BeginTime
	Write32(p, 240000); //EndTime 结束时间

	Write16(p, 0); //MapNum
	//Write32(p, 0); //MapIDList[]

	const int MAX_HYPER_SPACE_JUMBO_BEST_ITEM_MUM = 30;
	int BestItemIDList[MAX_HYPER_SPACE_JUMBO_BEST_ITEM_MUM] = { 27235 ,24206 ,31574 ,32407 };
	for (size_t i = 0; i < MAX_HYPER_SPACE_JUMBO_BEST_ITEM_MUM; i++)
	{
		Write32(p, BestItemIDList[i]); //BestItemIDList[]
	}
	
	Write32(p, 0); //LeftOpenBoxNum
	Write8(p, CfgType); //CfgType
	Write32(p, 0); //Para1

	Write16(p, 17); //ValueNum
	int Values[17] = {  };
	for (size_t i = 0; i < 17; i++)
	{
		Write32(p, Values[i]); //Values[]
	}

	Write16(p, 0); //MapListNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //MapList
		BYTE* pMapList = p;
		Write16(pMapList, 0); //len

		Write32(pMapList, 0); //MapID
		Write8(pMapList, 0); //Para1

		len = pMapList - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	Write16(p, 0); //HighBoxAwardNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //HighBoxAward
		BYTE* pHighBoxAward = p;
		Write16(pHighBoxAward, 0); //len

		Write32(pHighBoxAward, 0); //Exp
		Write32(pHighBoxAward, 0); //Money
		Write32(pHighBoxAward, 0); //SuperMoney
		Write16(pHighBoxAward, 0); //ItemNum
		for (size_t i = 0; i < n; i++)
		{ //ItemInfo
			BYTE* pItemInfo = pHighBoxAward;
			Write16(pItemInfo, 0); //len

			Write32(pItemInfo, 0); //ItemID
			Write32(pItemInfo, 0); //ItemNum
			Write32(pItemInfo, 0); //AvailPeriod
			Write8(pItemInfo, 0); //Status
			WriteUn(pItemInfo, 0); //ObtainTime
			Write32(pItemInfo, 0); //OtherAttribute
			Write16(pItemInfo, 0); //ItemType

			len = pItemInfo - pHighBoxAward;
			Set16(pHighBoxAward, (WORD)len);
			pHighBoxAward += len;
		}
		Write32(pHighBoxAward, 0); //Coupons
		Write32(pHighBoxAward, 0); //GuildPoint
		Write32(pHighBoxAward, 0); //LuckMoney
		Write8(pHighBoxAward, 0); //ExtendInfoNum
		for (size_t i = 0; i < n; i++)
		{ //ExtendInfoAward
			BYTE* pExtendInfoAward = pHighBoxAward;
			Write16(pExtendInfoAward, 0); //len

			Write16(pExtendInfoAward, 0); //Key
			Write32(pExtendInfoAward, 0); //AddValue
			Write32(pExtendInfoAward, 0); //TotalValue
			Write8(pExtendInfoAward, 0); //ShowPriority
			Write16(pExtendInfoAward, 0); //BuyNeedScore
			Write8(pExtendInfoAward, 0); //OneMatchMaxNum

			len = pExtendInfoAward - pHighBoxAward;
			Set16(pHighBoxAward, (WORD)len);
			pHighBoxAward += len;
		}
		Write32(pHighBoxAward, 0); //SpeedCoin

		len = pHighBoxAward - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	Write16(p, 0); //LowBoxAwardNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //LowBoxAward
		BYTE* pLowBoxAward = p;
		Write16(pLowBoxAward, 0); //len

		Write32(pLowBoxAward, 0); //Exp
		Write32(pLowBoxAward, 0); //Money
		Write32(pLowBoxAward, 0); //SuperMoney
		Write16(pLowBoxAward, 0); //ItemNum
		for (size_t i = 0; i < n; i++)
		{ //ItemInfo
			BYTE* pItemInfo = pLowBoxAward;
			Write16(pItemInfo, 0); //len

			Write32(pItemInfo, 0); //ItemID
			Write32(pItemInfo, 0); //ItemNum
			Write32(pItemInfo, 0); //AvailPeriod
			Write8(pItemInfo, 0); //Status
			WriteUn(pItemInfo, 0); //ObtainTime
			Write32(pItemInfo, 0); //OtherAttribute
			Write16(pItemInfo, 0); //ItemType

			len = pItemInfo - pLowBoxAward;
			Set16(pLowBoxAward, (WORD)len);
			pLowBoxAward += len;
		}
		Write32(pLowBoxAward, 0); //Coupons
		Write32(pLowBoxAward, 0); //GuildPoint
		Write32(pLowBoxAward, 0); //LuckMoney
		Write8(pLowBoxAward, 0); //ExtendInfoNum
		for (size_t i = 0; i < n; i++)
		{ //ExtendInfoAward
			BYTE* pExtendInfoAward = pLowBoxAward;
			Write16(pExtendInfoAward, 0); //len

			Write16(pExtendInfoAward, 0); //Key
			Write32(pExtendInfoAward, 0); //AddValue
			Write32(pExtendInfoAward, 0); //TotalValue
			Write8(pExtendInfoAward, 0); //ShowPriority
			Write16(pExtendInfoAward, 0); //BuyNeedScore
			Write8(pExtendInfoAward, 0); //OneMatchMaxNum

			len = pExtendInfoAward - pLowBoxAward;
			Set16(pLowBoxAward, (WORD)len);
			pLowBoxAward += len;
		}
		Write32(pLowBoxAward, 0); //SpeedCoin

		len = pLowBoxAward - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	Write32(p, 0); //Para2

	Write16(p, 0); //ValueNum2
	//Write32(p, 0); //Values2[]

	Write8(p, 0); //HasYuLeJiaNianHuaInfo
	/*
	for (size_t i = 0; i < n; i++)
	{ //YuLeJiaNianHuaInfo
		BYTE* pYuLeJiaNianHuaInfo = p;
		Write16(pYuLeJiaNianHuaInfo, 0); //len

		Write32(pYuLeJiaNianHuaInfo, 0); //Mode
		Write32(pYuLeJiaNianHuaInfo, 0); //Score
		Write8(pYuLeJiaNianHuaInfo, 0); //RankRangNum
		Write32(pYuLeJiaNianHuaInfo, 0); //RankRange[]
		Write8(pYuLeJiaNianHuaInfo, 0); //RankScoreNum
		Write32(pYuLeJiaNianHuaInfo, 0); //RankScore[]
		Write8(pYuLeJiaNianHuaInfo, 0); //InRecommend
		Write8(pYuLeJiaNianHuaInfo, 0); //InDoubleScore

		len = pYuLeJiaNianHuaInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	len = p - buf;
	SendToClient(Client, 24202, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestTimerChallenge2ndMoreHallInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseTimerChallenge2ndMoreHallInfo(Client);
}

void ResponseTimerChallenge2ndMoreHallInfo(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write16(p, 0); //ResultID

	Write16(p, 11); //ValueNum
	int Values[11] = { };
	for (size_t i = 0; i < 11; i++)
	{
		Write32(p, Values[i]); //Values[]
	}

	BYTE* pTimerChallenge2ndLevelAwardInfoNum = p;
	Write16(p, 0);
	try
	{
		YAML::Node Config = YAML::LoadFile("TimerChallenge2ndLevelAwardInfo-QQSpeed.yml");
		USHORT Num = (USHORT)Config.size();
		for (USHORT i = 0; i < Num; i++)
		{ //TimerChallenge2ndLevelAwardInfo
			BYTE* pTimerChallenge2ndLevelAwardInfo = p;
			Write16(pTimerChallenge2ndLevelAwardInfo, 0); //len

			YAML::Node TimerChallenge2ndLevelAwardInfo = Config[i];
			Write32(pTimerChallenge2ndLevelAwardInfo, TimerChallenge2ndLevelAwardInfo["Level"].as<int>());
			Write32(pTimerChallenge2ndLevelAwardInfo, TimerChallenge2ndLevelAwardInfo["JumpLevel"].as<int>());
			Write32(pTimerChallenge2ndLevelAwardInfo, TimerChallenge2ndLevelAwardInfo["BoxFlag"].as<int>());

			YAML::Node Checkpoint = TimerChallenge2ndLevelAwardInfo["Checkpoint"];
			BYTE CheckpointNum = (BYTE)Checkpoint.size();
			Write8(pTimerChallenge2ndLevelAwardInfo, CheckpointNum); //CheckpointNum
			for (BYTE i = 0; i < CheckpointNum; i++)
			{
				Write32(pTimerChallenge2ndLevelAwardInfo, Checkpoint[i].as<int>()); //CheckpointIdx[]
			}

			Write8(pTimerChallenge2ndLevelAwardInfo, TimerChallenge2ndLevelAwardInfo["IsDragonGate"].as<int>());

			len = pTimerChallenge2ndLevelAwardInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
		Set16(pTimerChallenge2ndLevelAwardInfoNum, Num);
	}
	catch (const std::exception&)
	{
		return;
	}

	Write8(p, 1); //HasDoorCfg
	{ //DoorCfg
		BYTE* pDoorCfg = p;
		Write16(pDoorCfg, 0); //len

		Write32(pDoorCfg, 0); //DoorPosNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //DoorPosInfo
			BYTE* pDoorPosInfo = pDoorCfg;
			Write16(pDoorPosInfo, 0); //len

			Write32(pDoorPosInfo, 0); //Level
			Write8(pDoorPosInfo, 0); //DoorPosDetailNum
			for (size_t i = 0; i < n; i++)
			{ //DoorPosDetailInfo
				BYTE* pDoorPosDetailInfo = pDoorPosInfo;
				Write16(pDoorPosDetailInfo, 0); //len

				Write32(pDoorPosDetailInfo, 0); //Checkpoint
				Write32(pDoorPosDetailInfo, 0); //AxisX
				Write32(pDoorPosDetailInfo, 0); //AxisY
				Write32(pDoorPosDetailInfo, 0); //AxisZ
				Write32(pDoorPosDetailInfo, 0); //X
				Write32(pDoorPosDetailInfo, 0); //Y
				Write32(pDoorPosDetailInfo, 0); //Z

				len = pDoorPosDetailInfo - pDoorPosInfo;
				Set16(pDoorPosInfo, (WORD)len);
				pDoorPosInfo += len;
			}

			len = pDoorPosInfo - pDoorCfg;
			Set16(pDoorCfg, (WORD)len);
			pDoorCfg += len;
		}
		*/

		len = pDoorCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 0); //AllowJumpLevelPerPlayer

	Write8(p, 0); //ShadowNpcNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //TimerChallengeShadowNpcInfo
		BYTE* pTimerChallengeShadowNpcInfo = p;
		Write16(pTimerChallengeShadowNpcInfo, 0); //len

		Write8(pTimerChallengeShadowNpcInfo, 0); //FileMd5Len
		Write16(pTimerChallengeShadowNpcInfo, 0); //Len
		Write8(pTimerChallengeShadowNpcInfo, 0); //HasNPCItem
		for (size_t i = 0; i < n; i++)
		{ //NPCItemInfo
			BYTE* pNPCItemInfo = pTimerChallengeShadowNpcInfo;
			Write16(pNPCItemInfo, 0); //len

			Write32(pNPCItemInfo, 0); //RoleID
			Write32(pNPCItemInfo, 0); //HairID
			Write32(pNPCItemInfo, 0); //FaceID
			Write32(pNPCItemInfo, 0); //GlassID
			Write32(pNPCItemInfo, 0); //CoatID
			Write32(pNPCItemInfo, 0); //GloveID
			Write32(pNPCItemInfo, 0); //TrousersID
			Write32(pNPCItemInfo, 0); //PersonarID
			Write32(pNPCItemInfo, 0); //TattooID
			Write32(pNPCItemInfo, 0); //WingID
			Write32(pNPCItemInfo, 0); //DecoratorID
			Write32(pNPCItemInfo, 0); //KartID
			Write32(pNPCItemInfo, 0); //ColorID
			Write32(pNPCItemInfo, 0); //TrackID
			Write32(pNPCItemInfo, 0); //EffectID
			Write32(pNPCItemInfo, 0); //BrandID
			Write32(pNPCItemInfo, 0); //TailID

			len = pNPCItemInfo - pTimerChallengeShadowNpcInfo;
			Set16(pTimerChallengeShadowNpcInfo, (WORD)len);
			pTimerChallengeShadowNpcInfo += len;
		}
		Write16(pTimerChallengeShadowNpcInfo, 0); //BossNameLen
		Write16(pTimerChallengeShadowNpcInfo, 0); //TimeNum
		Write(pTimerChallengeShadowNpcInfo, 0); //LevTimeCfg[]

		len = pTimerChallengeShadowNpcInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	
	BYTE* pBuffNum = p;
	Write8(p, 0); //BuffNum;
	try
	{
		YAML::Node Config = YAML::LoadFile("TimerChallengeOneBuffInfo-QQSpeed.yml");
		BYTE Num = (BYTE)Config.size();
		for (BYTE i = 0; i < Num; i++)
		{ //TimerChallengeOneBuffInfo
			BYTE* pTimerChallengeOneBuffInfo = p;
			Write16(pTimerChallengeOneBuffInfo, 0); //len

			YAML::Node TimerChallengeOneBuffInfo = Config[i];
			Write32(pTimerChallengeOneBuffInfo, TimerChallengeOneBuffInfo["BuffId"].as<int>());
			Write32(pTimerChallengeOneBuffInfo, TimerChallengeOneBuffInfo["Prob"].as<int>());
			Write8(pTimerChallengeOneBuffInfo, TimerChallengeOneBuffInfo["BuffType"].as<int>());
			Write8(pTimerChallengeOneBuffInfo, TimerChallengeOneBuffInfo["BuffLev"].as<int>());
			Write8(pTimerChallengeOneBuffInfo, TimerChallengeOneBuffInfo["ReduceDiscount"].as<int>());
			Write32(pTimerChallengeOneBuffInfo, TimerChallengeOneBuffInfo["KartId"].as<int>());
			Write32(pTimerChallengeOneBuffInfo, TimerChallengeOneBuffInfo["DailyExpKartTimeLimit"].as<int>());

			std::string str = TimerChallengeOneBuffInfo["BuffName"].as<std::string>();
			len = str.length();
			Write16(pTimerChallengeOneBuffInfo, (WORD)len); //BuffNameLen
			memcpy(pTimerChallengeOneBuffInfo, str.c_str(), len);
			pTimerChallengeOneBuffInfo += len;

			str = TimerChallengeOneBuffInfo["BuffDes"].as<std::string>();
			len = str.length();
			Write16(pTimerChallengeOneBuffInfo, (WORD)len); //BuffDesLen
			memcpy(pTimerChallengeOneBuffInfo, str.c_str(), len);
			pTimerChallengeOneBuffInfo += len;

			Write8(pTimerChallengeOneBuffInfo, TimerChallengeOneBuffInfo["Icon"].as<int>());
			Write8(pTimerChallengeOneBuffInfo, TimerChallengeOneBuffInfo["BuffShape"].as<int>());
			Write8(pTimerChallengeOneBuffInfo, TimerChallengeOneBuffInfo["IsCanLevUp"].as<int>());

			len = pTimerChallengeOneBuffInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
		Set8(pBuffNum, Num);
	}
	catch (const std::exception&)
	{
		return;
	}

	Write32(p, 3); //CurrentActiveValue

	Write8(p, 0); //GotBuffNum
	/*
	{ //TimerChallenge3PlayerHasGotBuffInfo
		BYTE* pTimerChallenge3PlayerHasGotBuffInfo = p;
		Write16(pTimerChallenge3PlayerHasGotBuffInfo, 0); //len

		Write32(pTimerChallenge3PlayerHasGotBuffInfo, 0); //BuffId
		Write8(pTimerChallenge3PlayerHasGotBuffInfo, 1); //BuffSatus
		Write8(pTimerChallenge3PlayerHasGotBuffInfo, 1); //Position
		Write8(pTimerChallenge3PlayerHasGotBuffInfo, 0); //HasExpTime

		len = pTimerChallenge3PlayerHasGotBuffInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	Write8(p, 80); //TriggerProb
	Write8(p, 15); //NoTriggerN
	Write8(p, 1); //ASDSwitch
	{ //RefitInfo
		BYTE* pRefitInfo = p;
		Write16(pRefitInfo, 0); //len

		Write8(pRefitInfo, 29); //SpeedRefitStar
		Write8(pRefitInfo, 29); //JetRefitStar
		Write8(pRefitInfo, 29); //SJetRefitStar
		Write8(pRefitInfo, 29); //AccuRefitStar

		len = pRefitInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 24258, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestPreGetCommonBoxAward(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	USHORT Type = Read16(Body);
	int AwardID = Read32(Body);
	UCHAR IsShowAllAward = Read8(Body);
	int EchoClient = Read32(Body);

	ResponsePreGetCommonBoxAward(Client, Type);
}

void ResponsePreGetCommonBoxAward(ClientNode* Client, USHORT Type)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write16(p, 0); //ResultID
	Write16(p, Type); //Type
	{ //AwardCfgArray
		BYTE* pAwardCfgArray = p;
		Write16(pAwardCfgArray, 0); //len

		Write8(pAwardCfgArray, 1); //ScoreAwardNum
		for (size_t i = 0; i < 1; i++)
		{ //ScoreAwardCfg
			BYTE* pScoreAwardCfg = pAwardCfgArray;
			Write16(pScoreAwardCfg, 0); //len

			Write8(pScoreAwardCfg, 1); //Type
			Write32(pScoreAwardCfg, 0); //StartScore
			{ //AwardInfo
				BYTE* pAwardInfo = pScoreAwardCfg;
				Write16(pAwardInfo, 0); //len

				Write32(pAwardInfo, 0); //Exp
				Write32(pAwardInfo, 0); //Money
				Write32(pAwardInfo, 0); //SuperMoney
				Write16(pAwardInfo, 0); //ItemNum
				for (size_t i = 0; i < 0; i++)
				{ //ItemInfo
					BYTE* pItemInfo = pAwardInfo;
					Write16(pItemInfo, 0); //len

					Write32(pItemInfo, 0); //ItemID
					Write32(pItemInfo, 0); //ItemNum
					Write32(pItemInfo, 0); //AvailPeriod
					Write8(pItemInfo, 0); //Status
					Write32(pItemInfo, 0); //ObtainTime
					Write32(pItemInfo, 0); //OtherAttribute
					Write16(pItemInfo, 0); //ItemType

					len = pItemInfo - pAwardInfo;
					Set16(pAwardInfo, (WORD)len);
					pAwardInfo += len;
				}

				Write32(pAwardInfo, 0); //Coupons
				Write32(pAwardInfo, 0); //GuildPoint
				Write32(pAwardInfo, 0); //LuckMoney

				Write8(pAwardInfo, 0); //ExtendInfoNum
				for (size_t i = 0; i < 0; i++)
				{ //ExtendInfoAward
					BYTE* pExtendInfoAward = pAwardInfo;
					Write16(pExtendInfoAward, 0); //len

					Write16(pExtendInfoAward, 0); //Key
					Write32(pExtendInfoAward, 0); //AddValue
					Write32(pExtendInfoAward, 0); //TotalValue
					Write8(pExtendInfoAward, 0); //ShowPriority
					Write16(pExtendInfoAward, 0); //BuyNeedScore
					Write8(pExtendInfoAward, 0); //OneMatchMaxNum

					len = pExtendInfoAward - pAwardInfo;
					Set16(pAwardInfo, (WORD)len);
					pAwardInfo += len;
				}

				Write32(pAwardInfo, 0); //SpeedCoin

				len = pAwardInfo - pScoreAwardCfg;
				Set16(pScoreAwardCfg, (WORD)len);
				pScoreAwardCfg += len;
			}
			Write8(pScoreAwardCfg, 0); //Status
			Write16(pScoreAwardCfg, 0); //ResultID
			Write16(pScoreAwardCfg, 0); //HasMoreInfo
			/*
			for (size_t i = 0; i < n; i++)
			{ //MoreInfo
				BYTE* pMoreInfo = pScoreAwardCfg;
				Write16(pMoreInfo, 0); //len

				Write16(pMoreInfo, 0); //AlreayGetNum
				Write16(pMoreInfo, 0); //MaxGetNum
				Write32(pMoreInfo, 0); //ItemID
				Write32(pMoreInfo, 0); //TimeStart
				Write32(pMoreInfo, 0); //TimeEnd
				Write32(pMoreInfo, 0); //NextAddNumLeftTime
				Write8(pMoreInfo, 0); //Num
				for (size_t i = 0; i < n; i++)
				{ //ComsumeItemArray
					BYTE* pComsumeItemArray = pMoreInfo;
					Write16(pComsumeItemArray, 0); //len

					Write32(pComsumeItemArray, 0); //ItemID
					Write32(pComsumeItemArray, 0); //StartScore
					Write32(pComsumeItemArray, 0); //DiscountStartScore

					len = pComsumeItemArray - pMoreInfo;
					Set16(pMoreInfo, (WORD)len);
					pMoreInfo += len;
				}
				Write8(pMoreInfo, 0); //ClientParaNum
				Write32(pMoreInfo, 0); //ClientParas[]
				Write32(pMoreInfo, 0); //DiscountTimeStart
				Write32(pMoreInfo, 0); //DiscountTimeEnd
				Write8(pMoreInfo, 0); //DiscountStatus
				Write32(pMoreInfo, 0); //LimitPara
				Write8(pMoreInfo, 0); //ItemExchangeType
				Write8(pMoreInfo, 0); //ItemLabelType
				Write16(pMoreInfo, 0); //ShowPriority
				Write8(pMoreInfo, 0); //Para0
				Write32(pMoreInfo, 0); //LastUpdateTime

				len = pMoreInfo - pScoreAwardCfg;
				Set16(pScoreAwardCfg, (WORD)len);
				pScoreAwardCfg += len;
			}
			*/

			Write32(pScoreAwardCfg, 0); //PosKey
			Write8(pScoreAwardCfg, 0); //AwardIdx
			Write8(pScoreAwardCfg, 0); //LabelStatus

			Write8(pScoreAwardCfg, 0); //ExtraAwardNum
			for (size_t i = 0; i < 0; i++)
			{ //ExtraAwardInfo
				BYTE* pExtraAwardInfo = pScoreAwardCfg;
				Write16(pExtraAwardInfo, 0); //len

				Write32(pExtraAwardInfo, 0); //Exp
				Write32(pExtraAwardInfo, 0); //Money
				Write32(pExtraAwardInfo, 0); //SuperMoney
				Write16(pExtraAwardInfo, 0); //ItemNum
				for (size_t i = 0; i < 0; i++)
				{ //ItemInfo
					BYTE* pItemInfo = pExtraAwardInfo;
					Write16(pItemInfo, 0); //len

					Write32(pItemInfo, 0); //ItemID
					Write32(pItemInfo, 0); //ItemNum
					Write32(pItemInfo, 0); //AvailPeriod
					Write8(pItemInfo, 0); //Status
					Write32(pItemInfo, 0); //ObtainTime
					Write32(pItemInfo, 0); //OtherAttribute
					Write16(pItemInfo, 0); //ItemType

					len = pItemInfo - pExtraAwardInfo;
					Set16(pExtraAwardInfo, (WORD)len);
					pExtraAwardInfo += len;
				}
				Write32(pExtraAwardInfo, 0); //Coupons
				Write32(pExtraAwardInfo, 0); //GuildPoint
				Write32(pExtraAwardInfo, 0); //LuckMoney

				Write8(pExtraAwardInfo, 0); //ExtendInfoNum
				for (size_t i = 0; i < 0; i++)
				{ //ExtendInfoAward
					BYTE* pExtendInfoAward = pExtraAwardInfo;
					Write16(pExtendInfoAward, 0); //len

					Write16(pExtendInfoAward, 0); //Key
					Write32(pExtendInfoAward, 0); //AddValue
					Write32(pExtendInfoAward, 0); //TotalValue
					Write8(pExtendInfoAward, 0); //ShowPriority
					Write16(pExtendInfoAward, 0); //BuyNeedScore
					Write8(pExtendInfoAward, 0); //OneMatchMaxNum

					len = pExtendInfoAward - pExtraAwardInfo;
					Set16(pExtraAwardInfo, (WORD)len);
					pExtraAwardInfo += len;
				}

				Write32(pExtraAwardInfo, 0); //SpeedCoin

				len = pExtraAwardInfo - pScoreAwardCfg;
				Set16(pScoreAwardCfg, (WORD)len);
				pScoreAwardCfg += len;
			}

			len = pScoreAwardCfg - pAwardCfgArray;
			Set16(pAwardCfgArray, (WORD)len);
			pAwardCfgArray += len;
		}
		len = pAwardCfgArray - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); //AwardID
	Write32(p, 0); //NextClearLeftTime
	Write8(p, 0); //HasMoreInfo
	Write32(p, 0); //Score
	Write16(p, 0); //LeftGetAwardNum
	Write8(p, 0); //Status
	Write8(p, 0); //EnableMergeSameItem
	Write32(p, 0); //TimeStart
	Write32(p, 0); //TimeEnd
	Write32(p, 0); //SCPara
	Write8(p, 0); //IsShowAllAward
	Write16(p, 0); //ValueNum
	//Write32(p, 0); //Values[]
	Write32(p, 0); //EchoClient

	len = p - buf;
	SendToClient(Client, 24465, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}


void RequestAddToMatchQueue(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body;;

	UINT uin = Read32(p);
	UINT time = Read32(p);

	USHORT matchQueueType = Read16(p);
	UINT para1 = Read32(p);
	UCHAR IsAllArea = Read8(p);
	UCHAR paraNum = Read8(p);
	for (size_t i = 0; i < paraNum; i++)
	{
		UINT para = Read32(p);
	}
	int MapID = Read32(p);

	ResponseAddToMatchQueue(Client, matchQueueType);

	Client->MapCompatibilityMode = true;
	Client->EndCheckPoint = 0;
	Client->TotalRound = 1;
	Client->Loop = false;

	Client->MapID = 426;
	Client->FinishTime = 0;
	Client->Round = 0;
	Client->PassedCheckPoint = 0;

	NotifyMatchResult(Client, matchQueueType);

	NotifyGameBegin(Client, NULL);
	NotifyKartPhysParam(Client, NULL);
	NotifyOtherKartStoneInfo(Client);
}

void ResponseAddToMatchQueue(ClientNode* Client, USHORT matchQueueType)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //resultId
	Write8(p, 0); //reasonLen
	Write16(p, matchQueueType); //matchQueueType
	Write32(p, 0); //Para0
	Write8(p, 0); //IsAllArea
	Write32(p, 0); //MapID

	len = p - buf;
	SendToClient(Client, 20079, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void NotifyMatchResult(ClientNode* Client, USHORT matchQueueType)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //resultId
	Write8(p, 0); //reasonLen

	Write16(p, matchQueueType); //matchQueueType
	Write8(p, 0); //hasEnterInfo
	/*
	for (size_t i = 0; i < n; i++)
	{ //enterInfo
		BYTE* penterInfo = p;
		Write16(penterInfo, 0); //len

		{ //addrInfo
			BYTE* paddrInfo = penterInfo;
			Write16(paddrInfo, 0); //len

			WriteUn(paddrInfo, 0); //svrId
			WriteUn(paddrInfo, 0); //svrIP
			WriteUn(paddrInfo, 0); //tcpPortNum
			WriteUn(paddrInfo, 0); //tcpPort[]
			WriteUn(paddrInfo, 0); //udpPortNum
			WriteUn(paddrInfo, 0); //udpPort[]
			Write8(paddrInfo, 0); //HasMultiIPInfo
			for (size_t i = 0; i < n; i++)
			{ //MultiIPInfo
				BYTE* pMultiIPInfo = paddrInfo;
				Write16(pMultiIPInfo, 0); //len

				Write8(pMultiIPInfo, 0); //IPCount
				Write32(pMultiIPInfo, 0); //ServerIP[]
				Write8(pMultiIPInfo, 0); //DomainLen

				len = pMultiIPInfo - paddrInfo;
				Set16(paddrInfo, (WORD)len);
				paddrInfo += len;
			}

			len = paddrInfo - penterInfo;
			Set16(penterInfo, (WORD)len);
			penterInfo += len;
		}
		WriteUn(penterInfo, 0); //roomId
		Write16(penterInfo, 0); //SceneID

		len = penterInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write32(p, Client->MapID); //mapID
	Write16(p, 1); //MatchedRoomType
	Write16(p, 10); //TeamMatchType
	Write8(p, 0); //PVPType
	Write32(p, 0); //PVPKey
	Write32(p, 12345678); //GameSeqIDHigh
	Write32(p, 87654321); //GameSeqIDLow
	Write16(p, 0); //ForceTransCount
	//Write32(p, 0); //ForceTransMsgID[]
	Write16(p, 10); //PlayerMatchingType
	Write8(p, 0); //paraNum
	//Write32(p, 0); //paraList[]

	len = p - buf;
	SendToClient(Client, 20081, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestEnterMatchRoom(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT uin = Read32(Body);
	UINT time = Read32(Body);

	//Un roomId = ReadUn(Body);
	ResponseEnterMatchRoom(Client);
}

void ResponseEnterMatchRoom(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //resultId
	Write8(p, 0); //reasonLen

	Write8(p, 0); //HasMemoryRaceInfo
	/*
	for (size_t i = 0; i < n; i++)
	{ //Info
		BYTE* pInfo = p;
		Write16(pInfo, 0); //len

		Write32(pInfo, 0); //Year
		Write32(pInfo, 0); //KartNum
		for (size_t i = 0; i < n; i++)
		{ //Kart
			BYTE* pKart = pInfo;
			Write16(pKart, 0); //len

			Write32(pKart, 0); //Uin
			Write32(pKart, 0); //KartId
			Write32(pKart, 0); //RefitCout
			Write16(pKart, 0); //MaxFlags
			Write16(pKart, 0); //WWeight
			Write16(pKart, 0); //SpeedWeight
			Write16(pKart, 0); //JetWeight
			Write16(pKart, 0); //SJetWeight
			Write16(pKart, 0); //AccuWeight
			Write32(pKart, 0); //ShapeRefitCount
			Write32(pKart, 0); //KartHeadRefitItemID
			Write32(pKart, 0); //KartTailRefitItemID
			Write32(pKart, 0); //KartFlankRefitItemID
			Write32(pKart, 0); //KartTireRefitItemID
			{ //KartRefitExInfo
				BYTE* pKartRefitExInfo = pKart;
				Write16(pKartRefitExInfo, 0); //len

				Write8(pKartRefitExInfo, 0); //SpeedRefitStar
				Write8(pKartRefitExInfo, 0); //JetRefitStar
				Write8(pKartRefitExInfo, 0); //SJetRefitStar
				Write8(pKartRefitExInfo, 0); //AccuRefitStar
				Write8(pKartRefitExInfo, 0); //SpeedAddRatio
				Write8(pKartRefitExInfo, 0); //JetAddRatio
				Write8(pKartRefitExInfo, 0); //SJetAddRatio
				Write8(pKartRefitExInfo, 0); //AccuAddRatio

				len = pKartRefitExInfo - pKart;
				Set16(pKart, (WORD)len);
				pKart += len;
			}
			Write32(pKart, 0); //SecondRefitCount
			Write16(pKart, 0); //Speed2Weight
			Write16(pKart, 0); //DriftVecWeight
			Write16(pKart, 0); //AdditionalZSpeedWeight
			Write16(pKart, 0); //AntiCollisionWeight
			Write16(pKart, 0); //LuckyValue
			Write16(pKart, 0); //RefitLuckyValueMaxWeight
			Write32(pKart, 0); //ShapeSuitID
			Write8(pKart, 0); //LegendSuitLevel
			Write32(pKart, 0); //LegendSuitLevelChoice
			Write32(pKart, 0); //ShapeLegendSuitID

			len = pKart - pInfo;
			Set16(pInfo, (WORD)len);
			pInfo += len;
		}
		Write32(pInfo, 0); //SuitNum
		for (size_t i = 0; i < n; i++)
		{ //Suit
			BYTE* pSuit = pInfo;
			Write16(pSuit, 0); //len

			Write16(pSuit, 0); //Idx
			Write16(pSuit, 0); //ItemNum
			Write32(pSuit, 0); //ItemID[]

			len = pSuit - pInfo;
			Set16(pInfo, (WORD)len);
			pInfo += len;
		}
		Write32(pInfo, 0); //RandSuitIdx

		len = pInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	len = p - buf;
	SendToClient(Client, 20082, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}




void RequestLeaveGame(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT uin = Read32(Body);
	UINT time = Read32(Body);
	/*
	UINT GameSeqIDHigh = Read32(Body);
	UINT GameSeqIDLow = Read32(Body);
	USHORT LeaveGameType = Read16(Body);
	UCHAR ParaNum = Read8(Body);
	//int[] ParaList[] = Read32(Body);
	*/

	int ParaList[1];
	ParaList[0] = Client->MapID;
	NotifyGameOver(Client, 1, 1, ParaList);
	ResponseLeaveGame(Client);
}

void ResponseLeaveGame(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write16(p, 0); //ResultID

	len = p - buf;
	SendToClient(Client, 424, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Response);
}


void NotifyRandRoomNameList(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	const char* RandRoomNameList[] = { "RandRoomName A","RandRoomName B","RandRoomName C" };
	BYTE Count = sizeof(RandRoomNameList) / sizeof(RandRoomNameList[0]);
	Write8(p, Count);
	for (BYTE i = 0; i < Count; i++)
	{ //RoomNames
		BYTE* pRoomNames = p;
		Write16(pRoomNames, 0); //len

		len = strlen(RandRoomNameList[i]);
		Write8(pRoomNames, (BYTE)len); //NameLen
		memcpy(pRoomNames, RandRoomNameList[i], len);
		pRoomNames += len;

		for (size_t i = 0; i < 20; i++)
		{
			Write8(pRoomNames, 0); //BaseModes[]
		}

		len = pRoomNames - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 760, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}


void NotifyTopListDesc(ClientNode* Client)
{
	BYTE* buf = new BYTE[32768]; BYTE* p = buf; size_t len;

	try
	{
		YAML::Node Config = YAML::LoadFile("TopListDesc-ZingSpeed.yml");
		USHORT Count = (USHORT)Config.size();

		Write16(p, Count);
		for (USHORT i = 0; i < Count; i++)
		{ //Descs
			BYTE* pDescs = p;
			Write16(pDescs, 0); //len


			YAML::Node Descs = Config[i];

			Write16(pDescs, Descs["ID"].as<int>());

			std::string Name = Descs["Name"].as<std::string>();
			std::string ValueDesc = Descs["ValueDesc"].as<std::string>();
			std::string TitleName = Descs["TitleName"].as<std::string>();

			memset(pDescs, 0, MaxNickName);
			strncpy_s((char*)pDescs, MaxNickName, Name.c_str(), Name.length());
			pDescs += MaxNickName;

			memset(pDescs, 0, MaxNickName);
			strncpy_s((char*)pDescs, MaxNickName, ValueDesc.c_str(), ValueDesc.length());
			pDescs += MaxNickName;

			memset(pDescs, 0, MaxNickName);
			strncpy_s((char*)pDescs, MaxNickName, TitleName.c_str(), TitleName.length());
			pDescs += MaxNickName;

			Write8(pDescs, 0); //HasOtherInfo
			/*
			{ //Other
				BYTE* pOther = pDescs;
				Write16(pOther, 0); //len

				//WriteString(pOther, 0); //IntParaDesc0[]
				//WriteString(pOther, 0); //IntParaDesc1[]
				//WriteString(pOther, 0); //StrParaDesc0[]
				//WriteString(pOther, 0); //StrParaDesc1[]

				len = pOther - pDescs;
				Set16(pDescs, (WORD)len);
				pDescs += len;
			}
			*/

			Write8(pDescs, Descs["IsEnableSelfRank"].as<bool>());

			len = pDescs - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}
	catch (const std::exception&)
	{
		printf("异常\n");
		delete buf;
		return;
	}
	

	len = p - buf;
	SendToClient(Client, 701, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
	delete buf;
}

void RequestTopList(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	USHORT ID = Read16(p);
	UCHAR Type = Read8(p);

	ResponseTopList(Client,ID, Type);
}

void ResponseTopList(ClientNode* Client, USHORT ID, UCHAR Type)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write16(p, ID); //ID

	Write16(p, 1); //Count
	for (size_t i = 0; i < 1; i++)
	{ //Items
		BYTE* pItems = p;
		Write16(pItems, 0); //len

		Write32(pItems, 1); //Uin

		const char NickName[] = u8"NPC";
		memset(pItems, 0, MaxNickName);
		strncpy_s((char*)pItems, MaxNickName, NickName, sizeof(NickName));
		pItems += MaxNickName;

		Write32(pItems, 0); //Value

		Write8(pItems, 0); //HasOtherInfo
		/*
		for (size_t i = 0; i < n; i++)
		{ //Other
			BYTE* pOther = pItems;
			Write16(pOther, 0); //len

			Write32(pOther, 0); //IntPara0
			Write32(pOther, 0); //IntPara1
			WriteString(pOther, 0); //StrPara0[]
			WriteString(pOther, 0); //StrPara1[]
			Write32(pOther, 0); //IntPara2
			Write8(pOther, 0); //HasMapInfo
			for (size_t i = 0; i < n; i++)
			{ //MapInfo
				BYTE* pMapInfo = pOther;
				Write16(pMapInfo, 0); //len

				Write8(pMapInfo, 0); //MapItemNum
				for (size_t i = 0; i < n; i++)
				{ //MapItemList
					BYTE* pMapItemList = pMapInfo;
					Write16(pMapItemList, 0); //len

					Write32(pMapItemList, 0); //Uin
					Write8(pMapItemList, 0); //Area

					len = pMapItemList - pMapInfo;
					Set16(pMapInfo, (WORD)len);
					pMapInfo += len;
				}

				len = pMapInfo - pOther;
				Set16(pOther, (WORD)len);
				pOther += len;
			}

			len = pOther - pItems;
			Set16(pItems, (WORD)len);
			pItems += len;
		}
		*/

		Write8(pItems, 0); //AreaId
		Write8(pItems, 0); //ParaNum
		Write32(pItems, 0); //Para[]

		Write8(pItems, 0); //AnchorNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //AnchorInfo
			BYTE* pAnchorInfo = pItems;
			Write16(pAnchorInfo, 0); //len

			Write32(pAnchorInfo, 0); //Uin
			Write32(pAnchorInfo, 0); //RoomID
			Write8(pAnchorInfo, 0); //StrNum

			len = pAnchorInfo - pItems;
			Set16(pItems, (WORD)len);
			pItems += len;
		}
		*/

		len = pItems - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); //SelfRank
	Write32(p, 0); //BeginRank
	Write32(p, 0); //UperPercentage

	len = p - buf;
	SendToClient(Client, 461, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyMatchSkillStoneInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)//宝石通知其他玩家
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UINT ItemID = Read32(Body);

	if (ItemID < 21977 && ItemID>22072) return;

	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Uin);
	Write32(p, ItemID);

	len = p - buf;

	//printf("[GameServer][ConnID:%d][NotifyMatchSkillStoneInfo]Uin:%d, Time:%d, ItemID:%d\n", Client->ConnID, Uin, Time, ItemID);

	USHORT RoomID = Client->RoomID;
	RoomNode* Room = GetRoom(RoomID);

	if (!Room)
	{
		return;
	}
	//LeaveRoom(Client, 0);

	//通知其它玩家
	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient && RoomClient != Client)
		{
			SendToClient(RoomClient, 910, buf, len, Client->GameID, FE_GAMELOGIC, RoomClient->ConnID, Response);
		}
	}
}
