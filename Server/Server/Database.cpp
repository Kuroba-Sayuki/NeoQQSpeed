#include "Database.h"
#include "Item.h"


#include "sqlite/sqlite3.h"
#pragma comment (lib,"sqlite3.lib")


sqlite3* UserDB;
sqlite3* PlayerDB;

void InitDB()
{
	sqlite3_config(SQLITE_CONFIG_SERIALIZED);
	sqlite3_initialize();

	//打开数据库
	sqlite3_open_v2("User.db", &UserDB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL);
	sqlite3_open_v2("Player.db", &PlayerDB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE |  SQLITE_OPEN_FULLMUTEX, NULL);

	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

	sql = "BEGIN"; //开始事务
	result = sqlite3_exec(UserDB, sql, NULL, NULL, NULL);


	//创建 用户表
	sql = "CREATE TABLE User (Uin INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, Name TEXT UNIQUE NOT NULL, Password TEXT, mail TEXT, Ban TEXT, HWID TEXT);";
	result = sqlite3_exec(UserDB, sql, NULL, NULL, NULL);
	//sql = "update sqlite_sequence set seq = 10000 where name = 'User'"; //Uin必须大于10000
	//result = sqlite3_exec(UserDB, sql, NULL, NULL, NULL);
	// 
	//创建 封禁表
	sql = "CREATE TABLE HWID (HWID TEXT);";
	result = sqlite3_exec(UserDB, sql, NULL, NULL, NULL);

	sql = "COMMIT"; //提交事务
	result = sqlite3_exec(UserDB, sql, NULL, NULL, NULL);


	sql = "BEGIN"; //开始事务
	result = sqlite3_exec(PlayerDB, sql, NULL, NULL, NULL);

	//创建 玩家表
	sql = R"(CREATE TABLE Player (
Uin INTEGER  PRIMARY KEY UNIQUE NOT NULL,
IsInTopList BOOLEAN  NOT NULL DEFAULT(0),
VipFlag INTEGER  NOT NULL DEFAULT(0),
ExFlag INTEGER  NOT NULL DEFAULT(0),
LastLoginTime INTEGER  NOT NULL DEFAULT(0),
VipLevel1 INTEGER  NOT NULL DEFAULT(1),
VipLevel2 INTEGER  NOT NULL DEFAULT(0),
VipLevel3 INTEGER  NOT NULL DEFAULT(0),
VipLevel4 INTEGER  NOT NULL DEFAULT(0),
VipLevel5 INTEGER  NOT NULL DEFAULT(0)
);

CREATE TABLE BaseInfo (
	Uin INTEGER  PRIMARY KEY REFERENCES Player (Uin) UNIQUE NOT NULL,
	NickName TEXT  UNIQUE NOT NULL,
	Gender CHAR  NOT NULL DEFAULT(0),
	Country CHAR  NOT NULL DEFAULT(0),
	License INTEGER  NOT NULL DEFAULT(4),
	Experience INTEGER  NOT NULL DEFAULT(0),
	SuperMoney INTEGER  NOT NULL DEFAULT(0),
	Money INTEGER  NOT NULL DEFAULT(30),
	WinNum INTEGER  NOT NULL DEFAULT(0),
	SecondNum INTEGER  NOT NULL DEFAULT(0),
	ThirdNum INTEGER  NOT NULL DEFAULT(0),
	TotalNum INTEGER  NOT NULL DEFAULT(0),
	CurHonor INTEGER  NOT NULL DEFAULT(0),
	TotalHonor INTEGER  NOT NULL DEFAULT(0),
	TodayHonor INTEGER  NOT NULL DEFAULT(0),
	RelaxTime INTEGER  NOT NULL DEFAULT(0),
	MonthDurationBefor INTEGER  NOT NULL DEFAULT(0),
	MonthDurationCur INTEGER  NOT NULL DEFAULT(0),
	Charm INTEGER  NOT NULL DEFAULT(0),
	DurationGame INTEGER  NOT NULL DEFAULT(0),
	DanceExp INTEGER  NOT NULL DEFAULT(0),
	Coupons INTEGER  NOT NULL DEFAULT(0),
	Admiration INTEGER  NOT NULL DEFAULT(0),
	LuckMoney INTEGER  NOT NULL DEFAULT(0),
	TeamWorkExp INTEGER  NOT NULL DEFAULT(0),
	AchievePoint INTEGER  NOT NULL DEFAULT(0),
	RegTime INTEGER  NOT NULL  DEFAULT(strftime('%s', 'now')),
	Signature TEXT  NOT NULL DEFAULT('')
);

