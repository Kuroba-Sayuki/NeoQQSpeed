#include "Message.h"
#include "Database.h"

#include <string>
#include "Item.h"


struct ShoppingCar
{
	int CommodityID;
	int PriceIdx;
	int DiscountCardID;
};

void ResponseBuyCommodity(ClientNode* Client, UINT Uin, UINT Time, const char* Reason, int BuyCount, ShoppingCar* aShoppingCar);


void RequestBuyCommodity(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	std::string Reason = "";
	char buffer[16] = "";


	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	//NickName[]
	Body += MaxNickName;


	int ReqBuySeq = Read32(Body);
	UINT DstUin = Read32(Body);
	UINT PlayerExp = Read32(Body);
	USHORT CommodityType = Read16(Body);
	printf("CommodityType:%d\n", CommodityType);
	int PayType = Read32(Body);
	printf("PayType:%d\n", PayType);
	int BuyCount = Read32(Body);
	if (BuyCount == 0)
	{
		return;
	}
	ShoppingCar* aShoppingCar = (ShoppingCar*)malloc(sizeof(ShoppingCar) * BuyCount);
	if (!aShoppingCar)
	{
		return;
	}
	for (int i = 0; i < BuyCount; i++)
	{ //ShoppingCar
		BYTE* pShoppingCar = Body;
		size_t len = Read16(pShoppingCar);

		aShoppingCar[i].CommodityID = Read32(pShoppingCar);
		printf("CommodityID:%d\n", aShoppingCar[i].CommodityID);
		aShoppingCar[i].PriceIdx = Read32(pShoppingCar);
		printf("PriceIdx:%d\n", aShoppingCar[i].PriceIdx);
		aShoppingCar[i].DiscountCardID = Read32(pShoppingCar);
		printf("DiscountCardID:%d\n", aShoppingCar[i].DiscountCardID);

		_itoa_s(aShoppingCar[i].CommodityID, buffer, 10);
		//Reason = Reason + "CommodityID:" + buffer;
		Reason = "温馨提示:";
		_itoa_s(aShoppingCar[i].PriceIdx, buffer, 10);
		//Reason = Reason + ", PriceIdx:" + buffer;
		Reason = "温馨提示:";
		_itoa_s(aShoppingCar[i].PriceIdx, buffer, 10);
		//Reason = Reason + ", DiscountCardID:" + buffer;
		Reason = "温馨提示:";
		//Reason += "\n";
		int ItemNum = 0;
		ItemInfo* items = (ItemInfo*)malloc(sizeof(ItemInfo) * BuyCount);
		int ItemType = EAIT_Unknown;
		switch (CommodityType)
		{
		//车辆及外改
		case(129):
		{
			ItemType = GetItemType(aShoppingCar[i].CommodityID);
			if (ItemType== EAIT_CAR)
			{
				//车辆
				PlayerDB_AddItem(DstUin, aShoppingCar[i].CommodityID, 1, -1, false, 1);
				items[i].ItemNum = 1;
				items[i].AvailPeriod = -1;
			}
			else
			{
				//外改 改装
				ItemNum = PlayerDB_GetItemNum(DstUin, aShoppingCar[i].CommodityID);
				printf("ItemNum:%d\n", ItemNum);
				if (ItemNum==0)
				{
					PlayerDB_AddItem(DstUin, aShoppingCar[i].CommodityID, 1, 0, false, 0);
				}
				else
				{
					PlayerDB_UpdateItem(DstUin, aShoppingCar[i].CommodityID,ItemNum+1);
				}
				items[i].ItemNum = ItemNum+1;
				printf("items[i].ItemNum:%d\n", items[i].ItemNum);
				items[i].AvailPeriod = 0;
				
			}
			
			break;
		}
		//宝石
		case(128):
		{
			//原本数量加1
			ItemNum = PlayerDB_GetItemNum(DstUin, aShoppingCar[i].CommodityID);
			if (ItemNum == 0)
			{
				PlayerDB_AddItem(DstUin, aShoppingCar[i].CommodityID, 1, 0, false, 0);
			}
			else
			{
				PlayerDB_UpdateItem(DstUin, aShoppingCar[i].CommodityID, ItemNum + 1);
			}
			items[i].ItemNum = ItemNum + 1;
			items[i].AvailPeriod = 0;
			break;
		}
		//宝石
		case(0):
		{
			//原本数量加1
			ItemNum = PlayerDB_GetItemNum(DstUin, aShoppingCar[i].CommodityID);
			if (ItemNum == 0)
			{
				PlayerDB_AddItem(DstUin, aShoppingCar[i].CommodityID, 1, 0, false, 0);
			}
			else
			{
				PlayerDB_UpdateItem(DstUin, aShoppingCar[i].CommodityID, ItemNum + 1);
			}
			items[i].ItemNum = ItemNum + 1;
			items[i].AvailPeriod = 0;
			break;
		}
	    //道具
		case(192):
		{
			//原本数量加1
			ItemNum = PlayerDB_GetItemNum(DstUin, aShoppingCar[i].CommodityID);
			if (ItemNum == 0)
			{
				PlayerDB_AddItem(DstUin, aShoppingCar[i].CommodityID, 1, 0, false, 0);
			}
			else
			{
				PlayerDB_UpdateItem(DstUin, aShoppingCar[i].CommodityID, ItemNum + 1);
			}
			items[i].ItemNum = ItemNum + 1;
			items[i].AvailPeriod = 0;
			break;
		}
		//扩展卡
		case(8385):
		{
			//原本数量加1
			ItemNum = PlayerDB_GetItemNum(DstUin, aShoppingCar[i].CommodityID);
			if (ItemNum == 0)
			{
				PlayerDB_AddItem(DstUin, aShoppingCar[i].CommodityID, 1, 0, false, 0);
			}
			else
			{
				PlayerDB_UpdateItem(DstUin, aShoppingCar[i].CommodityID, ItemNum + 1);
			}
			items[i].ItemNum = ItemNum + 1;
			items[i].AvailPeriod = 0;
			break;
		}
	    //防护改装
		case(8321):
		{
			//原本数量加1
			ItemNum = PlayerDB_GetItemNum(DstUin, aShoppingCar[i].CommodityID);
			if (ItemNum == 0)
			{
				PlayerDB_AddItem(DstUin, aShoppingCar[i].CommodityID, 1, 0, false, 0);
			}
			else
			{
				PlayerDB_UpdateItem(DstUin, aShoppingCar[i].CommodityID, ItemNum + 1);
			}
			items[i].ItemNum = ItemNum + 1;
			items[i].AvailPeriod = 0;
			break;
		}
		//魔法阵之类
		case(8384):
		{
			//原本数量加1
			ItemNum = PlayerDB_GetItemNum(DstUin, aShoppingCar[i].CommodityID);
			if (ItemNum == 0)
			{
				PlayerDB_AddItem(DstUin, aShoppingCar[i].CommodityID, 1, 0, false, 0);
			}
			else
			{
				PlayerDB_UpdateItem(DstUin, aShoppingCar[i].CommodityID, ItemNum + 1);
			}
			items[i].ItemNum = ItemNum + 1;
			items[i].AvailPeriod = 0;
			break;
		}
		default:
			PlayerDB_AddItem(DstUin, aShoppingCar[i].CommodityID, 1, -1, false, 0);
			items[i].ItemNum = 1;
			items[i].AvailPeriod = -1;
			break;
		}
	
		//PlayerDB_AddItem(DstUin, aShoppingCar[i].CommodityID, 1, -1, false,1);
		items[i].ItemID = aShoppingCar[i].CommodityID;
		items[i].Status = false;
	
		NotifyClientAddItem(Client, BuyCount, items);

		Body += len;
	}

	short AttachInfoLen = Read16(Body);
	UINT OtherInfo = Read32(Body);
	UINT FirstLoginTime = Read32(Body);
	UINT BuyNum = Read32(Body);
	UINT GuildID = Read32(Body);
	int ReduceCouponsID = Read32(Body);

#ifdef ZingSpeed
	Reason += u8"/C03购买成功";
#else
	Reason = "恭喜您购买成功.您购买的物品已经存放在您的物品栏中,快进入[/C03我的物品/C00]查看吧!";
#endif

	ResponseBuyCommodity(Client, Uin, Time, Reason.c_str(), BuyCount, aShoppingCar);
	free(aShoppingCar);
}

