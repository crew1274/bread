/*
 * DatabaseBridge.cpp
 *
 *  Created on: 2018年5月22日
 *      Author: 171104
 */

#include <app/DatabaseBridge.h>

//-----------------------------------------MES--------------------------------------------------//
MESBridge::MESBridge():logger(Logger::get("MESBridge")),
RFIDDB_CONNECTION_STRING("DSN=RFID;Uid=sa;Pwd=epc54321."),
MESSERIES_CONNECTION_STRING("DSN=MESSERIES;Uid=MESSERIES;Pwd=sa")
{
	/*測試資料庫連線*/
	ODBC::Connector::registerConnector();
	try
	{
		Session RFID("ODBC", RFIDDB_CONNECTION_STRING);
		if(RFID.isConnected())
		{
			logger.information("Test connect RFID database successful");
		}
		else
		{
			logger.error("Can not connect RFID database");
		}

		Session MESSERIES("ODBC", MESSERIES_CONNECTION_STRING);
		if(RFID.isConnected())
		{
			logger.information("Test connect MESSERIES database successful");
		}
		else
		{
			logger.error("Can not connect MESSERIES database");
		}
	}
	catch(Exception& e)
	{
		logger.error(e.displayText());
	}
	ODBC::Connector::unregisterConnector();
}
MESBridge::~MESBridge()
{
	// TODO Auto-generated destructor stub
}

std::string MESBridge::GotTargetNo(std::string TagID)
{
	std::string LotNO = "";
	try
	{
		Poco::Data::ODBC::Connector::registerConnector();
		Session session("ODBC", RFIDDB_CONNECTION_STRING);
		session.setConnectionTimeout(5);
		if (session.isConnected())
		{
			logger.information("MES RFID database connected");
			Statement select(session);
			select << "SELECT TOP 1 TargetNo FROM [e0516].[dbo].[IF_V_RFID_TAGINFO] WHERE TagID = '"<< TagID <<"' ORDER BY CreateTime DESC";
			select.execute();
			RecordSet rs(select);
			bool more = rs.moveFirst();
			if(more)
			{
				LotNO = rs[0].convert<std::string>();
			}
			else
			{
				logger.error("Can not find LotNO which TagID is %s in MES RFID database ", TagID);
			}
			return LotNO;
		}
		else
		{
			logger.error("MES RFID database can not connect");
			RedisBridge rb("127.0.0.1", 6379);
			return rb.get(TagID);
		}
		Data::ODBC::Connector::unregisterConnector();
	}
	catch(Exception& e)
	{
		logger.error(e.displayText());
	}
	return LotNO;
}

void MESBridge::RFIDCloneTimer(Poco::Timer& timer)
{
	logger.information("啟動RFID clone");
	std::string TagID;
	std::string TargetNo;
	std::string TagType;
	std::string LotNo;
	RedisBridge rb("127.0.0.1", 6379);
	try
	{
		Poco::Data::ODBC::Connector::registerConnector();
		Session session("ODBC", RFIDDB_CONNECTION_STRING);
		session.setConnectionTimeout(5);
		if (session.isConnected())
		{
			logger.information("RFID DB is connected! Ready to update");
			Statement select(session);
			select << "SELECT TagID, TargetNo, TagType, LotNo From [e0516].[dbo].[IF_V_RFID_TAGINFO] ORDER BY CreateTime DESC";
			select.execute();
			RecordSet rs(select);
			// iterate over all rows and columns
			bool more = rs.moveFirst();
			while(more)
			{
				TagID = rs[0].convert<std::string>();
				TargetNo = rs[1].convert<std::string>();
				TagType = rs[2].convert<std::string>();
				LotNo = rs[3].convert<std::string>();
				if(TagType=="L") //LotNO
				{
					if(!rb.sismember("L", LotNo))
					{
						rb.sadd("L", LotNo);
						rb.set(TagID, LotNo);
					}
				}
				else if(TagType=="E") //employee
				{
					if(!rb.sismember("E", TargetNo))
					{
						rb.sadd("E", TargetNo);
						rb.set(TagID, TargetNo);
					}
				}
				more = rs.moveNext();
			}
		}
		//disconnect
		Data::ODBC::Connector::unregisterConnector();
	}
	catch (Exception& e)
	{
		logger.error(e.displayText());
	}
	logger.information("完成RFID clone");
}