CREATE TABLE Item (
	Uin            INTEGER  REFERENCES Player (Uin) NOT NULL,
	ItemID         INTEGER  NOT NULL,
	ItemNum        INTEGER  NOT NULL DEFAULT(1),
	AvailPeriod    INTEGER  NOT NULL DEFAULT(-1),
	Status         BOOLEAN  NOT NULL DEFAULT(0),
	ObtainTime     INTEGER  NOT NULL DEFAULT(strftime('%s', 'now')),
	OtherAttribute INTEGER  NOT NULL DEFAULT(0),
	Type INTEGER  NOT NULL DEFAULT(0)
);

CREATE TABLE MapRecord (
	Uin				INTEGER  REFERENCES Player (Uin) NOT NULL,
	MapID			INTEGER  NOT NULL,
	Record			INTEGER  NOT NULL DEFAULT(-1),
	LastUpdateTime	INTEGER  NOT NULL DEFAULT(0)
);

CREATE TABLE Pet (
	Uin				INTEGER  REFERENCES Player (Uin) NOT NULL,
	PetId			INTEGER,
	Name			TEXT  UNIQUE,
	Experience	INTEGER,
	PL			INTEGER
);

CREATE TABLE KartStoneGroove (
	Uin					INTEGER  REFERENCES Player (Uin) NOT NULL,
	KartID				INTEGER  NOT NULL,
	ID					INTEGER  NOT NULL PRIMARY KEY AUTOINCREMENT
);

CREATE TABLE KartStone (
	ID					INTEGER  REFERENCES KartStoneGroove (ID) NOT NULL,
	StoneGrooveID		INTEGER  NOT NULL,
	StoneUseOccaType	INTEGER  NOT NULL,
	SkillStoneID		INTEGER  NOT NULL DEFAULT(0)
);

CREATE TABLE KartRefit (
	Uin						INTEGER  REFERENCES Player (Uin) NOT NULL,
	KartID					INTEGER  NOT NULL,
	RefitCout				INTEGER  NOT NULL DEFAULT(0),
	MaxFlags				INTEGER  NOT NULL DEFAULT(0),
	WWeight					INTEGER  NOT NULL DEFAULT(0),
	SpeedWeight				INTEGER  NOT NULL DEFAULT(0),
	JetWeight				INTEGER  NOT NULL DEFAULT(0),
	SJetWeight				INTEGER  NOT NULL DEFAULT(0),
	AccuWeight				INTEGER  NOT NULL DEFAULT(0),
	ShapeRefitCount			INTEGER  NOT NULL DEFAULT(0),
	KartHeadRefitItemID		INTEGER  NOT NULL DEFAULT(0),
	KartTailRefitItemID		INTEGER  NOT NULL DEFAULT(0),
	KartFlankRefitItemID	INTEGER  NOT NULL DEFAULT(0),
	KartTireRefitItemID		INTEGER  NOT NULL DEFAULT(0)
);
)";
	result = sqlite3_exec(PlayerDB, sql, NULL, NULL, NULL);


	sql = "COMMIT"; //提交事务
	result = sqlite3_exec(PlayerDB, sql, NULL, NULL, NULL);
}

void CloseDB()
{
	const char* sql = NULL;
	int result;

	sql = "VACUUM";
	result = sqlite3_exec(PlayerDB, sql, NULL, NULL, NULL);
	sqlite3_close(PlayerDB);

	sql = "VACUUM";
	result = sqlite3_exec(UserDB, sql, NULL, NULL, NULL);
	sqlite3_close(UserDB);

	sqlite3_shutdown();
}


BOOL UserDB_Register(const char* Username, const char* Password)
{
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

	sql = "INSERT INTO User (Name, Password) VALUES(?, ?)";
	result = sqlite3_prepare_v2(UserDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, Username, strlen(Username), SQLITE_TRANSIENT);
		sqlite3_bind_blob(stmt, 2, Password, strlen(Password), SQLITE_TRANSIENT);
		result = sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	return result == SQLITE_DONE;
}

DWORD UserDB_GetUin(const char* Username)
{
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	int uin = 0;

	sql = "SELECT Uin, Password  FROM User  WHERE Name=?;";
	result = sqlite3_prepare_v2(UserDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, Username, strlen(Username), SQLITE_TRANSIENT);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			uin = sqlite3_column_int(stmt, 0);
			uin += 10000; //必须大于10000,否则会出问题
		}
		else
		{
			//未注册
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	return uin;
}

