/*
 * DatabaseBridge.h
 *
 *  Created on: 2018年5月22日
 *      Author: 171104
 */

#ifndef SRC_APP_DATABASEBRIDGE_H_
#define SRC_APP_DATABASEBRIDGE_H_

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <cstdio>
#include <sstream>
#include <fstream>

#include "Poco/Util/AbstractConfiguration.h"
#include "Poco/Util/IniFileConfiguration.h"
#include "Poco/Data/RecordSet.h"
#include "Poco/Data/Session.h"
#include "Poco/Data/ODBC/Connector.h"
#include "Poco/Data/SQLite/Connector.h"
#include "Poco/Data/MySQL/MySQLException.h"
#include "Poco/Data/MySQL/Connector.h"
#include "Poco/Exception.h"
#include "Poco/Delegate.h"
#include "Poco/Thread.h"
#include "Poco/Redis/AsyncReader.h"
#include "Poco/Redis/Command.h"
#include "Poco/Redis/PoolableConnectionFactory.h"
#include "Poco/Timer.h"
#include "Poco/Thread.h"
#include "Poco/ThreadLocal.h"
#include "Poco/Stopwatch.h"
#include "Poco/Logger.h"
#include "Poco/AutoPtr.h"
#include "Poco/PatternFormatter.h"
#include "Poco/FormattingChannel.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/FileChannel.h"
#include "Poco/Message.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/Path.h"
#include "Poco/File.h"
#include "Poco/StringTokenizer.h"

#include "def.h"

using namespace std;
using namespace Poco;
using namespace Poco::Util;
using namespace Poco::Redis;
using namespace Poco::Data;
using namespace Poco::Data::Keywords;

struct RFIDTAG
{
	std::string TagID;
	std::string TargetNo;
	std::string TagType;
};

class MESBridge
{
public:
	MESBridge();
	virtual ~MESBridge();
	std::string GotTargetNo(std::string TagID);
	void RFIDCloneTimer(Poco::Timer& timer);
	bool TOTAL_HOLES(std::string LotNO, std::string ProcSeq, std::string &TOTAL_HOLES);
	bool getRD05M136(std::string ERP_ITEMNO, std::string ERP_ITEMVER, std::string MFVER, float& RD05M136); /*取得當站板厚*/

private:
	Logger& logger;
	AutoPtr<IniFileConfiguration> pconfig;
	const std::string ERP_CONNECTION_STRING;
	const std::string RFIDDB_CONNECTION_STRING;
	const std::string MESSERIES_CONNECTION_STRING;
protected:
	std::vector<recipe> Recipes;
};

class ClientBridge{
public:
	ClientBridge(std::string db);
	virtual ~ClientBridge();
//	bool Insert(std::string insert);
//	bool isExist(std::string query);
private:
	Logger& logger;
	Session* session;
	AutoPtr<IniFileConfiguration> pconfig;
protected:
};

class RedisBridge
{
public:
	RedisBridge(std::string host, int port);
	virtual ~RedisBridge();

	void cmd(std::vector<std::string> cmds);
	bool set(std::string key, std::string value);
	std::string get(std::string key);
	bool del(std::string key);

	bool sadd(std::string table, std::string key); //新增key
	bool sismember(std::string table, std::string key); //檢查key是否存在
	bool smembers(std::string table, std::vector<std::string> &keys); //擷取所有的value
	bool srem(std::string table, std::string key); //刪除key

	bool hmset(std::string table, std::map<std::string, std::string > fields);
	bool hgetall(std::string table, std::map<std::string, std::string> &fields);
private:
	Logger& logger;
	std::string _host;
	int _port;
	AutoPtr<IniFileConfiguration> pconfig;
	Redis::Client _redis;
protected:
};

class MyBridge{
public:
	MyBridge();
	virtual ~MyBridge();
	bool QueryLast(int &id);
	bool InsertMO(std::map<std::string, std::string> detailMO);
	void handleAlarm(AlarmNotification* pNf);
//	void UploadMO(Poco::Timer& timer);
private:
	Logger& logger;
	const std::string CONNECTION_STRING;
	std::set<std::string> StirngLsit;
protected:
};

class LocalBridge
{
public:
	struct ERRORmessage
	{
		std::string datetime;
		std::string message;
		std::string device;
	};
	LocalBridge(AutoPtr<AbstractConfiguration> _config);
	virtual ~LocalBridge();
	bool insertERROR(ERRORmessage message);
	bool getERROR(std::vector<ERRORmessage> &msg);
private:
	Logger& logger;
	AutoPtr<AbstractConfiguration> config;
	std::string DB;
	Path* db_path;

protected:
};

#endif /* SRC_APP_DATABASEBRIDGE_H_ */
