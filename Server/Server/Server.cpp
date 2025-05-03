#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <malloc.h>

#include "Message.h"
#include "Map.h"
#include "Item.h"
#include "Database.h"

#pragma comment (lib, "ws2_32.lib")

#pragma comment (lib,"HPSocket.lib")

#include "yaml-cpp/yaml.h"
#ifdef NDEBUG
#pragma comment (lib,"yaml-cpp.lib")
#else
#pragma comment (lib,"yaml-cppd.lib")
#endif



void StartQQServer(std::string IP, USHORT Port);


//udp cs cmds
#define UDP_CMD_LOGIN_SERVER			0x01
#define UDP_CMD_P2P_GAME_DATA			0x02
#define UDP_CMD_SHOW_MY_IP_PORT			0x03
#define UDP_CMD_RECV_OTHER_IP_PORT	0x04
#define UDP_CMD_HEART_BEAT				0x05

//udp cc cmds
#define UDP_CMD_TEST_CONNECTION_NO_RECEIVE			0x20
#define UDP_CMD_TEST_CONNECTION_RECEIVED			0x21
#define UDP_CMD_UDP_BIDIRECTIONAL_OK				0x22
#define UDP_CMD_CLIENT_HELLO						0x25
#define UDP_CMD_CLIENT_P2P_DATA						0x26

#define UDP_CMD_TEST_NETSPEED						0x27
#define UDP_CMD_TEST_NETSPEED_REPLY					0x28


#include "sqlite/sqlite3.h"
extern sqlite3* PlayerDB;


