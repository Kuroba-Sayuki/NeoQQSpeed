#include "Message.h"
#include "Database.h"

#include <stdio.h>
#include <malloc.h>
#include "yaml-cpp/yaml.h"
#include "oicq/tea.h"
#include <mutex>



std::map<UINT, ClientNode*> Clients;
std::mutex Mutex2;

void MyRequestGetUin(ClientNode* Client, BYTE* Body, size_t BodyLen);
void MyRequestRegister(ClientNode* Client, BYTE* Body, size_t BodyLen);

void AddClient(ClientNode* Client, UINT Uin) {
	Clients[Uin] = Client;
}

void DelClient(UINT Uin) {
	Mutex2.lock();
	Clients.erase(Uin);
	Mutex2.unlock();
}

ClientNode* GetClient(UINT Uin)
{
	ClientNode* Client = NULL;
	Mutex2.lock();
	auto iter = Clients.find(Uin);
	if (iter != Clients.end())
	{
		Client = iter->second;
}
	else
	{
		printf("Uin:%d offline\n", Uin);
	}
	Mutex2.unlock();
	return Client;
}
void OnRecvFromClient(ClientNode* Client, USHORT MsgID, BYTE* Body, size_t BodyLen)
{
	//printf("msgId:%d\n", MsgID);
#ifdef DEBUG
	if (MsgID != 538/*AckAntiBot*/ && MsgID != 562/*AckDpData*/)
	{
		printf("[GameServer][ConnID:%d]RecvMsg:%d\n", Client->ConnID, MsgID);
	}
#endif
	if (!Client->IsLogin)
	{
		if (MsgID > 100)
		{
			return; //未登录则不能执行太多操作
		}
	}

	switch (MsgID)
	{
		//MyServer
	case 1:
		MyRequestGetUin(Client, Body, BodyLen);
		break;
	case 2:
		MyRequestRegister(Client, Body, BodyLen);
		break;
		//DirServer
	case 90:
		RequestGetWorldInfo(Client, Body, BodyLen);
		break;
	case 91:
		RequestGetChatRoomList(Client, Body, BodyLen);
		break;
		//GameServer
	case 100:
		RequestLogin(Client, Body, BodyLen);
		break;
	case 101:
		RequestLogout(Client, Body, BodyLen);
		break;
	case 102:
		RequestGetRoomList(Client, Body, BodyLen);
		break;
	case 103:
		RequestCreateRoom(Client, Body, BodyLen);
		break;
	case 104://通知房间
		RequestEnterRoom(Client, Body, BodyLen);
		break;
	case 105:
		RequestLeaveRoom(Client, Body, BodyLen);
		break;
	case 106:
		break;
	case 107:
		RequestChangeSeatState(Client, Body, BodyLen);
		break;
	case 108:
		RequestKickFromRoom(Client, Body, BodyLen);
		break;
	case 109://准备就绪
		RequestChangeReadyState(Client, Body, BodyLen);
		break;
	case 110://开始游戏
		RequestStartGame(Client, Body, BodyLen);
		break;
	case 111://喊话
		RequestTalk(Client, Body, BodyLen);
		break;
	case 112:
		RequestHello(Client, Body, BodyLen);
		break;
	case 120:
		RequestPrepareReady(Client, Body, BodyLen);
		break;
	case 121:
		RequestChangeMap(Client, Body, BodyLen);
		break;
	case 122://响应搜索玩家
		RequestFindPlayerByQQ(Client, Body, BodyLen);
		break;
	case 123:
		//这里是结算触发的函数
		//通知客户端结算是另一个函数:NotifyRaceOver
		RequestReportCurrentInfo(Client, Body, BodyLen);
		break;
	case 124:
		RequestGetProp(Client, Body, BodyLen);
		break;
	case 130:
		//装备赛车、装扮相关
		RequestChangeItemStatus(Client, Body, BodyLen);
		break;
	case 133://改名		
		ResponseResetNameCard(Client, Body, BodyLen);
		break;
	case 135:
		RequestGetTaskList(Client, Body, BodyLen);
		break;
	case 138://紫钻练习场  跳过新手任务
		//ResponseSkipFreshTask(Client); //跳过新手任务
		RequestStartSingleTask(Client, Body, BodyLen);
		break;
	case 145:
		RequestGetLicenseInfo(Client, Body, BodyLen);
		break;
	case 151:
		RequestSwitchInterface(Client, Body, BodyLen);
		break;
	case 152:
		RequestGetSystemTaskList(Client, Body, BodyLen);
		break;
	case 153:
		RequestTransferByTCP(Client, Body, BodyLen);
		break;
	case 154:
		//更名卡更名 确认改名
		ResponseResetNameCard2(Client, Body, BodyLen);
	case 162:
		RequestChangeGender(Client, Body, BodyLen);
		break;
	case 163:
		RequestNewGetFriendList(Client, Body, BodyLen);
		break;
	case 165:
		RequestUseProp2(Client, Body, BodyLen);
		break;
	case 168:
		RequestUseItem(Client, Body, BodyLen);
		break;
	case 172:
		RequestGetLuckAwardList(Client, Body, BodyLen);
		break;
	case 174:
		RequestRefitKart(Client, Body, BodyLen);
		break;
	case 178://请求更新宠物名称
		//RequestUpdatePetName(Client, Body, BodyLen);
		break;
	case 179:
		RequestChangeUdpState(Client, Body, BodyLen);
		break;
	case 183:
		RequestSkipFreshTask(Client, Body, BodyLen);
		break;
	case 187:
		RequestGetRandomTask(Client, Body, BodyLen);
		break;
	case 253:
		RequestDanceChangeMusic(Client, Body, BodyLen);
		break;
	case 254:
		RequestChangeScene(Client, Body, BodyLen);
		break;
	case 257:
		RequestDanceChangeMode(Client, Body, BodyLen);
		break;
	case 258:
		RequestDanceChangeRandKeyFlag(Client, Body, BodyLen);
		break;
	case 259:
		RequestDancePrepareReady(Client, Body, BodyLen);
		break;
	case 260:
		RequestDanceStepStatistics(Client, Body, BodyLen);
		break;
	case 261:
		RequestDeleteOneItem(Client, Body, BodyLen);
		break;
	case 262:
		RequestLoverDanceChangeOneMode(Client, Body, BodyLen);
		break;
	case 263:
		RequestLoverDanceStepStatistics(Client, Body, BodyLen);
		break;
	case 271:
		//更新赛车改装信息
		RequestKartRefitInfo(Client, Body, BodyLen);
		break;
	case 277:
		RequestChangeModel(Client, Body, BodyLen);
		break;
	case 316://快速装备皮肤
		//ResponseSaveShapeRefit(Client, KartID,RefitItemNum,RefitItemID[],ShapeSuitID);		
		RequestSaveShapeRefit(Client, Body, BodyLen);
		break;
	case 317://皮肤按钮及数据
		//RequestSaveShapeRefit(Client, Body, BodyLen);
		RequestGetShapeRefitCfg(Client, Body, BodyLen);
		break;          
	case 351:
		RequestHeroMove(Client, Body, BodyLen);
		break;
	case 352:
		RequestEnterOutdoor(Client, Body, BodyLen);
		break;
	case 353:
		RequestLeaveOutdoor(Client, Body, BodyLen);
		break;
	case 357:
		RequestRegister2(Client, Body, BodyLen);
		break;
	case 360:
		RequestChangeChairState(Client, Body, BodyLen);
		break;
	case 363:
		RequestGetMaster(Client, Body, BodyLen);
		break;
	case 373:
		RequestGetAchieveList(Client, Body, BodyLen);
		break;
	case 376:
		RequestModifyPlayerSignature(Client, Body, BodyLen);
		break;
	case 422:
		RequestChangeSkinStatus(Client, Body, BodyLen);
		break;
	case 452:
		RequestGetShoppingCarItemList(Client, Body, BodyLen);
		break;
	case 461:
		RequestTopList(Client, Body, BodyLen);
		break;
	case 499:
		RequestTransferUDPOK(Client, Body, BodyLen);
		break;
	case 901://赛车镶嵌宝石
		RequestInlaySkillStone(Client, Body, BodyLen);
		break;
	case 903:
		//赛车宝石开孔
		RequestAddStoneGroove(Client, Body, BodyLen);
		break;
	case 912:
		//赛车宝石拆除
		RequestRemoveSkillStone(Client, Body, BodyLen);
		break;
	case 909:
		//宝石发动通知其他玩家
		NotifyMatchSkillStoneInfo(Client, Body, BodyLen);
		break;
	case 944:
		RequestFizzInfo(Client, Body, BodyLen);
		break;
	case 1500:
		RequestStartShadowChallenge(Client, Body, BodyLen);
		break;
	case 1501:
		RequestEndShadowChallenge(Client, Body, BodyLen);
		break;
	case 10501:
		RequestFishLogin(Client, Body, BodyLen);
		break;
	case 11451:
		RequestGetPlayerEventCfg(Client, Body, BodyLen);
		break;
	case 11452:
		RequestGetPlayerEvent(Client, Body, BodyLen);
		break;
	case 13710:
		RequestC2SGetKartAltasConfig(Client, Body, BodyLen);
		break;
	case 28331://激活
		//RequestShapeLegendOperate(Client, Body, BodyLen);
		break;
	case 28412://置顶星标
		//RequestShapeLegendOperate(Client, Body, BodyLen);
		break;
	case 424:
		RequestLeaveGame(Client, Body, BodyLen);
		break;
	case 1401:
		RequestC2GSign3Operate(Client, Body, BodyLen);
		break;
	case 16055:
		RequestGetPrivilegeIntroInfo(Client, Body, BodyLen);
		break;
	case 20079:
		RequestAddToMatchQueue(Client, Body, BodyLen);
		break;
	case 20082:
		RequestEnterMatchRoom(Client, Body, BodyLen);
		break;
	case 21111:
		RequestSecondGetKartRefitLimitCfg(Client, Body, BodyLen);
		break;
	case 24029:
		RequestGetActivityCenterInfo(Client, Body, BodyLen);
		break;
	case 24201:
		RequestGetItemTimeLimtCfg(Client, Body, BodyLen);
		break;
	case 24202:
		RequestHyperSpaceJumboGetCfg(Client, Body, BodyLen);
		break;
	case 24215:
		RequestGetKeyTranslateInfoNew(Client, Body, BodyLen);
		break;
	case 24258:
		RequestTimerChallenge2ndMoreHallInfo(Client, Body, BodyLen);
		break;
	case 24391:
		RequestWeRelayCommonCfg(Client, Body, BodyLen);
		break;
	case 24465:
		RequestPreGetCommonBoxAward(Client, Body, BodyLen);
		break;
	case 24721:
		RequestSSCOpenStatus(Client, Body, BodyLen);
		break;
	case 28421:	 
		RequestReplayOperate(Client, Body, BodyLen);
		break;
		//ShopServer
	case 20000:
		RequestBuyCommodity(Client, Body, BodyLen);
	default:
		break;
	}
}