bool MESBridge::TOTAL_HOLES(std::string LotNO, std::string ProcSeq, std::string &TOTAL_HOLES)
{
	ODBC::Connector::registerConnector();
	try
	{
		Session session("ODBC", MESSERIES_CONNECTION_STRING);
		if (session.isConnected())
		{
			logger.information("MES database is connected, Ready to look for TOTAL_HOLES with LotNo: %s", LotNO);
			Statement select(session);
			select << "  SELECT ATTRIBVALUE FROM [MESSERIES].[dbo].UNF_GET_WIPLOTLOG_ATTRIB ("
					"( SELECT TOP 1 LOGGROUPSERIAL FROM [MESSERIES].[dbo].UNF_GET_WIPLOTLOG('"<< LotNO << "') WHERE NODEID LIKE '%"<< ProcSeq <<"' ORDER BY 1"
					")) WHERE ATTRIBNO = 'RD05M29'";
			logger.debug(select.toString());
			select.execute();
			RecordSet rs(select);
			if(rs.moveFirst())
			{
				TOTAL_HOLES = rs[0].convert<std::string>();
				logger.information("LotNO:%s Got TOTAL_HOLES: %s", LotNO, TOTAL_HOLES);
				ODBC::Connector::unregisterConnector();
				return true;
			}
			else
			{
				logger.error("%s RecordSet is empty", LotNO);
			}
		}
		else
		{
			logger.error("Can not connect ERP database");
		}
	}
	catch(Exception& e)
	{
		logger.error(e.displayText());
	}
	ODBC::Connector::unregisterConnector();
	return false;
}

bool MESBridge::getRD05M136(std::string ERP_ITEMNO, std::string ERP_ITEMVER, std::string MFVER, float& RD05M136) /*取得當站板厚*/
{
	Poco::Data::ODBC::Connector::registerConnector();
	try
	{
		Session session("ODBC", MESSERIES_CONNECTION_STRING);
		if(!session.isConnected())
		{
			throw ApplicationException("資料庫MESSERIES連線異常");
		}
		Statement select(session);
		select << "SELECT TOP 1 ATTRIBVALUE FROM [MESSERIES].[dbo].[VW_IOT_TBLPRDOPATTRIB] "
				"WHERE ERP_ITEMNO = '" << ERP_ITEMNO << "' "
				"AND ERP_ITEMVER = '" << ERP_ITEMVER << "' "
				"AND MFVER = '" << MFVER << "' "
				"AND ATTRIBNO = 'RD05M136' AND OPNAME LIKE '壓合%' ORDER BY PRESSNO DESC ";
		select.execute();
		RecordSet rs(select);
		if(!rs.moveFirst())
		{
			throw ApplicationException("壓合站查無板厚資料");
		}
		StringTokenizer t(rs[0].convert<std::string>(), "~", StringTokenizer::TOK_TRIM | StringTokenizer::TOK_IGNORE_EMPTY);
		if(t.count() == 2)
		{
			RD05M136 = (NumberParser::parseFloat(t[0]) + NumberParser::parseFloat(t[1])) / 2;
		}
		else if(t.count() == 1)
		{
			//單邊
			Statement select_1(session);
			select_1 << "SELECT ISNULL(FGDCOPPERTHICK, '') FGDCOPPERTHICK, ISNULL(THICK, '') THICK FROM [MESSERIES].[dbo].[VW_IOT_TBLCAMPCBPRESS]"
						" WHERE ERP_ITEMNO = '" << ERP_ITEMNO << "' "
						" AND ERP_ITEMVER = '" << ERP_ITEMVER << "' "
						" AND MFVER = '" << MFVER << "'";
			logger.information(select_1.toString());
			select_1.execute();
			RecordSet result(select_1);
			logger.information("totalRowCount: %z", result.totalRowCount());
			bool more = result.moveFirst();
			double sum = 0;
			double temp;
			while(more)
			{
				if(NumberParser::tryParseFloat(result[0].convert<std::string>(), temp))
				{
					sum = sum + temp;
				}
				else
				{
					sum = sum + NumberParser::parseFloat(result[1].convert<std::string>());
				}
				more = result.moveNext();
			}
			RD05M136 = sum * 0.0254;
		}
		else
		{
			throw ApplicationException("板厚規格無法拆解");
		}
		return true;
	}
	catch(Exception& e)
	{
		logger.error(e.displayText());
	}
	ODBC::Connector::unregisterConnector();
	return false;
}
//----------------------------------------------------------------------------------------------//

