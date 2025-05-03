#include "Message.h"

#include <stdio.h>
#include <time.h>
#include <map>
#include <string>


#include "Database.h"
#include "Item.h"
#include "sqlite/sqlite3.h"
//#include <yaml-cpp/parser.h>
#include "yaml-cpp/yaml.h"
#include <iostream>
#include <sys/stat.h>
using namespace std;


extern sqlite3* PlayerDB;

wchar_t* UTF8ToW(char* str, int len)
{
	wchar_t* result = new wchar_t[len + 1];
	len = MultiByteToWideChar(CP_UTF8, 0, str, len, result, len);
	result[len] = 0;
	return result;
}

char* WToUTF8(wchar_t* str, int len)
{
	int len2 = len * 6; //UTF-8：每个字符用1-6个字节表示。
	char* result = new char[len2 + 1];
	len2 = WideCharToMultiByte(CP_UTF8, 0, str, len, result, len2, 0, NULL);
	result[len2] = 0;
	return result;
}

wchar_t* AToW(char* str, int len)
{
	wchar_t* result = new wchar_t[len + 1];
	len = MultiByteToWideChar(CP_ACP, 0, str, len, result, len);
	result[len] = 0;
	return result;
}

char* WToA(wchar_t* str, int len)
{
	int len2 = len * 2; //Ansi：每个字符用1-2个字节表示。
	char* result = new char[len2 + 1];
	len2 = WideCharToMultiByte(CP_ACP, 0, str, len, result, len2, 0, NULL);
	result[len2] = 0;
	return result;
}


// 2、将UTF8转换成GBK

std::string Utf8ToGbk(const char* src_str)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}


bool isFileExists_stat(char const* const name) {
	struct stat buffer;
	return (stat(name, &buffer) == 0);
}

void WritePlayerDBBaseInfo(BYTE*& p, UINT Uin)
{ //PlayerDBBaseInfo
	WORD len;
	BYTE* pPlayerDBBaseInfo = p;
	Write16(pPlayerDBBaseInfo, 0); //len

	std::string NickName = "";
	char Gender = 0;
	char Country = 0;
	UINT License = 0;
	UINT Experience = 0;
	UINT SuperMoney = 0;
	UINT Money = 0;
	UINT WinNum = 0;
	UINT SecondNum = 0;
	UINT ThirdNum = 0;
	UINT TotalNum = 0;
	UINT CurHonor = 0;
	UINT TotalHonor = 0;
	UINT TodayHonor = 0;
	UINT RelaxTime = 0;
	UINT MonthDurationBefor = 0;
	UINT MonthDurationCur = 0;
	UINT Charm = 0;
	UINT DurationGame = 0;
	UINT DanceExp = 0;
	UINT Coupons = 0;
	UINT Admiration = 0;
	UINT LuckMoney = 0;
	UINT TeamWorkExp = 0;
	UINT AchievePoint = 0;
	UINT RegTime = 0;
	std::string Signature = "";


	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	sql = "SELECT NickName,Gender,Country,License,Experience,SuperMoney,Money,WinNum,SecondNum,ThirdNum,TotalNum,CurHonor,TotalHonor,TodayHonor,RelaxTime,MonthDurationBefor,MonthDurationCur,Charm,DurationGame,DanceExp,Coupons,Admiration,LuckMoney,TeamWorkExp,AchievePoint,RegTime,Signature  FROM BaseInfo  WHERE Uin=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Uin);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			//NickName = Utf8ToGbk((char*)sqlite3_column_text(stmt, 0));//读取名称
			NickName = (char*)sqlite3_column_text(stmt, 0);//读取名称

            //printf("名字:%d\n", (char*)sqlite3_column_text(stmt, 0));
			

			Gender = sqlite3_column_int(stmt, 1);
			Country = sqlite3_column_int(stmt, 2);
			License = sqlite3_column_int(stmt, 3);
			Experience = sqlite3_column_int(stmt, 4);
			SuperMoney = sqlite3_column_int(stmt, 5);
			Money = sqlite3_column_int(stmt, 6);
			WinNum = sqlite3_column_int(stmt, 7);
			SecondNum = sqlite3_column_int(stmt, 8);
			ThirdNum = sqlite3_column_int(stmt, 9);
			TotalNum = sqlite3_column_int(stmt, 10);
			CurHonor = sqlite3_column_int(stmt, 11);
			TotalHonor = sqlite3_column_int(stmt, 12);
			TodayHonor = sqlite3_column_int(stmt, 13);
			RelaxTime = sqlite3_column_int(stmt, 14);
			MonthDurationBefor = sqlite3_column_int(stmt, 15);
			MonthDurationCur = sqlite3_column_int(stmt, 16);
			Charm = sqlite3_column_int(stmt, 17);
			DurationGame = sqlite3_column_int(stmt, 18);
			DanceExp = sqlite3_column_int(stmt, 19);
			Coupons = sqlite3_column_int(stmt, 20);
			Admiration = sqlite3_column_int(stmt, 21);
			LuckMoney = sqlite3_column_int(stmt, 22);
			TeamWorkExp = sqlite3_column_int(stmt, 23);
			AchievePoint = sqlite3_column_int(stmt, 24);
			RegTime = sqlite3_column_int(stmt, 25);
			Signature = (char*)sqlite3_column_text(stmt, 26);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	strcpy_s((char*)pPlayerDBBaseInfo, MaxNickName, NickName.c_str());
	pPlayerDBBaseInfo += MaxNickName;

	Write8(pPlayerDBBaseInfo, Gender|0x48);
	Write8(pPlayerDBBaseInfo, Country);
	Write32(pPlayerDBBaseInfo, License);
	Write32(pPlayerDBBaseInfo, Experience);
	Write32(pPlayerDBBaseInfo, SuperMoney); //点券
	Write32(pPlayerDBBaseInfo, Money); //酷币
	Write32(pPlayerDBBaseInfo, WinNum);
	Write32(pPlayerDBBaseInfo, SecondNum);
	Write32(pPlayerDBBaseInfo, ThirdNum);
	Write32(pPlayerDBBaseInfo, TotalNum);
	Write32(pPlayerDBBaseInfo, CurHonor);
	Write32(pPlayerDBBaseInfo, TotalHonor);
	Write32(pPlayerDBBaseInfo, TodayHonor);
	Write32(pPlayerDBBaseInfo, RelaxTime);
	Write32(pPlayerDBBaseInfo, MonthDurationBefor);
	Write32(pPlayerDBBaseInfo, MonthDurationCur);
	Write32(pPlayerDBBaseInfo, Charm);
	Write32(pPlayerDBBaseInfo, DurationGame);
	Write32(pPlayerDBBaseInfo, DanceExp);
	Write32(pPlayerDBBaseInfo, Coupons); //消费券
	Write32(pPlayerDBBaseInfo, Admiration);
	Write32(pPlayerDBBaseInfo, LuckMoney);
	Write32(pPlayerDBBaseInfo, TeamWorkExp);
	Write32(pPlayerDBBaseInfo, AchievePoint);
	Write32(pPlayerDBBaseInfo, RegTime);

	strcpy_s((char*)pPlayerDBBaseInfo, MaxSignature, Signature.c_str());
	pPlayerDBBaseInfo += MaxSignature;

	Write32(pPlayerDBBaseInfo, 0); //LuckyMatchPoint
	Write32(pPlayerDBBaseInfo, 0); //LuckyMatchNum
	Write16(pPlayerDBBaseInfo, 0); //LuckyMatchFirstNum
	Write8(pPlayerDBBaseInfo, 0); //LuckyMatchBestRand
	Write32(pPlayerDBBaseInfo, 0); //LuckyMatchWinNum
	Write32(pPlayerDBBaseInfo, 0); //FizzPointTotal
	Write16(pPlayerDBBaseInfo, 0); //FizzPointDaily
	Write16(pPlayerDBBaseInfo, 0); //FizzPointWeekly
	Write16(pPlayerDBBaseInfo, 0); //FizzPointLastWeek
	Write8(pPlayerDBBaseInfo, 0); //FizzLotteryStatus
	Write32(pPlayerDBBaseInfo, 0); //FizzLastUpdateTime
	Write32(pPlayerDBBaseInfo, 0); //SNSLocaleCode
	Write32(pPlayerDBBaseInfo, 0); //AuctionRightUnlockTime
	Write8(pPlayerDBBaseInfo, 0); //ReserveFlag
	Write32(pPlayerDBBaseInfo, 0); //CurConsumeScore
	Write32(pPlayerDBBaseInfo, 0); //HistoryConsumeScore
	Write8(pPlayerDBBaseInfo, 0); //CrashModeSponsorID
	Write32(pPlayerDBBaseInfo, 0); //Popularity
	Write32(pPlayerDBBaseInfo, 0); //LadderMatchAchievePoint
	Write32(pPlayerDBBaseInfo, 0); //Cash
	Write32(pPlayerDBBaseInfo, 0); //PlayerGPMIdentity
	Write32(pPlayerDBBaseInfo, 0); //PointTotalScore
	Write32(pPlayerDBBaseInfo, 0); //totalMapMedalNum
	Write32(pPlayerDBBaseInfo, 0); //lastMapMedalNum
	Write32(pPlayerDBBaseInfo, 0); //curMapMedalNum
	Write32(pPlayerDBBaseInfo, 0); //curMapMedalSeasonId
	Write32(pPlayerDBBaseInfo, 0); //lastClearMapMedalTime
#ifndef ZingSpeed
	Write32(pPlayerDBBaseInfo, 0); //max2048Score
	Write32(pPlayerDBBaseInfo, 0); //curPlay2048LeftNum
	Write32(pPlayerDBBaseInfo, 0); //CheerValue
	Write16(pPlayerDBBaseInfo, 0); //SeasonID
	Write32(pPlayerDBBaseInfo, 0); //LastCheerValue
	Write32(pPlayerDBBaseInfo, 0); //SpeedBean
	Write32(pPlayerDBBaseInfo, 0); //SpeedCoin
#endif

	len = pPlayerDBBaseInfo - p;
	Set16(p, len);
	p += len;
}

void WritePlayerGuildInfo(BYTE*& p, UINT Uin)
{ //PlayerGuildInfo
	WORD len;

	BYTE* pPlayerGuildInfo = p;
	Write16(pPlayerGuildInfo, 0); //len

	
	strcpy_s((char*)pPlayerGuildInfo, MaxNickName, ""); //GuildName
	
	pPlayerGuildInfo += MaxNickName;

	Write16(pPlayerGuildInfo, 0); //Duty
	Write32(pPlayerGuildInfo, 0); //GuildID
	Write32(pPlayerGuildInfo, 0); //Right
	Write32(pPlayerGuildInfo, 0); //Medal
	Write32(pPlayerGuildInfo, 0); //Contribute

	memset(pPlayerGuildInfo, 0, 9); //m_szDutyTitle[]:
	pPlayerGuildInfo += 9;

	len = pPlayerGuildInfo - p;
	Set16(p, len);
	p += len;
}

UINT WriteEquipedItem(BYTE*& p, UINT Uin)
{ //EquipedItem
	WORD len;

	BYTE* pItemNum = p;
	Write16(p, 0); //EquipedItemNum

	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	size_t i = 0;
	UINT KartItemId = 0;
	sql = "SELECT ItemID,ItemNum,AvailPeriod,Status,ObtainTime,OtherAttribute  FROM Item WHERE Uin = ?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Uin);
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			UINT ItemID = sqlite3_column_int(stmt, 0);
			BYTE Status = sqlite3_column_int(stmt, 3);

			if (Status)
			{ //PlayerItemInfo
				i++;
				if (GetItemType(ItemID) == EAIT_CAR)
				{
					KartItemId = ItemID;
				}

				BYTE* pItemInfo = p;
				Write16(pItemInfo, 0); //len

				Write32(pItemInfo, ItemID);
				Write32(pItemInfo, sqlite3_column_int(stmt, 1));
				Write32(pItemInfo, sqlite3_column_int(stmt, 2));
				Write8(pItemInfo, Status);
				Write32(pItemInfo, sqlite3_column_int(stmt, 4));
				Write32(pItemInfo, sqlite3_column_int(stmt, 5));
#ifndef ZingSpeed
				Write16(pItemInfo, 0); //ItemType
#endif
				len = pItemInfo - p;
				Set16(p, (WORD)len);
				p += len;
			}
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	Set16(pItemNum, (WORD)i);
	return KartItemId;
}