//DWORD WINAPI TestRefit(LPVOID lpvParam)
//{
//	UINT Uin = 10001;
//	int KartItemId = 10020;
//	int RefitItemId = 12376;
//
//	const char* sql = NULL;
//	sqlite3_stmt* stmt = NULL;
//	int result;
//
//	const char* Reason = u8"Sửa đổi thất bại!";
//	int EngineGetCount = 0;
//
//	int RefitCout = 0;
//	short MaxFlags = 0;
//	short WWeight = 0;
//	short SpeedWeight = 0;
//	short JetWeight = 0;
//	short SJetWeight = 0;
//	short AccuWeight = 0;
//	int ShapeRefitCount = 0;
//	int KartHeadRefitItemID = 0;
//	int KartTailRefitItemID = 0;
//	int KartFlankRefitItemID = 0;
//	int KartTireRefitItemID = 0;
//
//	for (size_t i = 0; i < 1; i++)
//	{
//		//GetItemNum
//		if (sqlite3_prepare_v2(PlayerDB, "SELECT ItemNum from Item WHERE Uin=? AND ItemID=?;", -1, &stmt, NULL) == SQLITE_OK) {
//			sqlite3_bind_int(stmt, 1, Uin);
//			sqlite3_bind_int(stmt, 2, RefitItemId);
//
//			if (sqlite3_step(stmt) == SQLITE_ROW) EngineGetCount = sqlite3_column_int(stmt, 0);
//		}
//		sqlite3_finalize(stmt);
//		stmt = NULL;
//		//GetItemNum
//
//		if (EngineGetCount > 0) {
//			//Get Info Kar
//			if (sqlite3_prepare_v2(PlayerDB, "SELECT RefitCout,MaxFlags,WWeight,SpeedWeight,JetWeight,SJetWeight,AccuWeight,ShapeRefitCount,KartHeadRefitItemID,KartTailRefitItemID,KartFlankRefitItemID,KartTireRefitItemID  FROM KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK) {
//				sqlite3_bind_int(stmt, 1, Uin);
//				sqlite3_bind_int(stmt, 2, KartItemId);
//				if (sqlite3_step(stmt) == SQLITE_ROW)
//				{
//					RefitCout = sqlite3_column_int(stmt, 0);
//					MaxFlags = sqlite3_column_int(stmt, 1);
//					WWeight = sqlite3_column_int(stmt, 2);
//					SpeedWeight = sqlite3_column_int(stmt, 3);
//					JetWeight = sqlite3_column_int(stmt, 4);
//					SJetWeight = sqlite3_column_int(stmt, 5);
//					AccuWeight = sqlite3_column_int(stmt, 6);
//
//					ShapeRefitCount = sqlite3_column_int(stmt, 7);
//					KartHeadRefitItemID = sqlite3_column_int(stmt, 8);
//					KartTailRefitItemID = sqlite3_column_int(stmt, 9);
//					KartFlankRefitItemID = sqlite3_column_int(stmt, 10);
//					KartTireRefitItemID = sqlite3_column_int(stmt, 11);
//				}
//			}
//			sqlite3_finalize(stmt);
//			stmt = NULL;
//			//Get Info Kart
//
//			if (EngineGetCount <= 1) {//Delete Item
//
//				if (sqlite3_prepare_v2(PlayerDB, "DELETE FROM Item  WHERE Uin=? AND ItemID=?;", -1, &stmt, NULL) == SQLITE_OK) {
//					sqlite3_bind_int(stmt, 1, Uin);
//					sqlite3_bind_int(stmt, 2, RefitItemId);
//					sqlite3_step(stmt);
//
//					//NotifyClientAddItem(Client, 0, RefitItemId);
//					//ResponseDeleteOneItem(Client, RefitItemId);
//				}
//			}
//			else
//			{
//				if (sqlite3_prepare_v2(PlayerDB, "UPDATE Item SET ItemNum=ItemNum-1 WHERE Uin = ? AND ItemID = ?;", -1, &stmt, NULL) == SQLITE_OK) {
//					sqlite3_bind_int(stmt, 1, Uin);
//					sqlite3_bind_int(stmt, 2, RefitItemId);
//					sqlite3_step(stmt);
//				}
//			}
//			sqlite3_finalize(stmt);
//			stmt = NULL;
//
//			switch (RefitItemId)
//			{
//			case 12376: //Đánh lửa+1 //点火装置+1
//				sql = "UPDATE KartRefit SET RefitCout=RefitCout+1,SJetWeight=SJetWeight+1  WHERE Uin=? AND KartID=?;";
//				Reason = u8"Sửa đổi thành công Đánh lửa+1";
//				break;
//			case 12377: //Bơm khí+1 //进气系统+1
//				sql = "UPDATE KartRefit SET RefitCout=RefitCout+1,AccuWeight=AccuWeight+1  WHERE Uin=? AND KartID=?;";
//				Reason = u8"Sửa đổi thành công Bơm khí+1";
//				break;
//			case 12378: //Nhiên liệu+1 //燃料系统+1
//				sql = "UPDATE KartRefit SET RefitCout=RefitCout+1,JetWeight=JetWeight+1  WHERE Uin=? AND KartID=?;";
//				Reason = u8"Sửa đổi thành công Nhiên liệu+1";
//				break;
//			case 12379: //Giảm Sóc+1 //悬挂系统+1
//				sql = "UPDATE KartRefit SET RefitCout=RefitCout+1,WWeight=WWeight+1  WHERE Uin=? AND KartID=?;";
//				Reason = u8"Sửa đổi thành công Giảm Sóc+1";
//				break;
//			case 12380: //Động cơ+1 //引擎装置+1
//				sql = "UPDATE KartRefit SET RefitCout=RefitCout+1,SpeedWeight=SpeedWeight+1  WHERE Uin=? AND KartID=?;";
//				Reason = u8"Sửa đổi thành công Động cơ+1";
//				break;
//			case 12381: //Đánh lửa-1
//				sql = "UPDATE KartRefit SET RefitCout=RefitCout+1,SJetWeight=SJetWeight-1  WHERE Uin=? AND KartID=? AND SJetWeight>0;";
//				Reason = u8"Sửa đổi thành công Đánh lửa-1";
//				break;
//			case 12382: //Bơm khí-1
//				sql = "UPDATE KartRefit SET RefitCout=RefitCout+1,AccuWeight=AccuWeight-1  WHERE Uin=? AND KartID=? AND AccuWeight>0;";
//				Reason = u8"Sửa đổi thành công Bơm khí-1";
//				break;
//			case 12383: //Nhiên liệu-1
//				sql = "UPDATE KartRefit SET RefitCout=RefitCout+1,JetWeight=JetWeight-1  WHERE Uin=? AND KartID=? AND JetWeight>0;";
//				Reason = u8"Sửa đổi thành công Nhiên liệu-1";
//				break;
//			case 12384: //Giảm Sóc-1
//				sql = "UPDATE KartRefit SET RefitCout=RefitCout+1,WWeight=WWeight-1  WHERE Uin=? AND KartID=? AND WWeight>0;";
//				Reason = u8"Sửa đổi thành công Giảm Sóc-1";
//				break;
//			case 12385: //Động cơ-1
//				sql = "UPDATE KartRefit SET RefitCout=RefitCout+1,SpeedWeight=SpeedWeight-1  WHERE Uin=? AND KartID=? AND SpeedWeight>0;";
//				Reason = u8"Sửa đổi thành công Động cơ-1";
//				break;
//			case 12386: //Đánh lửa+2
//				sql = "UPDATE KartRefit SET RefitCout=RefitCout+1, SJetWeight=SJetWeight+2  WHERE Uin=? AND KartID=?;";
//				Reason = u8"Sửa đổi thành công Đánh lửa+2";
//				break;
//			case 12387: //Bơm khí+2
//				sql = "UPDATE KartRefit SET RefitCout=RefitCout+1,AccuWeight=AccuWeight+2  WHERE Uin=? AND KartID=?;";
//				Reason = u8"Sửa đổi thành công Bơm khí+2";
//				break;
//			case 12388: //Nhiên liệu+2
//				sql = "UPDATE KartRefit SET RefitCout=RefitCout+1,JetWeight=JetWeight+2  WHERE Uin=? AND KartID=?;";
//				Reason = u8"Sửa đổi thành công Nhiên liệu+2";
//				break;
//			case 12389: //Giảm Sóc+2
//				sql = "UPDATE KartRefit SET RefitCout=RefitCout+1,WWeight=WWeight+2  WHERE Uin=? AND KartID=?;";
//				Reason = u8"Sửa đổi thành công Giảm Sóc+2";
//				break;
//			case 12390: //Động cơ+2
//				sql = "UPDATE KartRefit SET RefitCout=RefitCout+1,SpeedWeight=SpeedWeight+2  WHERE Uin=? AND KartID=?;";
//				Reason = u8"Sửa đổi thành công Động cơ+2";
//				break;
//			default:
//				sql = NULL;
//				break;
//			}
//
//			if (sql)
//			{
//				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
//				if (result == SQLITE_OK) {
//					sqlite3_bind_int(stmt, 1, Uin);
//					sqlite3_bind_int(stmt, 2, KartItemId);
//					result = sqlite3_step(stmt);
//				}
//				sqlite3_finalize(stmt);
//				stmt = NULL;
//			}
//
//		}
//
//	}
//	return 0;
//}


