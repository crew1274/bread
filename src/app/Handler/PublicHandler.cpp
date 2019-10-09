/*
 * PublicHandler.cpp
 *
 *  Created on: 2019年7月10日
 *      Author: 171104
 */

#include <app/Handler/PublicHandler.h>

PublicHandler::PublicHandler(Path _target): logger(Logger::get("PublicHandler")), target(_target) {
	// TODO Auto-generated constructor stub

}

PublicHandler::~PublicHandler()
{
	// TODO Auto-generated destructor stub
}

void PublicHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
	if(target.getExtension() == "html")
	{
		response.setContentType("text/html");
	}
	else if(target.getExtension() == "css")
	{
		response.setContentType("text/css");
	}
	else if(target.getExtension() == "js")
	{
		response.setContentType("application/javascript");
	}
	else if(target.getExtension() == "icon")
	{
		response.setContentType("image/x-icon");
	}
	else
	{
		response.setContentType("text/plain");
	}
	response.set("Access-Control-Allow-Origin", "*");
	response.setVersion(HTTPMessage::HTTP_1_1);
	response.setChunkedTransferEncoding(true);
	std::ostream& ostr = response.send();
	std::ifstream is(target.toString(Path::PATH_UNIX).c_str(), std::ios::binary);
	std::string content( (std::istreambuf_iterator<char>(is) ),(std::istreambuf_iterator<char>()));
	ostr << content;
	return;
}