void WriteVisbleInfo(BYTE*& p, UINT Uin, char SeatID, short PlayerID)
{ //PlayerRoomVisibleInfo
	WORD len;
	//int Identity = ID_IDENTIFY_QQLEVEL1 | ID_IDENTIFY_QQLEVEL2 | ID_IDENTIFY_QQLEVEL3 | ID_IDENTIFY_SPEEDMEMBER | ID_IDENTIFY_QQFLAG | ID_IDENTIFY_HAVEWORD | ID_IDENTIFY_FINISHQQFLAG | ID_IDENTIFY_JUDGE | ID_IDENTIFY_QQGAMEVIP | ID_IDENTIFY_GOLDNETBARIP | ID_IDENTIFY_NETBARMATCHIP;
	int Identity = ID_IDENTIFY_QQLEVEL1 | ID_IDENTIFY_QQLEVEL2 | ID_IDENTIFY_QQLEVEL3 | ID_IDENTIFY_SPEEDMEMBER | ID_IDENTIFY_QQFLAG | ID_IDENTIFY_HAVEWORD | ID_IDENTIFY_FINISHQQFLAG;

	BYTE* pPlayerRoomVisibleInfo = p;
	Write16(pPlayerRoomVisibleInfo, 0); //len

	Write32(pPlayerRoomVisibleInfo, Uin); //Uin
	Write32(pPlayerRoomVisibleInfo, Identity); //Identity
	Write8(pPlayerRoomVisibleInfo, SeatID); //SeatID
	Write8(pPlayerRoomVisibleInfo, 0); //TeamID
	Write16(pPlayerRoomVisibleInfo, PlayerID); //PlayerID
	Write8(pPlayerRoomVisibleInfo, 0); //Status

	WritePlayerDBBaseInfo(pPlayerRoomVisibleInfo, Uin);
	WritePlayerGuildInfo(pPlayerRoomVisibleInfo, Uin);
	WriteEquipedItem(pPlayerRoomVisibleInfo, Uin);

	Write16(pPlayerRoomVisibleInfo, 0); //PetNum
	/*
m_stPlayerRoomVisibleInfo[].m_astPetInfo[].m_uiUin
m_stPlayerRoomVisibleInfo[].m_astPetInfo[].m_iPetId
m_stPlayerRoomVisibleInfo[].m_astPetInfo[].m_szPetName[]:
m_stPlayerRoomVisibleInfo[].m_astPetInfo[].m_cIsProper
m_stPlayerRoomVisibleInfo[].m_astPetInfo[].m_usLevel
m_stPlayerRoomVisibleInfo[].m_astPetInfo[].m_ucStatus
m_stPlayerRoomVisibleInfo[].m_astPetInfo[].m_usPetSkillNum
m_stPlayerRoomVisibleInfo[].m_astPetInfo[].m_astPetSkillList[].m_usSkillID
m_stPlayerRoomVisibleInfo[].m_astPetInfo[].m_astPetSkillList[].m_ucStatus
m_stPlayerRoomVisibleInfo[].m_astPetInfo[].m_astPetSkillList[].m_cActive
m_stPlayerRoomVisibleInfo[].m_astPetInfo[].m_astPetSkillList[].m_iValue
m_stPlayerRoomVisibleInfo[].m_astPetInfo[].m_iStrengLevel
	*/

	{ //PlayerGuildMatchInfo
		BYTE* pPlayerGuildMatchInfo = pPlayerRoomVisibleInfo;
		Write16(pPlayerGuildMatchInfo, 0); //len

		Write32(pPlayerGuildMatchInfo, 0); //SelfPoint
		Write32(pPlayerGuildMatchInfo, 0); //WinNum
		Write32(pPlayerGuildMatchInfo, 0); //TotalNum

		len = pPlayerGuildMatchInfo - pPlayerRoomVisibleInfo;
		Set16(pPlayerRoomVisibleInfo, (WORD)len);
		pPlayerRoomVisibleInfo += len;
	}


	Write8(pPlayerRoomVisibleInfo, true); //IsInTopList
	Write8(pPlayerRoomVisibleInfo, 0); //LoverRaceOrPro
	Write8(pPlayerRoomVisibleInfo, 0); //TmpEffectNum
	//m_stPlayerRoomVisibleInfo[].m_aiTmpEffectItems[]


	Write8(pPlayerRoomVisibleInfo, 0); //OBState
	Write8(pPlayerRoomVisibleInfo, 0); //DebutOrX5
	Write8(pPlayerRoomVisibleInfo, 0); //RandKeyFlag


	Write16(pPlayerRoomVisibleInfo, 0x0106); //VipFlag

	Write8(pPlayerRoomVisibleInfo, 0); //HaveAppellation
	/*
m_stPlayerRoomVisibleInfo[].m_astCurAppellation[].m_u8Type
m_stPlayerRoomVisibleInfo[].m_astCurAppellation[].m_u8Level
m_stPlayerRoomVisibleInfo[].m_astCurAppellation[].m_u8Status
m_stPlayerRoomVisibleInfo[].m_astCurAppellation[].m_u8Difficulty
m_stPlayerRoomVisibleInfo[].m_astCurAppellation[].m_i32MapId
m_stPlayerRoomVisibleInfo[].m_astCurAppellation[].m_u32Value
	*/

	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

	int Level1 = 6;
	int Level2 = 0;
	int Level3 = 0;
	int Level4 = 0;
	int Level5 = 0;

	sql = "SELECT VipLevel1,VipLevel2,VipLevel3,VipLevel4,VipLevel5 FROM Player WHERE Uin = ?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Uin);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			Level1 = sqlite3_column_int(stmt, 0);//皇族
			Level2 = sqlite3_column_int(stmt, 1);//情侣
			Level3 = sqlite3_column_int(stmt, 2);//花园
			Level4 = sqlite3_column_int(stmt, 3);//魅力
			Level5 = sqlite3_column_int(stmt, 4);//钻皇
			//printf("VipLevel：%d\n", Level);
		}
		else
		{
			Level1 = 6;
			Level2 = 0;
			Level3 = 0;
			Level4 = 0;
			Level5 = 0;
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;


       //NobleInfo  皇族
		BYTE* pNobleInfo = pPlayerRoomVisibleInfo;
		Write16(pNobleInfo, 0); //len

		Write32(pNobleInfo, Uin); //NobleID
		Write8(pNobleInfo, Level1); //NobleLevel
		Write32(pNobleInfo, 1); //NoblePoint
		Write32(pNobleInfo, 30); //NobleLeftDays
		
		len = pNobleInfo - pPlayerRoomVisibleInfo;
		Set16(pPlayerRoomVisibleInfo, (WORD)len);
		pPlayerRoomVisibleInfo += len;
	
		Write8(pPlayerRoomVisibleInfo, 0); //HasCarryWizard
		/*
	m_stPlayerRoomVisibleInfo[].m_astCarryWizardInfo[].m_ushWizardID
	m_stPlayerRoomVisibleInfo[].m_astCarryWizardInfo[].m_szNickname[]:
	m_stPlayerRoomVisibleInfo[].m_astCarryWizardInfo[].m_ushWizardType
	m_stPlayerRoomVisibleInfo[].m_astCarryWizardInfo[].m_bOrder
	m_stPlayerRoomVisibleInfo[].m_astCarryWizardInfo[].m_bEvolutionBranch
	m_stPlayerRoomVisibleInfo[].m_astCarryWizardInfo[].m_bIsHuanHua
		*/

		{ //GuildVipBaseInfo//车队贵族
			BYTE* pGuildVipBaseInfo = pPlayerRoomVisibleInfo;
			Write16(pGuildVipBaseInfo, 0); //len

			Write8(pGuildVipBaseInfo, Level5); //GuildVipLevel 车队贵族等级
			Write32(pGuildVipBaseInfo, 0); //GuildVipPoint 车队贵族点数

			len = pGuildVipBaseInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}

		Write32(pPlayerRoomVisibleInfo, 0); //ExFlag

		Write8(pPlayerRoomVisibleInfo, 0); //HaveLDMBaseInfo
		/*
	m_stPlayerRoomVisibleInfo[].m_stPlayerLDMBaseInfo[].m_u8Grade
	m_stPlayerRoomVisibleInfo[].m_stPlayerLDMBaseInfo[].m_u32Score
	m_stPlayerRoomVisibleInfo[].m_stPlayerLDMBaseInfo[].m_u32MaxScore
	m_stPlayerRoomVisibleInfo[].m_stPlayerLDMBaseInfo[].m_u32MaxGrade
		*/

		Write8(pPlayerRoomVisibleInfo, 0); //HasWl
		/*
	m_stPlayerRoomVisibleInfo[].m_astPlayerWlVisibleInfo[].m_uiWonderLandID
	m_stPlayerRoomVisibleInfo[].m_astPlayerWlVisibleInfo[].m_szWonderLandName[]:
	m_stPlayerRoomVisibleInfo[].m_astPlayerWlVisibleInfo[].m_ushWonderLandDuty
	m_stPlayerRoomVisibleInfo[].m_astPlayerWlVisibleInfo[].m_ucSubType
	m_stPlayerRoomVisibleInfo[].m_astPlayerWlVisibleInfo[].m_bGuildGrade
		*/

		Write8(pPlayerRoomVisibleInfo, 1); //HasLoverVip //情侣贵族
		{ //情侣贵族
			BYTE* pLoverVipBaseInfo = pPlayerRoomVisibleInfo;
			Write16(pLoverVipBaseInfo, 0); //len

			Write8(pLoverVipBaseInfo, Level2); //LoverVipLevel 情侣贵族等级
			Write32(pLoverVipBaseInfo, 100); //LoverVipPoint 情侣贵族点数
			Write8(pLoverVipBaseInfo, 4100); //GrowRate

			len = pLoverVipBaseInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}

		Write8(pPlayerRoomVisibleInfo, 0); //0
		Write8(pPlayerRoomVisibleInfo, 1); //1
		{ //PersonalGardenBaseInfo
			BYTE* pPersonalGardenBaseInfo = pPlayerRoomVisibleInfo;
			Write16(pPersonalGardenBaseInfo, 0); //len

			Write8(pPersonalGardenBaseInfo, Level3); //GardenLevel 花园等级
			Write32(pPersonalGardenBaseInfo, 5); //GardenPoint 花园点数
			Write8(pPersonalGardenBaseInfo, 5); //GrowRate
			Write32(pPersonalGardenBaseInfo, 5); //GardenExpireTime
			{ //SimpleInfo
				BYTE* pSimpleInfo = pPersonalGardenBaseInfo;
				Write16(pSimpleInfo, 0); //len

				Write32(pSimpleInfo, 0); //WeekPopularity
				Write32(pSimpleInfo, 0); //TotalPopularity
				Write32(pSimpleInfo, 0); //LastUpdatePopularityTime
				Write8(pSimpleInfo, 0); //PrivateType


				len = pSimpleInfo - pPersonalGardenBaseInfo;
				Set16(pPersonalGardenBaseInfo, (WORD)len);
				pPersonalGardenBaseInfo += len;
			}

			len = pPersonalGardenBaseInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}	
		{ //ConsumeVipInfo  魅力达人
			BYTE* pConsumeVipInfo = pPlayerRoomVisibleInfo;
			Write16(pConsumeVipInfo, 0); //len

			Write32(pConsumeVipInfo, Level4); //VipLevel 魅力等级
			Write32(pConsumeVipInfo, 0); //CharmValueOfMonth
			Write32(pConsumeVipInfo, 0); //SearchTreasureNums
			Write32(pConsumeVipInfo, 0); //GetTreasureNums

			len = pConsumeVipInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		Write8(pPlayerRoomVisibleInfo, 0); //HaveBaby
		{ //LoverBabyInfo
			BYTE* pLoverBabyInfo = pPlayerRoomVisibleInfo;
			Write16(pLoverBabyInfo, 0); //len

			Write32(pLoverBabyInfo, 0); //LoverBabyID
			Write32(pLoverBabyInfo, 0); //ItemID
			Write32(pLoverBabyInfo, 0); //GrowValue
			Write32(pLoverBabyInfo, 0); //MentalValue
			Write32(pLoverBabyInfo, 0); //BabyProfileLevel
			Write32(pLoverBabyInfo, 0); //StrengthLevel
			Write32(pLoverBabyInfo, 0); //GrowLevel
			//WriteString(pLoverBabyInfo, 0); //LoveBabyNickName[]
			Write16(pLoverBabyInfo, 0); //LoverBabyStat
			Write16(pLoverBabyInfo, 0); //LoverBabyEquipStat
			Write16(pLoverBabyInfo, 0); //LoverBabyTransferdStatus

			len = pLoverBabyInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		{ //EmperorInfo //钻皇
			BYTE* pEmperorInfo = pPlayerRoomVisibleInfo;
			Write16(pEmperorInfo, 0); //len

			Write8(pEmperorInfo, Level5); //EmperorLevel 钻皇等级
			Write32(pEmperorInfo, 6666); //EmperorPoint 钻皇点数
			Write32(pEmperorInfo, 1); //EmperorLeftDays 钻皇到期时间
			Write8(pEmperorInfo, 31); //EmperorGrowRate 钻皇成长率

			len = pEmperorInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		len = pPlayerRoomVisibleInfo - p;
		Set16(p, len);
		p += len;
}



void RequestRegister2(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;


	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);


	sql = "BEGIN"; //开始事务
	result = sqlite3_exec(PlayerDB, sql, NULL, NULL, NULL);

	sql = "INSERT INTO Player (Uin,VipFlag,IsInTopList,VipLevel1,VipLevel2,VipLevel3,VipLevel4,VipLevel5) VALUES (?,?,?,?,?,?,?,?);";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, 0x0106);
		sqlite3_bind_int(stmt, 3, 1);
		sqlite3_bind_int(stmt, 4, 6);
		sqlite3_bind_int(stmt, 5, 0);
		sqlite3_bind_int(stmt, 6, 0);
		sqlite3_bind_int(stmt, 7, 0);
		sqlite3_bind_int(stmt, 8, 0);

		result = sqlite3_step(stmt);
	}
	else
	{
		printf("%s\n", sqlite3_errmsg(PlayerDB));
	}
	sqlite3_finalize(stmt);
	stmt = NULL;


	const char* Nickname = (char*)Body;
	Body += MaxNickName;

	BYTE Gender = Read8(Body);
	BYTE Country = Read8(Body);


	sql = "INSERT INTO BaseInfo (Uin,NickName,Gender,Country,Experience,SuperMoney,Coupons) VALUES (?,?,?,?,?,?,?);";//注册名称
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_text(stmt, 2, Nickname, strlen(Nickname), SQLITE_TRANSIENT);
		sqlite3_bind_int(stmt, 3, Gender);
		sqlite3_bind_int(stmt, 4, Country);
		sqlite3_bind_int(stmt, 5, 99999999);//经验
		sqlite3_bind_int(stmt, 6, 263666);//点券
		sqlite3_bind_int(stmt, 7, 3000);//消费券
		result = sqlite3_step(stmt);
	}
	else
	{
		printf("%s\n", sqlite3_errmsg(PlayerDB));
	}

	UINT RoleID = Read32(Body);
	UINT HairID = Read32(Body);
	UINT FaceID = Read32(Body);
	UINT CoatID = Read32(Body);
	UINT GloveID = Read32(Body);
	UINT TrousersID = Read32(Body);
	UINT KartID = Read32(Body);
	UINT ColorID = Read32(Body);

	PlayerDB_AddItem(Client->Uin, RoleID, 1, -1, true, 0);
	PlayerDB_AddItem(Client->Uin, HairID, 1, -1, true, 0);
	PlayerDB_AddItem(Client->Uin, FaceID, 1, -1, true, 0);
	PlayerDB_AddItem(Client->Uin, CoatID, 1, -1, true, 0);
	PlayerDB_AddItem(Client->Uin, GloveID, 1, -1, true, 0);
	PlayerDB_AddItem(Client->Uin, TrousersID, 1, -1, true, 0);
	PlayerDB_AddItem(Client->Uin, KartID, 1, -1, true, 1);
	PlayerDB_AddItem(Client->Uin, ColorID, 1, -1, true, 0);
	//3D 男装
	PlayerDB_AddItem(Client->Uin, 27178, 1, -1, true, 0);
	PlayerDB_AddItem(Client->Uin, 27179, 1, -1, true, 0);
	PlayerDB_AddItem(Client->Uin, 27180, 1, -1, true, 0);
	PlayerDB_AddItem(Client->Uin, 27181, 1, -1, true, 0);
	PlayerDB_AddItem(Client->Uin, 27182, 1, -1, true, 0);
	//3D 女装
	PlayerDB_AddItem(Client->Uin, 27190, 1, -1, true, 0);
	PlayerDB_AddItem(Client->Uin, 27191, 1, -1, true, 0);
	PlayerDB_AddItem(Client->Uin, 27192, 1, -1, true, 0);
	PlayerDB_AddItem(Client->Uin, 27193, 1, -1, true, 0);
	PlayerDB_AddItem(Client->Uin, 27194, 1, -1, true, 0);


	PlayerDB_AddItem(Client->Uin, 10993, 1, -1, false, 1); //傀儡
	PlayerDB_AddItem(Client->Uin, 12720, 1, -1, false, 1); //雷诺
	PlayerDB_AddItem(Client->Uin, 15225, 1, -1, false, 1); //飞碟
	PlayerDB_AddItem(Client->Uin, 20244, 1, -1, false, 1); //通天晓
	PlayerDB_AddItem(Client->Uin, 27519, 1, -1, false, 1); //沙暴
	PlayerDB_AddItem(Client->Uin, 10024, 1, -1, false, 1);//[A]战车
	//PlayerDB_AddItem(Client->Uin, 98830, 1, -1, false, 1);//[S]爆天雷诺
	//PlayerDB_AddItem(Client->Uin, 127438, 1, -1, false, 1);//[T3]六道魔尊
	//PlayerDB_AddItem(Client->Uin, 126347, 1, -1, false, 1);//[S]极星之翼
	//PlayerDB_AddItem(Client->Uin, 109563, 1, -1, false, 1);//[S]至尊·涅槃
	//PlayerDB_AddItem(Client->Uin, 119059, 1, -1, false, 1);//[S]至曜·影虎
	//PlayerDB_AddItem(Client->Uin, 127167, 1, -1, false, 0);//帝皇服装男
	//PlayerDB_AddItem(Client->Uin, 127180, 1, -1, false, 0);//帝皇服装女
	//PlayerDB_AddItem(Client->Uin, 120646, 1, -1, false, 1);//镇海
	//PlayerDB_AddItem(Client->Uin, 63429, 1, -1, false, 1);//终极幻影
	//PlayerDB_AddItem(Client->Uin, 115164, 1, -1, false, 1);//兰博基尼
	//PlayerDB_AddItem(Client->Uin, 123508, 1, -1, false, 1);//柯尼塞格
	//PlayerDB_AddItem(Client->Uin, 126345, 1, -1, false, 1);//源极之星
	//PlayerDB_AddItem(Client->Uin, 62773, 1, -1, false, 1);//战争雷诺
	//PlayerDB_AddItem(Client->Uin, 98637, 1, -1, false, 1);//地狱雷诺
	//PlayerDB_AddItem(Client->Uin, 126349, 1, -1, false, 1);//萌兔雷诺
	//PlayerDB_AddItem(Client->Uin, 119058, 1, -1, false, 1);//上古之神
	//PlayerDB_AddItem(Client->Uin, 102219, 1, -1, false, 1);//圣殿剑魂
	//PlayerDB_AddItem(Client->Uin, 106674, 1, -1, false, 1);//[S]至尊.爆天甲
	//PlayerDB_AddItem(Client->Uin, 106675, 1, -1, false, 1);//[S]终极众神之神
	//PlayerDB_AddItem(Client->Uin, 127620, 1, -1, false, 1);//[A]童心之翼
	//PlayerDB_AddItem(Client->Uin, 129287, 1, -1, false, 1);//阿尔法之芯
	//PlayerDB_AddItem(Client->Uin, 129099, 1, -1, false, 1);//至尊·火麒麟
	//PlayerDB_AddItem(Client->Uin, 127524, 1, -1, false, 1);//时许使者
	//PlayerDB_AddItem(Client->Uin, 123514, 1, -1, false, 1);//超核玩家·逐魂者
	//PlayerDB_AddItem(Client->Uin, 122196, 1, -1, false, 1);//至尊帝王·心悦会员
	//PlayerDB_AddItem(Client->Uin, 98263, 1, -1, false, 1);//黑曼巴
	//PlayerDB_AddItem(Client->Uin, 94841, 1, -1, false, 1);//冠军之魄
	//PlayerDB_AddItem(Client->Uin, 70047, 1, -1, false, 1);//终极迈凯伦
	//PlayerDB_AddItem(Client->Uin, 126351, 1, -1, false, 1);//穹音星鲸
	//PlayerDB_AddItem(Client->Uin, 124685, 1, -1, false, 1);//星煌
	//PlayerDB_AddItem(Client->Uin, 121862, 1, -1, false, 1);//SSC银天使
	//PlayerDB_AddItem(Client->Uin, 109560, 1, -1, false, 1);//圣域炽天使
	//PlayerDB_AddItem(Client->Uin, 126346, 1, -1, false, 1);//幻音兔女郎
	//PlayerDB_AddItem(Client->Uin, 101885, 1, -1, false, 1);//终极黑域电摩
	//PlayerDB_AddItem(Client->Uin, 85949, 1, -1, false, 1);//终极紫电神驹
	//PlayerDB_AddItem(Client->Uin, 83042, 1, -1, false, 1);//终极变形销魂
	PlayerDB_AddItem(Client->Uin, 126537, 1, -1, false, 1);//雷诺皮肤
	PlayerDB_AddItem(Client->Uin, 127359, 1, -1, false, 1);//雷诺皮肤
	//PlayerDB_AddItem(Client->Uin, 127441, 1, -1, false, 1);//[皮肤]六道魔尊
	//PlayerDB_AddItem(Client->Uin, 126357, 1, -1, false, 1);//[皮肤]黑域电魔-玄冥
	//PlayerDB_AddItem(Client->Uin, 126356, 1, -1, false, 1);//[皮肤]终极猛兽-幽冥骑士
	//PlayerDB_AddItem(Client->Uin, 124680, 1, -1, false, 1);//[皮肤]银天使-蓐收
	//PlayerDB_AddItem(Client->Uin, 123501, 1, -1, false, 1);//[皮肤]至尊·火鲤
	//PlayerDB_AddItem(Client->Uin, 123502, 1, -1, false, 1);//[皮肤]M18-祝融
	//PlayerDB_AddItem(Client->Uin, 123506, 1, -1, false, 1);//[皮肤]雷诺-车王盾铠
	//PlayerDB_AddItem(Client->Uin, 122203, 1, -1, false, 1);//[皮肤]白影神驹-赵云
	//PlayerDB_AddItem(Client->Uin, 120657, 1, -1, false, 1);//[皮肤]智慧之神-句芒
	//PlayerDB_AddItem(Client->Uin, 113415, 1, -1, false, 1);//[皮肤]智慧之神Kitty限定
	//PlayerDB_AddItem(Client->Uin, 110709, 1, -1, false, 1);//[皮肤]爆天甲-朋克
	//PlayerDB_AddItem(Client->Uin, 110710, 1, -1, false, 1);//[皮肤]至尊-冰凤
	PlayerDB_AddItem(Client->Uin, 127365, 1, -1, false, 1);//[皮肤]罗莎
	PlayerDB_AddItem(Client->Uin, 127360, 1, -1, false, 1);//[皮肤]罗莎


	//PlayerDB_AddItem(Client->Uin, 78292, 1, -1, false, 1);//[T3]创世之神
	//PlayerDB_AddItem(Client->Uin, 85942, 1, -1, false, 1);//[T3]圣域大天使
	//PlayerDB_AddItem(Client->Uin, 94838, 1, -1, false, 1);//[T3]上古魔尊
	//PlayerDB_AddItem(Client->Uin, 71452, 1, -1, false, 1);//[T3]神谕天尊
	//PlayerDB_AddItem(Client->Uin, 123512, 1, -1, false, 1);//[T2悟空
	////PlayerDB_AddItem(Client->Uin, 123237, 1, -1, false, 1);//[皮肤]悟空·一级形态
	PlayerDB_AddItem(Client->Uin, 123238, 1, -1, false, 1);//[皮肤]悟空·二级形态
	//PlayerDB_AddItem(Client->Uin, 123497, 1, -1, false, 1);//[T2]虚空魔王
	//PlayerDB_AddItem(Client->Uin, 124678, 1, -1, false, 1);//[T2]钢铁
	//PlayerDB_AddItem(Client->Uin, 124677, 1, -1, false, 1);//[T2]战斗
	//PlayerDB_AddItem(Client->Uin, 122198, 1, -1, false, 1);//[T2]神威·马超
	//PlayerDB_AddItem(Client->Uin, 123498, 1, -1, false, 1);//[T1]黄河魂·九鼎
	//PlayerDB_AddItem(Client->Uin, 122197, 1, -1, false, 1);//[T1]星际探索者
	//PlayerDB_AddItem(Client->Uin, 122206, 1, -1, false, 1);//[T2]烈域使者
	//PlayerDB_AddItem(Client->Uin, 120656, 1, -1, false, 1);//[T2]镇魔天王
	//PlayerDB_AddItem(Client->Uin, 127618, 1, -1, false, 1);//[皮肤]盘龙-暗影
	//PlayerDB_AddItem(Client->Uin, 127617, 1, -1, false, 1);//[皮肤]玄武-重明
	//PlayerDB_AddItem(Client->Uin, 127616, 1, -1, false, 1);//[皮肤]至尊-玉麒麟
	////PlayerDB_AddItem(Client->Uin, 127436, 1, -1, false, 1);//[皮肤]源极1级形态
	PlayerDB_AddItem(Client->Uin, 127437, 1, -1, false, 1);//[皮肤]源极2级形态
	//PlayerDB_AddItem(Client->Uin, 126532, 1, -1, false, 1);//[S]帝皇侠
	PlayerDB_AddItem(Client->Uin, 126355, 1, -1, false, 1);//[皮肤]源极之星XEA-黑曜石
	//PlayerDB_AddItem(Client->Uin, 124679, 1, -1, false, 1);//[S]万里江山
	PlayerDB_AddItem(Client->Uin, 123513, 1, -1, false, 1);//[皮肤]悟空-齐天大圣
	//PlayerDB_AddItem(Client->Uin, 123499, 1, -1, false, 1);//[S]炼狱之刃
	//////PlayerDB_AddItem(Client->Uin, 123237, 1, -1, false, 1);//[皮肤]一级形态
	//////PlayerDB_AddItem(Client->Uin, 123238, 1, -1, false, 1);//[皮肤]二级形态
	//PlayerDB_AddItem(Client->Uin, 122199, 1, -1, false, 1);//[皮肤]众神之神-哪吒
	//PlayerDB_AddItem(Client->Uin, 122200, 1, -1, false, 1);//[皮肤]镇海之戟-龙太子
	//PlayerDB_AddItem(Client->Uin, 119062, 1, -1, false, 1);//[皮肤]针尖王者-马超
	//PlayerDB_AddItem(Client->Uin, 117941, 1, -1, false, 1);//[皮肤]众神之神-星云
	//PlayerDB_AddItem(Client->Uin, 119063, 1, -1, false, 1);//[皮肤]至曜·耀虎
	//PlayerDB_AddItem(Client->Uin, 117934, 1, -1, false, 1);//[皮肤]迈凯伦-赵云
	//PlayerDB_AddItem(Client->Uin, 116735, 1, -1, false, 1);//[皮肤]爆天甲-黄忠
	//PlayerDB_AddItem(Client->Uin, 116740, 1, -1, false, 1);//[皮肤]黑夜传说-曜影
	//PlayerDB_AddItem(Client->Uin, 116734, 1, -1, false, 1);//[S]爆天电魔
	//PlayerDB_AddItem(Client->Uin, 115159, 1, -1, false, 1);//[皮肤]黑域电魔·张飞
	//PlayerDB_AddItem(Client->Uin, 115166, 1, -1, false, 1);//[皮肤]爆天雪
	//PlayerDB_AddItem(Client->Uin, 109299, 1, -1, false, 1);//[S]代达罗斯
	//PlayerDB_AddItem(Client->Uin, 106676, 1, -1, false, 1);//[S]至尊.盘龙
	//PlayerDB_AddItem(Client->Uin, 124683, 1, -1, false, 1);//[A]15周年纪念大Q吧
	//PlayerDB_AddItem(Client->Uin, 123507, 1, -1, false, 1);//[A]蓐收雷诺
	//PlayerDB_AddItem(Client->Uin, 122204, 1, -1, false, 1);//[A]星际雷诺
	//PlayerDB_AddItem(Client->Uin, 122201, 1, -1, false, 1);//[A]超星战神
	//PlayerDB_AddItem(Client->Uin, 119074, 1, -1, false, 1);//[A]虎王雷诺
	//PlayerDB_AddItem(Client->Uin, 117938, 1, -1, false, 1);//[A]阿斯达拉
	//PlayerDB_AddItem(Client->Uin, 117939, 1, -1, false, 1);//[A]凰
	//PlayerDB_AddItem(Client->Uin, 115162, 1, -1, false, 1);//[A]海派甜心
	//PlayerDB_AddItem(Client->Uin, 113416, 1, -1, false, 1);//[A]hellokt雷诺
	//PlayerDB_AddItem(Client->Uin, 109564, 1, -1, false, 1);//[A]魔王雷诺
	//PlayerDB_AddItem(Client->Uin, 106939, 1, -1, false, 1);//[A]先行者雷诺
	////PlayerDB_AddItem(Client->Uin, 25868, 1, -1, false, 1);//[A]雷诺-机关炮
	////PlayerDB_AddItem(Client->Uin, 25869, 1, -1, false, 1);//[A]雷诺-狼蛛
	////PlayerDB_AddItem(Client->Uin, 25870, 1, -1, false, 1);//[A]雷诺-瓦利
	//PlayerDB_AddItem(Client->Uin, 26149, 1, -1, false, 1);//[A]SSC专属天启
	//PlayerDB_AddItem(Client->Uin, 27234, 1, -1, false, 1);//[A]比亚迪E6
	//PlayerDB_AddItem(Client->Uin, 71458, 1, -1, false, 1);//[A]大圣雷诺
	//PlayerDB_AddItem(Client->Uin, 127621, 1, -1, false, 1);//[B]五菱宏光
	//PlayerDB_AddItem(Client->Uin, 28788, 1, -1, false, 1);//流金爵士
	////PlayerDB_AddItem(Client->Uin, 93349, 1, -1, false, 1);//双人车蓝
	////PlayerDB_AddItem(Client->Uin, 10133, 1, -1, false, 1);//树苗运输车

	//PlayerDB_AddItem(Client->Uin, 123003, 1, -1, false, 0);//稀世·梁祝化蝶套装
	//PlayerDB_AddItem(Client->Uin, 123013, 1, -1, false, 0);//稀世·梁祝化蝶套装nv
	//PlayerDB_AddItem(Client->Uin, 104099, 1, -1, false, 0);//稀世·仙履奇缘套装
	//PlayerDB_AddItem(Client->Uin, 104109, 1, -1, false, 0);//稀世·仙履奇缘套装女
	//PlayerDB_AddItem(Client->Uin, 100474, 1, -1, false, 0);//稀世·水龙吟套装
	//PlayerDB_AddItem(Client->Uin, 117727, 1, -1, false, 0);//稀世·创世死神套装
	//PlayerDB_AddItem(Client->Uin, 117737, 1, -1, false, 0);//稀世·创世之神套装nv
	//PlayerDB_AddItem(Client->Uin, 119299, 1, -1, false, 0);//稀世·墨龙套装
	//PlayerDB_AddItem(Client->Uin, 119309, 1, -1, false, 0);//稀世·墨龙套装 nv
	//PlayerDB_AddItem(Client->Uin, 121000, 1, -1, false, 0);//山海春神套装
	//PlayerDB_AddItem(Client->Uin, 121010, 1, -1, false, 0);//山海春神套装 nv
	//PlayerDB_AddItem(Client->Uin, 86567, 1, -1, false, 0);//玉影疾风套装 nv
	//PlayerDB_AddItem(Client->Uin, 86557, 1, -1, false, 0);//玉影疾风套装
	//PlayerDB_AddItem(Client->Uin, 86537, 1, -1, false, 0);//决胜皇都套装
	//PlayerDB_AddItem(Client->Uin, 86547, 1, -1, false, 0);//凤飞于天套装
	//PlayerDB_AddItem(Client->Uin, 86507, 1, -1, false, 0);//霓裳素帛套装 nv
	//PlayerDB_AddItem(Client->Uin, 86497, 1, -1, false, 0);//霓裳素帛套装
	//PlayerDB_AddItem(Client->Uin, 85475, 1, -1, false, 0);//江山美人套装
	//PlayerDB_AddItem(Client->Uin, 85465, 1, -1, false, 0);//千古一帝套装
	//PlayerDB_AddItem(Client->Uin, 85189, 1, -1, false, 0);//曼妙首尔套装 nv
	//PlayerDB_AddItem(Client->Uin, 85179, 1, -1, false, 0);//曼妙首尔套装
	//PlayerDB_AddItem(Client->Uin, 84543, 1, -1, false, 0);//那时云月套装 nv
	//PlayerDB_AddItem(Client->Uin, 84533, 1, -1, false, 0);//那时云月套装
	//PlayerDB_AddItem(Client->Uin, 76260, 1, -1, false, 0);//芦苇微微套装
	//PlayerDB_AddItem(Client->Uin, 76250, 1, -1, false, 0);//一笑奈何套装
	//PlayerDB_AddItem(Client->Uin, 71357, 1, -1, false, 0);//芈月套装
	//PlayerDB_AddItem(Client->Uin, 71347, 1, -1, false, 0);//义渠君套装
	//PlayerDB_AddItem(Client->Uin, 17932, 1, -1, false, 0);//双周年精品套装（女）
	//PlayerDB_AddItem(Client->Uin, 23240, 1, -1, false, 0);//小橘子套装（女）
	//PlayerDB_AddItem(Client->Uin, 23940, 1, -1, false, 0);//白勇太套装（男）

	//PlayerDB_AddItem(Client->Uin, 23950, 1, -1, false, 0);//WCG2011季军皇冠（男）
	//PlayerDB_AddItem(Client->Uin, 23952, 1, -1, false, 0);//WCG2011冠军皇冠（男）
	//PlayerDB_AddItem(Client->Uin, 24858, 1, -1, false, 0);//小橘子纪念版雷诺装（女）
	//PlayerDB_AddItem(Client->Uin, 25155, 1, -1, false, 0);//亚洲总决赛冠军套装（男）
	//PlayerDB_AddItem(Client->Uin, 25158, 1, -1, false, 0);//亚洲赛冠军车队套装（男）
	//PlayerDB_AddItem(Client->Uin, 26879, 1, -1, false, 0);//WCG中国区冠军皇冠（男）
	//PlayerDB_AddItem(Client->Uin, 26882, 1, -1, false, 0);//WCG中国区冠军皇冠（女）
	//PlayerDB_AddItem(Client->Uin, 27433, 1, -1, false, 0);//SSC2012个人冠军皇冠（男）
	//PlayerDB_AddItem(Client->Uin, 27436, 1, -1, false, 0);//SSC2012个人冠军皇冠（女）
	//PlayerDB_AddItem(Client->Uin, 27747, 1, -1, false, 0);//WCG2012冠军皇冠（男）
	//PlayerDB_AddItem(Client->Uin, 33104, 1, -1, false, 0);//都敏俊套装（男）
	//PlayerDB_AddItem(Client->Uin, 32852, 1, -1, false, 0);//都教授发型（男）
	//PlayerDB_AddItem(Client->Uin, 33103, 1, -1, false, 0);//都敏俊服饰（男）
	//PlayerDB_AddItem(Client->Uin, 33109, 1, -1, false, 0);//千颂伊套装（女）
	//PlayerDB_AddItem(Client->Uin, 33108, 1, -1, false, 0);//千颂伊服饰（女）
	//PlayerDB_AddItem(Client->Uin, 70230, 1, -1, false, 0);//圣诞公公套装（男）
	//PlayerDB_AddItem(Client->Uin, 70243, 1, -1, false, 0);//圣诞麋鹿套装（女）
	//PlayerDB_AddItem(Client->Uin, 73815, 1, -1, false, 0);//蓝色都敏俊套装（男）
	//PlayerDB_AddItem(Client->Uin, 73825, 1, -1, false, 0);//红玫颂伊套装（女）
	//PlayerDB_AddItem(Client->Uin, 75041, 1, -1, false, 0);//民国女神发饰（女）
	//PlayerDB_AddItem(Client->Uin, 75046, 1, -1, false, 0);//民国女神服饰（女）
	//PlayerDB_AddItem(Client->Uin, 80359, 1, -1, false, 0);//VANS潮流发饰（男）
	//PlayerDB_AddItem(Client->Uin, 80360, 1, -1, false, 0);//VANS潮流下装（男）
	//PlayerDB_AddItem(Client->Uin, 80361, 1, -1, false, 0);//VANS潮流上装（男）
	//PlayerDB_AddItem(Client->Uin, 80362, 1, -1, false, 0);//VANS潮流发饰（女）
	//PlayerDB_AddItem(Client->Uin, 80363, 1, -1, false, 0);//VANS潮流下装（女）
	//PlayerDB_AddItem(Client->Uin, 80367, 1, -1, false, 0);//VANS嘻哈上装（男）
	//PlayerDB_AddItem(Client->Uin, 80366, 1, -1, false, 0);//VANS嘻哈下装（男）
	//PlayerDB_AddItem(Client->Uin, 80365, 1, -1, false, 0);//VANS嘻哈发饰（男）
	//PlayerDB_AddItem(Client->Uin, 80368, 1, -1, false, 0);//VANS嘻哈发饰（女）
	//PlayerDB_AddItem(Client->Uin, 80369, 1, -1, false, 0);//VANS嘻哈下装（女）
	//PlayerDB_AddItem(Client->Uin, 80370, 1, -1, false, 0);//VANS嘻哈上装（女）
	//PlayerDB_AddItem(Client->Uin, 82203, 1, -1, false, 0);//抿嘴自信表情（男）
	//PlayerDB_AddItem(Client->Uin, 82204, 1, -1, false, 0);//月牙一笑表情（女）
	//PlayerDB_AddItem(Client->Uin, 85502, 1, -1, false, 0);//超凡英雄套装
	//PlayerDB_AddItem(Client->Uin, 92550, 1, -1, false, 0);//全民城市冠军背景
	//PlayerDB_AddItem(Client->Uin, 91074, 1, -1, false, 0);//全民秋季西部冠军背景
	//PlayerDB_AddItem(Client->Uin, 91071, 1, -1, false, 0);//全民秋季东部冠军背景
	//PlayerDB_AddItem(Client->Uin, 23957, 1, -1, false, 0);//版主团专属背景
	//PlayerDB_AddItem(Client->Uin, 31159, 1, -1, false, 0);//SSC2013个人冠军
	//PlayerDB_AddItem(Client->Uin, 26072, 1, -1, false, 0);//SSC2012全国总决赛冠军背景
	//PlayerDB_AddItem(Client->Uin, 26094, 1, -1, false, 0);//核心团专属背景
	//PlayerDB_AddItem(Client->Uin, 30799, 1, -1, false, 0);//WCG2013世界赛冠军背景
	//PlayerDB_AddItem(Client->Uin, 21089, 1, -1, false, 0);//名人堂选手挂饰
	//PlayerDB_AddItem(Client->Uin, 85565, 1, -1, false, 0);//SSC秋季赛个人冠军
	//PlayerDB_AddItem(Client->Uin, 73510, 1, -1, false, 0);//全民争霸全国冠军背景
	//PlayerDB_AddItem(Client->Uin, 23217, 1, -1, false, 0);//SSC前方记者专属背景
	//PlayerDB_AddItem(Client->Uin, 16893, 1, -1, false, 0);//QQ飞车最全能玩家背景
	//PlayerDB_AddItem(Client->Uin, 16892, 1, -1, false, 0);//QQ飞车最杰出玩家背景
	//PlayerDB_AddItem(Client->Uin, 16888, 1, -1, false, 0);//QQ飞车最全能玩家挂饰
	//PlayerDB_AddItem(Client->Uin, 16887, 1, -1, false, 0);//QQ飞车最杰出玩家挂饰

	//PlayerDB_AddItem(Client->Uin, 127622, 1, -1, false, 1);//宅急送电瓶车
	//PlayerDB_AddItem(Client->Uin, 127619, 1, -1, false, 1);//敦煌云影
	////PlayerDB_AddItem(Client->Uin, 127437, 1, -1, false, 1);//源极2级形态
	////PlayerDB_AddItem(Client->Uin, 126355, 1, -1, false, 1);//源极-大河守卫
	//PlayerDB_AddItem(Client->Uin, 127409, 1, -1, false, 1);//神圣天使兽
	//PlayerDB_AddItem(Client->Uin, 124686, 1, -1, false, 1);//樱寒冰魄
	////PlayerDB_AddItem(Client->Uin, 123513, 1, -1, false, 1);//悟空-齐天大圣
	////PlayerDB_AddItem(Client->Uin, 123238, 1, -1, false, 1);//悟空·二级形态
	//PlayerDB_AddItem(Client->Uin, 122211, 1, -1, false, 1);//问天-异星探索者
	//PlayerDB_AddItem(Client->Uin, 120655, 1, -1, false, 1);//黄金神兽装甲
	//PlayerDB_AddItem(Client->Uin, 119067, 1, -1, false, 1);//特洛伊之眼
	//PlayerDB_AddItem(Client->Uin, 119060, 1, -1, false, 1);//白金虎王
	//PlayerDB_AddItem(Client->Uin, 119061, 1, -1, false, 1);//幻音女王
	PlayerDB_AddItem(Client->Uin, 120246, 1, -1, false, 1);//爆天-曜影
	PlayerDB_AddItem(Client->Uin, 118729, 1, -1, false, 1);//爆天·曜影-高阶
	//PlayerDB_AddItem(Client->Uin, 118638, 1, -1, false, 1);//双生战姬
	//PlayerDB_AddItem(Client->Uin, 117936, 1, -1, false, 1);//甜心狸想
	//PlayerDB_AddItem(Client->Uin, 117935, 1, -1, false, 1);//战争之神
	//PlayerDB_AddItem(Client->Uin, 117933, 1, -1, false, 1);//战神天马
	//PlayerDB_AddItem(Client->Uin, 116799, 1, -1, false, 1);//童梦穿梭者
	//PlayerDB_AddItem(Client->Uin, 116732, 1, -1, false, 1);//暗黑牛魔王
	//PlayerDB_AddItem(Client->Uin, 115167, 1, -1, false, 1);//月光女神X
	//PlayerDB_AddItem(Client->Uin, 115165, 1, -1, false, 1);//千变罗刹
	//PlayerDB_AddItem(Client->Uin, 115163, 1, -1, false, 1);//鹊桥仙
	//PlayerDB_AddItem(Client->Uin, 115157, 1, -1, false, 1);//暗黑孙悟空
	//PlayerDB_AddItem(Client->Uin, 115156, 1, -1, false, 1);//黄金射手座
	//PlayerDB_AddItem(Client->Uin, 113413, 1, -1, false, 1);//S青龙偃月刀-关羽
	//PlayerDB_AddItem(Client->Uin, 113414, 1, -1, false, 1);//射手座-艾俄罗斯
	//PlayerDB_AddItem(Client->Uin, 113412, 1, -1, false, 1);//沙悟净
	//PlayerDB_AddItem(Client->Uin, 112176, 1, -1, false, 1);//冥王·哈迪斯
	//PlayerDB_AddItem(Client->Uin, 109350, 1, -1, false, 1);//迈凯伦600LT
	//PlayerDB_AddItem(Client->Uin, 109305, 1, -1, false, 1);//劳斯莱斯曜影
	//PlayerDB_AddItem(Client->Uin, 109303, 1, -1, false, 1);//劳斯莱斯QEX
	//PlayerDB_AddItem(Client->Uin, 107373, 1, -1, false, 1);//逐星者
	//PlayerDB_AddItem(Client->Uin, 106673, 1, -1, false, 1);//智慧之神
	//PlayerDB_AddItem(Client->Uin, 102951, 1, -1, false, 1);//双子守护者
	//PlayerDB_AddItem(Client->Uin, 102220, 1, -1, false, 1);//先锋雷诺
	//PlayerDB_AddItem(Client->Uin, 101884, 1, -1, false, 1);//强袭装甲
	//PlayerDB_AddItem(Client->Uin, 98261, 1, -1, false, 1);//武圣·关云长
	//PlayerDB_AddItem(Client->Uin, 97431, 1, -1, false, 1);//雷电神王
	//PlayerDB_AddItem(Client->Uin, 89936, 1, -1, false, 1);//终极烈焰新星
	//PlayerDB_AddItem(Client->Uin, 89940, 1, -1, false, 1);//星际传说
	//PlayerDB_AddItem(Client->Uin, 89750, 1, -1, false, 1);//银天使虎牙定制款
	//PlayerDB_AddItem(Client->Uin, 87727, 1, -1, false, 1);//雷怒帝王
	//PlayerDB_AddItem(Client->Uin, 87726, 1, -1, false, 1);//暗狱帝王
	//PlayerDB_AddItem(Client->Uin, 87725, 1, -1, false, 1);//苍穹帝王
	//PlayerDB_AddItem(Client->Uin, 85950, 1, -1, false, 1);//啸天雷诺
	//PlayerDB_AddItem(Client->Uin, 82248, 1, -1, false, 1);//雷霆霸主
	//PlayerDB_AddItem(Client->Uin, 78297, 1, -1, false, 1);//凤凰雷诺
	//PlayerDB_AddItem(Client->Uin, 77236, 1, -1, false, 1);//终极猛兽
	//PlayerDB_AddItem(Client->Uin, 74363, 1, -1, false, 1);//终极鬼战刀
	//PlayerDB_AddItem(Client->Uin, 68647, 1, -1, false, 1);//飞虎雷诺
	//PlayerDB_AddItem(Client->Uin, 68192, 1, -1, false, 1);//火焰奇兵
	//PlayerDB_AddItem(Client->Uin, 26930, 1, -1, false, 1);//钢铁雷诺
	//PlayerDB_AddItem(Client->Uin, 31914, 1, -1, false, 1);//黄金雷诺
	//PlayerDB_AddItem(Client->Uin, 64296, 1, -1, false, 1);//羊年专属雷诺
	//PlayerDB_AddItem(Client->Uin, 83718, 1, -1, false, 1);//纪念版黄金雷诺
	//PlayerDB_AddItem(Client->Uin, 93574, 1, -1, false, 1);//电玩雷诺
	//PlayerDB_AddItem(Client->Uin, 94846, 1, -1, false, 1);//天蓬雷诺

	//PlayerDB_AddItem(Client->Uin, 128797, 1, -1, false, 0);//瑰色午夜套装nv
	//PlayerDB_AddItem(Client->Uin, 128787, 1, -1, false, 0);//瑰色午夜套装
	//PlayerDB_AddItem(Client->Uin, 128491, 1, -1, false, 0);//烈麟青穹套装nv
	//PlayerDB_AddItem(Client->Uin, 128481, 1, -1, false, 0);//烈麟青穹套装
	//PlayerDB_AddItem(Client->Uin, 128431, 1, -1, false, 0);//梅染琼枝套装nv
	//PlayerDB_AddItem(Client->Uin, 128421, 1, -1, false, 0);//梅染琼枝套装
	//PlayerDB_AddItem(Client->Uin, 128341, 1, -1, false, 0);//花街灯昼套装
	//PlayerDB_AddItem(Client->Uin, 124561, 1, -1, false, 0);//白骨晶晶套装
	//PlayerDB_AddItem(Client->Uin, 124551, 1, -1, false, 0);//齐天大圣套装
	//PlayerDB_AddItem(Client->Uin, 110668, 1, -1, false, 0);//憨憨兔套装nv
	//PlayerDB_AddItem(Client->Uin, 110661, 1, -1, false, 0);//憨憨兔套装
	//PlayerDB_AddItem(Client->Uin, 109390, 1, -1, false, 0);//稀世·至尊套装nv
	//PlayerDB_AddItem(Client->Uin, 109380, 1, -1, false, 0);//稀世·至尊套装
	//PlayerDB_AddItem(Client->Uin, 13453, 1, -1, false, 0);//特种部队套装
	//PlayerDB_AddItem(Client->Uin, 17759, 1, -1, false, 0);//索隆冷酷魄力套装
	//PlayerDB_AddItem(Client->Uin, 17788, 1, -1, false, 0);//性感娜美套装
	//PlayerDB_AddItem(Client->Uin, 18658, 1, -1, false, 0);//初音活力套装
	//PlayerDB_AddItem(Client->Uin, 31742, 1, -1, false, 0);//艾伦cosplay制服
	//PlayerDB_AddItem(Client->Uin, 31748, 1, -1, false, 0);//三笠cosplay制服
	//PlayerDB_AddItem(Client->Uin, 66229, 1, -1, false, 0);//超人纪念珍藏套装
	//PlayerDB_AddItem(Client->Uin, 66228, 1, -1, false, 0);//超人纪念珍藏服饰
	//PlayerDB_AddItem(Client->Uin, 66238, 1, -1, false, 0);//超人纪念珍藏套装nv
	//PlayerDB_AddItem(Client->Uin, 66252, 1, -1, false, 0);//蝙蝠侠纪念珍藏套装
	//PlayerDB_AddItem(Client->Uin, 66265, 1, -1, false, 0);//蝙蝠侠纪念珍藏套装nv
	//PlayerDB_AddItem(Client->Uin, 69672, 1, -1, false, 0);//007专属套装
	//PlayerDB_AddItem(Client->Uin, 69671, 1, -1, false, 0);//007专属服饰
	//PlayerDB_AddItem(Client->Uin, 69681, 1, -1, false, 0);//007专属服饰nv
	//PlayerDB_AddItem(Client->Uin, 69682, 1, -1, false, 0);//007专属套装nv
	//PlayerDB_AddItem(Client->Uin, 127511, 1, -1, false, 0);//公开赛S2冠军背景
	//PlayerDB_AddItem(Client->Uin, 124670, 1, -1, false, 0);//超核玩家背景
	//PlayerDB_AddItem(Client->Uin, 120183, 1, -1, false, 0);//官方影讯团背景
	//PlayerDB_AddItem(Client->Uin, 118372, 1, -1, false, 0);//官方解说背景
	//PlayerDB_AddItem(Client->Uin, 118371, 1, -1, false, 0);//短视频小组背景
	//PlayerDB_AddItem(Client->Uin, 113652, 1, -1, false, 0);//耀眼明星
	//PlayerDB_AddItem(Client->Uin, 109447, 1, -1, false, 0);//官方巡查团背景
	//PlayerDB_AddItem(Client->Uin, 109400, 1, -1, false, 0);//迈凯伦600LT专属背景
	//PlayerDB_AddItem(Client->Uin, 109308, 1, -1, false, 0);//道具核心团背景
	//PlayerDB_AddItem(Client->Uin, 109307, 1, -1, false, 0);//原创作者专属背景
	//PlayerDB_AddItem(Client->Uin, 107084, 1, -1, false, 0);//官方赛事小组背景
	//PlayerDB_AddItem(Client->Uin, 106987, 1, -1, false, 0);//官方新闻小分队背景
	//PlayerDB_AddItem(Client->Uin, 85580, 1, -1, false, 0);//秀场2017年度美女主播
	//PlayerDB_AddItem(Client->Uin, 63420, 1, -1, false, 0);//周杰伦粉丝专属背景
	//PlayerDB_AddItem(Client->Uin, 60474, 1, -1, false, 0);//试车房专属


	//PlayerDB_AddItem(Client->Uin, 127760, 1, -1, false, 0);//源极之星X
	//PlayerDB_AddItem(Client->Uin, 119077, 1, -1, false, 0);//SPD女团
	//PlayerDB_AddItem(Client->Uin, 127614, 1, -1, false, 0);//紫莹幻境表情
	//PlayerDB_AddItem(Client->Uin, 127615, 1, -1, false, 0);//紫莹幻境表情女
	//PlayerDB_AddItem(Client->Uin, 127613, 1, -1, false, 0);//血族传说表情女
	//PlayerDB_AddItem(Client->Uin, 127612, 1, -1, false, 0);//血族传说表情
	//PlayerDB_AddItem(Client->Uin, 126241, 1, -1, false, 0);//素雪年华表情
	//PlayerDB_AddItem(Client->Uin, 126242, 1, -1, false, 0);//素雪年华表情nv
	//PlayerDB_AddItem(Client->Uin, 124847, 1, -1, false, 0);//绚丽夺目表情
	//PlayerDB_AddItem(Client->Uin, 124848, 1, -1, false, 0);//绚丽夺目表情nv
	//PlayerDB_AddItem(Client->Uin, 124564, 1, -1, false, 0);//热度小橘子NPC表情
	//PlayerDB_AddItem(Client->Uin, 123394, 1, -1, false, 0);//孙行者表情
	//PlayerDB_AddItem(Client->Uin, 123395, 1, -1, false, 0);//嫦娥仙女表情
	//PlayerDB_AddItem(Client->Uin, 116724, 1, -1, false, 0);//老爷爷
	//PlayerDB_AddItem(Client->Uin, 116725, 1, -1, false, 0);//老奶奶
	//PlayerDB_AddItem(Client->Uin, 113904, 1, -1, false, 0);//凤凰座-辉表情
	//PlayerDB_AddItem(Client->Uin, 113905, 1, -1, false, 0);//凤凰座-辉表情nv
	//PlayerDB_AddItem(Client->Uin, 113902, 1, -1, false, 0);//天马座-星矢表情
	//PlayerDB_AddItem(Client->Uin, 113903, 1, -1, false, 0);//天马座-星矢表情nv
	//PlayerDB_AddItem(Client->Uin, 113898, 1, -1, false, 0);//白鸟座-冰河表情
	//PlayerDB_AddItem(Client->Uin, 113899, 1, -1, false, 0);//白鸟座-冰河表情nv
	//PlayerDB_AddItem(Client->Uin, 111200, 1, -1, false, 0);//圣斗士星矢表情
	//PlayerDB_AddItem(Client->Uin, 111201, 1, -1, false, 0);//圣斗士星矢表情nv
	//PlayerDB_AddItem(Client->Uin, 113827, 1, -1, false, 0);//星球恋人座椅
	//PlayerDB_AddItem(Client->Uin, 113826, 1, -1, false, 0);//时空之恋座椅
	//PlayerDB_AddItem(Client->Uin, 113828, 1, -1, false, 0);//敦煌一梦座椅
	//PlayerDB_AddItem(Client->Uin, 116844, 1, -1, false, 0);//稀世·创世之神座椅
	//PlayerDB_AddItem(Client->Uin, 122127, 1, -1, false, 0);//稀世·梁祝化蝶座椅
	//PlayerDB_AddItem(Client->Uin, 127753, 1, -1, false, 0);//月桂仙宫座椅





	//PlayerDB_AddItem(Client->Uin, 126533, 1, -1, false, 1);//雷诺皮肤
	//PlayerDB_AddItem(Client->Uin, 126534, 1, -1, false, 1);//雷诺皮肤
	//PlayerDB_AddItem(Client->Uin, 126535, 1, -1, false, 1);//雷诺皮肤
	//PlayerDB_AddItem(Client->Uin, 126536, 1, -1, false, 1);//雷诺皮肤
	//PlayerDB_AddItem(Client->Uin, 129293, 1, -1, false);//貂蝉
	//PlayerDB_AddItem(Client->Uin, 129290, 1, -1, false);//女团
	//PlayerDB_AddItem(Client->Uin, 129289, 1, -1, false);//赤兔
	//PlayerDB_AddItem(Client->Uin, 129285, 1, -1, false);//龙王机甲

	PlayerDB_AddItem(Client->Uin, 10699, 1, -1, false, 1);//[B]高压
	PlayerDB_AddItem(Client->Uin, 11147, 1, -1, false, 1);//[A]针尖
	PlayerDB_AddItem(Client->Uin, 11376, 1, -1, false, 1);//[S]暴风雪
	PlayerDB_AddItem(Client->Uin, 24859, 1, -1, false, 1);//[A]纪念版白银雷诺


	PlayerDB_AddItem(Client->Uin, 21982, 1, -1, false, 0); //效率宝珠LV6
	PlayerDB_AddItem(Client->Uin, 21980, 1, -1, false, 0);//效率宝珠LV4
	PlayerDB_AddItem(Client->Uin, 21988, 1, -1, false, 0); //重生宝珠LV6
	PlayerDB_AddItem(Client->Uin, 21986, 1, -1, false, 0); //重生宝珠LV4
	PlayerDB_AddItem(Client->Uin, 21994, 1, -1, false, 0); //坚韧宝珠LV6
	//PlayerDB_AddItem(Client->Uin, 22000, 1, -1, false, 0); //苍天宝珠LV6
	//PlayerDB_AddItem(Client->Uin, 22006, 1, -1, false, 0); //大地宝珠LV6
	//PlayerDB_AddItem(Client->Uin, 22012, 1, -1, false, 0); //无双宝珠LV6
	//PlayerDB_AddItem(Client->Uin, 22018, 1, -1, false, 0); //后发宝珠LV6
	//PlayerDB_AddItem(Client->Uin, 22024, 1, -1, false, 0); //愤怒宝珠LV6

	PlayerDB_AddItem(Client->Uin, 27570, 200, 0, false, 0);//宝箱
	PlayerDB_AddItem(Client->Uin, 69010, 200, 0, false, 0);//宝箱
	//PlayerDB_AddItem(Client->Uin, 22107, 50, 0, false, 0);//飞车红包
	PlayerDB_AddItem(Client->Uin, 22642, 50, 0, false, 0);//裁判团礼包（坐骑）
	PlayerDB_AddItem(Client->Uin, 28286, 50, 0, false, 0);//宠物大礼包（宠物）
	//PlayerDB_AddItem(Client->Uin, 28283, 50, 0, false, 0);//iphone 5（表情）
	//PlayerDB_AddItem(Client->Uin, 88121, 1, 0, false, 0);//永久升级帝王宝箱
	//PlayerDB_AddItem(Client->Uin, 88122, 1, 0, false, 0);//永久天启宝箱
	//PlayerDB_AddItem(Client->Uin, 88123, 1, 0, false, 0);//永久黄金宝箱
	//PlayerDB_AddItem(Client->Uin, 88124, 1, 0, false, 0);//永久帝王宝箱
	//PlayerDB_AddItem(Client->Uin, 88125, 1, 0, false, 0);//永久银天使宝箱
	//PlayerDB_AddItem(Client->Uin, 88126, 1, 0, false, 0);//永久飞跃宝箱
	//PlayerDB_AddItem(Client->Uin, 96574, 1, 0, false, 0);//传说宝箱
	PlayerDB_AddItem(Client->Uin, 12376, 9999, 0, false, 0); //点火装置+1
	PlayerDB_AddItem(Client->Uin, 12377, 9999, 0, false, 0); //进气系统+1
	PlayerDB_AddItem(Client->Uin, 12378, 9999, 0, false, 0); //燃料系统+1
	PlayerDB_AddItem(Client->Uin, 12379, 9999, 0, false, 0); //悬挂系统+1
	PlayerDB_AddItem(Client->Uin, 12380, 9999, 0, false, 0); //引擎装置+1
	PlayerDB_AddItem(Client->Uin, 74886, 1, 0, false, 0); //雷诺-空军-龙头
	PlayerDB_AddItem(Client->Uin, 74887, 1, 0, false, 0); //雷诺-空军-尾翼
	PlayerDB_AddItem(Client->Uin, 74888, 1, 0, false, 0); //雷诺-空军-侧翼
	PlayerDB_AddItem(Client->Uin, 74889, 1, 0, false, 0); //雷诺-空军-车胎
	//道具
	PlayerDB_AddItem(Client->Uin, 10207, 1, 0, false, 0); //小喇叭
	PlayerDB_AddItem(Client->Uin, 10791, 1, -1, false, 0); //临时驾照
	PlayerDB_AddItem(Client->Uin, 10595, 3, 0, false, 0); //更名卡
	PlayerDB_AddItem(Client->Uin, 22081, 30, 0, false, 0); //宝石雕刻刀
	PlayerDB_AddItem(Client->Uin, 22082, 30, 0, false, 0); //宝石拆除刀

	sql = "COMMIT"; //提交事务
	result = sqlite3_exec(PlayerDB, sql, NULL, NULL, NULL);


	//ItemInfo Items[200];
	//ItemInfo* Items= new ItemInfo[200];


	ItemInfo Items[3000];
	size_t ItemNum = 0;
	bool HasMoreInfo = false;
	sql = "SELECT ItemID,ItemNum,AvailPeriod,Status,ObtainTime,OtherAttribute  FROM Item WHERE Uin = ?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Client->Uin);
	}
	else
	{
		printf("%s\n", sqlite3_errmsg(PlayerDB));
		sqlite3_finalize(stmt);
		stmt = NULL;
	}
	if (stmt)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			Items[ItemNum].ItemID = sqlite3_column_int(stmt, 0);
			Items[ItemNum].ItemNum = sqlite3_column_int(stmt, 1);
			Items[ItemNum].AvailPeriod = sqlite3_column_int(stmt, 2);
			Items[ItemNum].Status = sqlite3_column_int(stmt, 3);
			Items[ItemNum].ObtainTime = sqlite3_column_int(stmt, 4);
			Items[ItemNum].OtherAttribute = sqlite3_column_int(stmt, 5);
			Items[ItemNum].ItemType = 0;
			if (Items[ItemNum].Status && GetItemType(Items[ItemNum].ItemID) == EAIT_CAR)
			{
				Client->KartID = Items[ItemNum].ItemID;
				printf("KartID:%d\n", Client->KartID);
			}
			else if (Items[ItemNum].Status && GetItemType(Items[ItemNum].ItemID) == EAIT_PET)
			{
				Client->Pet->ID = Items[ItemNum].ItemID;
				GetPetInfo(Client);
			}
			ItemNum++;
			if (ItemNum >= 200)
			{
				HasMoreInfo = true;
				break;
			}
		}
	}
	ResponseRegister(Client, Nickname, Gender, Country, ItemNum, Items);
	if (stmt)
	{
		while (HasMoreInfo)
		{
			ItemNum = 0;
			HasMoreInfo = false;
			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				Items[ItemNum].ItemID = sqlite3_column_int(stmt, 0);
				Items[ItemNum].ItemNum = sqlite3_column_int(stmt, 1);
				Items[ItemNum].AvailPeriod = sqlite3_column_int(stmt, 2);
				Items[ItemNum].Status = sqlite3_column_int(stmt, 3);
				Items[ItemNum].ObtainTime = sqlite3_column_int(stmt, 4);
				Items[ItemNum].OtherAttribute = sqlite3_column_int(stmt, 5);
				Items[ItemNum].ItemType = 0;
				if (Items[ItemNum].Status && GetItemType(Items[ItemNum].ItemID) == EAIT_CAR)
				{
					Client->KartID = Items[ItemNum].ItemID;
					printf("KartID:%d\n", Client->KartID);
				}
				else if (Items[ItemNum].Status && GetItemType(Items[ItemNum].ItemID) == EAIT_PET)
				{
					Client->Pet->ID = Items[ItemNum].ItemID;
					GetPetInfo(Client);
				}

				ItemNum++;
				if (ItemNum >= 200)
				{
					HasMoreInfo = true;
					break;
				}
			}
			NotifyClientAddItem(Client, ItemNum, Items);
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
	}

	ResponseSkipFreshTask(Client); //跳过新手任务
}
void ResponseRegister(ClientNode* Client, const char* Nickname, BYTE  Gender, BYTE Country, UINT ItemNum, ItemInfo* Items)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, 0); //ResultID
	Write32(p, Client->Uin);

	strcpy_s((char*)p, MaxNickName, Nickname);
	p += MaxNickName;

	Write8(p, Gender);
	Write8(p, Country);

	Write16(p, ItemNum); //AddItemNum
	{
		for (size_t i = 0; i < ItemNum; i++)
		{
			BYTE* pItemInfo = p;
			Write16(pItemInfo, 0); //len

			Write32(pItemInfo, Items[i].ItemID);
			Write32(pItemInfo, Items[i].ItemNum);
			Write32(pItemInfo, Items[i].AvailPeriod);
			Write8(pItemInfo, Items[i].Status);
			Write32(pItemInfo, Items[i].ObtainTime);
			Write32(pItemInfo, Items[i].OtherAttribute);
#ifndef ZingSpeed
			Write16(pItemInfo, Items[i].ItemType); //ItemType
#endif

			len = pItemInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}

	Write16(p, 0); //AddTaskNum

	Write8(p, 0); //ReasonLen


	len = p - buf;
	SendToClient(Client, 129, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestLogin(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	UINT ClientVersion = Read32(Body);

	//QQNickName[20]
	Body += MaxQQNickName;
	//Gender
	Body += 1;
	//IconID
	Body += 1;
	UINT AttachIdentify = Read32(Body);
	UINT LoginType = Read32(Body);
	UINT LoaderTipVersion = Read32(Body);
	UINT ClientStatusContex = Read32(Body);
	BYTE IsSimplifyVersion = Read8(Body);
	UINT CurTopMapRecordVersion = Read32(Body);
	UINT CurKartRoomPushInfoVersion = Read32(Body);
	/*
m_sPreSvrID
m_u8TGPSignatureLen
m_uiRandomKey
m_stClientFingerprint.m_uiPID
m_stClientFingerprint.m_uiMacAddrHigh
m_stClientFingerprint.m_uiMacAddrLow
m_stClientFingerprint.m_uiCPUID
m_stClientFingerprint.m_uiBiosID
m_uiConnID
m_bUseTGP
m_usBarPriviSignatureLen
m_iNetBarLev
m_bReconnect
m_shLoginBuffLen
m_usClientKeyLen
m_uiPlayerFlag
m_ucLoginSrcType
m_ucRequestLoginType
m_ushNetBarTokenLen
	*/

	Client->Uin = Uin;

	Client->Pet = new PetInfo{ 0,"",0,0,0,0,0 };

	AddClient(Client, Uin);
	//printf("ClientVersion:%d, LoginType:%d\n", ClientVersion, LoginType);
	
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

	ItemInfo Items[200];
	//ItemInfo* Items=new ItemInfo[20000];
	//ItemInfo* Items = (ItemInfo*)malloc(sizeof(ItemInfo) * 20000);
	size_t ItemNum = 0;
	bool HasMoreInfo = false;

	sql = "SELECT ItemID,ItemNum,AvailPeriod,Status,ObtainTime,OtherAttribute  FROM Item WHERE Uin = ?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Client->Uin);
	}
	else
	{
		printf("%s\n", sqlite3_errmsg(PlayerDB));
		sqlite3_finalize(stmt);
		stmt = NULL;
	}
	if (stmt)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			Items[ItemNum].ItemID = sqlite3_column_int(stmt, 0);
			Items[ItemNum].ItemNum = sqlite3_column_int(stmt, 1);
			Items[ItemNum].AvailPeriod = sqlite3_column_int(stmt, 2);
			Items[ItemNum].Status = sqlite3_column_int(stmt, 3);
			Items[ItemNum].ObtainTime = sqlite3_column_int(stmt, 4);
			Items[ItemNum].OtherAttribute = sqlite3_column_int(stmt, 5);
			Items[ItemNum].ItemType = 0;
			if (Items[ItemNum].Status && GetItemType(Items[ItemNum].ItemID) == EAIT_CAR)
			{
				Client->KartID = Items[ItemNum].ItemID;
				//printf("KartID:%d\n", Client->KartID);
			}
			else if (Items[ItemNum].Status && GetItemType(Items[ItemNum].ItemID) == EAIT_PET)
			{
				Client->Pet->ID = Items[ItemNum].ItemID;
				GetPetInfo(Client);
			}

			ItemNum++;
			if (ItemNum >= 200)
			{
				HasMoreInfo = true;
				break;
			}
		}
	}


	ResponseLogin(Client, ClientVersion, LoginType, ItemNum, Items, HasMoreInfo);
	if (stmt)
	{
		while (HasMoreInfo)
		{
			ItemNum = 0;
			HasMoreInfo = false;
			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				Items[ItemNum].ItemID = sqlite3_column_int(stmt, 0);
				Items[ItemNum].ItemNum = sqlite3_column_int(stmt, 1);
				Items[ItemNum].AvailPeriod = sqlite3_column_int(stmt, 2);
				Items[ItemNum].Status = sqlite3_column_int(stmt, 3);
				Items[ItemNum].ObtainTime = sqlite3_column_int(stmt, 4);
				Items[ItemNum].OtherAttribute = sqlite3_column_int(stmt, 5);
				Items[ItemNum].ItemType = 0;
				if (Items[ItemNum].Status && GetItemType(Items[ItemNum].ItemID) == EAIT_CAR)
				{
					Client->KartID = Items[ItemNum].ItemID;
					//printf("KartID:%d\n", Client->KartID);
				}
				else if (Items[ItemNum].Status && GetItemType(Items[ItemNum].ItemID) == EAIT_PET)
				{
					Client->Pet->ID = Items[ItemNum].ItemID;
					GetPetInfo(Client);
				}

				ItemNum++;
				if (ItemNum >= 200)
				{
					HasMoreInfo = true;
					break;
				}
			}
			//这对于 ZingSpeed 好像不起作用
			NotifyLoginMoreInfo(Client, ItemNum, Items, HasMoreInfo);
			// 修复道具上限问题
			NotifyClientAddItem(Client, ItemNum, Items);
			ItemStatus NewStatus;
			for (size_t i = 0; i < ItemNum; i++)
			{
				if (Items[i].Status)
				{
					NewStatus.ItemID = Items[i].ItemID;
					NewStatus.NewStatus = true;
					ResponseChangeItemStatus(Client, 1, &NewStatus);
				}
			}
//#ifdef ZingSpeed
//			//那就采取这个方法
//			NotifyClientAddItem(Client, ItemNum, Items);
//			ItemStatus NewStatus;
//			for (size_t i = 0; i < ItemNum; i++)
//			{
//				if (Items[i].Status)
//				{
//					NewStatus.ItemID = Items[i].ItemID;
//					NewStatus.NewStatus = true;
//					ResponseChangeItemStatus(Client, 1, &NewStatus);
//				}
//			}
//#endif
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
	}
	NotifyClientVipFlag(Client);

	NotifyMsgBox(Client);
	NotifyRandRoomNameList(Client);
	NotifyTopListDesc(Client);
	NotifySpeed2Cfg(Client);
	NotifySvrConfig(Client);
	NotifySvrConfig2(Client);
