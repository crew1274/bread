/*
 * Prod.h
 *
 *  Created on: 2018年7月19日
 *      Author: 171104
 */

#ifndef SRC_APP_PROD_H_
#define SRC_APP_PROD_H_

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>
#include <cmath>
#include <fstream>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <list>

#include "Poco/DoubleByteEncoding.h"
#include "Poco/UTF8Encoding.h"
#include "Poco/TextConverter.h"
#include "Poco/TextEncoding.h"
#include "Poco/Net/FTPClientSession.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/MailMessage.h"
#include "Poco/Net/SMTPClientSession.h"
#include "Poco/Net/MailRecipient.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/PartHandler.h"
#include "Poco/Net/MessageHeader.h"
#include "Poco/Net/Socket.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Foundation.h"
#include "Poco/StreamCopier.h"
#include "Poco/NumberFormatter.h"
#include "Poco/NumberParser.h"
#include "Poco/Path.h"
#include "Poco/URI.h"
#include "Poco/Format.h"
#include "Poco/Util/Application.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Util/AbstractConfiguration.h"
#include "Poco/Util/IniFileConfiguration.h"
#include "Poco/LocalDateTime.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Thread.h"
#include "Poco/Runnable.h"
#include "Poco/ActiveMethod.h"
#include "Poco/ActiveResult.h"
#include "Poco/ActiveDispatcher.h"
#include "Poco/ThreadLocal.h"
#include "Poco/Stopwatch.h"
#include "Poco/Exception.h"
#include "Poco/Delegate.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/FormattingChannel.h"
#include "Poco/PatternFormatter.h"
#include "Poco/AutoPtr.h"
#include "Poco/Format.h"
#include "Poco/BinaryWriter.h"
#include "Poco/BinaryReader.h"
#include "Poco/TemporaryFile.h"
#include "Poco/FileStream.h"
#include "Poco/Buffer.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/Element.h"
#include "Poco/DOM/Text.h"
#include "Poco/DOM/NodeIterator.h"
#include "Poco/DOM/NodeFilter.h"
#include "Poco/DOM/AutoPtr.h"
#include "Poco/DOM/DOMWriter.h"
#include "Poco/XML/XMLWriter.h"
#include "Poco/HashTable.h"
#include "Poco/Random.h"
#include "Poco/Condition.h"
#include "Poco/Semaphore.h"
#include "Poco/Mutex.h"
#include "Poco/Timespan.h"
#include "Poco/Process.h"
#include "Poco/PipeStream.h"
#include "Poco/SharedMemory.h"
#include "Poco/CountingStream.h"
#include "Poco/NullStream.h"
#include "Poco/StreamCopier.h"
#include "Poco/Util/Application.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Util/AbstractConfiguration.h"
#include "Poco/Util/Subsystem.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/AutoPtr.h"
#include "Poco/Timer.h"
#include "Poco/Thread.h"
#include "Poco/ThreadLocal.h"
#include "Poco/ThreadPool.h"
#include "Poco/Activity.h"
#include "Poco/RunnableAdapter.h"
#include "Poco/Stopwatch.h"
#include "Poco/Task.h"
#include "Poco/TaskManager.h"
#include "Poco/TaskNotification.h"
#include "Poco/Observer.h"
#include "Poco/Logger.h"
#include "Poco/PatternFormatter.h"
#include "Poco/FormattingChannel.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/SplitterChannel.h"
#include "Poco/FileChannel.h"
#include "Poco/Message.h"
#include "Poco/Exception.h"
#include "Poco/File.h"
#include "Poco/JSON/Stringifier.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/Random.h"
#include "Poco/RandomStream.h"

#include "utility.h"
#include "app/DatabaseBridge.h"
#include "app/PLC.h"
#include "app/rfid_ds.h"
#include "hal/ppr_device.h"
#include "def.h"

using namespace std;
using namespace Poco;
using namespace Poco::JSON;
using namespace Poco::Net;
using namespace Poco::XML;
using namespace Poco::Dynamic;

struct Status
{
	std::string detail;
};

struct Record
{
	std::string Operator; //人員
	std::string LotNO;	//批號
	std::string PartNO; //料號
	std::string ProcSeq; //製程序
	std::string PlatingTime; //電鍍時間
	std::string PlatingAmp; //電鍍電流
	std::string Height; // 銑邊後尺寸-長
	std::string Width; // 銑邊後尺寸-寬
	std::string PlatingPnl; //投入片數
	std::string FinalAmpTime; //末端電鍍時間
	std::string FinalAmpPercentage; //末端電流百分比
	bool withDummy; //是否Dummy
};

struct WSEvent
{
	std::string Operator;
	std::string LotNO;
	Stopwatch sw;
};

class Prod
{
public:
	Prod(AutoPtr<AbstractConfiguration> _config, Rfid_ds* _RFID, MESBridge* _mb, MyBridge* _myb, RedisBridge* _rb,
			LocalBridge* _lb, ArangoBridge* _ab, PLC* _plc, WebSocket* _ws);
	~Prod();
	std::list<int> ConvertDistance(int number, int width, bool withDummy); //轉換放置位置
//	bool getProcSeq(std::string payload, std::string& ProcSeq);
//	bool RecipeRequest(std::string LotNO, std::string ProcSeq, std::string& response);
//	bool ParseXML(std::string payload, std::string target, uint length);
//	bool ExtractXML(std::string payload);
//	bool Rebuild(std::string &payload);
	bool CallAGV();

	void forPPR();
	bool StopPPR();
	void PPRloop();
	void BackgroundPPR(Timer& timer);
	bool startPPR(int index);
	void ActiveResponse();
	bool ReadyProd(JSON::Object::Ptr data);
	bool confirmProd();
	void handleAlarm(AlarmNotification* pNf);
	void toTSDB(const std::string& payload);
	void insertHistory();
	void updateHistory();
	std::string random_string( size_t length );

	JSON::Object::Ptr LoadProd();
	bool updateProd(JSON::Object::Ptr p);

	//生產使用物件
	Logger& logger;
	AutoPtr<AbstractConfiguration> config;
	Rfid_ds* RFID;
	MESBridge* mb;
	MyBridge* myb;
	RedisBridge* rb;
	LocalBridge* lb;
	ArangoBridge* ab;
//	PPRDEVICE* ppr;
	PLC* plc;
	WebSocket* ws;
	BasicEvent<JSON::Object> theEvent; //ws推播機制
	std::string receiveMSG; //ws接收資料
	bool isFine; //ws回復狀態
	Mutex ProdMutex; //生產互斥鎖
	Object Recipe;
	bool BreakPoint;
	int PPRstatus;
	int PRE_PPRstatus;

	int C_status;
	int PRE_C_status;

	int D_status;
	int PRE_D_status;

	std::map<int, std::string> PPRstatusMap;
	bool ActiveDetect;
    ActiveMethod<void, std::string, Prod> _ActiveMethod;
};

#endif /* SRC_APP_PROD_H_ */