//-----------------------------------------Redis--------------------------------------------------//
RedisBridge::RedisBridge(std::string host, int port): logger(Logger::get("RedisBridge")), _host(host), _port(port)
{
	// TODO Auto-generated constructor stub
	try
	{
		Poco::Timespan t(10, 0);
		_redis.connect(_host, _port, t);
		logger.information("Test connect redis server [%s:%d]", _host, _port);
	}
	catch(RedisException& e)
	{
		logger.error(e.message());
	}
}
RedisBridge::~RedisBridge(){
	// TODO Auto-generated destructor stub
}

void RedisBridge::cmd(std::vector<std::string> cmds)
{
	Redis::Array cmd;
	for(uint i=0; i< cmds.size(); i++)
	{
		cmd << cmds[i];
	}
	_redis.execute<Poco::Int64>(cmd);
}

bool RedisBridge::set(std::string key, std::string value)
{
	Command set = Command::set(key, value);
	try
	{
		if(_redis.execute<std::string>(set) == "OK")
		{
			logger.information("SET <%s> -> <%s>", key, value);
			return true;
		}
	}
	catch (RedisException& e)
	{
		logger.error(e.message());
	}
	return false;
}

bool RedisBridge::del(std::string key)
{
	Command del = Command::del(key);
	try
	{
		if(_redis.execute<Poco::Int64>(del) == 1)
		{
			logger.information("DEL <%s>", key);
			return true;
		}
	}
	catch (RedisException& e)
	{
		logger.error(e.message());
	}
	return false;
}

bool RedisBridge::srem(std::string table, std::string key)
{
	Command srem = Command::srem(table, key);
	try
	{
		if(_redis.execute<Poco::Int64>(srem) == 1)
		{
			return true;
		}
	}
	catch(RedisException& e)
	{
		logger.error(e.message());
	}
	return false;
}

bool RedisBridge::sadd(std::string table, std::string key)
{
	Command sadd = Command::sadd(table, key);
	try
	{
		if(_redis.execute<Poco::Int64>(sadd) == 1)
		{
			return true;
		}
	}
	catch (RedisException& e)
	{
		logger.error(e.message());
	}
	return false;
}

bool RedisBridge::sismember(std::string table, std::string key)
{
	Command sismember = Command::sismember(table, key); // check
	try
	{
		Poco::Int64 result = _redis.execute<Poco::Int64>(sismember);
		if(result == 0)//not exist
		{
			return false;
		}
		else if(result == 1) //is exist
		{
			return true;
		}
	}
	catch (RedisException& e)
	{
		logger.error(e.message());
	}
	return false;
}

bool RedisBridge::smembers(std::string table, std::vector<std::string> &keys)
{
	Command smembers = Command::smembers(table); // check
	try
	{
		Redis::Array result = _redis.execute<Redis::Array>(smembers);
		if(result.isNull())
		{
			logger.error("Table %s is Null from Redis", table);
		}
		else
		{
			keys.clear();
			for(uint i=0; i< result.size(); i++)
			{
				keys.push_back(result.get<Redis::BulkString>(i).value());
			}
		}
	}
	catch (RedisException& e)
	{
		logger.error(e.message());
	}
	return false;
}

bool RedisBridge::hmset(std::string table, std::map<std::string, std::string> fields)
{
	Command hmset = Command::hmset(table, fields);
	try
	{
		if(_redis.execute<std::string>(hmset) == "OK");
		{
			logger.information("HMSET <%s> OK", table);
			return true;
		}
	}
	catch (RedisException& e)
	{
		logger.error(e.message());
	}
	return false;
}