#ifndef ZingSpeed
	NotifySvrConfig3(Client);
	NotifyTopUIItemInfo(Client);
	NotifyRedPointInfo(Client);

	NotifySkillStoneKartInfo(Client);
#endif
}
void ResponseLogin(ClientNode* Client, UINT ClientVersion, UINT LoginType, UINT ItemNum, ItemInfo* Items, bool HasMoreInfo)
{
	BYTE* buf = new BYTE[32768];
	//BYTE buf[8192];
	BYTE* p = buf;
	size_t len;


	USHORT ResultID = 0;
	const char* Reason = "";
	int Identity = ID_IDENTIFY_QQLEVEL1 | ID_IDENTIFY_QQLEVEL2 | ID_IDENTIFY_QQLEVEL3 | ID_IDENTIFY_QQFLAG | ID_IDENTIFY_HAVEWORD | ID_IDENTIFY_FINISHQQFLAG;
	int IsInTopList = 0;
	if (!Client->IsLogin)
	{
		ResultID = 1;
		Reason = "the password is incorrect";
	}
	else
	{
		const char* sql = NULL;
		sqlite3_stmt* stmt = NULL;
		int result;

		sql = "SELECT VipFlag,IsInTopList  FROM Player  WHERE Uin=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, Client->Uin);
			result = sqlite3_step(stmt);
			if (result == SQLITE_ROW)
			{
				ResultID = 0;
				if (sqlite3_column_int(stmt, 0) != 0)
				{
					Identity |= ID_IDENTIFY_SPEEDMEMBER;
				}
				IsInTopList = sqlite3_column_int(stmt, 1);

			}
			else
			{
				ResultID = 1; //未注册
			}
		}
		else
		{
			ResultID = 1;
			Reason = sqlite3_errmsg(PlayerDB);
			printf("%s\n", Reason);
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
	}

	Write16(p, ResultID); //ResultID

	Write8(p, sizeof(Client->Key)); //KeyGameDataLength
	memcpy(p, Client->Key, sizeof(Client->Key));
	p += sizeof(Client->Key);

	Write32(p, Client->Uin);
	Write32(p, Identity);
	Write16(p, Client->ConnID); //PlayerID
	Write32(p, LoginType); //LoginType
	Write32(p, 4); //GameSvrdIdentity

	

	WritePlayerDBBaseInfo(p, Client->Uin);

	Write16(p, ItemNum); //ItemNum
	{
		for (size_t i = 0; i < ItemNum; i++)
		{
			BYTE* pItemInfo = p;
			Write16(pItemInfo, 0); //len

			Write32(pItemInfo, Items[i].ItemID);
			Write32(pItemInfo, Items[i].ItemNum);
			Write32(pItemInfo, Items[i].AvailPeriod);
			Write8(pItemInfo, Items[i].Status);
			Write32(pItemInfo, Items[i].ObtainTime);
			Write32(pItemInfo, Items[i].OtherAttribute);
#ifndef  ZingSpeed
			Write16(pItemInfo, Items[i].ItemType); //ItemType
#endif

			len = pItemInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}


	Write16(p, 0); //TaskNum
	/*
m_astPlayerDBTaskInfo[].m_iTaskID
m_astPlayerDBTaskInfo[].m_shOptionID
m_astPlayerDBTaskInfo[].m_cStatus
m_astPlayerDBTaskInfo[].m_bPrevStatus
m_astPlayerDBTaskInfo[].m_iProgressRate
m_astPlayerDBTaskInfo[].m_iOther
m_astPlayerDBTaskInfo[].m_uiTakeTime
m_astPlayerDBTaskInfo[].m_uiLastUpdateTime
	*/

	Write8(p, 0); //RecomCountry
	Write32(p, (UINT)time(nullptr)); //ServerTime

	len = strlen(Reason);
	Write8(p, (BYTE)len); //ReasonLen
	memcpy(p, Reason, len);
	p += len;

	{ //PlayerRelationInfo
		BYTE* pPlayerRelationInfo = p;
		Write16(pPlayerRelationInfo, 0); //len


		Write32(pPlayerRelationInfo, Client->Uin); //SrcUin
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

#ifdef ZingSpeed
	Write16(p, 0); //KartNum
	/*
	{ //KartRefitInfo
		BYTE* pKartRefitInfo = p;
		Write16(pKartRefitInfo, 0); //len

		Write32(pKartRefitInfo, Client->Uin); //Uin
		Write32(pKartRefitInfo, 20244); //KartId

		Write32(pKartRefitInfo, 1); //RefitCout
		Write16(pKartRefitInfo, 0b0100100000000000); //MaxFlags
		Write16(pKartRefitInfo, 29); //WWeight
		Write16(pKartRefitInfo, 29); //SpeedWeight
		Write16(pKartRefitInfo, 29); //JetWeight
		Write16(pKartRefitInfo, 29); //SJetWeight
		Write16(pKartRefitInfo, 29); //AccuWeight

		Write32(pKartRefitInfo, 0); //ShapeRefitCount
		Write32(pKartRefitInfo, 0); //KartHeadRefitItemID
		Write32(pKartRefitInfo, 0); //KartTailRefitItemID
		Write32(pKartRefitInfo, 0); //KartFlankRefitItemID
		Write32(pKartRefitInfo, 0); //KartTireRefitItemID

		len = pKartRefitInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
#endif

	BYTE* pMapNum = p;
	Write16(p, 0); //MapNum
	{
		const char* sql = NULL;
		sqlite3_stmt* stmt = NULL;
		int result;
		size_t i = 0;
		sql = "SELECT MapID,Record  FROM MapRecord WHERE Uin = ?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, Client->Uin);
			while (sqlite3_step(stmt) == SQLITE_ROW)
			{ //Record
				i++;

				BYTE* pRecord = p;
				Write16(pRecord, 0); //len

				Write32(pRecord, sqlite3_column_int(stmt, 0)); //MapID
				Write32(pRecord, sqlite3_column_int(stmt, 1)); //Record

				len = pRecord - p;
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
		Set16(pMapNum, (WORD)i);
	}

	BYTE* pPetNum = p;
	int PetNum = 0;
	Write16(p, 0); //PetNum
	
	{
		sqlite3_stmt* stmt = NULL;
		if (sqlite3_prepare_v2(PlayerDB, "SELECT PetId,Name,Experience,PL FROM Pet WHERE Uin=?;", -1, &stmt, NULL) == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, Client->Uin);

			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				PetNum++;
				BYTE* pPetInfo = p;
				Write16(pPetInfo, 0); //len

				Write32(pPetInfo, Client->Uin); //Uin
				Write32(pPetInfo, sqlite3_column_int(stmt, 0)); //PetId
				memcpy(pPetInfo, (char*)sqlite3_column_text(stmt, 1), 23);
				pPetInfo += 23;
				Write8(pPetInfo, 1); //IsProper
				UINT Level = GetPetLevel(sqlite3_column_int(stmt, 2));
				UINT Status = GetPetStatus(Level);
				Write16(pPetInfo, Level); //LevelLimit
				Write16(pPetInfo, Level); //Level
				Write32(pPetInfo, sqlite3_column_int(stmt, 2)); //CurLevelExp
				Write32(pPetInfo, GetPetMaxExp(Level)); //CurLevelMaxExp
				Write8(pPetInfo, Status); //Status
				Write16(pPetInfo, sqlite3_column_int(stmt, 3)); //PL
				Write16(pPetInfo, 100); //TotalPL
				YAML::Node PetSkills = GetPetConfig(sqlite3_column_int(stmt, 0));

				if (PetSkills.size() < Status) {
					Write16(pPetInfo, 0); //PetSkillNum
				}
				else
				{
					Write16(pPetInfo, Status); //PetSkillNum
					for (size_t i = 0; i < Status; i++)
					{ //PetSkillList
						BYTE* pPetSkillList = pPetInfo;
						Write16(pPetSkillList, 0); //len

						Write16(pPetSkillList, PetSkills[i]["SkillID"].as<UINT>()); //SkillID
						Write8(pPetSkillList, i + 1); //Status
						Write8(pPetSkillList, 1); //Active
						Write32(pPetSkillList, PetSkills[i]["Value"].as<float>() * Level); //Value

						len = pPetSkillList - pPetInfo;
						Set16(pPetInfo, (WORD)len);
						pPetInfo += len;
					}
				}

				Write32(pPetInfo, 0); //StrengLevel
				Write8(pPetInfo, 0); //TopStatus
				{ //CommonInfo
					BYTE* pCommonInfo = pPetInfo;
					Write16(pCommonInfo, 0); //len

					Write32(pCommonInfo, 0); //HuanHuaLevel
					Write8(pCommonInfo, 0); //CanHuanHua

					len = pCommonInfo - pPetInfo;
					Set16(pPetInfo, (WORD)len);
					pPetInfo += len;
				}

				len = pPetInfo - p;
				Set16(p, (WORD)len);
				p += len;
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
	}
	Set16(pPetNum, (WORD)PetNum);


	Write8(p, (bool)IsInTopList); //IsInTopList
	Write32(p, 0); //LastLoginTime
	Write32(p, 0); //MasterPoint
	Write8(p, 0); //LoginRealNameFlag

	//ExRightFlag[]
#ifndef ZingSpeed
	if (ClientVersion > 18457)
	{
		memset(p, 0, 24);
		p += 24;
	}
	else
#endif
	{
		memset(p, 0, 16);
		p += 16;
	}


	Write8(p, 0); //OverdueItemNum
	/*
m_astOverdueItem[%d].m_iItemID
m_astOverdueItem[%d].m_iOverdueTime
m_astOverdueItem[%d].m_iOtherAttr
	*/


	BYTE* pStoneKartNum = p;
	Write32(p, 0); //StoneKartNum
#ifdef ZingSpeed
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
#endif

	Write32(p, 0); //LockedMapID
	Write32(p, 0); //TotalGuildProsperity
	Write32(p, 0); //ClientStatusContex
	Write8(p, 0); //IsHoneyMoon
	Write8(p, HasMoreInfo); //HasMoreInfo
	Write16(p, 0); //CurSvrID


	//读取贵族数据库
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

	int Level1 = 6;
	int Level2 = 0;
	int Level3 = 0;
	int Level4 = 0;
	int Level5 = 0;

	sql = "SELECT VipLevel1,VipLevel2,VipLevel3,VipLevel4,VipLevel5 FROM Player WHERE Uin = ?;"; 
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Client->Uin);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			Level1 = sqlite3_column_int(stmt, 0);//皇族
			Level2 = sqlite3_column_int(stmt, 1);//情侣
			Level3 = sqlite3_column_int(stmt, 2);//花园
			Level4 = sqlite3_column_int(stmt, 3);//魅力
			Level5 = sqlite3_column_int(stmt, 4);//钻皇
			//printf("VipLevel：%d\n", Level);
		}
		else
		{
			Level1 = 6;
			Level2 = 0;
			Level3 = 0;
			Level4 = 0;
			Level5 = 0;
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	




	 //NobleInfo 皇族
		BYTE* pNobleInfo = p;
		Write16(pNobleInfo, 0); //len

		Write32(pNobleInfo, Client->Uin); //NobleID
		Write8(pNobleInfo, Level1); //NobleLevel 皇族等级
		Write32(pNobleInfo, 9500); //NoblePoint 皇族点数
		Write32(pNobleInfo, 30); //NobleLeftDays 皇族剩余天数

		len = pNobleInfo - p;
		Set16(p, (WORD)len);
		p += len;
	

	Write16(p, 6000); //ExtendItemNum
	Write8(p, 60); //UpdateOnlineInfoInterval


	 //GuildVipBaseInfo 车队贵族基本信息
		BYTE* pGuildVipBaseInfo = p;
		Write16(pGuildVipBaseInfo, 0); //len

		Write8(pGuildVipBaseInfo, Level5); //GuildVipLevel 车队贵族等级
		Write32(pGuildVipBaseInfo, 68910); //GuildVipPoint 车队贵族点数

		len = pGuildVipBaseInfo - p;
		Set16(p, (WORD)len);
		p += len;
	

	//GuildVipOtherInfo 车队贵族其他信息
		BYTE* pGuildVipOtherInfo = p;
		Write16(pGuildVipOtherInfo, 0); //len

		Write32(pGuildVipOtherInfo, 30); //GuildVipLeftDays 车队贵族离开天
		Write8(pGuildVipOtherInfo, 66); //CanReceiveGift 可以收到礼物

		len = pGuildVipOtherInfo - p;
		Set16(p, (WORD)len);
		p += len;
	

	Write8(p, 0); //HasLDMInfo
	/*
m_astPlayerLDMBaseInfo[%d].m_u8Grade
m_astPlayerLDMBaseInfo[%d].m_u32Score
m_astPlayerLDMBaseInfo[%d].m_u32MaxScore
m_astPlayerLDMBaseInfo[%d].m_u32MaxGrade
	*/

	Write32(p, 0x7FFFFFFF); //ForbiddenModeFreeTime
	Write32(p, 0x7FFFFFFF); //ForbiddenModeBitSet


	 //LoverVipBaseInfo 情侣贵族基本信息
		BYTE* pLoverVipBaseInfo = p;
		Write16(pLoverVipBaseInfo, 0); //len

		Write8(pLoverVipBaseInfo, Level2); //LoverVipLevel 情侣贵族等级
		Write32(pLoverVipBaseInfo, 96510); //LoverVipPoint 情侣贵族点数
		Write8(pLoverVipBaseInfo, 30); //GrowRate 情侣贵族成长率

		len = pLoverVipBaseInfo - p;
		Set16(p, (WORD)len);
		p += len;
	
	 //LoverVipOtherInfo 情侣Vip其他信息
		BYTE* pLoverVipOtherInfo = p;
		Write16(pLoverVipOtherInfo, 0); //len

		Write32(pLoverVipOtherInfo, 30); //LoverVipLeftDays 情人贵宾离开天
		Write8(pLoverVipOtherInfo, 1); //CanReceiveGift 可以收到礼物
		Write8(pLoverVipOtherInfo, 30); //ShowExpireTips 显示到期提示

		len = pLoverVipOtherInfo - p;
		Set16(p, (WORD)len);
		p += len;
	

	Write8(p, 0); //SkateTaskLevel
	Write32(p, 0); //SkateCoin
	Write8(p, 0); //SkateExpSkillLevel
	Write16(p, 0); //SkateCoinCountDaily
	Write32(p, 0); //SkateCoinHistoryTotal
	Write32(p, 0); //MaxDayPveScore
	Write32(p, 0); //MaxHistoryPveScore


	Write32(p, 0); //LoveValue 爱的价值

	Write8(p, 0); //HasCheerAddition
	/*
m_astCheerAdditon[%d].m_ushExpAddition
m_astCheerAdditon[%d].m_ushWageAddition
	*/

	{ //PersonalGardenBaseInfo 花园个人基本信息
		BYTE* pPersonalGardenBaseInfo = p;
		Write16(pPersonalGardenBaseInfo, 0); //len

		Write8(pPersonalGardenBaseInfo, Level3); //GardenLevel 花园等级
		Write32(pPersonalGardenBaseInfo, 36521); //GardenPoint 花园点数
		Write8(pPersonalGardenBaseInfo, 30); //GrowRate 成长率
		Write32(pPersonalGardenBaseInfo, 30); //GardenExpireTime 花园的到期时间
		{ //SimpleInfo 简单的信息
			BYTE* pSimpleInfo = pPersonalGardenBaseInfo;
			Write16(pSimpleInfo, 0); //len

			Write32(pSimpleInfo, 0); //WeekPopularity 周人气
			Write32(pSimpleInfo, 0); //TotalPopularity 总受欢迎
			Write32(pSimpleInfo, 0); //LastUpdatePopularityTime 最后更新流行时间
			Write8(pSimpleInfo, 0); //PrivateType 私人类型

			len = pSimpleInfo - pPersonalGardenBaseInfo;
			Set16(pPersonalGardenBaseInfo, (WORD)len);
			pPersonalGardenBaseInfo += len;
		}

		len = pPersonalGardenBaseInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ //ConsumeVipInfo VIP消费信息
		BYTE* pConsumeVipInfo = p;
		Write16(pConsumeVipInfo, 0); //len

		Write32(pConsumeVipInfo, Level4); //VipLevel 魅力等级
		Write32(pConsumeVipInfo, 463211); //CharmValueOfMonth 魅力值
		Write32(pConsumeVipInfo, 5694); //SearchTreasureNums
		Write32(pConsumeVipInfo, 5694); //GetTreasureNums

		len = pConsumeVipInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ //EmperorInfo
		BYTE* pEmperorInfo = p;
		Write16(pEmperorInfo, 0); //len

		Write8(pEmperorInfo, Level5); //EmperorLevel 钻皇等级
		Write32(pEmperorInfo, 6666); //EmperorPoint 钻皇点数
		Write32(pEmperorInfo, 30); //EmperorLeftDays 钻皇到期时间
		Write8(pEmperorInfo, 31); //EmperorGrowRate 钻皇成长率

		len = pEmperorInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ //EmperorOtherInfo
		BYTE* pEmperorOtherInfo = p;
		Write16(pEmperorOtherInfo, 0); //len

		Write32(pEmperorOtherInfo, 3000); //ExpiredTime 过期的时间
		Write8(pEmperorOtherInfo, 30); //ShowExpireTips  显示到期提示

		len = pEmperorOtherInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 0); //ShuttleScoreWeek


	{ //ActivityInfo 活动信息
		BYTE* pActivityInfo = p;
		Write16(pActivityInfo, 0); //len

		Write32(pActivityInfo, 6); //TotalActivity 总活动
		Write32(pActivityInfo, 136); //ActivityLevel 活动水平

		len = pActivityInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ //GansterScoreInfo 匪徒分数信息
		BYTE* pGansterScoreInfo = p;
		Write16(pGansterScoreInfo, 0); //len

		Write32(pGansterScoreInfo, 0); //GansterSeasonID
		Write32(pGansterScoreInfo, 0); //GansterScore
		Write32(pGansterScoreInfo, 0); //PoliceScore
		Write32(pGansterScoreInfo, 0); //TotalGansterScore

		len = pGansterScoreInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 0); //OlympicId
	Write32(p, 0); //NPCEliminateWinTimes

	{ //BorderInfo 边境信息
		BYTE* pBorderInfo = p;
		Write16(pBorderInfo, 0); //len

		Write32(pBorderInfo, 0); //SeasonID
		Write32(pBorderInfo, 0); //Zhanxun
		Write32(pBorderInfo, 0); //SeasonZhanxun

		len = pBorderInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 0); //ReduceReturnRate
	Write32(p, 0); //ReduceReturnAvailPeriod
	Write8(p, 0); //SpecialActivityStatus
	Write32(p, 0); //ThemeHouseDressDegreeValue

	Write8(p, 0); //3DRoomEnabled
	Write8(p, 0); //3DRoomEnabled_OB
	Write8(p, 0); //BuyZizuanPopupOpen

	Write32(p, 0); //BuyZizuanPopupLimit
	Write8(p, 0); //EnableReconnectOpt

	Write8(p, 0); //HasRankedMatchInfo
	/*
m_stRankedMatchGradeInfo[%d].m_uiGradeLevel
m_stRankedMatchGradeInfo[%d].m_uiChildLevel
m_stRankedMatchGradeInfo[%d].m_uiGradeScore
m_stRankedMatchGradeInfo[%d].m_uiMaxGradeScore
m_stRankedMatchGradeInfo[%d].m_uiTotalScore
	*/

	Write8(p, 0); //HasHuanLingChangeInfo
	/*
m_astHuanLingSuitInfo[%d].m_iSuitID
m_astHuanLingSuitInfo[%d].m_ucChangeLevel
	*/

	Write8(p, 0); //EquipSealType
	Write8(p, 0); //hCreditStarFlag
	Write8(p, 0); //PersonalPanelSelectRankedMatchFrameTag
	Write8(p, 0); //VersionURLLen
	Write8(p, 0); //PrivacyURLLen
	Write8(p, 0); //PersonalRankedMatchLevelShowTag
	Write16(p, 0); //LeftDeletingRoleDays
	Write8(p, 0); //OnlyRspEquippedItem
	Write32(p, 0); //LoginSwitchFlag1
	Write16(p, 0); //LeftUploadPLogNum


	len = p - buf;
	int MsgID = GetServerType(Client->ServerID);
	if (MsgID == Relax)
	{
		MsgID = 98;
	}
	else if (MsgID == Dance)
	{
		MsgID = 95;
	}
	else
	{
		MsgID = 100;
	}
	//printf("ServerID:%d\n", Client->ServerID);
	SendToClient(Client, MsgID, buf, len, Client->ConnID, FE_GAMESVRD, Client->ConnID, Response);

	delete[] buf;
}
void NotifyLoginMoreInfo(ClientNode* Client, UINT ItemNum, ItemInfo* Items, bool HasNext)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;


	Write16(p, ItemNum); //ItemNum
	{
		for (size_t i = 0; i < ItemNum; i++)
		{
			BYTE* pItemInfo = p;
			Write16(pItemInfo, 0); //len

			Write32(pItemInfo, Items[i].ItemID);
			Write32(pItemInfo, Items[i].ItemNum);
			Write32(pItemInfo, Items[i].AvailPeriod);
			Write8(pItemInfo, Items[i].Status);
			Write32(pItemInfo, Items[i].ObtainTime);
			Write32(pItemInfo, Items[i].OtherAttribute);
#ifndef ZingSpeed
			Write16(pItemInfo, Items[i].ItemType); //ItemType
#endif

			len = pItemInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}

	Write16(p, 0); //TaskNum
	/*
m_astPlayerDBTaskInfo[].m_iTaskID
m_astPlayerDBTaskInfo[].m_shOptionID
m_astPlayerDBTaskInfo[].m_cStatus
m_astPlayerDBTaskInfo[].m_bPrevStatus
m_astPlayerDBTaskInfo[].m_iProgressRate
m_astPlayerDBTaskInfo[].m_iOther
m_astPlayerDBTaskInfo[].m_uiTakeTime
m_astPlayerDBTaskInfo[].m_uiLastUpdateTime
	*/

	Write16(p, 0); //PetNum
	/*
m_astPetInfo[].m_uiUin
m_astPetInfo[].m_iPetId
m_astPetInfo[].m_szPetName[17]
m_astPetInfo[].m_cIsProper
m_astPetInfo[].m_shLevelLimit
m_astPetInfo[].m_usLevel
m_astPetInfo[].m_iCurLevelExp
m_astPetInfo[].m_iCurLevelMaxExp
m_astPetInfo[].m_ucStatus
m_astPetInfo[].m_usPL
m_astPetInfo[].m_usTotalPL
m_astPetInfo[].m_usPetSkillNum
m_astPetInfo[].m_astPetSkillList[].m_usSkillID
m_astPetInfo[].m_astPetSkillList[].m_ucStatus
m_astPetInfo[].m_astPetSkillList[].m_cActive
m_astPetInfo[].m_astPetSkillList[].m_iValue
m_astPetInfo[].m_iStrengLevel
m_astPetInfo[].m_ucTopStatus
m_astPetInfo[].stCommonInfo.m_iHuanHuaLevel
m_astPetInfo[].stCommonInfo.m_uchCanHuanHua
	*/

	Write16(p, 0); //MapNum
	/*
m_astRecord[].m_iMapID
m_astRecord[].m_iRecord
m_astRecord[].m_iLastUpdateTime
	*/


	Write16(p, 0); //OverdueItemNum
	/*
m_astOverdueItem[].m_iItemID
m_astOverdueItem[].m_iOverdueTime
m_astOverdueItem[].m_iOtherAttr
	*/

	Write16(p, 0); //StoneKartNum
	/*
m_astKartStoneGrooveInfo[].m_iKartID
m_astKartStoneGrooveInfo[].m_iStoneGrooveNum
m_astKartStoneGrooveInfo[].m_astStoneGrooveInfo[].m_iStoneUseOccaType
m_astKartStoneGrooveInfo[].m_astStoneGrooveInfo[].m_iSkillStoneID
	*/

	Write8(p, HasNext);


	len = p - buf;
	SendToClient(Client, 223, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}


void RequestLogout(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	UINT LogOutFlag = Read32(Body);
	int ServerKickReason = Read32(Body);

	ResponseLogout(Client);
	Client->Server->Disconnect(Client->ConnID);
}
void ResponseLogout(ClientNode* Client)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, 0); //ResultID
	Write8(p, 0); //ReasonLen

	len = p - buf;
	SendToClient(Client, 101, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestHello(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseHello(Client, Uin, Time);
}
void ResponseHello(ClientNode* Client, UINT Uin, UINT Time)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, 0); //ResultID
	Write8(p, 0); //ReasonLen
	Write32(p, Time); //Time
	Write32(p, Uin); //Uin
	Write32(p, (DWORD)time(nullptr)); //ServerTime
#ifndef ZingSpeed
	Write32(p, 0); //ServerMicroSecond
#endif

	len = p - buf;
	SendToClient(Client, 112, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}


void RequestChangeGender(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	BYTE NewGender = Read8(Body);


	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

	sql = "UPDATE BaseInfo SET Gender=?  WHERE Uin=? ;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, NewGender);
		sqlite3_bind_int(stmt, 2, Client->Uin);
		result = sqlite3_step(stmt);
	}
	else
	{
		printf("%s\n", sqlite3_errmsg(PlayerDB));
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	ResponseChangeGender(Client, NewGender, 0);
}
void ResponseChangeGender(ClientNode* Client, BYTE NewGender, BYTE UseType)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, 0); //ResultID

	Write8(p, NewGender);
	Write8(p, 0); //ReasonLen
