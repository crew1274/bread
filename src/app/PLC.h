/*
 * PLC.h
 *
 *  Created on: 2018年9月6日
 *      Author: 171104
 */

#ifndef PLC_H_
#define PLC_H_

#include "Poco/Thread.h"
#include "Poco/Runnable.h"
#include "Poco/ThreadLocal.h"
#include "Poco/Condition.h"
#include "Poco/Semaphore.h"
#include "Poco/ScopedLock.h"
#include "Poco/Mutex.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/ParseHandler.h"
#include "Poco/JSON/JSONException.h"
#include "Poco/Environment.h"
#include "Poco/Path.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"
#include "Poco/StreamCopier.h"
#include "Poco/Stopwatch.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/LoggingRegistry.h"
#include "Poco/Util/Application.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Util/AbstractConfiguration.h"
#include "Poco/Util/IniFileConfiguration.h"
#include "Poco/Timer.h"
#include "Poco/Thread.h"
#include "Poco/ThreadLocal.h"
#include "Poco/Stopwatch.h"
#include "Poco/Logger.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/FormattingChannel.h"
#include "Poco/PatternFormatter.h"
#include "Poco/AutoPtr.h"
#include "Poco/StringTokenizer.h"
#include "Poco/RegularExpression.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Path.h"
#include "Poco/URI.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>
#include <cstdio>
#include <iomanip>
#include <vector>
#include <unistd.h>
#include "../def.h"
#include "../utility.h"
#include "hal/omron_plc.h"

using namespace std;
using namespace Poco;
using namespace Poco::Util;
using namespace Poco::Net;
using namespace Poco::JSON;

class PLC: public OmronPLC //繼承 OmronPLC
{
public:
	PLC(std::string EngName, AutoPtr<AbstractConfiguration> _config, NotificationCenter *_nc);
	virtual ~PLC();
	void Load();
	void LoadAlarm(std::string fileName);
	void Base(Poco::Timer& timer);
	bool WritePoint_PLC(std::string CS_mode, int address, int write_value, bool dec_as_hex, int bit_position = 0);
	int ReadPoint_PLC(std::string CS_mode, uint address, uint bit_partition, bool dec_as_hex);
//	bool OverWrite(int TargetAddress, int TempAddress, int StatusAddress, int Value);
	bool OverWrite(std::string CS_mode,int TargetAddress, int bit_partition, int TempAddress, int StatusAddress, int Value, bool dec_as_hex);
	virtual bool WorkingCheck() {return false;};
	bool isWorking;
	std::vector<MultipleAreaRead> MultipleAreaReads; //點位資料
	std::vector<float> ratios; //倍率
	struct PLC_DATA
	{
		std::vector<MultipleAreaRead> MultipleAreaReads;
		std::vector<float> DATA_RATIO;
		std::vector<std::string> OPC_TAGNAME;
	}PLC_DATA;
	struct ALARM_DATA
	{
		std::vector<MultipleAreaRead> address;
		std::vector<uint> pre;
		std::vector<std::string> msg;
	}ALARM_DATA;
	virtual std::string getName() {return EngName;};
	std::string EngName;
	int pre;
protected:
	Logger& logger;
	AutoPtr<AbstractConfiguration> config;
	NotificationCenter *nc;
	stringstream ss;
	struct pipe np;
	Stopwatch _stopwatch;
	Mutex PLCmutex;
};

#endif /* PLC_H_ */