void SendToClient(ClientNode* Client, short MsgID, const BYTE* Data, DWORD Length, short SrcID, char SrcFE, short DstID, short MsgType, char Encrypt)
{
	CSHEAD* ProtocolHead;
	MSGHEAD* MsgHead;
	void* MsgBody;
	int TotalLength;

	int MsgLen = sizeof(MSGHEAD) + Length;
	MsgHead = (MSGHEAD*)malloc(MsgLen);
	if (!MsgHead)
	{
		return;
	}
	MsgHead->MsgID = htons(MsgID);
	MsgHead->MsgType = htons(MsgType);
	MsgHead->MsgSeq = htonl(-1);
	MsgHead->SrcFE = SrcFE;
	MsgHead->DstFE = FE_CLIENT;
	MsgHead->SrcID = htons(SrcID);
	MsgHead->DstID = htons(DstID);
	MsgHead->BodyLen = htons(sizeof(USHORT) + USHORT(Length));
	MsgBody = (void*)(MsgHead + 1);
	memcpy(MsgBody, Data, Length);
	if (Encrypt)
	{
		int EncryptLen = oi_symmetry_encrypt2_len(MsgLen);
		TotalLength = sizeof(CSHEAD) +  EncryptLen;
		if (!(ProtocolHead = (CSHEAD*)malloc(TotalLength)))
		{
			return;
		}
		oi_symmetry_encrypt2((BYTE*)MsgHead, MsgLen, Client->Key, (BYTE*)(ProtocolHead + 1), &EncryptLen);
		TotalLength = sizeof(CSHEAD) + EncryptLen;
	}
	else
	{
		TotalLength = sizeof(CSHEAD) + MsgLen;
		if (!(ProtocolHead = (CSHEAD*)malloc(TotalLength)))
		{
			return;
		}
		memcpy((MSGHEAD*)(ProtocolHead + 1), MsgHead, MsgLen);
	}
	free(MsgHead);

	ProtocolHead->TotalLength = htonl(TotalLength);
	ProtocolHead->Ver = htons(116);
	ProtocolHead->DialogID = htons(SrcID);
	ProtocolHead->Seq = htonl(-1);
	ProtocolHead->Uin = htonl(Client->Uin);
	ProtocolHead->BodyFlag = Encrypt;
	ProtocolHead->OptLength = 0;

	Client->Server->Send(Client->ConnID, (BYTE*)ProtocolHead, TotalLength);
#ifdef DEBUG
	printf("[ConnID:%d]SendMsg:%d\n", Client->ConnID, MsgID);
#endif
	free(ProtocolHead);
}