class GameServer : public CTcpServerListener, public CUdpNodeListener
{
public:
	CTcpServerPtr TCP;
	CUdpNodePtr UDP;

	unsigned short CheckSum(const BYTE* pBuffer, int nBufLen)
	{
		if (NULL == pBuffer || nBufLen < 1)
		{
			return 0;
		}

		unsigned long nSum = 0;
		unsigned short* pData = (unsigned short*)pBuffer;

		int nLen = nBufLen / 2;
		int nMod = nBufLen % 2;

		for (int i = 0; i < nLen; i++)
		{
			nSum += pData[i];
		}

		unsigned short nShort = 0;

		if (1 == nMod)
		{
			nShort = (unsigned short)pBuffer[nBufLen - 1];
			nSum += nShort;
		}
		nSum = (nSum >> 16) + (nSum & 0xffff);
		nSum += (nSum >> 16);
		nShort = ~((unsigned short)nSum);
		return nShort;
	}

	void SendUdpData(LPCTSTR lpszRemoteAddress, USHORT usRemotePort, short Cmd, const BYTE* Data, int Len, ULONG Seq, SHORT DialogID, ULONG Uin)
	{
		int BufferLength = sizeof(TransferHead) + Len;
		BYTE* Buffer = (BYTE*)malloc(BufferLength);
		if (!Buffer)
		{
			return;
		}
		TransferHead* ProtocolHead = (TransferHead*)Buffer;
		ProtocolHead->TotalLength = htons(BufferLength);
		ProtocolHead->Ver = 0;
		ProtocolHead->Seq = htonl(Seq);
		ProtocolHead->DialogID = htons(DialogID);
		ProtocolHead->Uin = htonl(Uin);
		ProtocolHead->TransferCmd = htons(Cmd);
		ProtocolHead->CheckSum = 0;
		ProtocolHead->OptLength = (UCHAR)Len;
		memcpy(ProtocolHead + 1, Data, Len);
		ProtocolHead->CheckSum = htons(CheckSum(Buffer, BufferLength));

		UDP->Send(lpszRemoteAddress, usRemotePort, Buffer, BufferLength);
		free(Buffer);
		printf("[UDP:Send]Cmd:%d\n", Cmd);
	}