bool RedisBridge::hgetall(std::string table, std::map<std::string, std::string> &fields)
{
	Command hgetall = Command::hgetall(table);
	try
	{
		Redis::Array result = _redis.execute<Redis::Array>(hgetall);
		if(result.isNull())
		{
			logger.error("Key %s is Null from Redis", table);
			return false;
		}
		else
		{
			logger.information("Total get %u", result.size());
			for(uint i=0; i< result.size(); i+=2)
			{
				fields[result.get<Redis::BulkString>(i).value()] = result.get<Redis::BulkString>(i+1).value();
			}
			return true;
		}
	}
	catch (RedisException& e)
	{
		logger.error(e.message());
	}
	return false;
}

std::string RedisBridge::get(std::string key)
{
	std::string value = "";
	try
	{
		Redis::BulkString result = _redis.execute<Redis::BulkString>(Command::get(key));
		if(result.isNull())
		{
			logger.error("Key %s is Null from Redis", key);
		}
		else
		{
			value = result.value();
		}
	}
	catch (Poco::Exception& e)
	{
		logger.error(e.message());
	}
	return value;
}

//----------------------------------------------------------------------------------------------//

//-----------------------------------------MySQLBridge--------------------------------------------------//

MyBridge::MyBridge(): logger(Logger::get("MyBridge")), CONNECTION_STRING("host=10.11.50.35;port=8877;db=test;user=root;password=123456;compress=true;auto-reconnect=true")
{
	StirngLsit.insert("LotNO");
	StirngLsit.insert("PartNO");
	StirngLsit.insert("Operator");
	StirngLsit.insert("Mode");
	StirngLsit.insert("StartTime");
	StirngLsit.insert("EndTime");
	StirngLsit.insert("PlatingStartTime");
	StirngLsit.insert("PlatingEndTime");
	StirngLsit.insert("target_number_0");
	StirngLsit.insert("target_number_1");
	StirngLsit.insert("target_number_2");
	StirngLsit.insert("target_number_3");
	StirngLsit.insert("target_number_4");
	StirngLsit.insert("target_number_5");
	StirngLsit.insert("target_number_6");
	StirngLsit.insert("target_number_7");

	StirngLsit.insert("knife_count_0");
	StirngLsit.insert("knife_count_1");
	StirngLsit.insert("knife_count_2");
	StirngLsit.insert("knife_count_3");
	StirngLsit.insert("knife_count_4");
	StirngLsit.insert("knife_count_5");
	StirngLsit.insert("knife_count_6");
	StirngLsit.insert("knife_count_7");
}

MyBridge::~MyBridge(){

}

bool MyBridge::InsertMO(std::map<std::string, std::string> detailMO)
{
	logger.debug("Ready Insert MO");
	try
	{
		Poco::Data::MySQL::Connector::registerConnector();
		Session session("MySQL", CONNECTION_STRING);
		if(session.isConnected())
		{
			Statement insert(session);
			insert << "INSERT INTO MO_VCP_30_BETA (";
			for(std::map<std::string, std::string>::iterator it=detailMO.begin(); it!=detailMO.end(); ++it)
			{
				insert << it->first;
				if(it != --detailMO.end())
				{
					insert << ", ";
				}
			}
			insert << ") VALUES (";
			for(std::map<std::string, std::string>::iterator it=detailMO.begin(); it!=detailMO.end(); ++it)
			{
				if(it->second == "")
				{
					insert << "NULL";
				}
				else
				{
					if(StirngLsit.count(it->first))
					{
						insert << "'" << it->second << "'";
					}
					else
					{
						insert << it->second;
					}
				}

				if(it != --detailMO.end())
				{
					insert << ", ";
				}
			}
			insert << ")";
			logger.information(insert.toString());
			insert.execute();
			return true;
		}
		else
		{
			logger.error("MySQL database not connected");
			return false;
		}
	}
	catch (Exception& e)
	{
		logger.error(e.displayText());
		return false;
	}
}