void ResponseBuyCommodity(ClientNode* Client, UINT Uin, UINT Time, const char* Reason, int BuyCount, ShoppingCar* aShoppingCar)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;


	Write16(p, 0); //ResultID
	Write32(p, Uin); //Uin
	Write32(p, Time); //Time

	memset(p, 0, MaxNickName); //NickName[]
	p += MaxNickName;

	Write32(p, 0); //ReqBuySeq
	Write32(p, 0); //DstUin
	Write32(p, 0); //PlayerExp

	Write16(p, 0); //CommodityType
	Write32(p, 0); //PayType
	Write32(p, BuyCount); //BuyCount
	for (int i = 0; i < BuyCount; i++)
	{
		BYTE* pShoppingCar = p;
		Write16(pShoppingCar, 0); //len

		Write32(pShoppingCar, aShoppingCar[i].CommodityID); //CommodityID
		Write32(pShoppingCar, aShoppingCar[i].PriceIdx); //PriceIdx
		Write32(pShoppingCar, aShoppingCar[i].DiscountCardID); //DiscountCardID

		len = pShoppingCar - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write16(p, 0); //AttachInfoLen
	Write32(p, 0); //OtherInfo
	Write32(p, 0); //FirstLoginTime

	len = strlen(Reason);
	Write16(p, (WORD)len); //ReasonLen
	memcpy(p, Reason, len);
	p += len;

	Write16(p, 0); //QuerySigLen
	Write8(p, 0); //CommMbItem
	Write8(p, 0); //MbItemNum
	/*
m_stItem[].m_cMbItemId
m_stItem[].m_cUse
m_stItem[].m_wContentLen
	*/


	Write32(p, 0); //ReduceCouponsID

	{ //MoneyInfo
		BYTE* pMoneyInfo = p;
		Write16(pMoneyInfo, 0); //len

		Write32(pMoneyInfo, 0); //money
		Write32(pMoneyInfo, 0); //superMoney
		Write32(pMoneyInfo, 0); //coupons
		Write32(pMoneyInfo, 0); //luckMoney
		Write32(pMoneyInfo, 0); //qbqd

		len = pMoneyInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 20000, buf, len, -1, FE_SHOPSVRD, Client->ConnID, Response);
}