	std::string IP;
	USHORT TcpPort;
	USHORT UdpPort;
	std::string BindIP;
	USHORT BindTcpPort;
	USHORT BindUdpPort;

	std::string QQServerIP;
	USHORT QQServerTcpPort;
	std::string QQServerBindIP;
	USHORT QQServerBindTcpPort;
	

	GameServer() :TCP(this), UDP(this)
	{
		try
		{
			YAML::Node config = YAML::LoadFile("config.yml");

			YAML::Node Server = config["Server"];
			IP = Server["IP"].as<std::string>();
			TcpPort = Server["TcpPort"].as<USHORT>();
			UdpPort = Server["UdpPort"].as<USHORT>();
			//std::string Name = Server["Name"].as<std::string>();
			std::string Name = Server["Name"].as<std::string>();
			InitDirServer(Name.c_str(), inet_addr(IP.c_str()), TcpPort, UdpPort);

			YAML::Node Bind = Server["Bind"];
			BindIP = Bind["IP"].as<std::string>();
			BindTcpPort = Bind["TcpPort"].as<USHORT>();
			BindUdpPort = Bind["UdpPort"].as<USHORT>();


			Server = config["QQServer"];
			QQServerIP = Server["IP"].as<std::string>();
			QQServerTcpPort = Server["TcpPort"].as<USHORT>();

			Bind = Server["Bind"];
			QQServerBindIP = Bind["IP"].as<std::string>();
			QQServerBindTcpPort = Bind["TcpPort"].as<USHORT>();

		}
		catch (const std::exception&)
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
			printf("加载配置文件出错\n");
			exit(0);
		}
	}

	void Run()
	{
		if (!TCP->Start(BindIP.c_str(), BindTcpPort))
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
			printf("[TCP][GameServer]Start Failed\n");
			return;
		}
		UDP->Start(BindIP.c_str(), BindUdpPort, CM_UNICAST, nullptr);
		StartQQServer(QQServerBindIP, QQServerBindTcpPort);

		/*char ch[4096];*/
		do
		{
			/*fputs(">", stdout);
			fgets(ch, 4096, stdin);
			if (_stricmp(ch, "Room\n") == 0)
			{
				PrintRoom();
			}
			else if(_stricmp(ch, "TestRefit\n") == 0)
			{
				for (size_t i = 0; i < 6; i++)
				{
					HANDLE hThread = CreateThread(NULL, 0, TestRefit, (LPVOID)NULL, 0, NULL);
					if (hThread)
					{
						CloseHandle(hThread);
					}
				}
			}*/
		} while (true);//while (_stricmp(ch, "Exit\n") != 0);
		//fputs("Bye\n", stdout);
		
		UDP->Stop();
		TCP->Stop();
	}

