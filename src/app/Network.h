/*
 * Network.h
 *
 *  Created on: 2018年7月10日
 *      Author: 171104
 */

#ifndef SRC_APP_NETWORK_H_
#define SRC_APP_NETWORK_H_

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <ostream>

#include "Poco/Net/ICMPSocket.h"
#include "Poco/Net/ICMPClient.h"
#include "Poco/Net/ICMPEventArgs.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/MailMessage.h"
#include "Poco/Net/MailRecipient.h"
#include "Poco/Net/SMTPClientSession.h"
#include "Poco/Net/StringPartSource.h"

#include "Poco/ThreadPool.h"
#include "Poco/Util/AbstractConfiguration.h"
#include "Poco/Util/IniFileConfiguration.h"
#include "Poco/Timer.h"
#include "Poco/Thread.h"
#include "Poco/ThreadLocal.h"
#include "Poco/RunnableAdapter.h"
#include "Poco/Stopwatch.h"
#include "Poco/Exception.h"
#include "Poco/Logger.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/FormattingChannel.h"
#include "Poco/PatternFormatter.h"
#include "Poco/AutoPtr.h"
#include "Poco/Timestamp.h"
#include "Poco/Process.h"
#include "Poco/PipeStream.h"
#include "Poco/StreamCopier.h"
#include "Poco/LocalDateTime.h"
#include "Poco/TextConverter.h"


#include "../def.h"
#include "../Utility.h"

using namespace std;
using namespace Poco;
using namespace Poco::Util;
using namespace Poco::Net;

class Network
{
public:
	Network(AutoPtr<AbstractConfiguration> _config);
	~Network();
	static int Execute_Command(const std::string& command, std::string& output, const std::string& mode);
	void Ping(Poco::Timer& timer);
	void SendMail(std::string payload);
	void DisconnectAlarm();
private:
	Logger& logger; // logger
	AutoPtr<AbstractConfiguration> config;
	RunnableAdapter<Network> runnable;
	Timestamp Last;
};

#endif /* SRC_APP_NETWORK_H_ */
