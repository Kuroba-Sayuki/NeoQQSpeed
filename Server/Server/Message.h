#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef _DEBUG
#define DEBUG
#endif


#include "HPSocket/HPSocket.h"

#include "Utils.h"

#ifdef ZingSpeed
constexpr int MaxNickName = 51;
constexpr int MaxQQNickName = 60;
constexpr int MaxSignature = 128;

constexpr int MAX_MUCH_SEATNUMINROOM = 6;
constexpr int MAXNPCNUMINROOM = 6;
#else
constexpr int MaxNickName = 17;
constexpr int MaxQQNickName = 20;
constexpr int MaxSignature = 64;

constexpr int MAX_MUCH_SEATNUMINROOM = 20;
constexpr int MAXNPCNUMINROOM = 20;
#endif

struct PetInfo
{
	UINT ID;
	char Name[23];
	UINT Exp;
	UINT MaxExp;
	UINT Level;
	UINT Status;
	UINT PL;
};

struct ClientNode
{
	ITcpServer* Server;
	LONG_PTR BufferPtr;
	LONG_PTR BufferSize;
	LONG_PTR BufferOffset;
	int ConnID;
	BOOL IsLogin;
	DWORD Uin;
	BYTE Key[16];
	USHORT ServerID;
	UINT KartID;

	//Room
	USHORT RoomID;
	USHORT GameID;
	BOOL IsReady;

	//
	UINT MapID;
	int FinishTime;
	int TotalRound;
	int Round;
	int PassedCheckPoint;
	int EndCheckPoint;
	bool Loop;
	bool MapCompatibilityMode;

	//Dance
	int Scores;

	//Outdoor
	USHORT ChatRoomID;

	PetInfo* Pet;
};

struct ItemStatus
{
	UINT ItemID;
	BYTE NewStatus;
};

struct ItemInfo
{
	UINT ItemID;
	int ItemNum;
	int AvailPeriod;
	bool Status;
	int ObtainTime;
	int OtherAttribute;
	int ItemType;
};


enum ServerType
{
	Game,
	Dance,
	Relax
};

void SendToClient(ClientNode* Client, short MsgID, const BYTE* Data, DWORD Length, short SrcID, char SrcFE, short DstID, short MsgType, char Encrypt = true);
void OnRecvFromClient(ClientNode* Client, USHORT MsgID, BYTE* Body, size_t BodyLen);
void OnFullReceive(ClientNode* Client, BYTE* Data, size_t Length);


void InitDirServer(const char* Name, UINT IP, USHORT TcpPort, USHORT UdpPort);
int GetServerType(int ServerID);



void WritePlayerDBBaseInfo(BYTE*& p, UINT Uin);
void WritePlayerGuildInfo(BYTE*& p, UINT Uin);
UINT WriteEquipedItem(BYTE*& p, UINT Uin);
void WriteVisbleInfo(BYTE*& p, UINT Uin, char SeatID, short PlayerID);



ClientNode* GetClient(UINT Uin);
void AddClient(ClientNode* Client, UINT Uin);
void DelClient(UINT Uin);