BOOL UserDB_CmpPassword(DWORD Uin, const char* Password)
{
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	BOOL ret = FALSE;

	sql = "SELECT Password  FROM User  WHERE Uin=?;";
	result = sqlite3_prepare_v2(UserDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Uin - 10000);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			char* pwd = (char*)sqlite3_column_blob(stmt, 0);
			if (strcmp(pwd, Password) == 0)
			{
				ret = TRUE;
			}
		}
		else
		{
			//未注册
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	return ret;
}

void PlayerDB_AddItem(DWORD Uin, DWORD ItemID, DWORD ItemNum, DWORD AvailPeriod, bool Status,int Type)
{
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

	sql = "INSERT INTO Item (Uin,ItemID,ItemNum,AvailPeriod,Status,Type) VALUES (?,?,?,?,?,?);";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Uin);
		sqlite3_bind_int(stmt, 2, ItemID);
		sqlite3_bind_int(stmt, 3, ItemNum);
		sqlite3_bind_int(stmt, 4, AvailPeriod);
		sqlite3_bind_int(stmt, 5, Status);
		sqlite3_bind_int(stmt, 6, Type);
		result = sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	if (GetItemType(ItemID) == EAIT_CAR)
	{
		sql = "INSERT INTO KartStoneGroove (Uin,KartID) VALUES (?,?);";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, Uin);
			sqlite3_bind_int(stmt, 2, ItemID);
			result = sqlite3_step(stmt);
		}
		sqlite3_finalize(stmt);
		stmt = NULL;

		//宝石信息
		int ID = 0;
		sql = "SELECT ID  FROM KartStoneGroove  WHERE Uin=? AND KartID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, Uin);
			sqlite3_bind_int(stmt, 2, ItemID);
			result = sqlite3_step(stmt);
			if (result == SQLITE_ROW)
			{
				ID = sqlite3_column_int(stmt, 0);
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;

		//分配2个宝石槽
		for (int StoneGrooveID = 1; StoneGrooveID <= 2; StoneGrooveID++)
		{
			sql = "INSERT INTO KartStone (ID,StoneGrooveID,StoneUseOccaType) VALUES (?,?,?);";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK) {
				sqlite3_bind_int(stmt, 1, ID);
				sqlite3_bind_int(stmt, 2, StoneGrooveID);
				sqlite3_bind_int(stmt, 3, 0); //圆形
				result = sqlite3_step(stmt);
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
		}

		//改装信息
		sql = "INSERT INTO KartRefit(Uin,KartID) VALUES (?,?);";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, Uin);
			sqlite3_bind_int(stmt, 2, ItemID);
			result = sqlite3_step(stmt);
		}
		sqlite3_finalize(stmt);
		stmt = NULL;

	}
}
ItemInfo PlayerDB_AddItem2(DWORD Uin, DWORD ItemID, DWORD ItemNum, int AvailPeriod, bool Status)
{
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	bool exists = false;
	ItemInfo Item = {};
	Item.ItemID = ItemID;
	sql = "SELECT ItemNum,AvailPeriod,Status,ObtainTime,OtherAttribute FROM Item WHERE Uin=? AND ItemID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Uin);
		sqlite3_bind_int(stmt, 2, ItemID);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			Item.ItemNum = sqlite3_column_int(stmt, 0);
			Item.AvailPeriod = sqlite3_column_int(stmt, 1);
			Item.Status = sqlite3_column_int(stmt, 2);
			Item.ObtainTime = sqlite3_column_int(stmt, 3);
			Item.OtherAttribute = sqlite3_column_int(stmt, 4);
			Item.ItemType = 0;
			exists = true;
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	if (Item.AvailPeriod >= 1 && Item.AvailPeriod != -1) { // 检查 AvailItem 是否 >= 1 且条件不是 -1
		int newAvailPeriod = AvailPeriod;
		if (AvailPeriod == -1) {
			newAvailPeriod = 0;
		}
		else if (Item.AvailPeriod > 1) {
			sql = "SELECT AvailPeriod FROM Item WHERE Uin=? AND ItemID=?";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK) {
				sqlite3_bind_int(stmt, 1, Uin);
				sqlite3_bind_int(stmt, 2, ItemID);
				result = sqlite3_step(stmt);
				if (result == SQLITE_ROW) {
					int currentAvailPeriod = sqlite3_column_int(stmt, 0);
					newAvailPeriod = currentAvailPeriod;
				}
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
		}

		sql = "UPDATE Item SET AvailPeriod=? WHERE Uin=? AND ItemID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, newAvailPeriod);
			sqlite3_bind_int(stmt, 2, Uin);
			sqlite3_bind_int(stmt, 3, ItemID);
			result = sqlite3_step(stmt);
		}
	}
	if (exists) {
		if (Item.AvailPeriod == 0) {
			//增加数量
			sql = "UPDATE Item SET ItemNum=ItemNum+? WHERE Uin=? AND ItemID=?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK) {
				sqlite3_bind_int(stmt, 1, ItemNum);
				sqlite3_bind_int(stmt, 2, Uin);
				sqlite3_bind_int(stmt, 3, ItemID);
				sqlite3_step(stmt);
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
			Item.ItemNum += ItemNum;
		}
		else if (Item.AvailPeriod > 0) {
			//增加有效期
			sql = "UPDATE Item SET AvailPeriod=AvailPeriod+? WHERE Uin=? AND ItemID=?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK) {
				sqlite3_bind_int(stmt, 1, AvailPeriod);
				sqlite3_bind_int(stmt, 2, Uin);
				sqlite3_bind_int(stmt, 3, ItemID);
				sqlite3_step(stmt);
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
			Item.AvailPeriod += AvailPeriod;
		}
		else
		{
			//重复获取相同永久物品 暂不处理
		}
	}
	else
	{
		Item.ItemNum = ItemNum;
		Item.AvailPeriod = AvailPeriod;
		Item.Status = Status;
		Item.ObtainTime = time(nullptr);
		Item.OtherAttribute = 0;
		Item.ItemType = 0;

		sql = "INSERT INTO Item (Uin,ItemID,ItemNum,AvailPeriod,Status) VALUES (?,?,?,?,?);";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, Uin);
			sqlite3_bind_int(stmt, 2, ItemID);
			sqlite3_bind_int(stmt, 3, ItemNum);
			sqlite3_bind_int(stmt, 4, AvailPeriod);
			sqlite3_bind_int(stmt, 5, Status);
			result = sqlite3_step(stmt);
		}
		sqlite3_finalize(stmt);
		stmt = NULL;

		if (GetItemType(ItemID) == EAIT_CAR)
		{
			sql = "INSERT INTO KartStoneGroove (Uin,KartID) VALUES (?,?);";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK) {
				sqlite3_bind_int(stmt, 1, Uin);
				sqlite3_bind_int(stmt, 2, ItemID);
				result = sqlite3_step(stmt);
			}
			sqlite3_finalize(stmt);
			stmt = NULL;

			//宝石信息
			int ID = 0;
			sql = "SELECT ID  FROM KartStoneGroove  WHERE Uin=? AND KartID=?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK) {
				sqlite3_bind_int(stmt, 1, Uin);
				sqlite3_bind_int(stmt, 2, ItemID);
				result = sqlite3_step(stmt);
				if (result == SQLITE_ROW)
				{
					ID = sqlite3_column_int(stmt, 0);
				}
			}
			sqlite3_finalize(stmt);
			stmt = NULL;

			//分配2个宝石槽
			for (int StoneGrooveID = 1; StoneGrooveID <= 2; StoneGrooveID++)
			{
				sql = "INSERT INTO KartStone (ID,StoneGrooveID,StoneUseOccaType) VALUES (?,?,?);";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK) {
					sqlite3_bind_int(stmt, 1, ID);
					sqlite3_bind_int(stmt, 2, StoneGrooveID);
					sqlite3_bind_int(stmt, 3, 0); //圆形
					result = sqlite3_step(stmt);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
			}

			//改装信息
			sql = "INSERT INTO KartRefit(Uin,KartID) VALUES (?,?);";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK) {
				sqlite3_bind_int(stmt, 1, Uin);
				sqlite3_bind_int(stmt, 2, ItemID);
				result = sqlite3_step(stmt);
			}
			sqlite3_finalize(stmt);
			stmt = NULL;

		}
	}
	return Item;
}
void PlayerDB_UpdateItem(DWORD Uin, DWORD ItemID, DWORD ItemNum)
{
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

	sql = "UPDATE Item SET ItemNum=?  WHERE Uin=? AND ItemID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, ItemNum);
		sqlite3_bind_int(stmt, 2, Uin);
		sqlite3_bind_int(stmt, 3, ItemID);
		result = sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	
}

