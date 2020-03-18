/*
 * WebSocketHandler.cpp
 *
 *  Created on: 2019年7月10日
 *      Author: 171104
 */

#include <app/Handler/WebSocketHandler.h>

WebSocketHandler::WebSocketHandler(Prod* _prod):
logger(Logger::get("WebSocketHandler")), prod(_prod)
{
	// TODO Auto-generated constructor stub

}

WebSocketHandler::~WebSocketHandler() {
	// TODO Auto-generated destructor stub
}

void WebSocketHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
	Object inner;
	std::ostringstream oss;
	inner.set("ClientAddress", request.clientAddress().toString());
	try
	{
		char buffer[2048];
		memset(buffer, 0, sizeof(buffer));
		std::string receiveMSG;
		int flags;
		int ret;
		WebSocket ws(request, response);
		if(!ws.receiveFrame(&buffer, sizeof(buffer), flags))
		{
			logger.error("receiveFrame error, maybe connection failed");
			return;
		}
		receiveMSG = buffer;
		memset(buffer, 0, sizeof(buffer));
		logger.information("receive: %s", receiveMSG);
		if(!AuthCheck(receiveMSG))
		{
			inner.set("isAllowed", false);
			inner.set("MSG", "驗證失敗");
			inner.stringify(oss);
			ws.sendFrame(oss.str().c_str(), oss.str().size());
			ws.close();
			return;
		}
		if(role == Role::producer and !prod->ProdMutex.tryLock())
		{
			inner.set("isAllowed", false);
			inner.set("MSG", "producer只允許一位使用者連線");
			inner.stringify(oss);
			ws.sendFrame(oss.str().c_str(), oss.str().size());
			ws.close();
			return;
		}
		inner.set("isAllowed", true);
		inner.set("MSG", "驗證成功");
		inner.set("toast", "已建立連線");
		inner.stringify(oss);
		ws.sendFrame(oss.str().c_str(), oss.str().size());
		prod->theEvent += delegate(this, &WebSocketHandler::onEvent);
		prod->BreakPoint = false;
		while(1)
		{
			oss.clear();
			oss.str("");
			inner.clear();
			memset(buffer, 0, sizeof(buffer));
			inner.set("status", prod->isFine);
			inner.set("payload", payload);
			payload.clear();
			inner.stringify(oss);
			ws.sendFrame(oss.str().c_str(), oss.str().size(), flags);
			ret = ws.receiveFrame(buffer, sizeof(buffer), flags);
			prod->receiveMSG = buffer;
			if((ret == 0 && (flags & WebSocket::FRAME_OP_BITMASK) == WebSocket::FRAME_OP_CLOSE) || prod->BreakPoint)
			{
				logger.warning("Connection lost from %s", request.clientAddress().toString());
				ws.close();
				break;
			}
			Thread::sleep(1000);
		}
	}
	catch (Exception& e)
	{
		logger.error(e.displayText());
	}
	prod->theEvent -= delegate(this, &WebSocketHandler::onEvent);
	prod->ProdMutex.unlock();
	logger.information("WebSocket connection closed");
	return;
}


bool WebSocketHandler::AuthCheck(std::string payload)
{
	try
	{
		Parser parser;
		Object::Ptr MainObject = parser.parse(payload).extract<Poco::JSON::Object::Ptr>();
		if(MainObject->has("User") && MainObject->has("Password"))
		{
			//身分認證
			std::string user = MainObject->getValue<std::string>("User");
			std::string password = MainObject->getValue<std::string>("Password");
			if(user == "admin" && password == "admin")
			{
				role = Role::admin;
			}
			else if(user == "test" && password == "test")
			{
				role = Role::producer;
			}
			else if(user == "watcher" && password == "watcher")
			{
				role = Role::watcher;
			}
			return true;
		}
	}
	catch(Exception& e)
	{
		logger.error(e.displayText());
	}
	role = Role::unknown;
	return false;
}

void WebSocketHandler::onEvent(const void* pSender, Object& inner) /*覆加訊息拆解*/
{
	for(Object::Iterator it = inner.begin(); it!= inner.end(); it++)
	{
		payload.set(it->first, it->second);
	}
}