void MyBridge::handleAlarm(AlarmNotification* pNf)
{
	logger.debug("準備新增警報:%s", pNf->msg);
	try
	{
		MySQL::Connector::registerConnector();
		Session session("MySQL", CONNECTION_STRING);
		if(session.isConnected())
		{
			Statement insert(session);
			insert << "INSERT INTO ALARM_VCP_30 (Detail, Status) VALUES ('";
			insert << pNf->msg << "'," << pNf->status << ")";
			logger.information(insert.toString());
			insert.execute();
		}
		else
		{
			logger.error("MySQL database not connected");
		}
	}
	catch (Exception& e)
	{
		logger.error(e.displayText());
	}
	pNf->release();
}

//----------------------------------------------------------------------------------------------//
ArangoBridge::ArangoBridge(std::string _host, int _port, std::string _database):
logger(Logger::get("ArangoBridge")), token(""), database(_database)
{
	session = new HTTPClientSession(_host, _port); //建立HTTP session
	TokenTimer = new Timer(0, 1000 * 60 * 60); //初始化Timer，每小時更新一次token
	TokenTimer->start(TimerCallback<ArangoBridge>(*this, &ArangoBridge::getToken));
}

ArangoBridge::~ArangoBridge(){}

JSON::Object::Ptr ArangoBridge::Bridge(std::string method, std::string path, JSON::Object paylod)
{
	HTTPRequest request(method, path, HTTPMessage::HTTP_1_1);
	request.add("Authorization", token);
	request.setContentType("application/json");
	if(method != "HTTP_POST")
	{
		std::stringstream ss;
		paylod.stringify(ss);
		request.setContentLength(ss.str().size());
	}

	std::ostream& BodyOstream = session->sendRequest(request); // sends request, returns open stream
	paylod.stringify(BodyOstream);

	HTTPResponse response;
	istream& rs = session->receiveResponse(response);
	string s((istreambuf_iterator<char>(rs)), istreambuf_iterator<char>());
	JSON::Parser parser;
	return parser.parse(s).extract<JSON::Object::Ptr>();
}

void ArangoBridge::getToken(Timer& timer)
{
	try
	{
		JSON::Object paylod;
		paylod.set("username", "root");
		paylod.set("password", "root");
		JSON::Object::Ptr ret = Bridge(HTTPRequest::HTTP_POST, "/_open/auth", paylod);
		token = "Bearer " + ret->get("jwt").convert<std::string>();
	}
    catch (Exception &e)
    {
        logger.error(e.displayText());
    }
}

//----------------------------------------------------------------------------------------------//

LocalBridge::LocalBridge(AutoPtr<AbstractConfiguration> _config):
logger(Logger::get("LocalBridge")), config(_config), DB("storage.db")
{
	db_path = new Path (config->getString("application.dir"), Path::PATH_UNIX);
	db_path->setFileName(DB);
	SQLite::Connector::registerConnector();
	Session session("SQLite", db_path->toString());
	try
	{
	    session << "CREATE TABLE POTION( ID INTEGER PRIMARY KEY AUTOINCREMENT,"
	    "DATETIME TIMESTAMP DEFAULT (datetime('now','localtime')), POTION TEXT NOT NULL, ACTION TEXT NOT NULL)", now;
	    logger.information("建立自動添加記錄資料表成功");
	}
	catch (Exception &e)
	{
		logger.error(e.displayText());
	}
	try
	{
	    session << "CREATE TABLE ERROR( ID INTEGER PRIMARY KEY AUTOINCREMENT,"
	    "DATETIME TIMESTAMP DEFAULT (datetime('now','localtime')), MESSAGE TEXT NOT NULL, DEVICE TEXT NULL, STATUS BOOL NOT NULL)", now;
	    logger.information("建立異常履歷資料表成功");
	}
	catch (Exception &e)
	{
		logger.error(e.displayText());
	}
	try
	{
	    session << "CREATE TABLE MO( ID INTEGER PRIMARY KEY AUTOINCREMENT, "
	    "STARTDATETIME TIMESTAMP DEFAULT (datetime('now','localtime')), ENDDATETIME TIMESTAMP DEFAULT (datetime('now','localtime')),"
	    "LOTNO VARCHAR NOT NULL, PARTNO VARCHAR NOT NULL, RANDOMSTRING VARCHAR NOT NULL, SOURCE VARCHAR NOT NULL, PPR_or_DC VARCHAR NOT NULL)", now;
	    logger.information("建立生產履歷資料表成功");
	}
    catch (Exception &e)
    {
        logger.error(e.displayText());
    }
}