void PlayerDB_UpdateKartRefitItemStatus(DWORD Uin, DWORD ItemID, DWORD status) {
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

	sql = "UPDATE Item SET Status=?  WHERE Uin=? AND ItemID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, status);
		sqlite3_bind_int(stmt, 2, Uin);
		sqlite3_bind_int(stmt, 3, ItemID);
		result = sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
}

DWORD PlayerDB_GetMapRecord(DWORD Uin, int MapID)
{
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

	int ret = -1;
	sql = "SELECT Record  FROM MapRecord  WHERE Uin=? AND MapID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Uin);
		sqlite3_bind_int(stmt, 2, MapID);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			ret = sqlite3_column_int(stmt, 0);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	return ret;
}

int PlayerDB_GetItemNum(DWORD Uin, DWORD ItemID)
{
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

	int ret = -1;
	sql = "SELECT ItemNum  FROM Item  WHERE Uin=? AND ItemID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Uin);
		sqlite3_bind_int(stmt, 2, ItemID);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			ret = sqlite3_column_int(stmt, 0);
		}
		else
		{
			ret = 0;

		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	return ret;

}

bool PlayerDB_SetMapRecord(DWORD Uin, int MapID, DWORD Record)
{
	bool NewRecord = true;
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

	sql = "SELECT Record  FROM MapRecord  WHERE Uin=? AND MapID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Uin);
		sqlite3_bind_int(stmt, 2, MapID);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			if ((DWORD)sqlite3_column_int(stmt, 0) < Record)
			{
				NewRecord = false;
			}
			else
			{
				sqlite3_finalize(stmt);
				stmt = NULL;
				sql = "UPDATE MapRecord SET Record=?  WHERE Uin=? AND MapID=?;";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK) {
					sqlite3_bind_int(stmt, 1, Record);
					sqlite3_bind_int(stmt, 2, Uin);
					sqlite3_bind_int(stmt, 3, MapID);
					result = sqlite3_step(stmt);
				}
			}
		}
		else
		{
			sqlite3_finalize(stmt);
			stmt = NULL;
			sql = "INSERT INTO MapRecord (Uin, MapID, Record) VALUES(?, ?, ?)";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK) {
				sqlite3_bind_int(stmt, 1, Uin);
				sqlite3_bind_int(stmt, 2, MapID);
				sqlite3_bind_int(stmt, 3, Record);
				result = sqlite3_step(stmt);
			}
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	return NewRecord;
}