#ifndef ZingSpeed
	Write8(p, UseType);
#endif

	len = p - buf;
	SendToClient(Client, 162, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyChangeItemStatus(ClientNode* Client, UINT Uin, USHORT ItemNum, ItemStatus* ItemStatus)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write32(p, Uin);

	Write16(p, ItemNum);
	for (size_t i = 0; i < ItemNum; i++)
	{
		BYTE* pItemStatus = p;
		Write16(pItemStatus, 0); //len

		Write32(pItemStatus, ItemStatus[i].ItemID);
		Write32(pItemStatus, 1); //ItemNum
		Write32(pItemStatus, -1); //AvailPeriod
		Write8(pItemStatus, ItemStatus[i].NewStatus); //Status
		Write32(pItemStatus, 1); //ObtainTime
		Write32(pItemStatus, 0); //OtherAttribute
		Write16(pItemStatus, 0); //ItemType

		len = pItemStatus - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); //SpecFlag

	Write16(p, 0); //KartNum

	Write8(p, 0); //ChangeType

#ifndef ZingSpeed
	Write8(p, 0); //EquipNiChangItemNum

#endif



	len = p - buf;
	SendToClient(Client, 527, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Notify);
}

void RequestChangeItemStatus(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	USHORT ItemNum = Read16(Body);

	if (ItemNum == 0)
	{
		return;
	}
	ItemStatus* aItemStatus = (ItemStatus*)malloc(sizeof(ItemStatus) * ItemNum);
	if (!aItemStatus)
	{
		return;
	}
	for (size_t i = 0; i < ItemNum; i++)
	{
		BYTE* pItemStatus = Body;
		USHORT len = Read16(pItemStatus);

		aItemStatus[i].ItemID = Read32(pItemStatus);
		aItemStatus[i].NewStatus = Read8(pItemStatus);

		Body += len;
	}
	/*
m_ucSpecFlag
m_ucChangeType
m_usItemNum
m_stEquipedItem[]
m_iNewKartID
	*/


	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

	sql = "BEGIN"; //开始事务
	result = sqlite3_exec(PlayerDB, sql, NULL, NULL, NULL);

	for (size_t i2 = 0; i2 < ItemNum; i2++)
	{
		if (aItemStatus[i2].NewStatus && GetItemType(aItemStatus[i2].ItemID) == EAIT_CAR)
		{
			Client->KartID = aItemStatus[i2].ItemID;
			printf("NewKartID:%d\n", Client->KartID);
		}
		else if (aItemStatus[i2].NewStatus && GetItemType(aItemStatus[i2].ItemID) == EAIT_PET)
		{
			Client->Pet->ID = aItemStatus[i2].ItemID;
			GetPetInfo(Client);
		}
		
		size_t i = 0;
		sql = "UPDATE Item SET Status=? WHERE Uin=? AND ItemID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, aItemStatus[i2].NewStatus);
			sqlite3_bind_int(stmt, 2, Client->Uin);
			sqlite3_bind_int(stmt, 3, aItemStatus[i2].ItemID);
			result = sqlite3_step(stmt);

			printf("ItemID:%d\n", aItemStatus[i2].ItemID);
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
	}

	sql = "COMMIT"; //提交事务
	result = sqlite3_exec(PlayerDB, sql, NULL, NULL, NULL);

	ResponseChangeItemStatus(Client, ItemNum, aItemStatus);//通知客户端更新道具状态
	do
	{
		RoomNode* Room = GetRoom(Client->RoomID);
		if (!Room)
		{
			break;
		}
		for (char i = 0; i < 6; i++)
		{
			ClientNode* RoomClient = Room->Player[i];
			if (RoomClient && RoomClient != Client)
			{
				NotifyChangeItemStatus(RoomClient, Client->Uin, ItemNum, aItemStatus);
			}
		}
	} while (false);
	free(aItemStatus);
}
void ResponseChangeItemStatus(ClientNode* Client, USHORT ItemNum, ItemStatus* ItemStatus)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, 0); //ResultID

	Write16(p, ItemNum);

	for (size_t i = 0; i < ItemNum; i++)
	{
		BYTE* pItemStatus = p;
		Write16(pItemStatus, 0); //len

		Write32(pItemStatus, ItemStatus[i].ItemID);
		Write8(pItemStatus, ItemStatus[i].NewStatus);

		len = pItemStatus - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); //ReasonLen

	Write8(p, 0); //SpecFlag
	Write8(p, 0); //ChangeType


	len = p - buf;
	SendToClient(Client, 130, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
//入库装备皮肤
void RequestChangeSkinStatus(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	printf("Uin值：%d\n", Uin);
	printf("Time值：%d\n", Time);
	int ItemId = Read32(Body);
	printf("ItemId值：%d\n", ItemId);

	USHORT ItemNum = 1;
	printf("ItemNum值：%d\n", ItemNum);

	//查询一下此物品的type类型
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

	int Type=NULL;
	sql = "SELECT Type  FROM Item  WHERE Uin=? AND ItemId=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Uin);
		sqlite3_bind_int(stmt, 2, ItemId);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			Type = sqlite3_column_int(stmt, 0);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	//如果type不是车辆直接返回
	if (Type == 0)
	{
		return;
	}
	if (ItemNum == 0)
	{
		return;
	}
	//封装数据
	ItemStatus* aItemStatus = (ItemStatus*)malloc(sizeof(ItemStatus) * ItemNum);
	if (!aItemStatus)
	{
		return;
	}
	for (size_t i = 0; i < ItemNum; i++)
	{
		BYTE* pItemStatus = Body;
		USHORT len = Read16(pItemStatus);

		//aItemStatus[i].ItemID = Read32(pItemStatus);
		aItemStatus[i].ItemID = ItemId;
		//aItemStatus[i].NewStatus = Read8(pItemStatus);
		aItemStatus[i].NewStatus = 1;

		Body += len;
	}
	//将装备的所有车辆下掉
	sql = "BEGIN"; //开始事务
	result = sqlite3_exec(PlayerDB, sql, NULL, NULL, NULL);

	sql = "UPDATE Item SET Status=0 WHERE Uin=? AND Type=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, Type);
		result = sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	sql = "COMMIT"; //提交事务
	result = sqlite3_exec(PlayerDB, sql, NULL, NULL, NULL);

	//装备新车辆
	sql = "BEGIN"; //开始事务
	result = sqlite3_exec(PlayerDB, sql, NULL, NULL, NULL);

	for (size_t i2 = 0; i2 < ItemNum; i2++)
	{

		if (aItemStatus[i2].ItemID == 127359)
		{
			aItemStatus[i2].ItemID = 126537;
		}
		if (aItemStatus[i2].ItemID == 127360)
		{
			aItemStatus[i2].ItemID = 127365;
		}
		if (aItemStatus[i2].ItemID == 126355)
		{
			aItemStatus[i2].ItemID = 127437;
		}
		if (aItemStatus[i2].ItemID == 123513)
		{
			aItemStatus[i2].ItemID = 123238;
		}
		if (aItemStatus[i2].ItemID == 120246)
		{
			aItemStatus[i2].ItemID = 118729;
		}

		/*if (aItemStatus[i2].NewStatus && GetItemType(aItemStatus[i2].ItemID) == EAIT_CAR)
		{*/
		Client->KartID = aItemStatus[i2].ItemID;
		printf("NewKartID:%d\n", Client->KartID);
		/*}*/

		size_t i = 0;
		sql = "UPDATE Item SET Status=? WHERE Uin=? AND ItemID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, aItemStatus[i2].NewStatus);
			sqlite3_bind_int(stmt, 2, Client->Uin);
			sqlite3_bind_int(stmt, 3, aItemStatus[i2].ItemID);
			result = sqlite3_step(stmt);

			printf("ItemID:%d\n", aItemStatus[i2].ItemID);
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
	}

	sql = "COMMIT"; //提交事务
	result = sqlite3_exec(PlayerDB, sql, NULL, NULL, NULL);

	ResponseChangeItemStatus(Client, ItemNum, aItemStatus);
	do
	{
		RoomNode* Room = GetRoom(Client->RoomID);
		if (!Room)
		{
			break;
		}
		for (char i = 0; i < 6; i++)
		{
			ClientNode* RoomClient = Room->Player[i];
			if (RoomClient && RoomClient != Client)
			{
				NotifyChangeItemStatus(RoomClient, Client->Uin, ItemNum, aItemStatus);
			}
		}
	} while (false);
	free(aItemStatus);
}

void RequestChangeModel(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	BYTE NewModel = Read8(Body);
	//UCHAR UseType = Read8(Body);

	ResponseChangeModel(Client, NewModel);
	RoomNode* Room = GetRoom(Client->RoomID);
	if (!Room)
	{
		return;
	}
	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient && RoomClient != Client)
		{
			NotifyChangeModel(RoomClient, Client->Uin, NewModel);
		}
	}
}
void ResponseChangeModel(ClientNode* Client, BYTE NewModel)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, 0); //ResultID
	Write8(p, 0); //ReasonLen
	Write8(p, NewModel);