LocalBridge::~LocalBridge(){}

bool LocalBridge::insertMO(recipe rcp)
{
	SQLite::Connector::registerConnector();
	Session session("SQLite", db_path->toString());
	Statement insert(session);
    insert << "INSERT INTO MO(LOTNO, PARTNO, RANDOMSTRING, SOURCE, PPR_or_DC) VALUES(?, ?, ?, ?, ?)"
    		 , use(rcp.LOTNO), use(rcp.PARTNO), use(rcp.RANDOMSTRING), use(rcp.SOURCE), use(rcp.PPR_or_DC);
    logger.information(insert.toString());
    insert.execute();
    return true;
}

bool LocalBridge::updateMO()
{
	LocalDateTime now;
	SQLite::Connector::registerConnector();
	Session session("SQLite", db_path->toString());
	Statement update(session);
	update << "UPDATE MO set ENDDATETIME = '"+ DateTimeFormatter::format(now, "%Y-%m-%d %H:%M:%S")
	+"' WHERE ID = (SELECT MAX(id) FROM MO )";
    logger.information(update.toString());
    update.execute();
    return true;
}

bool LocalBridge::getMO(std::vector<recipe> &MOs)
{
	MOs.clear();
	SQLite::Connector::registerConnector();
	Session session("SQLite", db_path->toString());
	recipe rcp;
	Statement select(session);
    select << "SELECT STARTDATETIME, ENDDATETIME, LOTNO, PARTNO, RANDOMSTRING, SOURCE, PPR_or_DC FROM MO ORDER by ID DESC",
    		into(rcp.STARTDATETIME), into(rcp.ENDDATETIME), into(rcp.LOTNO), into(rcp.PARTNO), into(rcp.RANDOMSTRING),
			into(rcp.SOURCE), into(rcp.PPR_or_DC), range(0, 1);
    while (!select.done())
    {
        select.execute();
        MOs.push_back(rcp);
    }
    return true;
}

bool LocalBridge::insertPotion(std::string potion, std::string action)
{
	SQLite::Connector::registerConnector();
	Session session("SQLite", db_path->toString());
	Statement insert(session);
	POTIONmessage Pmsg;
	Pmsg.potion = potion;
	Pmsg.action = action;
    insert << "INSERT INTO POTION(POTION, ACTION) VALUES(?, ?)", use(Pmsg.potion), use(Pmsg.action);
    insert.execute();
    return true;
}

bool LocalBridge::getPotion(std::vector<POTIONmessage> &msg)
{
	msg.clear();
	SQLite::Connector::registerConnector();
	Session session("SQLite", db_path->toString());
	POTIONmessage Pmsg;
	Statement select(session);
    select << "SELECT DATETIME, POTION, ACTION FROM POTION ORDER BY ID DESC", into(Pmsg.datetime),
    		into(Pmsg.potion), into(Pmsg.action), range(0, 1);
    while (!select.done())
    {
        select.execute();
        msg.push_back(Pmsg);
    }
    return true;
}

bool LocalBridge::insertERROR(std::string message, std::string device, bool status)
{
	ERRORmessage Message;
	Message.message = message;
	Message.device = device;
	Message.status = status;
	SQLite::Connector::registerConnector();
	Session session("SQLite", db_path->toString());
	Statement insert(session);
    insert << "INSERT INTO ERROR(MESSAGE, DEVICE, STATUS) VALUES(?, ?, ?)", use(Message.message), use(Message.device), use(Message.status);
    insert.execute();
    return true;
}

bool LocalBridge::getERROR(std::vector<ERRORmessage> &msg)
{
	msg.clear();
	SQLite::Connector::registerConnector();
	Session session("SQLite", db_path->toString());
	ERRORmessage Emsg;
	Statement select(session);
    select << "SELECT DATETIME, MESSAGE, DEVICE, STATUS FROM ERROR ORDER by ID DESC LIMIT 200", into(Emsg.datetime),
    		into(Emsg.message), into(Emsg.device), into(Emsg.status), range(0, 1);
    while (!select.done())
    {
        select.execute();
        msg.push_back(Emsg);
    }
    return true;
}