//封包
void RequestGetWorldInfo(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseGetWorldInfo(ClientNode* Client, UINT ClientVersion, UINT MasterVersion, UINT SlaveVersion);
void RequestGetChatRoomList(ClientNode* Client, BYTE* Body, size_t BodyLen);



void RequestRegister2(ClientNode* Client, BYTE* Body, size_t BodyLen);


void RequestLogin(ClientNode* Client, BYTE* Body, size_t BodyLen);

void RequestLogout(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseLogout(ClientNode* Client);

#include "Room.h"


void RequestHello(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseHello(ClientNode* Client, UINT Uin, UINT Time);

void RequestPrepareReady(ClientNode* Client, BYTE* Body, size_t BodyLen);

void NotifyGameBegin(ClientNode* Client, ClientNode* Player[]);
void NotifyKartPhysParam(ClientNode* Client, ClientNode* Player[]);
void NotifyRaceBegin(ClientNode* Client);

void RequestReportCurrentInfo(ClientNode* Client, BYTE* Body, size_t BodyLen);

void NotifyCountDown(ClientNode* Client, UINT WinnerUin, bool WinnerNewRecord, UINT FinTime);
void NotifyRaceShow(ClientNode* Client);
void NotifyRaceOver(ClientNode* Client);
void NotifyGameOver(ClientNode* Client, USHORT LeaveGameType, BYTE ParaNum, int* ParaList);

void RequestChangeItemStatus(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseChangeItemStatus(ClientNode* Client, USHORT ItemNum, ItemStatus* ItemStatus);

void RequestGetLicenseInfo(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseGetLicenseInfo(ClientNode* Client);

void RequestGetSystemTaskList(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseGetSystemTaskList(ClientNode* Client, UINT TaskVersion);

void RequestChangeGender(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseChangeGender(ClientNode* Client, BYTE NewGender, BYTE UseType);

void RequestSkipFreshTask(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseSkipFreshTask(ClientNode* Client);

void RequestStartShadowChallenge(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseStartShadowChallenge(ClientNode* Client);
void RequestEndShadowChallenge(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseEndShadowChallenge(ClientNode* Client, int ResultID);

void RequestFishLogin(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseFishLogin(ClientNode* Client);

void RequestGetPlayerEventCfg(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseGetPlayerEventCfg(ClientNode* Client);
void RequestGetPlayerEvent(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseGetPlayerEvent(ClientNode* Client, BYTE TargetType, BYTE TimeType);

void NotifyMsgBox(ClientNode* Client);


void RequestTransferByTCP(ClientNode* Client, BYTE* Body, size_t BodyLen);
void NotifyTranferByTCP(ClientNode* Client, UINT SrcUin, USHORT SrcPlayerID, int Seq, BYTE* Buff, int Bufflen);

void RequestGetTaskList(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseGetTaskList(ClientNode* Client);

void RequestUseProp2(ClientNode* Client, BYTE* Body, size_t BodyLen);

void RequestChangeModel(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseChangeModel(ClientNode* Client, BYTE NewModel);
void NotifyChangeModel(ClientNode* Client, UINT Uin, BYTE NewModel);
//VIP类
void NotifyClientVipFlag(ClientNode* Client);






void NotifySvrConfig(ClientNode* Client);
void NotifySvrConfig2(ClientNode* Client);
void NotifySvrConfig3(ClientNode* Client);

void NotifySkillStoneKartInfo(ClientNode* Client);
void NotifyOtherKartStoneInfo(ClientNode* Client);


void RequestChangeUdpState(ClientNode* Client, BYTE* Body, size_t BodyLen);
void RequestTransferUDPOK(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseTransferUDPOK(ClientNode* Client);
//赛车宝石类
void RequestInlaySkillStone(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseInlaySkillStone(ClientNode* Client, int KartID, int StoneGrooveID, int SkillStoneID);




void RequestGetProp(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseGetProp(ClientNode* Client);

void NotifySpeed2Cfg(ClientNode* Client);

void RequestKartRefitInfo(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseKartRefitInfo(ClientNode* Client);

void RequestNewGetFriendList(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseNewGetFriendList(ClientNode* Client);

void RequestFizzInfo(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseFizzInfo(ClientNode* Client);

void RequestGetShoppingCarItemList(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseGetShoppingCarItemList(ClientNode* Client);

void RequestReplayOperate(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseReplayOperate(ClientNode* Client, UCHAR Cmd);

void RequestGetKeyTranslateInfoNew(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseGetKeyTranslateInfoNew(ClientNode* Client);

void RequestC2SGetKartAltasConfig(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseC2SGetKartAltasConfig(ClientNode* Client, UINT CfgVersion);

void RequestGetMaster(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseGetMaster(ClientNode* Client);

void RequestSSCOpenStatus(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseSSCOpenStatus(ClientNode* Client);

void RequestSwitchInterface(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseSwitchInterface(ClientNode* Client);

void RequestSecondGetKartRefitLimitCfg(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseSecondGetKartRefitLimitCfg(ClientNode* Client);

void RequestGetPrivilegeIntroInfo(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseGetPrivilegeIntroInfo(ClientNode* Client);

void RequestGetActivityCenterInfo(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseGetActivityCenterInfo(ClientNode* Client);

void NotifyTopUIItemInfo(ClientNode* Client);

void NotifyRedPointInfo(ClientNode* Client);

void RequestGetRandomTask(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseGetRandomTask(ClientNode* Client);

void RequestGetItemTimeLimtCfg(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponsetGetItemTimeLimtCfg(ClientNode* Client, UINT UpdateTime);

void RequestGetShapeRefitCfg(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseGetShapeRefitCfg(ClientNode* Client);

void RequestWeRelayCommonCfg(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseWeRelayCommonCfg(ClientNode* Client);

void RequestC2GSign3Operate(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseC2GSign3Operate(ClientNode* Client, UCHAR OperateType);

void NotifyClientAddItem(ClientNode* Client, UINT ItemNum, ItemInfo* Items);
void ResponseRegister(ClientNode* Client, const char* Nickname, BYTE  Gender, BYTE Country, UINT ItemNum, ItemInfo* Items);
void NotifyUpdateKartSkillStoneInfo(ClientNode* Client, int KartID);
void ResponseLogin(ClientNode* Client, UINT ClientVersion, UINT LoginType, UINT ItemNum, ItemInfo* Items, bool HasMoreInfo);
void NotifyLoginMoreInfo(ClientNode* Client, UINT ItemNum, ItemInfo* Items, bool HasNext);

void RequestDeleteOneItem(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseDeleteOneItem(ClientNode* Client, int ItemID, int ResultID, const char* Reason);

void RequestRefitKart(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseRefitKart(ClientNode* Client, int KartItemId, const char* Reason, short inc_SJetWeight, short inc_AccuWeight, short inc_JetWeight, short inc_WWeight, short inc_SpeedWeight);

void RequestHyperSpaceJumboGetCfg(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseHyperSpaceJumboGetCfg(ClientNode* Client, UCHAR CfgType);

void RequestTimerChallenge2ndMoreHallInfo(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseTimerChallenge2ndMoreHallInfo(ClientNode* Client);

void RequestPreGetCommonBoxAward(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponsePreGetCommonBoxAward(ClientNode* Client, USHORT Type);


void RequestAddToMatchQueue(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseAddToMatchQueue(ClientNode* Client, USHORT matchQueueType);

void NotifyMatchResult(ClientNode* Client, USHORT matchQueueType);

void RequestEnterMatchRoom(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseEnterMatchRoom(ClientNode* Client);



void RequestLeaveGame(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseLeaveGame(ClientNode* Client);

void RequestModifyPlayerSignature(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseModifyPlayerSignature(ClientNode* Client, char* Signature);

void RequestFindPlayerByQQ(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseFindPlayerByQQ(ClientNode* Client, UINT DstUin);

void RequestGetAchieveList(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseGetAchieveList(ClientNode* Client, UINT DstUin);

void NotifyRandRoomNameList(ClientNode* Client);

void NotifyTopListDesc(ClientNode* Client);
void RequestTopList(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseTopList(ClientNode* Client, USHORT ID, UCHAR Type);

void RequestChangeSkinStatus(ClientNode* Client, BYTE* Body, size_t BodyLen);

void RequestUseItem(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseUseItem(ClientNode* Client, int ItemID, UINT ItemNum, ItemInfo* Items,int giftType);
void NotifyUseItemResult(ClientNode* Client, int ItemID, int UseItemNum, int CurrentItemNum);





void RequestGetLuckAwardList(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseGetLuckAwardList(ClientNode* Client, UINT Time, int ExchangeID, ItemInfo* items);

//快速装备传奇皮肤
void RequestSaveShapeRefit(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseSaveShapeRefit(ClientNode* Client, int KartItemID, int RefitItemNum, int RefitItemID[], int ShapeSuitID);
//void ResponseSaveShapeRefit(ClientNode* Client, int KartID, int RefitItemNum, int RefitItemID[], int ShapeSuitID);
//void ResponseSaveShapeRefit(ClientNode* Client, int KartID, int RefitItemNum, int KartHeadRefitItemID, int KartTailRefitItemID, int KartFlankRefitItemID, int KartTireRefitItemID, int ShapeSuitID);
//void ResponseSaveShapeRefit(ClientNode* Client, int KartItemID, UINT Uin, int KartHeadRefitItemID, int KartTailRefitItemID, int KartFlankRefitItemID, int KartTireRefitItemID, int ShapeSuitID);

void ResponseResetNameCard(ClientNode* Client, BYTE* Body, size_t BodyLen);//响应修改名称
void ResponseResetNameCard2(ClientNode* Client, BYTE* Body, size_t BodyLen);
void NotifyMatchSkillStoneInfo(ClientNode* Client, BYTE* Body, size_t BodyLen);//宝石通知其他玩家
//赛车宝石镶嵌拆除
void RequestAddStoneGroove(ClientNode* Client, BYTE* Body, size_t BodyLen); //903 赛车宝石开孔
void ResponseAddStoneGroove(ClientNode* Client, int KartID, int CarveKnifeID, int StoneGrooveType); //903 赛车宝石开孔

void RequestRemoveSkillStone(ClientNode* Client, BYTE* Body, size_t BodyLen); //912赛车宝石拆除
void ResponseRemoveSkillStone(ClientNode* Client, int KartID, int StoneGrooveID, int RemovalToolID, int SkillStoneID); //912赛车宝石拆除


void RequestStartSingleTask(ClientNode* Client, BYTE* Body, size_t BodyLen);//紫钻练习场
void ResponseStartSingleTask(ClientNode* Client);//紫钻练习场

#include "Room.h"
#include "Dance.h"
#include "Outdoor.h"

void RequestBuyCommodity(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseKartRefitInfoMoreInfo(ClientNode* Client, size_t num);
#include "Pet.h";