void OnFullReceive(ClientNode* Client, BYTE* Data, size_t Length)
{
	if(Length < sizeof(CSHEAD))
	{
		Client->Server->Disconnect(Client->ConnID);
		return;
	}
	CSHEAD* ProtocolHead = (CSHEAD*)Data;
	BYTE* Body;
	DWORD HeadLen = sizeof(CSHEAD) + ProtocolHead->OptLength;
	if(Length < HeadLen)
	{
		Client->Server->Disconnect(Client->ConnID);
		return;
	}

	if (ProtocolHead->OptLength)
	{
		Body = (BYTE*)(ProtocolHead + 1);
		//官方服务器应该根据这个进行查表获取key
		//而我们偷懒,把它作为登录密码,这样其实不安全,并且它是明文传输的

		char* Password = new char[ProtocolHead->OptLength + 1];
		memcpy(Password, Body, ProtocolHead->OptLength);
		Password[ProtocolHead->OptLength] = 0;
		//printf("CmpPassword:%s\n", Password);
#ifdef ZingSpeed
		if (strcmp("!", Password) == 0) //TODO: 这个密码是固定的,不安全.
		{
			Client->IsLogin = TRUE;
		}
#else
		if (UserDB_CmpPassword(ntohl(ProtocolHead->Uin), Password))
		{
			Client->IsLogin = TRUE;
		}
#endif
		delete[] Password;
	}

	DWORD BodyLen = Length - HeadLen;
	if (BodyLen >= sizeof(MSGHEAD))
	{
		MSGHEAD* MsgHead = (MSGHEAD*)((DWORD)Data + HeadLen);
		if (ProtocolHead->BodyFlag)
		{
			Body = (BYTE*)malloc(BodyLen);
			if (!Body)
			{
				return;
			}
			if (oi_symmetry_decrypt2((BYTE*)MsgHead, BodyLen, Client->Key, Body, (int*)&BodyLen) == false)
			{
				printf("[ConnID:%d]解密失败\n", Client->ConnID);
				free(Body);
				return;
			}
		}
		else
		{
			Body = (BYTE*)malloc(BodyLen);
			if (!Body)
			{
				return;
			}
			memcpy(Body, MsgHead, BodyLen);
		}
		MsgHead = (MSGHEAD*)Body;
		Body = (BYTE*)Body + sizeof(MSGHEAD);
		BodyLen = BodyLen - sizeof(MSGHEAD);

		if (ProtocolHead->OptLength)
		{
			Client->ServerID = ntohs(MsgHead->DstID);
		}
		OnRecvFromClient(Client, ntohs(MsgHead->MsgID), Body, ntohs(MsgHead->BodyLen));

		free(MsgHead);
	}
	else
	{
		printf("[ConnID:%d]不支持请求检查更新\n", Client->ConnID);
		Client->Server->Disconnect(Client->ConnID);
	}

}