BOOL PlayerDB_DeleteItem(DWORD Uin, int ItemID)
{
	sqlite3_stmt* stmt = NULL;
	BOOL result = FALSE;

	if (sqlite3_prepare_v2(PlayerDB, "DELETE FROM Item  WHERE  Uin=? AND ItemID = ?;", -1, &stmt, NULL) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Uin);
		sqlite3_bind_int(stmt, 2, ItemID);

		if (sqlite3_step(stmt) == SQLITE_DONE)
		{
			result = TRUE;
		}
	}

	sqlite3_finalize(stmt);
	stmt = NULL;

	return result;
}

void PlayerDB_AddStoneGroove(ClientNode* Client, int KartID, int StoneGrooveType)
{
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

	int ID = -1;
	int StoneGrooveID = -1;
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
	else
	{
		sql = "SELECT StoneGrooveID FROM KartStone WHERE ID = ? ORDER BY StoneGrooveID ASC;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, ID);
			while (sqlite3_step(stmt) == SQLITE_ROW)
			{ //StoneGrooveInfo
				StoneGrooveID = sqlite3_column_int(stmt, 0);
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;

		if (StoneGrooveID > -1) {
			StoneGrooveID++;
			sql = "INSERT INTO KartStone (ID,StoneGrooveID,StoneUseOccaType) VALUES (?,?,?);";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK) {
				sqlite3_bind_int(stmt, 1, ID);
				sqlite3_bind_int(stmt, 2, StoneGrooveID);
				sqlite3_bind_int(stmt, 3, StoneGrooveType);
				result = sqlite3_step(stmt);
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
	}
}

int PlayerDB_RemoveSkillStone(ClientNode* Client, int KartID, int StoneGrooveID)
{
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

	int ID = -1;
	int SkillStoneID = -1;
	sql = "SELECT ID FROM KartStoneGroove WHERE Uin=? AND KartID=?;";
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
		return -1;
	}
	else
	{
		sql = "SELECT SkillStoneID FROM KartStone WHERE ID=? AND StoneGrooveID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, ID);
			sqlite3_bind_int(stmt, 2, StoneGrooveID);
			result = sqlite3_step(stmt);
			if (result == SQLITE_ROW)
			{
				SkillStoneID = sqlite3_column_int(stmt, 0);
			}
		}
		if (SkillStoneID > -1) {
			sqlite3_finalize(stmt);
			stmt = NULL;
			sql = "UPDATE KartStone SET SkillStoneID=0 WHERE ID=? AND StoneGrooveID=?";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK) {
				sqlite3_bind_int(stmt, 1, ID);
				sqlite3_bind_int(stmt, 2, StoneGrooveID);
				sqlite3_step(stmt);
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
	}
	return SkillStoneID;
}