#ifndef ZingSpeed
	Write8(p, 0); //UseType
	Write8(p, 0); //TDCExcluItemNum
	//Write32(p, 0); //TDCExclusiveItemID[]
#endif

	len = p - buf;
	SendToClient(Client, 277, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyChangeModel(ClientNode* Client, UINT Uin, BYTE NewModel)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write32(p, Uin);
	Write8(p, NewModel);
#ifndef ZingSpeed
	Write8(p, 0); //TDCExcluItemNum
	//Write32(p, 0); //TDCExclusiveItemID[]
#endif

	len = p - buf;
	SendToClient(Client, 278, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
//VIP测试
void NotifyClientVipFlag(ClientNode* Client)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write32(p, Client->Uin); //Uin
	Write16(p, 0x0107); //VipFlag
	Write16(p, 0); //MsgLen
	Write16(p, 0); //VipGrowRate

	len = p - buf;
	SendToClient(Client, 948, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}


void RequestInlaySkillStone(ClientNode* Client, BYTE* Body, size_t BodyLen)//赛车镶嵌宝石
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	int KartID = Read32(Body);
	int StoneGrooveID = Read32(Body);
	int SkillStoneID = Read32(Body);

	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;


	int ID = -1;
	sql = "SELECT ID  FROM KartStoneGroove  WHERE Uin=? AND KartID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Uin);
		sqlite3_bind_int(stmt, 2, KartID);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			ID = sqlite3_column_int(stmt, 0);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	if (ID != -1)
	{
		sql = "UPDATE KartStone SET StoneUseOccaType=?,SkillStoneID=?  WHERE ID=? AND StoneGrooveID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, 0);
			sqlite3_bind_int(stmt, 2, SkillStoneID);
			sqlite3_bind_int(stmt, 3, ID);
			sqlite3_bind_int(stmt, 4, StoneGrooveID);
			result = sqlite3_step(stmt);
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
	}
	NotifyUpdateKartSkillStoneInfo(Client, KartID);
	ResponseInlaySkillStone(Client, KartID, StoneGrooveID, SkillStoneID);
}
void ResponseInlaySkillStone(ClientNode* Client, int KartID, int StoneGrooveID, int SkillStoneID)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write16(p, 0); //ReasonLen
	Write32(p, KartID);
	Write32(p, StoneGrooveID);
	Write32(p, SkillStoneID);

	len = p - buf;
	SendToClient(Client, 901, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void NotifyUpdateKartSkillStoneInfo(ClientNode* Client, int KartID)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

	int ID = -1;
	sql = "SELECT ID  FROM KartStoneGroove  WHERE Uin=? AND KartID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, KartID);
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

	Write32(p, Client->Uin); //Uin
	{ //KartStoneInfo
		BYTE* pKartStoneInfo = p;
		Write16(pKartStoneInfo, 0); //len

		Write32(pKartStoneInfo, KartID); //KartID

		BYTE* pStoneGrooveNum = pKartStoneInfo;
		Write32(pKartStoneInfo, 0); //StoneGrooveNum
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

					BYTE* pStoneGrooveInfo = pKartStoneInfo;
					Write16(pStoneGrooveInfo, 0); //len

					Write32(pStoneGrooveInfo, sqlite3_column_int(stmt, 0)); //StoneUseOccaType
					Write32(pStoneGrooveInfo, sqlite3_column_int(stmt, 1)); //SkillStoneID

					len = pStoneGrooveInfo - pKartStoneInfo;
					Set16(pKartStoneInfo, (WORD)len);
					pKartStoneInfo += len;
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


		len = pKartStoneInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 906, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}


void NotifyKickFromServer(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ReasonID
	Write32(p, Client->Uin); //SrcUin
	Write8(p, 0); //ReasonLen
	Write8(p, 0); //AntiCheatMode
	Write8(p, 0); //KickType

	len = p - buf;
	SendToClient(Client, 900, buf, len, Client->ServerID, FE_GAMESVRD, Client->ConnID, Notify);
}
void ResponseKartRefitInfoMoreInfo(ClientNode* Client, size_t num);

void ResponseKartRefitInfoMoreInfo(ClientNode* Client, size_t num)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	BYTE* pKartNum = p;
	Write16(p, 0); //KartNum
	bool HasMoreInfo = false;
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	size_t i = 0;
	sql = "SELECT KartID,RefitCout,MaxFlags,WWeight,SpeedWeight,JetWeight,SJetWeight,AccuWeight,ShapeRefitCount,KartHeadRefitItemID,KartTailRefitItemID,KartFlankRefitItemID,KartTireRefitItemID  FROM KartRefit WHERE Uin = ? LIMIT ?,150;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, num * 100);
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{ //KartRefitInfo
			if (i < 100) {
				i++;
				BYTE* pKartRefitInfo = p;
				Write16(pKartRefitInfo, 0); //len

				Write32(pKartRefitInfo, Client->Uin); //Uin
				Write32(pKartRefitInfo, sqlite3_column_int(stmt, 0)); //KartId

				Write32(pKartRefitInfo, sqlite3_column_int(stmt, 1)); //RefitCout
				Write16(pKartRefitInfo, sqlite3_column_int(stmt, 2)); //MaxFlags
				Write16(pKartRefitInfo, sqlite3_column_int(stmt, 3)); //WWeight
				Write16(pKartRefitInfo, sqlite3_column_int(stmt, 4)); //SpeedWeight
				Write16(pKartRefitInfo, sqlite3_column_int(stmt, 5)); //JetWeight
				Write16(pKartRefitInfo, sqlite3_column_int(stmt, 6)); //SJetWeight
				Write16(pKartRefitInfo, sqlite3_column_int(stmt, 7)); //AccuWeight

				Write32(pKartRefitInfo, sqlite3_column_int(stmt, 8)); //ShapeRefitCount
				Write32(pKartRefitInfo, sqlite3_column_int(stmt, 9)); //KartHeadRefitItemID
				Write32(pKartRefitInfo, sqlite3_column_int(stmt, 10)); //KartTailRefitItemID
				Write32(pKartRefitInfo, sqlite3_column_int(stmt, 11)); //KartFlankRefitItemID
				Write32(pKartRefitInfo, sqlite3_column_int(stmt, 12)); //KartTireRefitItemID
				{ //KartRefitExInfo
					BYTE* pKartRefitExInfo = pKartRefitInfo;
					Write16(pKartRefitExInfo, 0); //len

					Write8(pKartRefitExInfo, 0); //SpeedRefitStar
					Write8(pKartRefitExInfo, 0); //JetRefitStar
					Write8(pKartRefitExInfo, 0); //SJetRefitStar
					Write8(pKartRefitExInfo, 0); //AccuRefitStar
					Write8(pKartRefitExInfo, 0); //SpeedAddRatio
					Write8(pKartRefitExInfo, 0); //JetAddRatio
					Write8(pKartRefitExInfo, 0); //SJetAddRatio
					Write8(pKartRefitExInfo, 0); //AccuAddRatio

					len = pKartRefitExInfo - pKartRefitInfo;
					Set16(pKartRefitInfo, (WORD)len);
					pKartRefitInfo += len;
				}

				len = pKartRefitInfo - p;
				Set16(p, (WORD)len);
				p += len;
			}
			else
			{
				HasMoreInfo = true;
				break;
			}
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
#ifdef DEBUG
	printf("ResponseKartRefitInfoMoreInfo HasMoreInfo: %d num: %d\n", HasMoreInfo, i);
#endif
	Set16(pKartNum, (WORD)i);

	Write8(p, HasMoreInfo); //HasNext

	len = p - buf;
	SendToClient(Client, 318, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
	if (HasMoreInfo) {
		num++;
		ResponseKartRefitInfoMoreInfo(Client, num);
	}
}

void RequestKartRefitInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseKartRefitInfo(Client);
}
void ResponseKartRefitInfo(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write32(p, Client->Uin); //Uin

	bool HasMoreInfo = false;

	BYTE* pKartNum = p;
	Write16(p, 0); //KartNum
	{
		const char* sql = NULL;
		sqlite3_stmt* stmt = NULL;
		int result;
		size_t i = 0;
		sql = "SELECT KartID,RefitCout,MaxFlags,WWeight,SpeedWeight,JetWeight,SJetWeight,AccuWeight,ShapeRefitCount,KartHeadRefitItemID,KartTailRefitItemID,KartFlankRefitItemID,KartTireRefitItemID  FROM KartRefit WHERE Uin = ?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, Client->Uin);
			while (sqlite3_step(stmt) == SQLITE_ROW)
			{ //KartRefitInfo
			   if (i < 100) {
				  i++;

				  BYTE* pKartRefitInfo = p;
				  Write16(pKartRefitInfo, 0); //len

				  Write32(pKartRefitInfo, Client->Uin); //Uin
				  Write32(pKartRefitInfo, sqlite3_column_int(stmt, 0)); //KartId		
				  Write32(pKartRefitInfo, sqlite3_column_int(stmt, 1)); //RefitCout
				  Write16(pKartRefitInfo, sqlite3_column_int(stmt, 2)); //MaxFlags
				  Write16(pKartRefitInfo, sqlite3_column_int(stmt, 3)); //WWeight
				  Write16(pKartRefitInfo, sqlite3_column_int(stmt, 4)); //SpeedWeight
				  Write16(pKartRefitInfo, sqlite3_column_int(stmt, 5)); //JetWeight
				  Write16(pKartRefitInfo, sqlite3_column_int(stmt, 6)); //SJetWeight
				  Write16(pKartRefitInfo, sqlite3_column_int(stmt, 7)); //AccuWeight

				  Write32(pKartRefitInfo, sqlite3_column_int(stmt, 8)); //ShapeRefitCount
				  Write32(pKartRefitInfo, sqlite3_column_int(stmt, 9)); //KartHeadRefitItemID
				  Write32(pKartRefitInfo, sqlite3_column_int(stmt, 10)); //KartTailRefitItemID
				  Write32(pKartRefitInfo, sqlite3_column_int(stmt, 11)); //KartFlankRefitItemID
				  Write32(pKartRefitInfo, sqlite3_column_int(stmt, 12)); //KartTireRefitItemID
				  //printf("OldShapeSuitID值：%d\n", sqlite3_column_int(stmt, 13));
				  { //KartRefitExInfo
				  	  BYTE* pKartRefitExInfo = pKartRefitInfo;
					  Write16(pKartRefitExInfo, 0); //len

					  Write8(pKartRefitExInfo, 0); //SpeedRefitStar
					  Write8(pKartRefitExInfo, 0); //JetRefitStar
					  Write8(pKartRefitExInfo, 0); //SJetRefitStar
					  Write8(pKartRefitExInfo, 0); //AccuRefitStar
					  Write8(pKartRefitExInfo, 0); //SpeedAddRatio
					  Write8(pKartRefitExInfo, 0); //JetAddRatio
					  Write8(pKartRefitExInfo, 0); //SJetAddRatio
					  Write8(pKartRefitExInfo, 0); //AccuAddRatio

					  len = pKartRefitExInfo - pKartRefitInfo;
					  Set16(pKartRefitInfo, (WORD)len);
					  pKartRefitInfo += len;
				  }

				  len = pKartRefitInfo - p;
				  Set16(p, (WORD)len);
				  p += len;
			 
			   }
			   else
			   {
			 	   HasMoreInfo = true;
			       break;
			   }
			}
		}    
		else
		{
			printf("%s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
		Set16(pKartNum, (WORD)i);
	}

#ifndef ZingSpeed
	Write32(p, 0); //MaxLuckyWeight
	Write32(p, 0); //LuckyValue
	Write32(p, 0); //MaxLuckyValue
	Write8(p, 0); //LevelNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //LuckyValueInfo
		BYTE* pLuckyValueInfo = p;
		Write16(pLuckyValueInfo, 0); //len

		Write32(pLuckyValueInfo, 0); //Level
		Write32(pLuckyValueInfo, 0); //LuckyValue

		len = pLuckyValueInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	Write8(p, 0); //HasMoreInfo
#endif



	len = p - buf;
	SendToClient(Client, 271, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
	if (HasMoreInfo) {
		ResponseKartRefitInfoMoreInfo(Client, 1);
	}
}


void RequestNewGetFriendList(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseNewGetFriendList(Client);
}
void ResponseNewGetFriendList(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write32(p, Client->Uin); //Uin
	Write16(p, 0); //MaxFriendNum
	Write16(p, 0); //TotalFriendNum
	Write16(p, 0); //FriendGroupNum
	/*
	for (size_t i = 0; i < 1; i++)
	{ //FriendGroupInfo
		BYTE* pFriendGroupInfo = p;
		Write16(pFriendGroupInfo, 0); //len

		Write16(pFriendGroupInfo, 0); //GroupID
		//GroupName[]
		pFriendGroupInfo += MaxNickName;
		Write16(pFriendGroupInfo, 0); //FriendNum
		for (size_t i = 0; i < 1; i++)
		{ //Friend
			BYTE* pFriend = pFriendGroupInfo;
			Write16(pFriend, 0); //len

			Write32(pFriend, 0); //FriendUin
			Write32(pFriend, 0); //RelationFlag
			Write32(pFriend, 0); //Degree
			Write8(pFriend, 0); //Flag
			Write32(pFriend, 0); //ReadedEventID

			len = pFriend - pFriendGroupInfo;
			Set16(pFriendGroupInfo, (WORD)len);
			pFriendGroupInfo += len;
		}

		len = pFriendGroupInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write8(p, 0); //ReasonLen

	len = p - buf;
	SendToClient(Client, 163, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}


void NotifyClientAddItem(ClientNode* Client, UINT ItemNum, ItemInfo* Items)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write32(p, Client->Uin); //SrcUin
	Write16(p, ItemNum); //ItemNum
	{
		for (size_t i = 0; i < ItemNum; i++)
		{
			BYTE* pItemInfo = p;
			Write16(pItemInfo, 0); //len

			Write32(pItemInfo, Items[i].ItemID);
			Write32(pItemInfo, Items[i].ItemNum);
			Write32(pItemInfo, Items[i].AvailPeriod);
			Write8(pItemInfo, Items[i].Status);
			Write32(pItemInfo, Items[i].ObtainTime);
			Write32(pItemInfo, Items[i].OtherAttribute);
#ifndef ZingSpeed
			Write16(pItemInfo, Items[i].ItemType); //ItemType
#endif

			len = pItemInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}

	Write16(p, 0); //AttachLen
	Write16(p, 0); //AddKartNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //AddKartRefitInfo
		BYTE* pAddKartRefitInfo = p;
		Write16(pAddKartRefitInfo, 0); //len

		Write32(pAddKartRefitInfo, 0); //Uin
		Write32(pAddKartRefitInfo, 0); //KartId
		Write32(pAddKartRefitInfo, 0); //RefitCout
		Write16(pAddKartRefitInfo, 0); //MaxFlags
		Write16(pAddKartRefitInfo, 0); //WWeight
		Write16(pAddKartRefitInfo, 0); //SpeedWeight
		Write16(pAddKartRefitInfo, 0); //JetWeight
		Write16(pAddKartRefitInfo, 0); //SJetWeight
		Write16(pAddKartRefitInfo, 0); //AccuWeight
		Write32(pAddKartRefitInfo, 0); //ShapeRefitCount
		Write32(pAddKartRefitInfo, 0); //KartHeadRefitItemID
		Write32(pAddKartRefitInfo, 0); //KartTailRefitItemID
		Write32(pAddKartRefitInfo, 0); //KartFlankRefitItemID
		Write32(pAddKartRefitInfo, 0); //KartTireRefitItemID

		len = pAddKartRefitInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write8(p, 0); //IncRedPacketNum

	len = p - buf;
	SendToClient(Client, 529, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestDeleteOneItem(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	int ItemID = Read32(Body);

	int ListItemSkipDelete[] = {
		10020,//Kart D
		10074,//Decal

		10043,//
		10044,//
		10045,//

		10050,//
		10051,//
		10032,//

		27178,//
		27179,//
		27180,//
		27181,//
		27182,//

		27190,//
		27191,//
		27192,//
		27193,//
		27194,//
	};

	BOOL ItemSkipDelete = TRUE;
	for (int i = 0; i < sizeof(ListItemSkipDelete); i++) {
		if (ListItemSkipDelete[i] == ItemID) {
			ItemSkipDelete = FALSE;
			break;
		}
	}

	int ResultID = 0;
	const char* Reason = "";

	if (ItemSkipDelete)
	{
		PlayerDB_DeleteItem(Uin, ItemID);
	}
	else
	{
		ResultID = 1;
		Reason = "无法删除默认项目!";
	}

	ResponseDeleteOneItem(Client, ItemID, ResultID, Reason);
}
void ResponseDeleteOneItem(ClientNode* Client, int ItemID, int ResultID, const char* Reason)
{
	BYTE* buf = new BYTE[1024 * 8]; BYTE* p = buf; size_t len;

	Write16(p, ResultID); //ResultID

	Write32(p, Client->Uin); //Uin
	Write32(p, ItemID); //ItemID

	len = strlen(Reason);
	Write8(p, (BYTE)len); //ReasonLen
	memcpy(p, Reason, len);
	p += len;

	len = p - buf;
	SendToClient(Client, 261, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
//抽奖
void RequestUseItem(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	int ItemId = Read32(Body);
	printf("ItemId值：%d\n", ItemId);
	printf("Uin值：%d\n", Uin);

	int TreasureBoxID = Read32(p);
	//printf("TreasureBoxID值：%d\n", TreasureBoxID);
	//{ //Key
	//	BYTE* pKey = p;
	//	len = Read16(pKey);

	//	int KeyType = Read32(pKey);
	//	for (size_t i = 0; i < 1; i++)
	//	{ //KeyEntry
	//		BYTE* pKeyEntry = pKey;
	//		len = Read16(pKeyEntry);

	//		int KeyID = Read32(pKeyEntry);
	//		int Num = Read32(pKeyEntry);

	//		pKey += len;
	//	}
	//	bool IsNotConsume = Read8(pKey);
	//	
	//	p += len;
	//}
	//bool SpecialIdx = Read8(p);
	////printf("SpecialIdx值：%d\n", SpecialIdx);
	//USHORT MutiOpenNum = Read16(p);
	////printf("MutiOpenNum值：%d\n", MutiOpenNum);
	//USHORT OpenBoxSrcType = Read16(p);
	////printf("OpenBoxSrcType值：%d\n", OpenBoxSrcType);
	//UCHAR SelectAwardNum = Read8(p);
	////printf("SelectAwardNum值：%d\n", SelectAwardNum);
	//for (size_t i = 0; i < 1; i++)
	//{ //AwardIndexInfo
	//	BYTE* pAwardIndexInfo = p;
	//	len = Read16(pAwardIndexInfo);

	//	UCHAR AwardIdx = Read8(pAwardIndexInfo);
	//	//printf("AwardIdx值：%d\n", AwardIdx);
	//	int ItemID = Read32(pAwardIndexInfo);
	//	//printf("ItemID值：%d\n", ItemID);
	//	p += len;
	//}

	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	int CurrentItemNum = -1;

	//查询道具数量
	sql = "SELECT ItemNum  FROM Item  WHERE Uin=? AND ItemId=?;";//SQL语句
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL); //申明对象
	if (result == SQLITE_OK) {//返回值:成功
		sqlite3_bind_int(stmt, 1, Uin);//给SQL语句绑定整型参数
		sqlite3_bind_int(stmt, 2, ItemId);//给SQL语句绑定整型参数
		result = sqlite3_step(stmt);//执行SQL3语句
		if (result == SQLITE_ROW)//查询成功
		{
			CurrentItemNum = sqlite3_column_int(stmt, 0);//CurrentItemNum变量= SQL3语句取到的值
			printf("ItemNum值：%d\n", CurrentItemNum);//打印
		}
		else//如果返回其他
		{
			CurrentItemNum = 0;//CurrentItemNum为零
		}
	}
	sqlite3_finalize(stmt);//销毁SQL3
	stmt = NULL;//设置预编译语句为空
	if (CurrentItemNum == 0)
	{
		return;
	}

	sql = "UPDATE Item SET ItemNum=ItemNum-1  WHERE Uin=? AND ItemId=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Uin);
		sqlite3_bind_int(stmt, 2, ItemId);
		result = sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	printf("物品代码：%d\n", ItemId);
	CurrentItemNum--;
	NotifyUseItemResult(Client, ItemId, 1, CurrentItemNum);
	//抽奖 随机出来   查询当前物品是否拥有 有的话不做操作  没有的话加入库存
	char FilePath[MAX_PATH];
	sprintf_s(FilePath,".\\Gift\\%d.yml", ItemId);
	YAML::Node GiftBox;
	if (isFileExists_stat(FilePath))
	{
		GiftBox = YAML::LoadFile(FilePath);
	}
	else
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		printf("缺少Gift%d的配置文件\n", ItemId);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN| FOREGROUND_RED);
		return;
	}
	int giftType = GiftBox["Type"].as<int>();
	ItemInfo Items[1];
	//解决越界问题

	int randomNum = GiftBox["Items"].size() - 1;
	int random = (rand() % (randomNum - 0 + 1)) + 0;
	//取消限制
	//int random = (rand() % (GiftBox["Items"].size() - 0 + 1)) + 0;
	//限制100
	/*int random = 0;

	if (ItemId==27570)
	{
		random = (rand () % (145 - 0 + 1)) + 0;
	}
	if (ItemId==69010)
	{
		random = (rand() % (125 - 0 + 1)) + 0;
	}
	if (ItemId == 28286)
	{
		random = (rand() % (140 - 0 + 1)) + 0;
	}
	if (ItemId == 22642)
	{
		random = (rand() % (140 - 0 + 1)) + 0;
	}
	*/
	printf("rand值：%d\n", random);
	Items[0].ItemID = GiftBox["Items"][random]["ItemId"].as<UINT>();
	Items[0].ItemNum = GiftBox["Items"][random]["ItemNum"].as<int>();
	Items[0].AvailPeriod = GiftBox["Items"][random]["AvailPeriod"].as<int>();
	Items[0].Status = false;
	Items[0].ItemType = GiftBox["Items"][random]["ItemType"].as<int>();
	
	ResponseUseItem(Client, ItemId, 1, Items, giftType);
	bool IsHave = false;
	//查询是否拥有此物品  拥有的话不加库存
	sql = "SELECT ItemNum  FROM Item  WHERE Uin=? AND ItemId=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Uin);
		sqlite3_bind_int(stmt, 2, Items[0].ItemID);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			IsHave = true;
		}
		else
		{
			IsHave = false;
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	if (IsHave==true)
	{
		return;
	}
	//加库存并通知
	if (Items[0].ItemID== 127359)
	{
		PlayerDB_AddItem(Uin, 126537, Items[0].ItemNum, Items[0].AvailPeriod, Items[0].Status, Items[0].ItemType);
	}
	if (Items[0].ItemID == 127360)
	{
		PlayerDB_AddItem(Uin, 127365, Items[0].ItemNum, Items[0].AvailPeriod, Items[0].Status, Items[0].ItemType);
	}
	if (Items[0].ItemID == 120246)
	{
		PlayerDB_AddItem(Uin, 118729, Items[0].ItemNum, Items[0].AvailPeriod, Items[0].Status, Items[0].ItemType);
	}
	PlayerDB_AddItem(Uin, Items[0].ItemID, Items[0].ItemNum, Items[0].AvailPeriod, Items[0].Status, Items[0].ItemType);

	if(ItemId== 28286)
	{
		const char* Name = "我的宠物";
		int Experience = 1165342;
		int PL = 100;

		sql = "INSERT INTO Pet (Uin,PetId,Name,Experience,PL) VALUES (?,?,?,?,?);";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, Uin);
			sqlite3_bind_int(stmt, 2, Items[0].ItemID);
			sqlite3_bind_text(stmt, 3, Name, strlen(Name), SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt, 4, Experience);
			sqlite3_bind_int(stmt, 5, PL);
			result = sqlite3_step(stmt);
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
	}




	NotifyClientAddItem(Client, Items[0].ItemNum, Items);
	ItemStatus NewStatus;
	NewStatus.ItemID = Items[0].ItemID;
	NewStatus.NewStatus = Items[0].Status;
	ResponseChangeItemStatus(Client, 1, &NewStatus);
}

void ResponseUseItem(ClientNode* Client, int ItemID, UINT ItemNum, ItemInfo* Items,int giftType)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;
	Write16(p, 0); //Uin
	Write32(p, Client->Uin); //Uin
	{ //TreasureBoxToClient
		BYTE* pTreasureBoxToClient = p;
		Write16(pTreasureBoxToClient, 0); //len

		Write32(pTreasureBoxToClient, ItemID); //BoxID
		Write32(pTreasureBoxToClient, 1); //BoxType
		{ //Key
			BYTE* pKey = pTreasureBoxToClient;
			Write16(pKey, 0); //len

			Write32(pKey, 0); //KeyType
			for (size_t i = 0; i < ItemNum; i++)
			{ //KeyEntry
				BYTE* pKeyEntry = pKey;
				Write16(pKeyEntry, 0); //len

				Write32(pKeyEntry, 0); //KeyID
				Write32(pKeyEntry, 0); //Num

				len = pKeyEntry - pKey;
				Set16(pKey, (WORD)len);
				pKey += len;
			}
			Write8(pKey, 0); //IsNotConsume

			len = pKey - pTreasureBoxToClient;
			Set16(pTreasureBoxToClient, (WORD)len);
			pTreasureBoxToClient += len;
		}
		{ //AwardEntry
			BYTE* pAwardEntry = pTreasureBoxToClient;
			Write16(pAwardEntry, 0); //len

			Write32(pAwardEntry, 0); //Exp
			Write32(pAwardEntry, 0); //Money
			Write32(pAwardEntry, 0); //SuperMoney
			Write16(pAwardEntry, ItemNum); //ItemNum
			for (size_t i = 0; i < ItemNum; i++)
			{ //ItemInfo
				BYTE* pItemInfo = pAwardEntry;
				Write16(pItemInfo, 0); //len

				Write32(pItemInfo, Items[i].ItemID); //ItemID
				Write32(pItemInfo, Items[i].ItemNum); //ItemNum
				Write32(pItemInfo, Items[i].AvailPeriod); //AvailPeriod
				Write8(pItemInfo, Items[i].Status); //Status
				Write32(pItemInfo, Items[i].ObtainTime); //ObtainTime
				Write32(pItemInfo, Items[i].OtherAttribute); //OtherAttribute
				Write16(pItemInfo, Items[i].ItemType); //ItemType

				len = pItemInfo - pAwardEntry;
				Set16(pAwardEntry, (WORD)len);
				pAwardEntry += len;
			}
			Write32(pAwardEntry, 0); //Coupons
			Write32(pAwardEntry, 0); //GuildPoint
			Write32(pAwardEntry, 0); //LuckMoney
			Write8(pAwardEntry, 0); //ExtendInfoNum
			for (size_t i = 0; i < ItemNum; i++)
			{ //ExtendInfoAward
				BYTE* pExtendInfoAward = pAwardEntry;
				Write16(pExtendInfoAward, 0); //len

				Write16(pExtendInfoAward, 0); //Key
				Write32(pExtendInfoAward, 0); //AddValue
				Write32(pExtendInfoAward, 0); //TotalValue
				Write8(pExtendInfoAward, 0); //ShowPriority
				Write16(pExtendInfoAward, 0); //BuyNeedScore
				Write8(pExtendInfoAward, 0); //OneMatchMaxNum

				len = pExtendInfoAward - pAwardEntry;
				Set16(pAwardEntry, (WORD)len);
				pAwardEntry += len;
			}
			Write32(pAwardEntry, 0); //SpeedCoin

			len = pAwardEntry - pTreasureBoxToClient;
			Set16(pTreasureBoxToClient, (WORD)len);
			pTreasureBoxToClient += len;
		}

		len = pTreasureBoxToClient - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); //ReasonLen
	Write32(p, 0); //Reserve
	Write32(p, 0); //MaxLuckVaue
	Write32(p, 0); //LuckValue
	Write32(p, 0); //TotalLuckValue
	Write32(p, 0); //OpenDate
	Write32(p, 0); //CloseDate
	Write32(p, 0); //OpenTime
	Write32(p, 0); //CloseTime

	len = p - buf;
//	Write32(p, ItemID); //ItemID  宝箱id
//	printf("response itemid:%d/n", ItemID);
//	Write16(p, ItemNum); //ItemID
//	{
//		for (size_t i = 0; i < ItemNum; i++)
//		{
//			BYTE* pItemInfo = p;
//			Write16(pItemInfo, 0); //len
//
//			Write32(pItemInfo, Items[i].ItemID);
//			printf("response 物品:%d/n", Items[i].ItemID);
//			Write32(pItemInfo, Items[i].ItemNum);
//			Write32(pItemInfo, Items[i].AvailPeriod);
//			Write8(pItemInfo, Items[i].Status);
//			Write32(pItemInfo, Items[i].ObtainTime);
//			Write32(pItemInfo, Items[i].OtherAttribute);
//#ifndef ZingSpeed
//			Write16(pItemInfo, Items[i].ItemType); //ItemType
//#endif
//
//			len = pItemInfo - p;
//			Set16(p, (WORD)len);
//			p += len;
//		}
//	}
//
//	Write16(p, 0); //AttachLen
//	Write16(p, 0); //AddKartNum
//	Write8(p, 0); //IncRedPacketNum
//
//	len = p - buf;
	SendToClient(Client, 168, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void NotifyUseItemResult(ClientNode* Client, int ItemID, int UseItemNum, int CurrentItemNum)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write32(p, ItemID); //ItemID
	Write32(p, UseItemNum); //UseItemNum
	Write32(p, CurrentItemNum); //CurrentItemNum

	len = p - buf;
	SendToClient(Client, 556, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestRefitKart(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	int KartItemId = Read32(Body);

	UCHAR KartNameLen = Read8(Body);
	Body += KartNameLen;

	int RefitItemId = Read32(Body);

	int Reserved = Read32(Body);
#ifndef ZingSpeed
	short RefitTimes = Read16(Body);
#endif
	//printf("[RequestRefitKart] KartItemId: %d, KartNameLen: %d, RefitItemId: %d, Reserved: %d\n", KartItemId, KartNameLen, RefitItemId, Reserved);

	const char* Reason = u8"Sửa đổi thất bại!";

	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

	int RefitItemNum = 0;
	//GetItemNum
	if (sqlite3_prepare_v2(PlayerDB, "SELECT ItemNum from Item WHERE Uin=? AND ItemID=?;", -1, &stmt, NULL) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, RefitItemId);

		if (sqlite3_step(stmt) == SQLITE_ROW) RefitItemNum = sqlite3_column_int(stmt, 0);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	//GetItemNum

	if (RefitItemNum > 0) {
		short MaxFlags = 0;
		short WWeight = 0;
		short SpeedWeight = 0;
		short JetWeight = 0;
		short SJetWeight = 0;
		short AccuWeight = 0;

		RefitItemNum--;
		if (RefitItemNum <= 0) {//Delete Item
			if (sqlite3_prepare_v2(PlayerDB, "DELETE FROM Item  WHERE Uin=? AND ItemID=?;", -1, &stmt, NULL) == SQLITE_OK) {
				sqlite3_bind_int(stmt, 1, Client->Uin);
				sqlite3_bind_int(stmt, 2, RefitItemId);
				sqlite3_step(stmt);
			}
		}
		else
		{
			if (sqlite3_prepare_v2(PlayerDB, "UPDATE Item SET ItemNum=ItemNum-1 WHERE Uin = ? AND ItemID = ?;", -1, &stmt, NULL) == SQLITE_OK) {
				sqlite3_bind_int(stmt, 1, Client->Uin);
				sqlite3_bind_int(stmt, 2, RefitItemId);
				sqlite3_step(stmt);
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;

		short OldWWeight = 0;
		short OldSpeedWeight = 0;
		short OldJetWeight = 0;
		short OldSJetWeight = 0;
		short OldAccuWeight = 0;

		sql = "SELECT RefitCout,MaxFlags,WWeight,SpeedWeight,JetWeight,SJetWeight,AccuWeight,ShapeRefitCount,KartHeadRefitItemID,KartTailRefitItemID,KartFlankRefitItemID,KartTireRefitItemID  FROM KartRefit WHERE Uin=? AND KartID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, Client->Uin);
			sqlite3_bind_int(stmt, 2, KartItemId);
			if (sqlite3_step(stmt) == SQLITE_ROW)
			{
				OldWWeight = sqlite3_column_int(stmt, 2);
				OldSpeedWeight = sqlite3_column_int(stmt, 3);
				OldJetWeight = sqlite3_column_int(stmt, 4);
				OldSJetWeight = sqlite3_column_int(stmt, 5);
				OldAccuWeight = sqlite3_column_int(stmt, 6);
			}
		}

		sqlite3_finalize(stmt);
		stmt = NULL;

		//NotifyUseItemResult(Client, RefitItemId, 1, RefitItemNum);

		switch (RefitItemId)
		{
		case 12376: ////点火装置+1
			//限制改装超过29时不增加数据
			if (OldSJetWeight >= 29)
			{
				SJetWeight = 0;
				Reason = "改装失败,超出限制29！";
				break;
			}
			else
			{
				SJetWeight = 1;
				Reason = "恭喜你改装成功！";
				NotifyUseItemResult(Client, RefitItemId, 1, RefitItemNum);
				break;
			}
		case 12377: ////进气系统+1
			if (OldAccuWeight >= 29)
			{
				AccuWeight = 0;
				Reason = "改装失败,超出限制29！";
				break;
			}
			else
			{
				AccuWeight = 1;
				Reason = "恭喜你改装成功！";
				NotifyUseItemResult(Client, RefitItemId, 1, RefitItemNum);
				break;
			}
		case 12378: //Nhiên liệu+1 //燃料系统+1
			if (OldJetWeight >= 29)
			{
				JetWeight = 0;
				Reason = "改装失败,超出限制29！";
				break;
			}
			else
			{
				JetWeight = 1;
				Reason = "恭喜你改装成功！";
				NotifyUseItemResult(Client, RefitItemId, 1, RefitItemNum);
				break;
			}
		case 12379: //Giảm Sóc+1 //悬挂系统+1
			if (OldWWeight >= 29)
			{
				WWeight = 0;
				Reason = "改装失败,超出限制29！";
				break;
			}
			else {
				WWeight = 1;
				Reason = "恭喜你改装成功！";
				NotifyUseItemResult(Client, RefitItemId, 1, RefitItemNum);
				break;
			}
		case 12380: //Động cơ+1 //引擎装置+1
			if (OldSpeedWeight >= 29)
			{
				SpeedWeight = 0;
				Reason = "改装失败,超出限制29！";
				break;
			}
			else {
				SpeedWeight = 1;
				Reason = "恭喜你改装成功！";
				NotifyUseItemResult(Client, RefitItemId, 1, RefitItemNum);
				break;
			}
		case 12381: //Đánh lửa-1
			if (OldSJetWeight == 0)
			{
				SJetWeight = 0;
				Reason = "改装失败,已经没有可以改装的了.";
				break;
			}
			else
			{
				SJetWeight = -1;
				Reason = "恭喜你改装成功！";
				NotifyUseItemResult(Client, RefitItemId, 1, RefitItemNum);
				break;
			}
		case 12382: //Bơm khí-1
			if (OldAccuWeight == 0)
			{
				AccuWeight = 0;
				Reason = "改装失败,已经没有可以改装的了.";
				break;
			}
			else {
				AccuWeight = 0;
				Reason = "恭喜你改装成功！";
				NotifyUseItemResult(Client, RefitItemId, 1, RefitItemNum);
				break;
			}
		case 12383: //Nhiên liệu-1
			if (OldJetWeight == 0)
			{
				JetWeight = 0;
				Reason = "改装失败,已经没有可以改装的了.";
				break;
			}
			else {
				JetWeight = -1;
				Reason = "恭喜你改装成功！";
				NotifyUseItemResult(Client, RefitItemId, 1, RefitItemNum);
				break;
			}
		case 12384: //Giảm Sóc-1
			if (OldWWeight == 0)
			{
				WWeight = 0;
				Reason = "改装失败,已经没有可以改装的了.";
				break;
			}
			else {
				WWeight = -1;
				Reason = "恭喜你改装成功！";
				NotifyUseItemResult(Client, RefitItemId, 1, RefitItemNum);
				break;
			}
		case 12385: //Động cơ-1
			if (OldSpeedWeight == 0)
			{
				SpeedWeight = 0;
				Reason = "改装失败,已经没有可以改装的了.";
				break;
			}
			else {
				SpeedWeight = -1;
				Reason = "恭喜你改装成功！";
				NotifyUseItemResult(Client, RefitItemId, 1, RefitItemNum);
				break;
			}
		case 12386: //Đánh lửa+2
			if (OldSJetWeight >= 28)
			{
				SJetWeight = 0;
				Reason = "改装失败,超出限制29！";
				break;
			}
			else {
				SJetWeight = 2;
				Reason = "恭喜你改装成功！";
				NotifyUseItemResult(Client, RefitItemId, 1, RefitItemNum);
				break;
			}
		case 12387: //Bơm khí+2
			if (OldAccuWeight >= 28)
			{
				AccuWeight = 0;
				Reason = "改装失败,超出限制29！";
				break;
			}
			else
			{
				AccuWeight = 2;
				Reason = "恭喜你改装成功！";
				NotifyUseItemResult(Client, RefitItemId, 1, RefitItemNum);
				break;
			}
		case 12388: //Nhiên liệu+2
			if (OldJetWeight >= 28)
			{
				JetWeight = 0;
				Reason = "改装失败,超出限制29！";
				break;
			}
			else {
				JetWeight = 2;
				Reason = "恭喜你改装成功！";
				NotifyUseItemResult(Client, RefitItemId, 1, RefitItemNum);
				break;
			}
		case 12389: //Giảm Sóc+2
			if (OldWWeight >= 28)
			{
				WWeight = 0;
				Reason = "改装失败,超出限制29！";
				break;
			}
			else {
				WWeight = 2;
				Reason = "恭喜你改装成功！";
				NotifyUseItemResult(Client, RefitItemId, 1, RefitItemNum);
				break;
			}
		case 12390: //Động cơ+2
			if (OldSpeedWeight >= 28)
			{
				SpeedWeight = 0;
				Reason = "改装失败,超出限制29！";
				break;
			}
			else {
				SpeedWeight = 2;
				Reason = "恭喜你改装成功！";
				NotifyUseItemResult(Client, RefitItemId, 1, RefitItemNum);
				break;
			}
		}

		sql = "UPDATE KartRefit SET RefitCout=RefitCout+1, SJetWeight=SJetWeight+?,AccuWeight=AccuWeight+?,JetWeight=JetWeight+?,WWeight=WWeight+?,SpeedWeight=SpeedWeight+?  WHERE Uin=? AND KartID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, SJetWeight);
			sqlite3_bind_int(stmt, 2, AccuWeight);
			sqlite3_bind_int(stmt, 3, JetWeight);
			sqlite3_bind_int(stmt, 4, WWeight);
			sqlite3_bind_int(stmt, 5, SpeedWeight);

			sqlite3_bind_int(stmt, 6, Client->Uin);
			sqlite3_bind_int(stmt, 7, KartItemId);
			result = sqlite3_step(stmt);
		}
		sqlite3_finalize(stmt);
		stmt = NULL;

		ResponseRefitKart(Client, KartItemId, Reason, SJetWeight, AccuWeight, JetWeight, WWeight, SpeedWeight);
	}
}
void ResponseRefitKart(ClientNode* Client, int KartItemId, const char* Reason, short inc_SJetWeight, short inc_AccuWeight, short inc_JetWeight, short inc_WWeight, short inc_SpeedWeight)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, 0); //Result
	Write32(p, Client->Uin); //Uin
	Write32(p, KartItemId); //KartItemId

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

	sql = "SELECT RefitCout,MaxFlags,WWeight,SpeedWeight,JetWeight,SJetWeight,AccuWeight,ShapeRefitCount,KartHeadRefitItemID,KartTailRefitItemID,KartFlankRefitItemID,KartTireRefitItemID  FROM KartRefit WHERE Uin=? AND KartID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, KartItemId);
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

	sqlite3_finalize(stmt);
	stmt = NULL;

	{ //Increasement

		BYTE* pIncreasement = p;
		Write16(pIncreasement, 0); //len

		Write32(pIncreasement, Client->Uin); //Uin
		Write32(pIncreasement, KartItemId); //KartId
		Write32(pIncreasement, 1); //RefitCout
		Write16(pIncreasement, 0); //MaxFlags
		Write16(pIncreasement, inc_WWeight); //WWeight
		Write16(pIncreasement, inc_SpeedWeight); //SpeedWeight
		Write16(pIncreasement, inc_JetWeight); //JetWeight
		Write16(pIncreasement, inc_SJetWeight); //SJetWeight
		Write16(pIncreasement, inc_AccuWeight); //AccuWeight
		Write32(pIncreasement, 0); //ShapeRefitCount
		Write32(pIncreasement, 0); //KartHeadRefitItemID
		Write32(pIncreasement, 0); //KartTailRefitItemID
		Write32(pIncreasement, 0); //KartFlankRefitItemID
		Write32(pIncreasement, 0); //KartTireRefitItemID
#ifndef ZingSpeed
		Write32(pIncreasement, 0); //SecondRefitCount
		Write16(pIncreasement, 0); //Speed2Weight
		Write16(pIncreasement, 0); //DriftVecWeight
		Write16(pIncreasement, 0); //AdditionalZSpeedWeight
		Write16(pIncreasement, 0); //AntiCollisionWeight
		Write16(pIncreasement, 0); //LuckyValue
		Write16(pIncreasement, 0); //RefitLuckyValueMaxWeight
		Write32(pIncreasement, 0); //ShapeSuitID
		Write8(pIncreasement, 0); //LegendSuitLevel
		Write32(pIncreasement, 0); //LegendSuitLevelChoice
		Write32(pIncreasement, 0); //ShapeLegendSuitID
#endif

		len = pIncreasement - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ //CurAttr
		BYTE* pCurAttr = p;
		Write16(pCurAttr, 0); //len

		Write32(pCurAttr, Client->Uin); //Uin
		Write32(pCurAttr, KartItemId); //KartId
		Write32(pCurAttr, RefitCout); //RefitCout
		Write16(pCurAttr, MaxFlags); //MaxFlags
		Write16(pCurAttr, WWeight); //WWeight
		Write16(pCurAttr, SpeedWeight); //SpeedWeight
		Write16(pCurAttr, JetWeight); //JetWeight
		Write16(pCurAttr, SJetWeight); //SJetWeight
		Write16(pCurAttr, AccuWeight); //AccuWeight
		Write32(pCurAttr, ShapeRefitCount); //ShapeRefitCount
		Write32(pCurAttr, KartHeadRefitItemID); //KartHeadRefitItemID
		Write32(pCurAttr, KartTailRefitItemID); //KartTailRefitItemID
		Write32(pCurAttr, KartFlankRefitItemID); //KartFlankRefitItemID
		Write32(pCurAttr, KartTireRefitItemID); //KartTireRefitItemID
#ifndef ZingSpeed
		Write32(pCurAttr, 0); //SecondRefitCount
		Write16(pCurAttr, 0); //Speed2Weight
		Write16(pCurAttr, 0); //DriftVecWeight
		Write16(pCurAttr, 0); //AdditionalZSpeedWeight
		Write16(pCurAttr, 0); //AntiCollisionWeight
		Write16(pCurAttr, 0); //LuckyValue
		Write16(pCurAttr, 0); //RefitLuckyValueMaxWeight
		Write32(pCurAttr, 0); //ShapeSuitID
		Write8(pCurAttr, 0); //LegendSuitLevel
		Write32(pCurAttr, 0); //LegendSuitLevelChoice
		Write32(pCurAttr, 0); //ShapeLegendSuitID
#endif
		len = pCurAttr - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 500); //ExpAward
	Write32(p, 250); //MoneyAward

	len = strlen(Reason);
	Write16(p, (WORD)len); //ReasonLen
	memcpy(p, Reason, len);
	p += len;

#ifndef ZingSpeed
	Write16(p, 0); //RefitTimes
	Write16(p, 0); //ResponseIdx
	Write8(p, 0); //LastIdx
	Write32(p, 0); //LuckyValue
#endif

	len = p - buf;
	SendToClient(Client, 174, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}


void RequestModifyPlayerSignature(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);

	char Signature[MaxSignature] = {};
	memcpy(Signature, p, MaxSignature);
	p += MaxSignature;

	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

	sql = "UPDATE BaseInfo SET Signature=? WHERE Uin=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, Signature, strnlen_s(Signature, MaxSignature), SQLITE_TRANSIENT);
		sqlite3_bind_int(stmt, 2, Client->Uin);
		result = sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;


	ResponseModifyPlayerSignature(Client, Signature);
}

void ResponseModifyPlayerSignature(ClientNode* Client, char* Signature)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write8(p, 0); //ReasonLen

	//Signature[]
	memcpy(p, Signature, MaxSignature);
	p += MaxSignature;


	len = p - buf;
	SendToClient(Client, 376, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestFindPlayerByQQ(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body;;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);

	char NickName[MaxNickName] = {};
	memcpy(NickName, p, MaxNickName);
	p += MaxNickName;

	UINT DstUin = Read32(p);
	UINT DstNobleID = Read32(p);
	UCHAR Contex = Read8(p);
#ifndef ZingSpeed
	UCHAR AreaIdDst = Read8(p);
	UCHAR IgnoreByQQ2Msg = Read8(p);
	UCHAR SimpleInfo = Read8(p);
#endif

	ResponseFindPlayerByQQ(Client, DstUin);
}

void ResponseFindPlayerByQQ(ClientNode* Client, UINT DstUin)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;
	UINT KartItemId = 0;

	Write16(p, 0); //ResultID
	{ //PlayerInfo
		BYTE* pPlayerInfo = p;
		Write16(pPlayerInfo, 0); //len

		Write32(pPlayerInfo, DstUin); //Uin
		Write32(pPlayerInfo, 0); //Identity
		Write16(pPlayerInfo, 0); //PlayerID

		WritePlayerDBBaseInfo(pPlayerInfo, DstUin);
		WritePlayerGuildInfo(pPlayerInfo, DstUin);
		KartItemId = WriteEquipedItem(pPlayerInfo, DstUin);

		Write32(pPlayerInfo, 0); //MasterPoint
		Write32(pPlayerInfo, 0); //TotalGuildProsperity

		Write16(pPlayerInfo, 0x0107); //VipFlag
		Write16(pPlayerInfo, 0); //VipGrowRate

		Write8(pPlayerInfo, 0); //AppellationNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //Record
			BYTE* pRecord = pPlayerInfo;
			Write16(pRecord, 0); //len

			Write8(pRecord, 0); //Type
			Write8(pRecord, 0); //Level
			Write8(pRecord, 0); //Status
			Write8(pRecord, 0); //Difficulty
			Write32(pRecord, 0); //MapId
			Write32(pRecord, 0); //Value

			len = pRecord - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		*/


		const char* sql = NULL;
		sqlite3_stmt* stmt = NULL;
		int result;

		int Level1 = 6;
		int Level2 = 0;
		int Level3 = 0;
		int Level4 = 0;
		int Level5 = 0;

		sql = "SELECT VipLevel1,VipLevel2,VipLevel3,VipLevel4,VipLevel5 FROM Player WHERE Uin = ?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, DstUin);
			result = sqlite3_step(stmt);
			if (result == SQLITE_ROW)
			{
				Level1 = sqlite3_column_int(stmt, 0);//皇族
				Level2 = sqlite3_column_int(stmt, 1);//情侣
				Level3 = sqlite3_column_int(stmt, 2);//花园
				Level4 = sqlite3_column_int(stmt, 3);//魅力
				Level5 = sqlite3_column_int(stmt, 4);//钻皇
				//printf("VipLevel：%d\n", Level);
			}
			else
			{
				Level1 = 6;
				Level2 = 0;
				Level3 = 0;
				Level4 = 0;
				Level5 = 0;
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;









		{//NobleInfo
			BYTE* pNobleInfo = pPlayerInfo;
			Write16(pNobleInfo, 0); //len

			Write32(pNobleInfo, DstUin); //NobleID
			Write8(pNobleInfo, Level1); //NobleLevel
			Write32(pNobleInfo, 5401); //NoblePoint
			Write32(pNobleInfo, 0); //NobleLeftDays

			len = pNobleInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}

		Write8(pPlayerInfo, 0); //HasCarryWizard
		/*
		 //CarryWizardInfo
			BYTE* pCarryWizardInfo = pPlayerInfo;
			Write16(pCarryWizardInfo, 0); //len

			Write16(pCarryWizardInfo, 0); //WizardID
			WriteString(pCarryWizardInfo, 0); //NickName[]
			Write16(pCarryWizardInfo, 0); //WizardType
			Write8(pCarryWizardInfo, 0); //Order
			Write8(pCarryWizardInfo, 0); //EvolutionBranch
			Write8(pCarryWizardInfo, 0); //IsHuanHua

			len = pCarryWizardInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		
		*/


		 {//GuildVipBaseInfo
			BYTE* pGuildVipBaseInfo = pPlayerInfo;
			Write16(pGuildVipBaseInfo, 0); //len

			Write8(pGuildVipBaseInfo, Level5); //GuildVipLevel
			Write32(pGuildVipBaseInfo, 6571); //GuildVipPoint

			len = pGuildVipBaseInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		 }


		Write8(pPlayerInfo, 0); //HasLDMInfo
		Write8(pPlayerInfo, 1); //HasLoverVip
		
		for (size_t i = 0; i < 1; i++)
		{ //LoverVipInfo
			BYTE* pLoverVipInfo = pPlayerInfo;
			Write16(pLoverVipInfo, 0); //len

			Write8(pLoverVipInfo, Level2); //LoverVipLevel
			Write32(pLoverVipInfo, 0); //LoverVipPoint
			Write8(pLoverVipInfo, 0); //GrowRate

			len = pLoverVipInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		

		Write8(pPlayerInfo, 0); //IsShowMounts
		Write8(pPlayerInfo, 1); //HasGarden
		for (size_t i = 0; i < 1; i++)
		{ //PersonalGardenBaseInfo
			BYTE* pPersonalGardenBaseInfo = pPlayerInfo;
			Write16(pPersonalGardenBaseInfo, 0); //len

			Write8(pPersonalGardenBaseInfo, Level3); //GardenLevel
			Write32(pPersonalGardenBaseInfo, 49876); //GardenPoint
			Write8(pPersonalGardenBaseInfo, 30); //GrowRate
			Write32(pPersonalGardenBaseInfo, 0); //GardenExpireTime
			{ //SimpleInfo
				BYTE* pSimpleInfo = pPersonalGardenBaseInfo;
				Write16(pSimpleInfo, 0); //len

				Write32(pSimpleInfo, 0); //WeekPopularity
				Write32(pSimpleInfo, 0); //TotalPopularity
				Write32(pSimpleInfo, 0); //LastUpdatePopularityTime
				Write8(pSimpleInfo, 0); //PrivateType

				len = pSimpleInfo - pPersonalGardenBaseInfo;
				Set16(pPersonalGardenBaseInfo, (WORD)len);
				pPersonalGardenBaseInfo += len;
			}

			len = pPersonalGardenBaseInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		

		Write8(pPlayerInfo, 1); //isConsumeVip
		Write32(pPlayerInfo, Level4); //ConsumeVipLevel
		Write32(pPlayerInfo, 0); //SearchTreasureNums
		Write32(pPlayerInfo, 0); //GetTreasureNums
		Write32(pPlayerInfo, 0); //ConsumeVipCharmVlaueOfMonth
		{ //EmperorInfo
			BYTE* pEmperorInfo = pPlayerInfo;
			Write16(pEmperorInfo, 0); //len

			Write8(pEmperorInfo, Level5); //EmperorLevel
			Write32(pEmperorInfo, 4566); //EmperorPoint
			Write32(pEmperorInfo, 0); //EmperorLeftDays
			Write8(pEmperorInfo, 0); //EmperorGrowRate

			len = pEmperorInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		{//EmperorOtherInfo
			BYTE* pEmperorOtherInfo = pPlayerInfo;
			Write16(pEmperorOtherInfo, 0); //len

			Write32(pEmperorOtherInfo, 0); //ExpiredTime
			Write8(pEmperorOtherInfo, 0); //ShowExpireTips

			len = pEmperorOtherInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		{ //ActivityInfo
			BYTE* pActivityInfo = pPlayerInfo;
			Write16(pActivityInfo, 0); //len

			Write32(pActivityInfo, 0); //TotalActivity
			Write32(pActivityInfo, 0); //ActivityLevel

			len = pActivityInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}

		Write8(pPlayerInfo, 0); //HaveWakedKartAttributeAddInfo
		/*
		 //WakeKartAttributeAddInfo
			BYTE* pWakeKartAttributeAddInfo = pPlayerInfo;
			Write16(pWakeKartAttributeAddInfo, 0); //len

			Write16(pWakeKartAttributeAddInfo, 0); //AccAddValue
			Write16(pWakeKartAttributeAddInfo, 0); //SpeedAddValue
			Write16(pWakeKartAttributeAddInfo, 0); //JetAddValue
			Write16(pWakeKartAttributeAddInfo, 0); //SJetAddValue

			len = pWakeKartAttributeAddInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		    */
		

		Write8(pPlayerInfo, 0); //HaveLoverBabyInfo
		/*
		 //FindLoverBabyInfo
			BYTE* pFindLoverBabyInfo = pPlayerInfo;
			Write16(pFindLoverBabyInfo, 0); //len

			Write32(pFindLoverBabyInfo, 0); //BabyStatus
			Write32(pFindLoverBabyInfo, 0); //LoverBabyItemID
			Write32(pFindLoverBabyInfo, 0); //LoverBabyID
			Write32(pFindLoverBabyInfo, 0); //BabyGrowLevel
			Write32(pFindLoverBabyInfo, 0); //BabyStrengthLevel
			Write16(pFindLoverBabyInfo, 0); //LoverBabyStat
			Write16(pFindLoverBabyInfo, 0); //LoverBabyEquipStat
			Write16(pFindLoverBabyInfo, 0); //LoverBabyTransferdStatus

			len = pFindLoverBabyInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		
		*/

		{//GansterScoreInfo
			BYTE* pGansterScoreInfo = pPlayerInfo;
			Write16(pGansterScoreInfo, 0); //len

			Write32(pGansterScoreInfo, 0); //GansterSeasonID
			Write32(pGansterScoreInfo, 0); //GansterScore
			Write32(pGansterScoreInfo, 0); //PoliceScore
			Write32(pGansterScoreInfo, 0); //TotalGansterScore

			len = pGansterScoreInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}

		Write32(pPlayerInfo, 0); //OlympicId
		Write32(pPlayerInfo, 0); //LastOlympicUpdateTime
		Write32(pPlayerInfo, 0); //NPCEliminateWinTimes
		{ //BorderInfo
			BYTE* pBorderInfo = pPlayerInfo;
			Write16(pBorderInfo, 0); //len

			Write32(pBorderInfo, 0); //SeasonID
			Write32(pBorderInfo, 0); //Zhanxun
			Write32(pBorderInfo, 0); //SeasonZhanxun

			len = pBorderInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		Write8(pPlayerInfo, 0); //SpecialActivityID
		Write32(pPlayerInfo, 0); //ThemeHouseCollectValue
		Write8(pPlayerInfo, 0); //HaveSecondLoverBabyInfo
		{ //SecondLoverBabyInfo
			BYTE* pSecondLoverBabyInfo = pPlayerInfo;
			Write16(pSecondLoverBabyInfo, 0); //len

			Write32(pSecondLoverBabyInfo, 0); //BabyStatus
			Write32(pSecondLoverBabyInfo, 0); //LoverBabyItemID
			Write32(pSecondLoverBabyInfo, 0); //LoverBabyID
			Write32(pSecondLoverBabyInfo, 0); //BabyGrowLevel
			Write32(pSecondLoverBabyInfo, 0); //BabyStrengthLevel
			Write16(pSecondLoverBabyInfo, 0); //LoverBabyStat
			Write16(pSecondLoverBabyInfo, 0); //LoverBabyEquipStat
			Write16(pSecondLoverBabyInfo, 0); //LoverBabyTransferdStatus

			len = pSecondLoverBabyInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		Write8(pPlayerInfo, 0); //EquipBaby
		Write8(pPlayerInfo, 0); //ActiveStatus
		Write32(pPlayerInfo, 0); //HelperLev

		Write8(pPlayerInfo, 0); //HasRankedMatchInfo
		/*
		//RankedMatchGradeInfo
			BYTE* pRankedMatchGradeInfo = pPlayerInfo;
			Write16(pRankedMatchGradeInfo, 0); //len

			Write32(pRankedMatchGradeInfo, 0); //GradeLevel
			Write32(pRankedMatchGradeInfo, 0); //ChildLevel
			Write32(pRankedMatchGradeInfo, 0); //GradeScore
			Write32(pRankedMatchGradeInfo, 0); //MaxGradeScore
			Write32(pRankedMatchGradeInfo, 0); //TotalScore

			len = pRankedMatchGradeInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		
		*/

		Write16(pPlayerInfo, 0); //PlayerSeasonInfoNums
		/*
		for (size_t i = 0; i < n; i++)
		{ //PlayerSeasonInfos
			BYTE* pPlayerSeasonInfos = pPlayerInfo;
			Write16(pPlayerSeasonInfos, 0); //len

			Write16(pPlayerSeasonInfos, 0); //SeasonID
			Write16(pPlayerSeasonInfos, 0); //GradeLevel
			Write8(pPlayerSeasonInfos, 0); //ChildLevel
			Write16(pPlayerSeasonInfos, 0); //HighGradeLevel
			Write8(pPlayerSeasonInfos, 0); //HighChildLevel
			Write8(pPlayerSeasonInfos, 0); //SeasonInfoProcesedTag
			Write8(pPlayerSeasonInfos, 0); //SeasonComputeInfoTipTag
			Write16(pPlayerSeasonInfos, 0); //SeasonInheritTag
			Write8(pPlayerSeasonInfos, 0); //SeasonInheritTipTag
			Write16(pPlayerSeasonInfos, 0); //SpeedGodNums
			Write32(pPlayerSeasonInfos, 0); //SpeedRoundNums
			Write32(pPlayerSeasonInfos, 0); //SpeedFinishRoundNums
			Write32(pPlayerSeasonInfos, 0); //TotalRankIndex
			Write32(pPlayerSeasonInfos, 0); //ContiFinishRoundNums
			Write32(pPlayerSeasonInfos, 0); //SpeedKillAllRoundNums
			Write32(pPlayerSeasonInfos, 0); //PropRoundNums
			Write32(pPlayerSeasonInfos, 0); //PropFinishRoundNums
			Write32(pPlayerSeasonInfos, 0); //ContiFinishPropRoundNums
			Write32(pPlayerSeasonInfos, 0); //PropKillAllRoundNums
			Write8(pPlayerSeasonInfos, 0); //LastRoundIndex
			Write32(pPlayerSeasonInfos, 0); //OpenDate
			Write32(pPlayerSeasonInfos, 0); //CloseDate
			Write32(pPlayerSeasonInfos, 0); //HistroyContFinishRoundNums

			len = pPlayerSeasonInfos - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		*/

		Write8(pPlayerInfo, 0); //IsHelperAppOnline
		Write32(pPlayerInfo, 0); //RankedMatchSpeedGodNums
		Write8(pPlayerInfo, 0); //EquipSealType

		Write8(pPlayerInfo, 0); //HasHuanLingChangeInfo
		/*
		{ //HuanLingSuitInfo
			BYTE* pHuanLingSuitInfo = pPlayerInfo;
			Write16(pHuanLingSuitInfo, 0); //len

			Write32(pHuanLingSuitInfo, 0); //SuitID
			Write8(pHuanLingSuitInfo, 0); //ChangeLevel

			len = pHuanLingSuitInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		*/

		Write8(pPlayerInfo, 0); //SpecialActivityIDNum
		Write8(pPlayerInfo, 0); //PersonalPanelSelectRankedMatchFrameTag

		Write8(pPlayerInfo, 0); //SpecialActivityInfoNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //SpecialActivityInfo
			BYTE* pSpecialActivityInfo = pPlayerInfo;
			Write16(pSpecialActivityInfo, 0); //len

			Write8(pSpecialActivityInfo, 0); //ProfessionLicenseInfoNum
			for (size_t i = 0; i < n; i++)
			{ //ProfessionLicenseInfo
				BYTE* pProfessionLicenseInfo = pSpecialActivityInfo;
				Write16(pProfessionLicenseInfo, 0); //len

				Write8(pProfessionLicenseInfo, 0); //BigLevel
				Write8(pProfessionLicenseInfo, 0); //SmallLevel

				len = pProfessionLicenseInfo - pSpecialActivityInfo;
				Set16(pSpecialActivityInfo, (WORD)len);
				pSpecialActivityInfo += len;
			}

			len = pSpecialActivityInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		*/
		Write8(pPlayerInfo, 0); //PersonalRankedMatchLevelShowTag

		Write8(pPlayerInfo, 0); //ItemNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //ItemOrder
			BYTE* pItemOrder = pPlayerInfo;
			Write16(pItemOrder, 0); //len

			Write32(pItemOrder, 0); //KartID
			Write32(pItemOrder, 0); //Order
			Write32(pItemOrder, 0); //BackGroundID

			len = pItemOrder - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		*/

		Write8(pPlayerInfo, 0); //ExRightFlagLen

		Write8(pPlayerInfo, 0); //EquipNiChangItemNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //NiChangPavilionEquipItemInfo
			BYTE* pNiChangPavilionEquipItemInfo = pPlayerInfo;
			Write16(pNiChangPavilionEquipItemInfo, 0); //len

			Write32(pNiChangPavilionEquipItemInfo, 0); //ItemId
			Write16(pNiChangPavilionEquipItemInfo, 0); //CurrentEquipColorationPlanId

			len = pNiChangPavilionEquipItemInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		*/

		len = pPlayerInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 1); //IsOnLine
	{ //PositionInfo
		BYTE* pPositionInfo = p;
		Write16(pPositionInfo, 0); //len

		Write8(pPositionInfo, 0); //ServerType
		Write16(pPositionInfo, 0); //Status
		Write32(pPositionInfo, 0); //ServerID
		{ //RoomInfo
			BYTE* pRoomInfo = pPositionInfo;
			Write16(pRoomInfo, 0); //len

			Write16(pRoomInfo, 0); //RoomID
			Write16(pRoomInfo, 0); //BaseGameMode
			Write16(pRoomInfo, 0); //SubGameMode
			Write8(pRoomInfo, 0); //CurrentPlayerNum
			Write8(pRoomInfo, 0); //TotalSeatNum
			Write8(pRoomInfo, 0); //Status
			Write32(pRoomInfo, 0); //MapID
			Write8(pRoomInfo, 0); //Flag

			//RoomName[]
			memset(pRoomInfo, 0, MaxRoomName);
			pRoomInfo += MaxRoomName;

			Write8(pRoomInfo, 0); //HasFBInfo
			/*
			{ //FBInfo
				BYTE* pFBInfo = pRoomInfo;
				Write16(pFBInfo, 0); //len

				Write8(pFBInfo, 0); //AvailableLev
				Write8(pFBInfo, 0); //Lev
				Write8(pFBInfo, 0); //SubLev
				Write32(pFBInfo, 0); //BossID
				Write8(pFBInfo, 0); //FBSeason

				len = pFBInfo - pRoomInfo;
				Set16(pRoomInfo, (WORD)len);
				pRoomInfo += len;
			}
			*/
#ifndef ZingSpeed
			Write8(pRoomInfo, 0); //SpeFlag
			Write32(pRoomInfo, 0); //RoomNo
			Write16(pRoomInfo, 0); //SvrId
			Write16(pRoomInfo, 0); //SceneID
			Write8(pRoomInfo, 0); //CurrentOBNum
			Write8(pRoomInfo, 0); //TotalOBNum
			Write8(pRoomInfo, 0); //SpecialActivityStatus
			Write8(pRoomInfo, 0); //AllowAutoStart
			Write32(pRoomInfo, 0); //Gender
			Write32(pRoomInfo, 0); //LocaleCode
			Write8(pRoomInfo, 0); //TalkRoomBGID
			Write8(pRoomInfo, 0); //SpecialActivityIDNum

			Write8(pRoomInfo, 0); //SpecialActivityInfoNum
			/*
			for (size_t i = 0; i < n; i++)
			{ //SpecialActivityInfo
				BYTE* pSpecialActivityInfo = pRoomInfo;
				Write16(pSpecialActivityInfo, 0); //len

				Write8(pSpecialActivityInfo, 0); //ProfessionLicenseInfoNum
				for (size_t i = 0; i < n; i++)
				{ //ProfessionLicenseInfo
					BYTE* pProfessionLicenseInfo = pSpecialActivityInfo;
					Write16(pProfessionLicenseInfo, 0); //len

					Write8(pProfessionLicenseInfo, 0); //BigLevel
					Write8(pProfessionLicenseInfo, 0); //SmallLevel

					len = pProfessionLicenseInfo - pSpecialActivityInfo;
					Set16(pSpecialActivityInfo, (WORD)len);
					pSpecialActivityInfo += len;
				}

				len = pSpecialActivityInfo - pRoomInfo;
				Set16(pRoomInfo, (WORD)len);
				pRoomInfo += len;
			}
			*/

			Write8(pRoomInfo, 0); //ValueNum
			//Write32(pRoomInfo, 0); //EnterRoomValue[]
			Write8(pRoomInfo, 0); //ParaNum
			//Write32(pRoomInfo, 0); //ParaList[]
			Write32(pRoomInfo, 0); //GSvrRoomNo

			//OwnerName[]
			memset(pRoomInfo, 0, MaxNickName);
			pRoomInfo += MaxNickName;
#endif
			len = pRoomInfo - pPositionInfo;
			Set16(pPositionInfo, (WORD)len);
			pPositionInfo += len;
		}
		Write8(pPositionInfo, 0); //StatusLen
		Write8(pPositionInfo, 0); //CanJoin

		len = pPositionInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); //ReasonLen
	{ //PlayerRelationInfo
		BYTE* pPlayerRelationInfo = p;
		Write16(pPlayerRelationInfo, 0); //len

		Write32(pPlayerRelationInfo, 0); //SrcUin
		Write32(pPlayerRelationInfo, 0); //RelationFlag
		Write32(pPlayerRelationInfo, 0); //RelationUin

		//RelationNickName[]
		memset(pPlayerRelationInfo,0, MaxNickName);
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
	{ //CurKartAttr
		BYTE* pCurKartAttr = p;
		Write16(pCurKartAttr, 0); //len


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
		//int OldShapeSuitID = 0;

		sql = "SELECT RefitCout,MaxFlags,WWeight,SpeedWeight,JetWeight,SJetWeight,AccuWeight,ShapeRefitCount,KartHeadRefitItemID,KartTailRefitItemID,KartFlankRefitItemID,KartTireRefitItemID  FROM KartRefit WHERE Uin=? AND KartID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, Client->Uin);
			sqlite3_bind_int(stmt, 2, KartItemId);
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
				//OldShapeSuitID = sqlite3_column_int(stmt, 12);
			}
		}
		else
		{
			printf("%s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
		stmt = NULL;

		Write32(pCurKartAttr, DstUin); //Uin
		Write32(pCurKartAttr, KartItemId); //KartId
		Write32(pCurKartAttr, RefitCout); //RefitCout
		Write16(pCurKartAttr, MaxFlags); //MaxFlags
		Write16(pCurKartAttr, WWeight); //WWeight
		Write16(pCurKartAttr, SpeedWeight); //SpeedWeight
		Write16(pCurKartAttr, JetWeight); //JetWeight
		Write16(pCurKartAttr, SJetWeight); //SJetWeight
		Write16(pCurKartAttr, AccuWeight); //AccuWeight
		Write32(pCurKartAttr, ShapeRefitCount); //ShapeRefitCount
		Write32(pCurKartAttr, KartHeadRefitItemID); //KartHeadRefitItemID
		Write32(pCurKartAttr, KartTailRefitItemID); //KartTailRefitItemID
		Write32(pCurKartAttr, KartFlankRefitItemID); //KartFlankRefitItemID
		Write32(pCurKartAttr, KartTireRefitItemID); //KartTireRefitItemID
		{ //KartRefitExInfo
			BYTE* pKartRefitExInfo = pCurKartAttr;
			Write16(pKartRefitExInfo, 0); //len

			Write8(pKartRefitExInfo, 0); //SpeedRefitStar
			Write8(pKartRefitExInfo, 0); //JetRefitStar
			Write8(pKartRefitExInfo, 0); //SJetRefitStar
			Write8(pKartRefitExInfo, 0); //AccuRefitStar
			Write8(pKartRefitExInfo, 0); //SpeedAddRatio
			Write8(pKartRefitExInfo, 0); //JetAddRatio
			Write8(pKartRefitExInfo, 0); //SJetAddRatio
			Write8(pKartRefitExInfo, 0); //AccuAddRatio

			len = pKartRefitExInfo - pCurKartAttr;
			Set16(pCurKartAttr, (WORD)len);
			pCurKartAttr += len;
		}
#ifndef ZingSpeed
		Write32(pCurKartAttr, 0); //SecondRefitCount
		Write16(pCurKartAttr, 0); //Speed2Weight
		Write16(pCurKartAttr, 0); //DriftVecWeight
		Write16(pCurKartAttr, 0); //AdditionalZSpeedWeight
		Write16(pCurKartAttr, 0); //AntiCollisionWeight
		Write16(pCurKartAttr, 0); //LuckyValue
		Write16(pCurKartAttr, 0); //RefitLuckyValueMaxWeight
		Write32(pCurKartAttr, 0); //ShapeSuitID
		Write8(pCurKartAttr, 0); //LegendSuitLevel
		Write32(pCurKartAttr, 0); //LegendSuitLevelChoice
		Write32(pCurKartAttr, 0); //ShapeLegendSuitID
#endif
		len = pCurKartAttr - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); //PetNum

	//for (size_t i = 0; i < 1; i++)
	//{ //PetInfo
	//	BYTE* pPetInfo = p;
	//	Write16(pPetInfo, 0); //len

	//	Write32(pPetInfo, 0); //Uin
	//	Write32(pPetInfo, 0); //PetId
	//	//WriteString(pPetInfo, 0); //PetName[]
	//	Write8(pPetInfo, 0); //IsProper
	//	Write16(pPetInfo, 0); //Level
	//	Write8(pPetInfo, 0); //Status
	//	Write16(pPetInfo, 0); //PetSkillNum
	//	for (size_t i = 0; i < 1; i++)
	//	{ //PetSkillList
	//		BYTE* pPetSkillList = pPetInfo;
	//		Write16(pPetSkillList, 0); //len

	//		Write16(pPetSkillList, 0); //SkillID
	//		Write8(pPetSkillList, 0); //Status
	//		Write8(pPetSkillList, 0); //Active
	//		Write32(pPetSkillList, 0); //Value

	//		len = pPetSkillList - pPetInfo;
	//		Set16(pPetInfo, (WORD)len);
	//		pPetInfo += len;
	//	}
	//	Write32(pPetInfo, 0); //StrengLevel
	//	Write8(pPetInfo, 0); //TopStatus
	//	{ //CommonInfo
	//		BYTE* pCommonInfo = pPetInfo;
	//		Write16(pCommonInfo, 0); //len

	//		Write32(pCommonInfo, 0); //HuanHuaLevel
	//		Write8(pCommonInfo, 0); //CanHuanHua

	//		len = pCommonInfo - pPetInfo;
	//		Set16(pPetInfo, (WORD)len);
	//		pPetInfo += len;
	//	}
	//	Write16(pPetInfo, 0); //PKStar
	//	Write32(pPetInfo, 0); //PetSkinID

	//	len = pPetInfo - p;
	//	Set16(p, (WORD)len);
	//	p += len;
	//}
	

	Write8(p, 1); //IsInTopList
	{ //KartStoneGrooveInfo
		BYTE* pKartStoneGrooveInfo = p;
		Write16(pKartStoneGrooveInfo, 0); //len

		Write32(pKartStoneGrooveInfo, 0); //KartID
		Write32(pKartStoneGrooveInfo, 0); //StoneGrooveNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //StoneGrooveInfo
			BYTE* pStoneGrooveInfo = pKartStoneGrooveInfo;
			Write16(pStoneGrooveInfo, 0); //len

			Write32(pStoneGrooveInfo, 0); //StoneUseOccaType
			Write32(pStoneGrooveInfo, 0); //SkillStoneID

			len = pStoneGrooveInfo - pKartStoneGrooveInfo;
			Set16(pKartStoneGrooveInfo, (WORD)len);
			pKartStoneGrooveInfo += len;
		}
		*/

		len = pKartStoneGrooveInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); //Contex

	{ //PlayerWlVisibleInfo
		BYTE* pPlayerWlVisibleInfo = p;
		Write16(pPlayerWlVisibleInfo, 0); //len

		Write32(pPlayerWlVisibleInfo, 0); //WonderLandID

		//WonderLandName[]
		memset(pPlayerWlVisibleInfo, 0, MaxNickName);
		pPlayerWlVisibleInfo += MaxNickName;

		Write16(pPlayerWlVisibleInfo, 0); //WonderLandDuty
		Write8(pPlayerWlVisibleInfo, 0); //SubType
		Write8(pPlayerWlVisibleInfo, 0); //GuildGrade

		len = pPlayerWlVisibleInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 0); //SkateCoin
	Write32(p, 0); //MaxDayPveScore
	Write32(p, 0); //MaxHistoryPveScore

	{ //EquipedCarBattleModeSkillInfoList
		BYTE* pEquipedCarBattleModeSkillInfoList = p;
		Write16(pEquipedCarBattleModeSkillInfoList, 0); //len

		Write8(pEquipedCarBattleModeSkillInfoList, 0); //SkillNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //SkillInfo
			BYTE* pSkillInfo = pEquipedCarBattleModeSkillInfoList;
			Write16(pSkillInfo, 0); //len

			Write16(pSkillInfo, 0); //SkillId
			Write8(pSkillInfo, 0); //SkillLevel

			len = pSkillInfo - pEquipedCarBattleModeSkillInfoList;
			Set16(pEquipedCarBattleModeSkillInfoList, (WORD)len);
			pEquipedCarBattleModeSkillInfoList += len;
		}
		*/

		len = pEquipedCarBattleModeSkillInfoList - p;
		Set16(p, (WORD)len);
		p += len;
	}
#ifndef ZingSpeed
	Write8(p, 0); //HasCCVisibleInfo
	/*
	{ //ChumCircleInfo
		BYTE* pChumCircleInfo = p;
		Write16(pChumCircleInfo, 0); //len

		Write32(pChumCircleInfo, 0); //ChumCircleID

		len = pChumCircleInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	Write8(p, 0); //HasCheerAddition
	/*
	{ //CheerAdditon
		BYTE* pCheerAdditon = p;
		Write16(pCheerAdditon, 0); //len

		Write16(pCheerAdditon, 0); //ExpAddition
		Write16(pCheerAdditon, 0); //WageAddition

		len = pCheerAdditon - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	Write32(p, 0); //LoveValue
	Write16(p, 0); //CanEnterChannelFalg[]
	Write8(p, 0); //SpecialActivityID
	Write32(p, 0); //ThemeHouseDressDegreeValue
	Write8(p, 0); //AreaIdDst
	Write8(p, 0); //SpecialActivityIDNum
	Write8(p, 0); //SimpleInfo
#endif

	len = p - buf;
	SendToClient(Client, 122, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestGetAchieveList(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT DstUin = Read32(p);

	ResponseGetAchieveList(Client, DstUin);
}

void ResponseGetAchieveList(ClientNode* Client, UINT DstUin)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write32(p, DstUin); //DstUin

	Write16(p, 0); //Result
	Write16(p, 1); //AchieveNum
	for (size_t i = 0; i < 1; i++)
	{ //Achieves
		BYTE* pAchieves = p;
		Write16(pAchieves, 0); //len

		Write16(pAchieves, 7937); //ID
		Write8(pAchieves, 0); //ShowType
		Write8(pAchieves, 0); //LogicType
		Write8(pAchieves, 0); //Status
		Write16(pAchieves, 0); //Progress
		Write16(pAchieves, 0); //TotalProgress
		Write32(pAchieves, 0); //FinTime
		Write16(pAchieves, 0); //AchieveValue
		Write32(pAchieves, 0); //AchieveFlagType

		len = pAchieves - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 373, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

//幸运币未写完
void RequestGetLuckAwardList(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int ExchangeID = Read32(p);
	printf("ExchangeId:%d\n", ExchangeID);
	ItemInfo items[16];
	for (size_t i = 0; i < 16; i++)
	{
		items[i].ItemID = 88866;
		items[i].ItemNum = 1;
		items[i].AvailPeriod = -1;
		items[i].ItemType = 1;
		items[i].Status = false;
	}
	ResponseGetLuckAwardList(Client, Time, ExchangeID, items);
}

void ResponseGetLuckAwardList(ClientNode* Client,UINT Time,int ExchangeID,ItemInfo* items)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write32(p, Time); //Time
	Write32(p, ExchangeID); //ExchangeID
	Write8(p, 16); //AwardNum
	for (size_t i = 0; i < 16; i++)
	{ //LuckAwardList
		BYTE* pLuckAwardList = p;
		Write16(pLuckAwardList, 0); //len

		Write8(pLuckAwardList, 7); //AwardType
		Write32(pLuckAwardList, items[i].ItemID); //AwardPara
		Write8(pLuckAwardList, i); //AwardPostion
		Write8(pLuckAwardList, 0); //AwardDesLen
		Write32(pLuckAwardList, items[i].ItemNum); //ItemNum
		Write32(pLuckAwardList, items[i].AvailPeriod); //AvailPeriod

		len = pLuckAwardList - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 172, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
//todo 160  幸运币抽奖结果 未写


//传奇皮肤传递数据
void RequestSaveShapeRefit(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int KartItemID = Read32(p);
	UCHAR RefitItemNum = Read8(p);
	int KartHeadRefitItemID = Read32(p);
	int KartTailRefitItemID = Read32(p);
	int KartFlankRefitItemID = Read32(p);
	int KartTireRefitItemID = Read32(p);

	int ShapeSuitID = Read32(p);
	//printf("ShapeSuitID值：%d\n", ShapeSuitID);
	int RefitItemID[4];

	RefitItemID[0] = KartHeadRefitItemID;
	RefitItemID[1] = KartTailRefitItemID;
	RefitItemID[2] = KartFlankRefitItemID;
	RefitItemID[3] = KartTireRefitItemID;

	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	int oldKartHeadRefitItemID = 0;
	int oldKartTailRefitItemID = 0;
	int oldKartFlankRefitItemID = 0;
	int oldKartTireRefitItemID = 0;

	//查询道具数量
	sql = "SELECT KartHeadRefitItemID,KartTailRefitItemID,KartFlankRefitItemID,KartTireRefitItemID  FROM KartRefit  WHERE Uin=? AND KartID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Uin);
		sqlite3_bind_int(stmt, 2, KartItemID);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			oldKartHeadRefitItemID = sqlite3_column_int(stmt, 1);
			oldKartTailRefitItemID = sqlite3_column_int(stmt, 2);
			oldKartFlankRefitItemID = sqlite3_column_int(stmt, 3);
			oldKartTireRefitItemID = sqlite3_column_int(stmt, 4);

		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	/*if (oldKartHeadRefitItemID!=0)
	{
		PlayerDB_UpdateKartRefitItemStatus(Uin, oldKartHeadRefitItemID, 0);
	}
	if (oldKartTailRefitItemID != 0)
	{
		PlayerDB_UpdateKartRefitItemStatus(Uin, oldKartTailRefitItemID, 0);
	}
	if (oldKartFlankRefitItemID != 0)
	{
		PlayerDB_UpdateKartRefitItemStatus(Uin, oldKartFlankRefitItemID, 0);
	}
	if (oldKartTireRefitItemID != 0)
	{
		PlayerDB_UpdateKartRefitItemStatus(Uin, oldKartTireRefitItemID, 0);
	}*/
	// 更新新数据
	sql = "UPDATE KartRefit SET KartHeadRefitItemID=?,KartTailRefitItemID=?,KartFlankRefitItemID=?,KartTireRefitItemID=?  WHERE Uin=? AND KartID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, KartHeadRefitItemID);
		sqlite3_bind_int(stmt, 2, KartTailRefitItemID);
		sqlite3_bind_int(stmt, 3, KartFlankRefitItemID);
		sqlite3_bind_int(stmt, 4, KartTireRefitItemID);
		sqlite3_bind_int(stmt, 5, Uin);
		sqlite3_bind_int(stmt, 6, KartItemID);

		result = sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;


	

	/*if (KartHeadRefitItemID != 0)
	{
		PlayerDB_UpdateKartRefitItemStatus(Uin, KartHeadRefitItemID, 1);
	}
	if (KartTailRefitItemID != 0)
	{
		PlayerDB_UpdateKartRefitItemStatus(Uin, KartTailRefitItemID, 1);
	}
	if (KartFlankRefitItemID != 0)
	{
		PlayerDB_UpdateKartRefitItemStatus(Uin, KartFlankRefitItemID, 1);
	}
	if (KartTireRefitItemID != 0)
	{
		PlayerDB_UpdateKartRefitItemStatus(Uin, KartTireRefitItemID, 1);
	}*/
	ResponseSaveShapeRefit(Client, KartItemID, RefitItemNum, RefitItemID, ShapeSuitID);
}

///传奇皮肤快速装备
void ResponseSaveShapeRefit(ClientNode* Client, int KartItemID, int RefitItemNum, int RefitItemID[], int ShapeSuitID)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write8(p, 0); //ReasonLen
	Write32(p, KartItemID); //KartItemID
	{ //KartRefitExInfo
		BYTE* pKartRefitExInfo = p;
		Write16(pKartRefitExInfo, 0); //len

		Write8(pKartRefitExInfo, 0); //SpeedRefitStar
		Write8(pKartRefitExInfo, 0); //JetRefitStar
		Write8(pKartRefitExInfo, 0); //SJetRefitStar
		Write8(pKartRefitExInfo, 0); //AccuRefitStar
		Write8(pKartRefitExInfo, 0); //SpeedAddRatio
		Write8(pKartRefitExInfo, 0); //JetAddRatio
		Write8(pKartRefitExInfo, 0); //SJetAddRatio
		Write8(pKartRefitExInfo, 0); //AccuAddRatio

		len = pKartRefitExInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, RefitItemNum); //RefitItemNum
	Write32(p, RefitItemID[0]); //RefitItemID[]
	Write32(p, RefitItemID[1]); //RefitItemID[]
	Write32(p, RefitItemID[2]); //RefitItemID[]
	Write32(p, RefitItemID[3]); //RefitItemID[]

	Write32(p, ShapeSuitID); //ShapeSuitID
	{ //RefitClientInfo
		BYTE* pRefitClientInfo = p;
		Write16(pRefitClientInfo, 0); //len

		Write32(pRefitClientInfo, Client->Uin); //Uin
		Write32(pRefitClientInfo, KartItemID); //KartId
		Write32(pRefitClientInfo, 0); //RefitCout
		Write16(pRefitClientInfo, 0); //MaxFlags
		Write16(pRefitClientInfo, 0); //WWeight
		Write16(pRefitClientInfo, 0); //SpeedWeight
		Write16(pRefitClientInfo, 0); //JetWeight
		Write16(pRefitClientInfo, 0); //SJetWeight
		Write16(pRefitClientInfo, 0); //AccuWeight
		Write32(pRefitClientInfo, 0); //ShapeRefitCount
		Write32(pRefitClientInfo, RefitItemID[0]); //KartHeadRefitItemID
		Write32(pRefitClientInfo, RefitItemID[1]); //KartTailRefitItemID
		Write32(pRefitClientInfo, RefitItemID[2]); //KartFlankRefitItemID
		Write32(pRefitClientInfo, RefitItemID[3]); //KartTireRefitItemID
		{ //KartRefitExInfo
			BYTE* pKartRefitExInfo = pRefitClientInfo;
			Write16(pKartRefitExInfo, 0); //len

			Write8(pKartRefitExInfo, 0); //SpeedRefitStar
			Write8(pKartRefitExInfo, 0); //JetRefitStar
			Write8(pKartRefitExInfo, 0); //SJetRefitStar
			Write8(pKartRefitExInfo, 0); //AccuRefitStar
			Write8(pKartRefitExInfo, 0); //SpeedAddRatio
			Write8(pKartRefitExInfo, 0); //JetAddRatio
			Write8(pKartRefitExInfo, 0); //SJetAddRatio
			Write8(pKartRefitExInfo, 0); //AccuAddRatio

			len = pKartRefitExInfo - pRefitClientInfo;
			Set16(pRefitClientInfo, (WORD)len);
			pRefitClientInfo += len;
		}
		Write32(pRefitClientInfo, 0); //SecondRefitCount
		Write16(pRefitClientInfo, 0); //Speed2Weight
		Write16(pRefitClientInfo, 0); //DriftVecWeight
		Write16(pRefitClientInfo, 0); //AdditionalZSpeedWeight
		Write16(pRefitClientInfo, 0); //AntiCollisionWeight
		Write16(pRefitClientInfo, 0); //LuckyValue
		Write16(pRefitClientInfo, 0); //RefitLuckyValueMaxWeight
		int suitid = 0;
		if (ShapeSuitID == 127359) {
			suitid = 126537;
		}
		else if (ShapeSuitID == 127360) {
			suitid = 127365;
		}
		else if (ShapeSuitID == 120246) {
			suitid = 118729;
		}
		else if (ShapeSuitID == 126355) {
			suitid = 127437;
		}
		else if (ShapeSuitID == 123513) {
			suitid = 123238;
		}
		else {
			suitid = ShapeSuitID;
		}
		Write32(pRefitClientInfo, suitid); //ShapeSuitID
		Write8(pRefitClientInfo, 0); //LegendSuitLevel
		Write32(pRefitClientInfo, 0); //LegendSuitLevelChoice
		Write32(pRefitClientInfo, ShapeSuitID); //ShapeLegendSuitID

		len = pRefitClientInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 316, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}


void ResponseResetNameCard(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	ItemInfo Items[1]{};
	size_t ItemNum = 0;

	bool HasMoreInfo = false;
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	sql = "SELECT ItemID,ItemNum,AvailPeriod,Status,ObtainTime,OtherAttribute  FROM Item WHERE Uin = ?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Client->Uin);
	}
	else
	{
		sqlite3_finalize(stmt);
		stmt = NULL;
	}
	if (stmt)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			/*Items[ItemNum].ItemID = sqlite3_column_int(stmt, 0);
			Items[ItemNum].ItemNum = sqlite3_column_int(stmt, 1);
			Items[ItemNum].AvailPeriod = sqlite3_column_int(stmt, 2);
			Items[ItemNum].Status = sqlite3_column_int(stmt, 3);
			Items[ItemNum].ObtainTime = sqlite3_column_int(stmt, 4);
			Items[ItemNum].OtherAttribute = sqlite3_column_int(stmt, 5);
			Items[ItemNum].ItemType = 0;*/


			Items[0].ItemID = 10595;
			Items[0].ItemNum = sqlite3_column_int(stmt, 1);
			ItemNum = sqlite3_column_int(stmt, 1);
			Items[0].AvailPeriod = 0;
			Items[0].Status = false;
			Items[0].ObtainTime = Time;
			Items[0].OtherAttribute = 0;
			Items[0].ItemType = 0;




			ItemNum++;
			if (ItemNum >= 200)
			{
				HasMoreInfo = true;
				break;
			}
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;


	BYTE buf[8192]; BYTE* p = buf; size_t len;

	char buff[17];

	memset(buff, 0, 17);
	
	memcpy(buff, Body, 17);//转换完毕


	printf("将要修改的名称:%s\n", buff);
	string Reason = "";
//	bool repeat = msql.MMYSQL_isNameRepeat(buff);
//	if (repeat)
//	{
//#ifdef ZingSpeed
//		Reason = u8"对不起,您选择的名字已经存在,请重新输入!";
//#else
//		Reason = "对不起,您选择的名字已经存在,请重新输入!";
//#endif // ZingSpeed
//		ResponseStartGame(Client, Reason.c_str());
//		return;
//	}

	if (ItemNum < 1)
	{
		Reason = "你还没有更名卡哦!";
		ResponseStartGame(Client, Reason.c_str());
		return;
	}

	ResponseResetNameCard2(Client, Body, BodyLen);
	memset(buf, 0, 19);
	memcpy(p + 2, buff, strlen(buff));
	len = 19;

	SendToClient(Client, 133, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
		/*const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;*/
	//sql = "UPDATE BaseInfo SET NickName=? WHERE Uin=?;";

	//result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	//if (result == SQLITE_OK) {
	//	sqlite3_bind_text(stmt, 1, buff, strlen(buff), SQLITE_TRANSIENT);
	//	printf("buff值%s\n", buff);
	//	sqlite3_bind_int(stmt, 2, Client->Uin);
	//	result = sqlite3_step(stmt);
	//	
	//	//printf("ItemID:%d\n", aItemStatus[i2].ItemID);
	//}
	//sqlite3_finalize(stmt);
	//stmt = NULL;
}

void ResponseResetNameCard2(ClientNode* Client, BYTE* Body, size_t BodyLen)
{

	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	string Reason = "";

	BYTE buf[8192]; BYTE* p = buf; size_t len;

	char buff[17];

	memcpy(buff, Body, 17);

	memset(buf, 0, 19);
	memcpy(p + 2, buff, strlen(buff));
	len = 19;


	SendToClient(Client, 154, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
	

	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

#ifdef ZingSpeed
	Reason = u8"恭喜您改名成功,请重新登录游戏以便及时生效!";
#else
	Reason = "恭喜您改名成功,请重新登录游戏以便及时生效!";
#endif // ZingSpeed

	//没有刷新道具数量 其次在点击改名卡的时候应该检查有没有改名卡

	ItemInfo Items[1]{};
	ItemStatus NewStatus;

	size_t ItemNum = 0;
	bool HasMoreInfo = false;

	/*const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;*/
	sql = "SELECT ItemID,ItemNum,AvailPeriod,Status,ObtainTime,OtherAttribute  FROM Item WHERE Uin = ?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Client->Uin);
	}
	else
	{
		printf("%s\n", sqlite3_errmsg(PlayerDB));
		sqlite3_finalize(stmt);
		stmt = NULL;
	}
	if (stmt)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			/*Items[ItemNum].ItemID = sqlite3_column_int(stmt, 0);
			Items[ItemNum].ItemNum = sqlite3_column_int(stmt, 1);
			Items[ItemNum].AvailPeriod = sqlite3_column_int(stmt, 2);
			Items[ItemNum].Status = sqlite3_column_int(stmt, 3);
			Items[ItemNum].ObtainTime = sqlite3_column_int(stmt, 4);
			Items[ItemNum].OtherAttribute = sqlite3_column_int(stmt, 5);
			Items[ItemNum].ItemType = 0;*/


			Items[0].ItemID = 10595;
			Items[0].ItemNum = sqlite3_column_int(stmt, 1);
			Items[0].AvailPeriod = 0;
			Items[0].Status = false;
			Items[0].ObtainTime = Time;
			Items[0].OtherAttribute = 0;
			Items[0].ItemType = 0;




			ItemNum++;
			if (ItemNum >= 200)
			{
				HasMoreInfo = true;
				break;
			}
		}
	}


#ifdef ZingSpeed
	//ZingSpeed得用这个
	NotifyClientAddItem(Client, 1, Items);
	if (Items[0].Status)
	{
		NewStatus.ItemID = Items[0].ItemID;
		NewStatus.NewStatus = true;
		ResponseChangeItemStatus(Client, Items[0].ItemNum, &NewStatus);
	}
#else
	//这对于 ZingSpeed 好像不起作用
	NotifyLoginMoreInfo(Client, 1, Items, false);
	NotifyClientAddItem(Client, 1, Items);
	if (Items[0].Status)
	{
		NewStatus.ItemID = Items[0].ItemID;
		NewStatus.NewStatus = true;
		//ResponseChangeItemStatus(Client, Items[0].ItemNum, &NewStatus);
	}
#endif



	/*const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;*/
	//char Newname = NULL;

	sql = "UPDATE BaseInfo SET NickName=? WHERE Uin=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		//Newname = Utf8ToGbk(buff);
		//printf("Newname:%s\n", buff);
	    sqlite3_bind_text(stmt, 1, buff, strlen(buff), NULL);
		//Utf8ToGbk((char*)sqlite3_bind_text(stmt, 1, buff, strlen(buff), SQLITE_TRANSIENT));
		printf("已成功修改名称为:%s\n", buff);
		sqlite3_bind_int(stmt, 2, Client->Uin);
		result = sqlite3_step(stmt);

		//printf("ItemID:%d\n", aItemStatus[i2].ItemID);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	ResponseStartGame(Client, Reason.c_str());
}