void RequestFishLogin(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseFishLogin(Client);
}
void ResponseFishLogin(ClientNode* Client)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, 0); //Result
	Write32(p, Client->Uin); //Uin

	{ //BaseFishInfo
		BYTE* pBaseFishInfo = p;
		Write16(pBaseFishInfo, 0); //len

		Write32(pBaseFishInfo, 0); //Experience
		Write32(pBaseFishInfo, 0); //Money
		Write8(pBaseFishInfo, 0); //FishStockNum
		Write8(pBaseFishInfo, 0); //RubbishCount

		len = pBaseFishInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ //TodayInfo
		BYTE* pTodayInfo = p;
		Write16(pTodayInfo, 0); //len

		Write8(pTodayInfo, 0); //RestStealCount
		Write8(pTodayInfo, 0); //RestVisitStrangerCount
		Write8(pTodayInfo, 0); //RestClearRubbishCount

		len = pTodayInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); //FishStockNum
	/*
m_astFishStockInfo[].m_uchFishStockID
m_astFishStockInfo[].m_uchMaturityCount
m_astFishStockInfo[].m_uchTotalGrowStage
m_astFishStockInfo[].m_uchCurrentGrowStage
m_astFishStockInfo[].m_uchFishNum
m_astFishStockInfo[].m_uchStealedCount
m_astFishStockInfo[].m_uchFeedInfo
m_astFishStockInfo[].m_iFishItemID
m_astFishStockInfo[].m_uiFishStartTime
m_astFishStockInfo[].m_uiMaturityTime
m_astFishStockInfo[].m_uiCurGrowStageEndTime
	*/

	Write8(p, 0); //ReasonLen


	len = p - buf;
	SendToClient(Client, 10501, buf, len, 0, 32, Client->ConnID, Response);
}


void RequestGetPlayerEventCfg(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseGetPlayerEventCfg(Client);
}
void ResponseGetPlayerEventCfg(ClientNode* Client)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, 0); //ResultID
	Write32(p, 0); //GetFriendsEventInterval
	Write32(p, 0); //GetOtherEventInterval
	Write32(p, 0); //FilterTimeLimit

	len = p - buf;
	SendToClient(Client, 11451, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestGetPlayerEvent(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	BYTE TargetType = Read8(Body);
	BYTE TimeType = Read8(Body);

	//m_ushTarGetNum
	//m_astTarGetInfo[].m_uiUin
	//m_astTarGetInfo[].m_uiEventID

	ResponseGetPlayerEvent(Client, TargetType, TimeType);
}
void ResponseGetPlayerEvent(ClientNode* Client, BYTE TargetType, BYTE TimeType)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, 0); //ResultID
	Write8(p, TargetType); //
	Write8(p, TimeType); //
	Write16(p, 0); //Num
	/*
m_astEvent[].m_uiUin
m_astEvent[].m_ucType
m_astEvent[].m_uiEventID
m_astEvent[].m_ucIntNum
m_astEvent[].m_auiPara[]
m_astEvent[].m_ucShortNum
m_astEvent[].m_aushPara[]
m_astEvent[].m_uStrNum
m_astEvent[].m_astPara[].m_szStr[]
m_astEvent[].m_uiTime
	*/

	len = p - buf;
	SendToClient(Client, 11452, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}


void RequestChangeUdpState(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	BYTE State = Read8(Body);
	printf("UdpState:%d\n", State);
}

