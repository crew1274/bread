/*
 * Network.cpp
 *
 *  Created on: 2018年7月10日
 *      Author: 171104
 */

#include <app/Network.h>

Network::Network(AutoPtr<AbstractConfiguration> _config):
	logger(Logger::get("Network")), config(_config)
{
	// TODO Auto-generated constructor stub
	Last = Timestamp();
	token = "";
//	getToken("root", "root");
//	cout << token << endl;
//	getToken("root", "root");
//	cout << token << endl;
//	getVersion();
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
void Network::Ping(Timer& timer)
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

void Network::getVersion()
{
	HTTPClientSession session("10.11.0.156", 8529);
	HTTPRequest request(HTTPRequest::HTTP_GET, "/_api/version", HTTPMessage::HTTP_1_1);
	HTTPResponse response;
	request.add("Authorization", token);
	request.setContentType("application/json");
	session.sendRequest(request);
	istream& rs = session.receiveResponse(response);
	string s((istreambuf_iterator<char>(rs)), istreambuf_iterator<char>());
	cout << s << endl;
}

void Network::getToken(std::string username, std::string password)
{
		HTTPClientSession session("10.11.0.156", 8529);
		HTTPRequest request(HTTPRequest::HTTP_POST, "/_open/auth", HTTPMessage::HTTP_1_1);
		request.add("Authorization", token);
		request.setContentType("application/json");
		std::stringstream ss;

		JSON::Object paylod;
		paylod.set("username", username);
		paylod.set("password", password);

		paylod.stringify(ss);
		request.setContentLength(ss.str().size());

		std::ostream& BodyOstream = session.sendRequest(request); // sends request, returns open stream
		paylod.stringify(BodyOstream);
		HTTPResponse response;
		istream& rs = session.receiveResponse(response);
		string s((istreambuf_iterator<char>(rs)), istreambuf_iterator<char>());

		Parser parser;
		JSON::Object::Ptr ret = parser.parse(s).extract<JSON::Object::Ptr>();
		token = "Bearer "+ret->get("jwt").convert<std::string>();
}


