/*
 * Network.cpp
 *
 *  Created on: 2018年7月10日
 *      Author: 171104
 */

#include <app/Network.h>

Network::Network(AutoPtr<AbstractConfiguration> _config):
	logger(Logger::get("Network")),
	config(_config),
	runnable(*this, &Network::DisconnectAlarm)
{
	// TODO Auto-generated constructor stub
	Last = Timestamp();
}

Network::~Network()
{
	// TODO Auto-generated destructor stub
}


/**
 * @brief Ping
 *
 * @param[in] address Address to ping.
 * @param[in] max_attempts Number of attempts to try and ping.
 * @param[out] details Details of failure if one occurs.
 *
 * @return True if responsive, false otherwise.
 *
 * @note { I am redirecting stderr to stdout.  I would recommend
 *         capturing this information separately.}
 */
void Network::Ping(Poco::Timer& timer)
{
	ICMPClient icmpClient(IPAddress::IPv4);
	//if(icmpClient.ping(pconfig->getString("PLC.IP", "192.168.3.208")))
	if(!icmpClient.ping("192.168.1.200"))
	{
		logger.error("偵測到網路斷線");
	    std::vector<std::string> args(1);
	    args.push_back("restart");
	    Poco::Pipe outPipe;
	    ProcessHandle ph = Process::launch("/etc/init.d/networking", args, 0, &outPipe, 0);
	    Poco::PipeInputStream i(outPipe);
	    std::string msg((std::istreambuf_iterator<char>(i)), std::istreambuf_iterator<char>());
	    logger.fatal(msg);
	    ph.wait();
	}
}

void Network::DisconnectAlarm()
{
	try
	{

	}
	catch(Exception& e)
	{
		logger.error(e.displayText());
	}
}

void Network::SendMail(std::string payload)
{
	try
	{
		MailMessage message;
		message.setSender(MailMessage::encodeWord("黃偉鑫")+"<wilson-huang@cht-pt.com.tw>");
		//message.addRecipient(MailRecipient(MailRecipient::PRIMARY_RECIPIENT, "hlhsieh@cht-pt.com.tw", MailMessage::encodeWord("謝欣龍")));
		//message.addRecipient(MailRecipient(MailRecipient::PRIMARY_RECIPIENT, "kyrie_chang@cht-pt.com.tw", MailMessage::encodeWord("張康庭")));
		//message.addRecipient(MailRecipient(MailRecipient::PRIMARY_RECIPIENT, "matic.chiu@cht-pt.com.tw", MailMessage::encodeWord("邱信暐")));
		//message.addRecipient(MailRecipient(MailRecipient::PRIMARY_RECIPIENT, "pt.liu@cht-pt.com.tw", MailMessage::encodeWord("劉邦慈")));
		message.addRecipient(MailRecipient(MailRecipient::PRIMARY_RECIPIENT, "wilson-huang@cht-pt.com.tw", MailMessage::encodeWord("黃偉鑫")));
		//message.addRecipient(MailRecipient(MailRecipient::CC_RECIPIENT, "wilson-huang@cht-pt.com.tw", MailMessage::encodeWord("黃偉鑫")));
		//message.addRecipient(MailRecipient(MailRecipient::CC_RECIPIENT, "henry_jeng@cht-pt.com.tw", MailMessage::encodeWord("鄭智鴻")));

		message.setSubject(MailMessage::encodeWord(config->getString("PLC.ID"))+"Edge"+MailMessage::encodeWord("有異常狀況, 請盡快處理"));
		std::string content;
		content += "Dear all \r\n\r\n";
		content += Utility::NowTime(false);
		content += payload;
		message.addContent(new StringPartSource(content));
		//message.addAttachment("logo", new StringPartSource(logo, "image/gif"));
		SMTPClientSession session("192.168.1.200");
		session.login();
		session.sendMessage(message);
		session.close();
		Last = Timestamp();
	}
	catch (Exception& e)
	{
		logger.error(e.displayText());
	}
}