void RequestTransferUDPOK(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseTransferUDPOK(Client);
}
void ResponseTransferUDPOK(ClientNode* Client)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, 0); //ResultID

	len = p - buf;
	SendToClient(Client, 499, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
/*
999
--------------------CNotifyUDPOKPara--------------------
m_unUin
m_unTime
m_nSrcDlg
m_unSrcUin
m_nInfoLength
--------------------------------------------------------
*/





void RequestFizzInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseFizzInfo(Client);
}
void ResponseFizzInfo(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	{ //FizzBaseInfo
		BYTE* pFizzBaseInfo = p;
		Write16(pFizzBaseInfo, 0); //len

		Write16(pFizzBaseInfo, 0); //FizzPointDaily
		Write16(pFizzBaseInfo, 0); //FizzPointWeekly
		Write32(pFizzBaseInfo, 0); //FizzPointTotal
		Write8(pFizzBaseInfo, 0); //FizzEmblem

		len = pFizzBaseInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); //FizzTaskNum
	/*
	for (size_t i = 0; i < 0; i++)
	{ //FizzTaskInfo
		BYTE* pFizzTaskInfo = p;
		Write16(pFizzTaskInfo, 0); //len

		Write8(pFizzTaskInfo, 0); //TaskID
		Write8(pFizzTaskInfo, 0); //Status
		Write32(pFizzTaskInfo, 0); //TotalProgress
		Write32(pFizzTaskInfo, 0); //CurrentProgress
		Write16(pFizzTaskInfo, 0); //FizzPoint
		Write8(pFizzTaskInfo, 0); //DisplayPri
		Write8(pFizzTaskInfo, 0); //SpecialFlag

		len = pFizzTaskInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write8(p, 0); //FizzLotteryNum
	/*
	for (size_t i = 0; i < 0; i++)
	{ //FizzLotteryInfo
		BYTE* pFizzLotteryInfo = p;
		Write16(pFizzLotteryInfo, 0); //len

		Write8(pFizzLotteryInfo, 0); //LotteryType
		Write8(pFizzLotteryInfo, 0); //Status
		Write16(pFizzLotteryInfo, 0); //FizzPoint
		Write8(pFizzLotteryInfo, 0); //ControlledItemNum
		Write32(pFizzLotteryInfo, 0); //ControlledItem[]

		len = pFizzLotteryInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	len = p - buf;
	SendToClient(Client, 944, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestGetShoppingCarItemList(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	//ResponseGetShoppingCarItemList(Client);
}
void ResponseGetShoppingCarItemList(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	{ //ShoppingCarItemList
		BYTE* pShoppingCarItemList = p;
		Write16(pShoppingCarItemList, 0); //len

		Write8(pShoppingCarItemList, 1); //ItemNum
		Write32(pShoppingCarItemList, 10020); //Item[]

		len = pShoppingCarItemList - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 452, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestGetKeyTranslateInfoNew(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseGetKeyTranslateInfoNew(Client);
}
void ResponseGetKeyTranslateInfoNew(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, 0); //Uin
	Write16(p, 0); //ResultID
	Write8(p, 0); //ReasonLen
	{ //KeyTranslateInfo
		BYTE* pKeyTranslateInfo = p;
		Write16(pKeyTranslateInfo, 0); //len

		Write8(pKeyTranslateInfo, 0); //KeyTranslateDefaultKeyNum1
		Write8(pKeyTranslateInfo, 0); //KeyTranslateNewKeyNum1
		Write8(pKeyTranslateInfo, 0); //KeyTranslateDefaultKeyNum2
		Write8(pKeyTranslateInfo, 0); //KeyTranslateNewKeyNum2

		len = pKeyTranslateInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 24215, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestC2SGetKartAltasConfig(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UINT CfgVersion = Read32(Body);

	ResponseC2SGetKartAltasConfig(Client, CfgVersion);
}
void ResponseC2SGetKartAltasConfig(ClientNode* Client, UINT CfgVersion)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write32(p, CfgVersion); //CfgVersion
	Write8(p, 0); //Result

	Write16(p, 0); //KartAltasCfgNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //KartAltasConfig
		BYTE* pKartAltasConfig = p;
		Write16(pKartAltasConfig, 0); //len

		Write32(pKartAltasConfig, 0); //ItemID
		WriteUn(pKartAltasConfig, 0); //KartAltasUrlLen
		Write32(pKartAltasConfig, 0); //KartJetNum

		len = pKartAltasConfig - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	len = p - buf;
	SendToClient(Client, 13710, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestGetMaster(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseGetMaster(Client);
}
void ResponseGetMaster(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write32(p, Client->Uin); //Uin
	Write8(p, 0); //AccStatus
	Write32(p, 0); //MasterUin
	Write32(p, 0); //AccTime
	Write32(p, 0); //GraduateTime
	Write16(p, 0); //RecentPMAwardNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //RecentPMAwards
		BYTE* pRecentPMAwards = p;
		Write16(pRecentPMAwards, 0); //len

		Write16(pRecentPMAwards, 0); //RecentPMAwardDesLen

		len = pRecentPMAwards - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write8(p, 0); //ReasonLen

	len = p - buf;
	SendToClient(Client, 363, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestSSCOpenStatus(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseSSCOpenStatus(Client);
}
void ResponseSSCOpenStatus(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write8(p, 0); //Satus
	Write8(p, 0); //TipsLen

	len = p - buf;
	SendToClient(Client, 24721, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestSwitchInterface(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	bool InterfaceType = Read8(Body);

	ResponseSwitchInterface(Client);
}
void ResponseSwitchInterface(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write32(p, 0); //Uin

	len = p - buf;
	SendToClient(Client, 151, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestSecondGetKartRefitLimitCfg(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseSecondGetKartRefitLimitCfg(Client);
}
void ResponseSecondGetKartRefitLimitCfg(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write32(p, Client->Uin); //Uin
	{ //KartRefitLimitCfg
		BYTE* pKartRefitLimitCfg = p;
		Write16(pKartRefitLimitCfg, 0); //len

		Write16(pKartRefitLimitCfg, 0); //OpenKartTypeNum
		//Write32(pKartRefitLimitCfg, 0); //OpenKartTypeList[]
		Write32(pKartRefitLimitCfg, 0); //MinFormerRefitTime
		Write16(pKartRefitLimitCfg, 0); //ExcludeKartTypeNum
		//Write32(pKartRefitLimitCfg, 0); //ExcludeKartTypeList[]
		Write16(pKartRefitLimitCfg, 0); //ForbidListNum
		//Write32(pKartRefitLimitCfg, 0); //ForbidRefitKartID[]

		len = pKartRefitLimitCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ //RefitItemWeightTypeCfg
		BYTE* pRefitItemWeightTypeCfg = p;
		Write16(pRefitItemWeightTypeCfg, 0); //len

		Write8(pRefitItemWeightTypeCfg, 0); //TypeNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //ItemWeightLevel
			BYTE* pItemWeightLevel = pRefitItemWeightTypeCfg;
			Write16(pItemWeightLevel, 0); //len

			Write8(pItemWeightLevel, 0); //ItemWeightLevel
			Write16(pItemWeightLevel, 0); //MinWeight
			Write16(pItemWeightLevel, 0); //MaxWeight
			Write16(pItemWeightLevel, 0); //ExpAward

			len = pItemWeightLevel - pRefitItemWeightTypeCfg;
			Set16(pRefitItemWeightTypeCfg, (WORD)len);
			pRefitItemWeightTypeCfg += len;
		}
		*/
		len = pRefitItemWeightTypeCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ //RefitItemCfg
		BYTE* pRefitItemCfg = p;
		Write16(pRefitItemCfg, 0); //len

		Write16(pRefitItemCfg, 0); //ItemNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //RefitItem
			BYTE* pRefitItem = pRefitItemCfg;
			Write16(pRefitItem, 0); //len

			Write32(pRefitItem, 0); //ItemID
			Write8(pRefitItem, 0); //ItemType
			Write8(pRefitItem, 0); //ItemWeightLevel
			Write16(pRefitItem, 0); //Value
			Write32(pRefitItem, 0); //FragmentItemID
			Write32(pRefitItem, 0); //FragmentNumToOneItem
			Write16(pRefitItem, 0); //IsItemOpen

			len = pRefitItem - pRefitItemCfg;
			Set16(pRefitItemCfg, (WORD)len);
			pRefitItemCfg += len;
		}
		*/

		len = pRefitItemCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 21111, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestGetPrivilegeIntroInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);

	ResponseGetPrivilegeIntroInfo(Client);
}
void ResponseGetPrivilegeIntroInfo(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, 0); //Uin
	Write32(p, 0); //CharmValueOfMonth
	Write32(p, 0); //VipLevel
	Write16(p, 0); //PrivilegeInfoNums
	/*
	for (size_t i = 0; i < n; i++)
	{ //PrivilegeInfos
		BYTE* pPrivilegeInfos = p;
		Write16(pPrivilegeInfos, 0); //len

		WriteUn(pPrivilegeInfos, 0); //NextLevelCharmVlaue
		WriteUn(pPrivilegeInfos, 0); //SearchTreasureNums
		WriteUn(pPrivilegeInfos, 0); //GetTreasureNums
		WriteUn(pPrivilegeInfos, 0); //CritCardNums
		WriteUn(pPrivilegeInfos, 0); //MechLargerCardNums
		WriteUn(pPrivilegeInfos, 0); //MallBuyLevel
		WriteUn(pPrivilegeInfos, 0); //HasAlreadyReceived
		WriteUn(pPrivilegeInfos, 0); //AwardItemNums
		for (size_t i = 0; i < n; i++)
		{ //FirstReachLevelItemInfos
			BYTE* pFirstReachLevelItemInfos = pPrivilegeInfos;
			Write16(pFirstReachLevelItemInfos, 0); //len

			WriteUn(pFirstReachLevelItemInfos, 0); //itemID
			WriteUn(pFirstReachLevelItemInfos, 0); //itemNums
			Write32(pFirstReachLevelItemInfos, 0); //AvailPeriod
			WriteUn(pFirstReachLevelItemInfos, 0); //isBestQuality

			len = pFirstReachLevelItemInfos - pPrivilegeInfos;
			Set16(pPrivilegeInfos, (WORD)len);
			pPrivilegeInfos += len;
		}

		len = pPrivilegeInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	//WriteUn(p, 0); //AnouncementInfo[]

	len = p - buf;
	SendToClient(Client, 16055, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestGetItemTimeLimtCfg(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UINT UpdateTime = Read32(Body);

	ResponsetGetItemTimeLimtCfg(Client, UpdateTime);
}
void ResponsetGetItemTimeLimtCfg(ClientNode* Client, UINT UpdateTime)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write16(p, 0); //ResultID

	Write32(p, 0); //ItemNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //ItemTimeLimit
		BYTE* pItemTimeLimit = p;
		Write16(pItemTimeLimit, 0); //len

		Write32(pItemTimeLimit, 0); //ItemID
		Write32(pItemTimeLimit, 0); //ExpireTime

		len = pItemTimeLimit - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write32(p, UpdateTime); //UpdateTime

	len = p - buf;
	SendToClient(Client, 24201, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

//皮肤显示传递数据
void RequestGetShapeRefitCfg(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	ResponseGetShapeRefitCfg(Client);

}


//皮肤显示应用
void ResponseGetShapeRefitCfg(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;
	YAML::Node ShapeRefitCfg = YAML::LoadFile(".\\Skin\\ShapeRefitCfg.yml");

	Write16(p, 0); //ResultID
	Write16(p, 0); //KartShapeRefitSuitNum

	Write16(p, ShapeRefitCfg["ShapeRefitLegend"].size()); //KartShapeRefitLegendNum

	for (size_t i = 0; i < ShapeRefitCfg["ShapeRefitLegend"].size(); i++)
	{ //ShapeRefitLegend
		BYTE* pShapeRefitLegend = p;
		Write16(pShapeRefitLegend, 0); //len

		Write32(pShapeRefitLegend, ShapeRefitCfg["ShapeRefitLegend"][i]["KartID"].as<int>()); //KartID
		Write8(pShapeRefitLegend, 0); //KarNameLen
		Write16(pShapeRefitLegend, 0); //Grade
		Write8(pShapeRefitLegend, 0); //GradeResourceLen
		Write8(pShapeRefitLegend, 0); //LevelNum
		Write32(pShapeRefitLegend, ShapeRefitCfg["ShapeRefitLegend"][i]["ShapeSuitID"].as<int>()); //ShapeSuitID
		{ //MaxConsumInfo
			BYTE* pMaxConsumInfo = pShapeRefitLegend;
			Write16(pMaxConsumInfo, 0); //len

			Write32(pMaxConsumInfo, 0); //ItemID
			Write32(pMaxConsumInfo, 0); //ItemNum
			Write32(pMaxConsumInfo, 0); //ItemPeriod

			len = pMaxConsumInfo - pShapeRefitLegend;
			Set16(pShapeRefitLegend, (WORD)len);
			pShapeRefitLegend += len;
		}
		Write8(pShapeRefitLegend, 0); //LegendSkinType

		len = pShapeRefitLegend - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 317, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestWeRelayCommonCfg(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseWeRelayCommonCfg(Client);
}
void ResponseWeRelayCommonCfg(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write32(p, 0); //Uin
	{ //SeasonCfg
		BYTE* pSeasonCfg = p;
		Write16(pSeasonCfg, 0); //len

		Write32(pSeasonCfg, 0); //SeasonId
		Write32(pSeasonCfg, 0); //OpenDate
		Write32(pSeasonCfg, 0); //CloseDate
		Write32(pSeasonCfg, 0); //OpenDayNum
		Write32(pSeasonCfg, 0); //BroadCastContentLen
		Write32(pSeasonCfg, 0); //OpenTimeTipsLen

		const int MAX_SPEEDRALLY_OPEN_DAY_NUM = 1;
		for (size_t i = 0; i < MAX_SPEEDRALLY_OPEN_DAY_NUM; i++)
		{ //tDayOpenCfg
			BYTE* ptDayOpenCfg = pSeasonCfg;
			Write16(ptDayOpenCfg, 0); //len

			Write32(ptDayOpenCfg, 0); //DayID
			Write32(ptDayOpenCfg, 0); //OpenTimeNum
			/*
			for (size_t i = 0; i < MAX_OPEN_TIME_NUM; i++)
			{ //tOpenTimeCfg
				BYTE* ptOpenTimeCfg = ptDayOpenCfg;
				Write16(ptOpenTimeCfg, 0); //len

				Write32(ptOpenTimeCfg, 0); //OpenTime
				Write32(ptOpenTimeCfg, 0); //CloseTime
				Write32(ptOpenTimeCfg, 0); //TiyanOpenTime
				Write32(ptOpenTimeCfg, 0); //TiyanCloseTime

				len = ptOpenTimeCfg - ptDayOpenCfg;
				Set16(ptDayOpenCfg, (WORD)len);
				ptDayOpenCfg += len;
			}
			*/
			len = ptDayOpenCfg - pSeasonCfg;
			Set16(pSeasonCfg, (WORD)len);
			pSeasonCfg += len;
		}
		Write32(pSeasonCfg, 0); //LuckyBoxIndex
		Write32(pSeasonCfg, 0); //LuckyBoxPriority
		Write32(pSeasonCfg, 0); //LuckyBoxDayLimit

		len = pSeasonCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); //RankNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //RankAwardInfo
		BYTE* pRankAwardInfo = p;
		Write16(pRankAwardInfo, 0); //len

		Write32(pRankAwardInfo, 0); //RankType
		Write32(pRankAwardInfo, 0); //AwardNum
		for (size_t i = 0; i < n; i++)
		{ //BaseRankAwardToClient
			BYTE* pBaseRankAwardToClient = pRankAwardInfo;
			Write16(pBaseRankAwardToClient, 0); //len

			Write32(pBaseRankAwardToClient, 0); //Index
			Write32(pBaseRankAwardToClient, 0); //RankBegin
			Write32(pBaseRankAwardToClient, 0); //RankEnd
			{ //Award
				BYTE* pAward = pBaseRankAwardToClient;
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

				len = pAward - pBaseRankAwardToClient;
				Set16(pBaseRankAwardToClient, (WORD)len);
				pBaseRankAwardToClient += len;
			}
			Write32(pBaseRankAwardToClient, 0); //Status

			len = pBaseRankAwardToClient - pRankAwardInfo;
			Set16(pRankAwardInfo, (WORD)len);
			pRankAwardInfo += len;
		}

		len = pRankAwardInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write32(p, 0); //ActiveAwardNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //ActiveAwardToClient
		BYTE* pActiveAwardToClient = p;
		Write16(pActiveAwardToClient, 0); //len

		Write32(pActiveAwardToClient, 0); //Index
		Write32(pActiveAwardToClient, 0); //ActiveValueLimit
		Write32(pActiveAwardToClient, 0); //RankLevLimit
		{ //Award
			BYTE* pAward = pActiveAwardToClient;
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

			len = pAward - pActiveAwardToClient;
			Set16(pActiveAwardToClient, (WORD)len);
			pActiveAwardToClient += len;
		}

		len = pActiveAwardToClient - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write32(p, 0); //CarTypeNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //CarGuide
		BYTE* pCarGuide = p;
		Write16(pCarGuide, 0); //len

		Write32(pCarGuide, 0); //CarType
		Write32(pCarGuide, 0); //CarNum
		for (size_t i = 0; i < n; i++)
		{ //GuideCarInfo
			BYTE* pGuideCarInfo = pCarGuide;
			Write16(pGuideCarInfo, 0); //len

			Write32(pGuideCarInfo, 0); //CarID

			len = pGuideCarInfo - pCarGuide;
			Set16(pCarGuide, (WORD)len);
			pCarGuide += len;
		}

		len = pCarGuide - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write32(p, 0); //TiyanCarTypeNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //TiyanCarGuide
		BYTE* pTiyanCarGuide = p;
		Write16(pTiyanCarGuide, 0); //len

		Write32(pTiyanCarGuide, 0); //CarType
		Write32(pTiyanCarGuide, 0); //CarNum
		for (size_t i = 0; i < n; i++)
		{ //GuideCarInfo
			BYTE* pGuideCarInfo = pTiyanCarGuide;
			Write16(pGuideCarInfo, 0); //len

			Write32(pGuideCarInfo, 0); //CarID

			len = pGuideCarInfo - pTiyanCarGuide;
			Set16(pTiyanCarGuide, (WORD)len);
			pTiyanCarGuide += len;
		}

		len = pTiyanCarGuide - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	{ //LuckyAwardToClient
		BYTE* pLuckyAwardToClient = p;
		Write16(pLuckyAwardToClient, 0); //len

		Write32(pLuckyAwardToClient, 0); //LuckBoxGet
		{ //Award
			BYTE* pAward = pLuckyAwardToClient;
			Write16(pAward, 0); //len

			Write32(pAward, 0); //Exp
			Write32(pAward, 0); //Money
			Write32(pAward, 0); //SuperMoney
			Write16(pAward, 0); //ItemNum
			/*
			for (size_t i = 0; i < n; i++)
			{ //ItemInfo
				BYTE* pItemInfo = pAward;
				Write16(pItemInfo, 0); //len

				Write32(pItemInfo, 0); //ItemID
				Write32(pItemInfo, 0); //ItemNum
				Write32(pItemInfo, 0); //AvailPeriod
				Write8(pItemInfo, 0); //Status
				Write32(pItemInfo, 0); //ObtainTime
				Write32(pItemInfo, 0); //OtherAttribute
				Write16(pItemInfo, 0); //ItemType

				len = pItemInfo - pAward;
				Set16(pAward, (WORD)len);
				pAward += len;
			}
			*/
			Write32(pAward, 0); //Coupons
			Write32(pAward, 0); //GuildPoint
			Write32(pAward, 0); //LuckMoney
			Write8(pAward, 0); //ExtendInfoNum
			/*
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
			*/
			Write32(pAward, 0); //SpeedCoin

			len = pAward - pLuckyAwardToClient;
			Set16(pLuckyAwardToClient, (WORD)len);
			pLuckyAwardToClient += len;
		}

		len = pLuckyAwardToClient - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 24391, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
//赛车宝石镶嵌和拆除
void RequestAddStoneGroove(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int KartID = Read32(p);
	int CarveKnifeID = Read32(p);
	int StoneGrooveType = Read32(p);
#ifdef DEBUG
	printf("RequestAddStoneGroove： %d %d %d\n", KartID, CarveKnifeID, StoneGrooveType);
#endif
	PlayerDB_AddStoneGroove(Client, KartID, StoneGrooveType);
	NotifyUpdateKartSkillStoneInfo(Client, KartID);
	ResponseAddStoneGroove(Client, KartID, CarveKnifeID, StoneGrooveType);
}
void ResponseAddStoneGroove(ClientNode* Client, int KartID, int CarveKnifeID, int StoneGrooveType)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, 0); //ResultID
	Write32(p, 0); //ReasonLen
	Write32(p, KartID); //KartID
	Write32(p, CarveKnifeID); //CarveKnifeID
	Write32(p, StoneGrooveType); //StoneGrooveType

	len = p - buf;
	SendToClient(Client, 903, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestRemoveSkillStone(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int KartID = Read32(p);
	int StoneGrooveID = Read32(p);
	int RemovalToolID = Read32(p);
	int SkillStoneID = PlayerDB_RemoveSkillStone(Client, KartID, StoneGrooveID);
#ifdef DEBUG
	printf("RequestRemoveSkillStone： %d %d %d\n", KartID, StoneGrooveID, RemovalToolID);
	printf("SkillStoneID:%d\n", SkillStoneID);
#endif
	ItemInfo item = PlayerDB_AddItem2(Uin, SkillStoneID, 1, 0, 0);
	ItemInfo Items[1] = { item };
	NotifyClientAddItem(Client, 1, Items);
	NotifyUpdateKartSkillStoneInfo(Client, KartID);
	ResponseRemoveSkillStone(Client, KartID, StoneGrooveID, RemovalToolID, SkillStoneID);
}
void ResponseRemoveSkillStone(ClientNode* Client, int KartID, int StoneGrooveID, int RemovalToolID, int SkillStoneID)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, 0); //ResultID
	Write32(p, KartID); //KartID
	Write32(p, StoneGrooveID); //StoneGrooveID
	Write32(p, RemovalToolID); //RemovalToolID
	Write32(p, SkillStoneID); //ObtainSkillStoneID

	len = p - buf;
	SendToClient(Client, 912, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}