private:
	virtual EnHandleResult OnPrepareListen(ITcpServer* pSender, SOCKET soListen)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
		printf("[TCP][GameServer]Start Success\n");

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED);
#ifdef ZingSpeed
		printf("---- Usage steps ----\n");
		printf("  1.Go to the game directory\n");
		printf("  2.Edit the DirSvrInfo1.ini file to look like this:\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
		printf("[ServerInfo1]\nIP=%s\nPorts=%d,%d,%d\nServerID=1\n\n", IP.c_str(), TcpPort, TcpPort, TcpPort);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED);

		printf("  3.Edit the QQSvrInfo.ini file to look like this:\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
		printf("[ServerInfo1]\nIP=%s\nPorts=%d,%d,%d\n\n", QQServerIP.c_str(), QQServerTcpPort, QQServerTcpPort, QQServerTcpPort);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED);

		printf("  4.Replace ZingSpeedLauncher.exe file\n");
		printf("  5.Run ZingSpeedLauncher.exe to launch the game\n");
#else
		printf("  ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("  ■■■　　　　　■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("  ■■　　　■　　　■■■　　■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("  ■■　　　■　　　■■■　　■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("  ■■　　　■■■■■■■　　■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("  ■■　　　　　■■■　　　　　　　■■　　　　　　■■　　　　　　■■　　　　　　　■\n");
		printf("  ■■■■　　　　■■■■　　■■■■　　　■■　　　■　　　　■■■■　　　■　　　■\n");
		printf("  ■■■■■■　　　■■■　　■■■■■■■　　　　　■　　　■■■■■　　　　■■■■\n");
		printf("  ■　　　■■■　　　■■　　■■■■■　　　　　　　■　　　■■■■■■■　　　　　■\n");
		printf("  ■■　　　■■　　　■■　　■■■■　　　■■　　　■　　　■■■■■　　　■　　　■\n");
		printf("  ■■　　　　　　　■■■　　　　　■　　　■　　　　■　　　■■■■■　　　■　　　■\n");
		printf("  ■■■　　　　　■■■■■　　　　■■　　　　　　　■　　　■■■■■■　　　　　■■\n");
		printf("  ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("  --------------------------------------------------------------------------------------\n");
		printf("  日志及使用声明\n");
		printf("  1.数据库里的VipLevel是贵族等级.\n\n");
		printf("  2.BaseInfo里的Type除了车子和车皮肤是1其他全部填0,否则开游戏就会崩游戏.\n\n");
		printf("  3.CDK兑换现在有问题,SQL3数据库的问题解决不了.\n\n");
		printf("  4.至于服务端都修了什么自己看吧,懒得说明了.\n\n");
		printf("  本程序仅限学习研究使用,请在下载24小时内删除.\n");
		printf("  如若使用者违反其中国法律法规,使用者则承担其法律责任.\n");
		printf("  --------------------------------------------------------------------------------------\n");
#endif

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

		LoadMapConfig();
		LoadItemConfig();
		LoadPetLevelConfig();
		InitDB();
		InitChatRoom();
		return HR_OK;
	}

	virtual EnHandleResult OnAccept(ITcpServer* pSender, CONNID dwConnID, UINT_PTR soClient)
	{
		printf("[GameServer][ConnID:%d]OnAccept\n", dwConnID);

		ClientNode* Client = new ClientNode;
		memset(Client, 0, sizeof(ClientNode));
		Client->Server = pSender;
		Client->ConnID = dwConnID;
		memcpy(Client->Key, "hi!!Fish!!gogogo", 16);//hi!!Fish!!gogogo
		Client->KartID = 10020;

		pSender->SetConnectionExtra(dwConnID, Client);
		return HR_OK;
	}

	virtual EnHandleResult OnHandShake(ITcpServer* pSender, CONNID dwConnID)
	{
		//printf("[GameServer][ConnID:%d]OnHandShake\n", dwConnID);
		return HR_IGNORE;
	}

	virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
	{
		ClientNode* Client;
		pSender->GetConnectionExtra(dwConnID, (PVOID*)&Client);
		//printf("[GameServer][ConnID:%d]OnReceive\n", dwConnID);

		long left;
		long need;
		const int MaxBuf = 8192;
		void* p;
		do
		{
			if (Client->BufferPtr)
			{
				if (Client->BufferOffset < sizeof(int))
				{
					left = min(iLength, int(sizeof(int) - Client->BufferOffset));
					memcpy((void*)(Client->BufferPtr + Client->BufferOffset), pData, left);
					Client->BufferOffset += left;
					if (Client->BufferOffset < sizeof(int))
					{
						break;
					}
					pData += left;
					iLength -= left;
				}
				need = ntohl(*(u_long*)Client->BufferPtr);
				left = need - Client->BufferOffset;
				if (need > Client->BufferSize)
				{
					if(need > 65535) //If the data packet sent by the client is too large, it will not be processed.
					{
						Client->Server->Disconnect(Client->ConnID);
						break;
					}
					if (left > MaxBuf)
					{
						left = max(iLength, MaxBuf);
						Client->BufferSize = Client->BufferOffset + left;
					}
					else
					{
						Client->BufferSize = need;
					}
					p = realloc((void*)Client->BufferPtr, Client->BufferSize);
					if (!p)
					{
						printf("内存不足\n");
						TCP->Stop();
						break;
					}
					Client->BufferPtr = (ULONG_PTR)p;
				}
				if (left >= 0)
				{
					left = min(iLength, left);
					memcpy((void*)(Client->BufferPtr + Client->BufferOffset), pData, left);
					Client->BufferOffset += left;
					if (Client->BufferOffset < need)
					{
						break;
					}
					OnFullReceive(Client, (BYTE*)Client->BufferPtr, need);
					pData += left;
					iLength -= left;
				}
				free((void*)Client->BufferPtr);
				Client->BufferPtr = 0;
				Client->BufferOffset = 0;
			}
			while (iLength > 0)
			{
				if (iLength < sizeof(int))
				{
					need = sizeof(int);
				}
				else
				{
					need = ntohl(*(u_long*)pData);
				}
				if (need > iLength)
				{
					if (need > MaxBuf)
					{
						Client->BufferSize = max(iLength, MaxBuf);
					}
					else
					{
						Client->BufferSize = need;
					}
					p = malloc(Client->BufferSize);
					if (!p)
					{
						printf("内存不足\n");
						TCP->Stop();
						break;
					}
					Client->BufferPtr = (ULONG_PTR)p;
					memcpy((void*)Client->BufferPtr, pData, iLength);
					Client->BufferOffset = iLength;
					break;
				}
				else
				{
					OnFullReceive(Client, (BYTE*)pData, need);
					pData += need;
					iLength -= need;
				}
			}
		} while (false);
		return HR_OK;
	}

	virtual EnHandleResult OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
	{
		//printf("[GameServer][ConnID:%d]OnSend\n", dwConnID);
		return HR_OK;
	}

	virtual EnHandleResult OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
	{
		ClientNode* Client;
		pSender->GetConnectionExtra(dwConnID, (PVOID*)&Client);
		printf("[GameServer][ConnID:%d]OnClose\n", dwConnID);

		LeaveRoom(Client, 0);
		LeaveOutdoor(Client);

		if (Client->BufferPtr)
		{
			free((PVOID)Client->BufferPtr);
		}
		DelClient(Client->Uin);
		delete Client;
		return HR_OK;
	}

	virtual EnHandleResult OnShutdown(ITcpServer* pSender)
	{
		printf("[GameServer]Close\n");

		CloseDB();
		return HR_OK;
	}
private:
	virtual EnHandleResult OnPrepareListen(IUdpNode* pSender, SOCKET soListen)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
		printf("[UDP]Start Success\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		return HR_OK;
	}
	virtual EnHandleResult OnSend(IUdpNode* pSender, LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pData, int iLength)
	{
		return HR_OK;
	}
	virtual EnHandleResult OnReceive(IUdpNode* pSender, LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pData, int iLength)
	{
		TransferHead* ProtocolHead = (TransferHead*)pData;
		if (iLength < sizeof(TransferHead) && ntohs(ProtocolHead->TotalLength) > iLength)
		{
			return HR_OK;
		}
		UINT Seq = ntohl(ProtocolHead->Seq);
		short DialogID = ntohs(ProtocolHead->DialogID);
		UINT Uin = ntohl(ProtocolHead->Uin);
		short TransferCmd = ntohs(ProtocolHead->TransferCmd);
		UCHAR OptLength = ProtocolHead->OptLength;
#ifdef DEBUG
		printf("[UDP:OnReceive]Uin:%d, TransferCmd:%d, OptLength:%d\n", Uin, TransferCmd, OptLength);
#endif
		size_t len = 0;
		BYTE* p = PBYTE(ProtocolHead + 1);
		UCHAR option[127];
		switch (TransferCmd)
		{
		case UDP_CMD_LOGIN_SERVER:
		{
			in_addr LocalIP;
			LocalIP.S_un.S_addr = Read32(p);
			USHORT LocalPort = Read16(p);
#ifdef DEBUG
			printf("LocalIP:%s, LocalPort:%d\n", inet_ntoa(LocalIP), LocalPort);
#endif



			p = option;
			
			Write32(p, inet_addr(lpszRemoteAddress));

			len = p - option;
			//SendUdpData(lpszRemoteAddress, usRemotePort, UDP_CMD_LOGIN_SERVER, option, len, Seq, DialogID, Uin);
			break;
		}

		case UDP_CMD_P2P_GAME_DATA:
		{
			//while (OptLength > 0)
			//{
			//	short PlayerID = Read16(p);
			//	UINT PlayerUin = Read32(p);
			//	UINT Time = Read32(p);
			//	printf("PlayerID:%d, PlayerUin:%d, Time:%d\n", PlayerID, PlayerUin, Time);
			//	OptLength -= 10;
			//}
			break;
		}
		case UDP_CMD_SHOW_MY_IP_PORT:
		{
			short SrcPlayerID = Read16(p); //src player id
			UINT SrcUin = Read32(p); //src player uin
			in_addr LocalIP;
			LocalIP.S_un.S_addr = Read32(p);
			USHORT LocalPort = Read16(p);
#ifdef DEBUG
			printf("SrcPlayerID:%d, SrcUin:%d\n", SrcPlayerID, SrcUin);
#endif
			p = option;
			Write16(p, SrcPlayerID); //SrcPlayerID
			Write32(p, SrcUin); //SrcUin
			Write32(p, LocalIP.S_un.S_addr); //SrcOuterIP
			Write16(p, LocalPort); //SrcOuterPort
			Write32(p, inet_addr(lpszRemoteAddress)); //SrcInerIP
			Write16(p, usRemotePort); //SrcInnerPort


			len = p - option;
			//SendUdpData(lpszRemoteAddress, usRemotePort, UDP_CMD_RECV_OTHER_IP_PORT, option, len, Seq, DialogID, Uin);
			break;
		}
		case UDP_CMD_HEART_BEAT:
		{
			p = option;

			len = p - option;
			SendUdpData(lpszRemoteAddress, usRemotePort, UDP_CMD_HEART_BEAT, option, len, Seq, DialogID, Uin);
			break;
		}
		default:
			break;
		}


		return HR_OK;
	}
	virtual EnHandleResult OnError(IUdpNode* pSender, EnSocketOperation enOperation, int iErrorCode, LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE* pBuffer, int iLength)
	{
		return HR_OK;
	}
	virtual EnHandleResult OnShutdown(IUdpNode* pSender)
	{
		return HR_OK;
	}

};

int main()
{
#ifdef ZingSpeed
	SetConsoleTitle(TEXT("ZingSpeedServer"));
#else
	SetConsoleTitle(TEXT("QQSpeedServer"));
	CONSOLE_CURSOR_INFO cursor_info = { 1, 0 };
#endif
	GameServer Server;
	Server.Run();
	return 0;
}
