#include <iostream>
#include <sstream>
#include <istream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sys/socket.h>	//socket
#include <arpa/inet.h>	//inet_addr

#include "boost/lambda/lambda.hpp"
#include <iostream>
#include <iterator>
#include <algorithm>


#include "Poco/Foundation.h"
#include "Poco/DoubleByteEncoding.h"
#include "Poco/UTF8Encoding.h"
#include "Poco/TextConverter.h"
#include "Poco/TextEncoding.h"
#include "Poco/Net/FTPClientSession.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/MailMessage.h"
#include "Poco/Net/SMTPClientSession.h"
#include "Poco/Net/MailRecipient.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/PartHandler.h"
#include "Poco/Net/MessageHeader.h"
#include "Poco/Net/Socket.h"
#include "Poco/String.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/NetException.h"
#include "Poco/StringTokenizer.h"
#include "Poco/JSON/Stringifier.h"
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
#include "Poco/Timer.h"
#include "Poco/Thread.h"
#include "Poco/Runnable.h"
#include "Poco/ActiveMethod.h"
#include "Poco/ActiveResult.h"
#include "Poco/ActiveDispatcher.h"
#include "Poco/ThreadLocal.h"
#include "Poco/Stopwatch.h"
#include "Poco/Exception.h"
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
#include "Poco/NotificationCenter.h"
#include "Poco/Notification.h"
#include "Poco/Observer.h"
#include "Poco/NObserver.h"
#include "Poco/BasicEvent.h"
#include "Poco/Delegate.h"
#include "Poco/Logger.h"
#include "Poco/PatternFormatter.h"
#include "Poco/FormattingChannel.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/SplitterChannel.h"
#include "Poco/FileChannel.h"
#include "Poco/Message.h"
#include "Poco/Exception.h"
#include "Poco/File.h"

#include "app/PLC.h"
#include "app/Prod.h"
#include "app/Network.h"
#include "app/rfid_ds.h"
#include "app/DatabaseBridge.h"
#include "hal/ppr_device.h"
#include "app/Handler/PublicHandler.h"
#include "app/Handler/WebSocketHandler.h"
#include "app/Handler/ApiHandler.h"

using namespace std;
using namespace Poco;
using namespace Poco::JSON;
using namespace Poco::Util;
using namespace Poco::Net;

#if 0
class ApiRequestHandler: public HTTPRequestHandler
{
public:
	ApiRequestHandler(Prod* _prod): logger(Logger::get("ApiRequestHandler")), prod(_prod){}
	void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
	{
		URI RequestPath(request.getURI());
		std::vector<std::string> PathSegments;
		RequestPath.getPathSegments(PathSegments);

		response.setVersion(HTTPMessage::HTTP_1_1);
		response.setChunkedTransferEncoding(true);
		response.setContentType("application/json");
		response.set("Access-Control-Allow-Origin", "*");
		std::ostream& ostr = response.send();
		Object MainObject;
		Object::Ptr ExtractObject;
		if(PathSegments.size()<2)
		{
			MainObject.set("Hello", "World");
		}
		else if(PathSegments[1] == "EdgeMode")
		{
			response.setStatusAndReason(HTTPResponse::HTTP_OK);

			if(request.getMethod() == HTTPRequest::HTTP_POST) //切換Edge模式
			{
//				Object::Ptr inner = new Poco::JSON::Object;
				std::istream &i = request.stream();
				string s((istreambuf_iterator<char>(i)), istreambuf_iterator<char>());
				logger.information("API[EdgeMode] receive: %s", s);
				Parser parser;
				ExtractObject = parser.parse(s).extract<Object::Ptr>();
				int target_value;
				std::string target_mode;
				if(ExtractObject->has("EdgeMode")) //切換Edge mode
				{
					if(ExtractObject->get("EdgeMode").convert<bool>())
					{
						//EdgeMode turn on
						logger.information("切換Edge模式為on");
						target_value = 0;
						target_mode = "EdgeMode";
					}
					else
					{
						//EdgeMode turn off
						logger.information("切換Edge模式為off");
						target_value = 256;
						target_mode = "EdgeBypass";
					}
					prod->plc->WritePoint_PLC("H", 511, target_value, false, 0);
					if(prod->plc->ReadPoint_PLC("H", 511, 0, false) != target_value)
					{
						logger.error("切換至%s失敗", target_mode);
						MainObject.set("result", false);
						MainObject.set("toast", "切換至"+target_mode+"失敗");
						MainObject.set("MSG", "切換至"+target_mode+"失敗");
					}
					else
					{
						logger.information("切換至%s成功", target_mode);
						MainObject.set("result", true);
						MainObject.set("toast", "切換至"+target_mode+"成功");
						MainObject.set("MSG", "切換至"+target_mode+"成功");
					}
				}
				if(ExtractObject->has("AutoMode")) //切換設備模式
				{
					if(ExtractObject->get("AutoMode").convert<bool>())
					{
						logger.information("切換AutoMode為on");
						prod->plc->WritePoint_PLC("H", 0, 256, false, 0);
					}
					else
					{
						logger.information("切換AutoMode為off");
						prod->plc->WritePoint_PLC("H", 0, 0, false, 0);
					}
				}
				if(ExtractObject->has("ManualMode"))
				{
					if(ExtractObject->get("ManualMode").convert<bool>())
					{
						logger.information("切換ManualMode為on");
						prod->plc->WritePoint_PLC("H", 0, 256, false, 2);
					}
					else
					{
						logger.information("切換ManualMode為off");
						prod->plc->WritePoint_PLC("H", 0, 0, false, 2);
					}
				}
			}
			else if(request.getMethod() == HTTPRequest::HTTP_GET) //查詢Edge模式
			{
				if(prod->plc->ReadPoint_PLC("H", 511, 0, false) == 0)
				{
					MainObject.set("EdgeMode", true);
				}
				else
				{
					MainObject.set("EdgeMode", false);
				}
			}
		}
		else if(PathSegments[1] == "barcode")
		{
			std::istream &i = request.stream();
			string s((istreambuf_iterator<char>(i)), istreambuf_iterator<char>());
			logger.debug("barcode Request Body: %s", s);

			Parser parser;
			//Poco::Dynamic::Var var = parser.parse(s);
			Object::Ptr object = parser.parse(s).extract<Object::Ptr>();
			if(!object->isNull("RECIPE"))
			{
				logger.information("取得來自Barcode的生產要求LotNO: %s ", object->get("RECIPE").convert<std::string>());
				std::string LotNO = object->get("RECIPE").convert<std::string>();
				MainObject.set("LotNO", LotNO);
				prod->counter = 60;
				prod->barcode = LotNO;
			}
		}
		else if(PathSegments[1] == "CallAGV")
		{
			logger.information("呼叫AGV");
			try
			{
				struct sockaddr_in server;
				char message[1000] = "ExCallCar$";
				char server_reply[2000];

				//Create socket
				int sock = socket(AF_INET , SOCK_STREAM , 0);
				if (sock == -1)
				{
					throw ApplicationException("Could not create socket");
				}
				logger.information("Socket created");
				server.sin_addr.s_addr = inet_addr(prod->config->getString("AGV.IP", "192.168.2.13").c_str());
				server.sin_family = AF_INET;
				server.sin_port = htons(prod->config->getInt("AGV.PORT", 504));

				//Connect to remote server
				if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
				{
					throw ApplicationException("connect failed. Error");
				}
				logger.information("Socket Connected");

				//Send some data
				if( send(sock , message , strlen(message) , 0) < 0)
				{
					throw ApplicationException("Message Send failed");
				}
				//Receive a reply from the server
				if( recv(sock , server_reply , 2000 , 0) < 0)
				{
					throw ApplicationException("recv failed");
				}
				puts(server_reply);
				std::string res = server_reply;
				logger.information("AGV Server reply : %s", res);
				StringTokenizer temp(res, "$", StringTokenizer::TOK_TRIM | StringTokenizer::TOK_IGNORE_EMPTY);
				if(temp[0] == "ExCallCarResponse")
				{
					MainObject.set("toast", "AGV呼叫成功");
					MainObject.set("MSG", "AGV呼叫成功");
				}
				else
				{
					throw ApplicationException("AGV回應:"+res);
				}
			}
		    catch (Exception &e)
		    {
		        logger.error(e.displayText());
				MainObject.set("Exception", e.displayText());
				MainObject.set("toast", "AGV呼叫失敗");
				MainObject.set("MSG", "AGV呼叫失敗");
		    }
		}
		else if(PathSegments[1] == "CleanRecipe")
		{
			logger.information("清除上筆參數資料");
			if(prod->plc->WritePoint_PLC("DM", 11900, 0, false))
			{
				logger.information("DM11900  清除完成");
				MainObject.set("toast", "清除完成");
			}
			else
			{
				logger.warning("DM11900  清除失敗");
				MainObject.set("toast", "清除失敗");
			}
		}
		else if(PathSegments[1] == "now" && request.getMethod() == HTTPRequest::HTTP_GET)
		{
			response.setStatusAndReason(HTTPResponse::HTTP_OK);
			int status = prod->plc->ReadPoint_PLC("DM", 3500, 0, false);
			if(status == 1)
			{
				MainObject.set("LotNO", prod->rb->get("lastLotNO"));
				MainObject.set("Status", "電鍍中");
				MainObject.set("TotalPlatingAmp", prod->rb->get("LastTotalPlatingAmp"));
				MainObject.set("PlatingAmp", prod->rb->get("LastPlatingAmp"));
			}
			else
			{
				MainObject.set("LotNO", "");
				MainObject.set("Status", "");
				MainObject.set("TotalPlatingAmp", "");
				MainObject.set("PlatingAmp", "");
			}

			if(prod->plc->ReadPoint_PLC("H", 511, 0, false) == 0)
			{
				MainObject.set("EdgeMode", true);
			}
			else
			{
				MainObject.set("EdgeMode", false);
			}
			MainObject.set("DM3500", status);
		}
		else if(PathSegments[1] == "Upload")
		{
			response.setStatusAndReason(HTTPResponse::HTTP_OK);
			logger.information("啟動上傳程式");
			Pipe outPipe;
			std::vector<std::string> args;
			args.push_back("lib/MariaDB.py");
			ProcessHandle PH = Process::launch("python", args, prod->config->getString("application.dir")+"/python/", 0, &outPipe, &outPipe);
			PipeInputStream istr(outPipe);
			PH.wait();
		}
		else if(PathSegments[1] == "Predict" && request.getMethod() == HTTPRequest::HTTP_POST)
		{
			response.setStatusAndReason(HTTPResponse::HTTP_OK);
			logger.information("啟動預測");
			Pipe outPipe;
			std::vector<std::string> args;
			try
			{
				std::istream &i = request.stream();
				string s((istreambuf_iterator<char>(i)), istreambuf_iterator<char>());
				logger.trace("receive: %s", s);
				Parser parser;
				Prod prodCopy(prod->config, prod->RFID, prod->mb, prod->myb, prod->rb, prod->lb, prod->ppr, prod->plc, prod->ws);
				std::string LotNO;
				std::string ProcSeq;
				std::string PlatingPnl;
				std::string payload;
				ExtractObject = parser.parse(s).extract<Object::Ptr>();
				if(ExtractObject->has("LotNO") && ExtractObject->has("ProcSeq") && ExtractObject->has("PlatingPnl"))
				{
					LotNO = ExtractObject->get("LotNO").convert<std::string>();
					ProcSeq = ExtractObject->get("ProcSeq").convert<std::string>();
					PlatingPnl = ExtractObject->get("PlatingPnl").convert<std::string>();
				}
				else
				{
					throw ApplicationException("參數缺少");
				}
				prodCopy.RecipeRequest(LotNO, ProcSeq, payload);
				std::string target = "鍍銅-VOP孔";
				if(!prodCopy.ParseXML(payload, target, target.size()))
				{
					throw ApplicationException("非"+target+"站製程序");
				}
				prodCopy.Rebuild(payload); //參數重解
				prodCopy.ExtractXML(payload);
				logger.information("準備取得目前鑽孔製程參數");
				target = "VOP鑽孔";
				std::string TotalHoles;
				for(int i = NumberParser::parse(ProcSeq); i>0; i--)
				{
					logger.trace("looking for %d...", i);
					payload.clear();
					prodCopy.RecipeRequest(LotNO, NumberFormatter::format(i), payload);
					if(prodCopy.ParseXML(payload, target, target.size()))
					{
						logger.information("取得%sVOP鑽孔製程參數", LotNO);
						prodCopy.mb->TOTAL_HOLES(LotNO, NumberFormatter::format(i), TotalHoles);
						break;
					}
					if(i == 1)
					{
						throw Poco::ApplicationException("%s查無鑽孔製程參數", LotNO);
					}
				}
				args.push_back("-W");
				args.push_back("ignore");
				args.push_back(prod->config->getString("ALGORITHM.VM_PROGRAM_NAME", "main_pred.py"));
				args.push_back(prodCopy.MO["Spec"]); //規格 -> 孔銅規格
				args.push_back(TotalHoles);	//總鑽孔數
				args.push_back(prodCopy.MO["AR"]); //綜橫比
				args.push_back(prodCopy.MO["MIN_DRILL_PATH"]);	//最小鑽徑
				args.push_back(PlatingPnl);	//數量
				args.push_back(prodCopy.MO["PLATING_AREA"]); //電鍍面積
				args.push_back(LotNO); //批號
				for(uint i=0; i< args.size(); i++)
				{
					logger.information("%s", args[i]);
				}
				ProcessHandle PH = Process::launch("python", args, prod->config->getString("application.dir")+"/python/", 0, &outPipe, &outPipe);
				PipeInputStream istr(outPipe);
				PH.wait();
				string result((istreambuf_iterator<char>(istr)), istreambuf_iterator<char>());
				logger.information("預測完成:%s", result);
				if(trim(result) == "OK")
				{
					logger.information("預測成功，準備撈取資料", result);
					std::map<std::string, std::string> temp;
					if(prodCopy.rb->hgetall("predict:"+LotNO, temp))
					{
						Object MinnerObject;
						for (std::map<std::string, std::string>::iterator it=temp.begin(); it!=temp.end(); ++it)
						{
							MinnerObject.set(it->first, it->second);
						}
						MainObject.set("Result", MinnerObject);
						MainObject.set("toast", "預測成功");
					}
					else
					{
						throw ApplicationException("Redis查無資料");
					}
				}
				else
				{
					throw ApplicationException("預測失敗");
				}
			}
		    catch (Exception &e)
		    {
		        logger.error(e.displayText());
				MainObject.set("Exception", e.displayText());
				MainObject.set("toast", "預測失敗");
		    }
		}
		else if(PathSegments[1] == "NIA-10" && PathSegments[2] == "Predict" && request.getMethod() == HTTPRequest::HTTP_POST)
		{
			response.setStatusAndReason(HTTPResponse::HTTP_OK);
			logger.information("啟動NIA-10預測");
			Pipe outPipe;
			std::vector<std::string> args;
			args.push_back("-W");
			args.push_back("ignore");
			args.push_back("NIA_10_VM.py");
			try
			{
				std::istream &i = request.stream();
				string s((istreambuf_iterator<char>(i)), istreambuf_iterator<char>());
				logger.information("receive: %s", s);
				Parser parser;
				ExtractObject = parser.parse(s).extract<Object::Ptr>();
				if(ExtractObject->has("NITIME1") && ExtractObject->has("NITIME2") && ExtractObject->has("NITIME3")
				&& ExtractObject->has("AUTIME1") && ExtractObject->has("AUTIME2") && ExtractObject->has("AUTIME3")
				&& ExtractObject->has("AUCURRENT1") && ExtractObject->has("AUCURRENT2") && ExtractObject->has("AUCURRENT3")
				&& ExtractObject->has("LotNO"))
				{
					args.push_back(ExtractObject->get("NITIME1").convert<std::string>());
					args.push_back(ExtractObject->get("NITIME2").convert<std::string>());
					args.push_back(ExtractObject->get("NITIME3").convert<std::string>());
					args.push_back(ExtractObject->get("AUTIME1").convert<std::string>());
					args.push_back(ExtractObject->get("AUTIME2").convert<std::string>());
					args.push_back(ExtractObject->get("AUTIME3").convert<std::string>());
					args.push_back(ExtractObject->get("AUCURRENT1").convert<std::string>());
					args.push_back(ExtractObject->get("AUCURRENT2").convert<std::string>());
					args.push_back(ExtractObject->get("AUCURRENT3").convert<std::string>());
					args.push_back(ExtractObject->get("LotNO").convert<std::string>());
				}
				else
				{
					throw ApplicationException("參數缺少");
				}
				for(uint i=0; i< args.size(); i++)
				{
					logger.information(args[i]);
				}
				ProcessHandle PH = Process::launch("python", args, prod->config->getString("application.dir")+"/python/NIA-10/", 0, &outPipe, &outPipe);
				PipeInputStream istr(outPipe);
				PH.wait();
				string result((istreambuf_iterator<char>(istr)), istreambuf_iterator<char>());
				logger.information("預測完成:%s", result);
				StringTokenizer t(result, ":", StringTokenizer::TOK_TRIM | StringTokenizer::TOK_IGNORE_EMPTY);
				if(t[0] == "OK")
				{
					logger.information("預測成功，準備撈取資料");
					MainObject.set("spec", NumberParser::parseFloat(t[1]));
					MainObject.set("1071", NumberParser::parseFloat(t[2]));
					MainObject.set("toast", "預測成功");
				}
				else
				{
					throw ApplicationException("預測失敗");
				}
			}
		    catch (Exception &e)
		    {
		        logger.error(e.displayText());
				MainObject.set("Exception", e.displayText());
				MainObject.set("toast", "預測失敗");
		    }
		}
		else if(PathSegments[1] == "RFID" && request.getMethod() == HTTPRequest::HTTP_POST)
		{
			std::istream &i = request.stream();
			std::string s((istreambuf_iterator<char>(i)), istreambuf_iterator<char>());
			logger.debug("RFID Event Request Body: %s", s);
			StringTokenizer t4(s, "=", StringTokenizer::TOK_TRIM | StringTokenizer::TOK_IGNORE_EMPTY);
			std::string payload = t4[1];
			logger.trace("payload: %s", payload);
			if(payload != "NULL" && payload.size()>3)
			{
				std::string tail = payload.substr(payload.size()-2, 2);
				if(tail == "a1" || tail == "A1")
				{
					prod->_WSEvent.sw.restart();
					prod->_WSEvent.Operator = prod->mb->GotTargetNo(payload);
					logger.information("api偵測到人員卡: %s", prod->_WSEvent.Operator);
				}
				else if(tail == "d4" || tail == "D4")
				{
					prod->_WSEvent.sw.restart();
					prod->_WSEvent.LotNO = prod->mb->GotTargetNo(payload);
					logger.information("api偵測到Run Card: %s", prod->_WSEvent.LotNO);
				}
			}
		}
		LocalDateTime now;
		MainObject.set("Timestamp", DateTimeFormatter::format(now, "%Y-%m-%d %H:%M:%S"));
		MainObject.stringify(ostr);
		return;
	}
private:
	Logger& logger;
	Prod* prod;
};
#endif
class RequestHandlerFactory: public HTTPRequestHandlerFactory
{
public:
	RequestHandlerFactory(Prod* _prod):logger(Logger::get("Router")), prod(_prod){}
	//路由控制
	HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request)
	{
//		logger.information("Request from: %s to %s%s", request.clientAddress().toString(), request.getHost(), request.getURI());
		URI RequestPath(request.getURI());
		std::vector<std::string> PathSegments;
		RequestPath.getPathSegments(PathSegments);
		Path ErrorPage(prod->config->getString("application.dir")+"/public/error.html", Path::PATH_UNIX);
		Path target(prod->config->getString("application.dir"), Path::PATH_UNIX);
		//check ws connection
		if(request.find("Upgrade") != request.end() && Poco::icompare(request["Upgrade"], "websocket") == 0)
		{
			logger.information("websocket 連線請求");
			return new WebSocketHandler(prod);
		}
		else if(PathSegments.size() == 0)
		{
			target.pushDirectory("dist");
			target.setFileName("index.html");
			return new PublicHandler(target);
		}
		else if(PathSegments[0] == "fonts" || PathSegments[0] == "css"
				|| PathSegments[0] == "js" || PathSegments[0] == "img"
				|| PathSegments[0] == "index.html" || PathSegments[0] == "favicon.ico")
		{
			target.pushDirectory("dist");
			for(uint i=0; i<PathSegments.size(); i++)
			{
				if(i == PathSegments.size()-1)
				{
					target.setFileName(PathSegments[i]);
				}
				else
				{
					target.pushDirectory(PathSegments[i]);
				}
			}
			File f(target);
			if(!f.exists() || !f.isFile())
			{
				logger.error("Can't find %s", target.toString(Path::PATH_UNIX));
				target = ErrorPage;
			}
			return new PublicHandler(target);
		}
		else if(PathSegments[0] == "public")
		{
			for(uint i=0; i<PathSegments.size(); i++)
			{
				if(i == PathSegments.size()-1)
				{
					target.setFileName(PathSegments[i]);
				}
				else
				{
					target.pushDirectory(PathSegments[i]);
				}
			}
			File f(target);
			if(!f.exists() || !f.isFile())
			{
				logger.warning("Can't find %s", target.toString(Path::PATH_UNIX));
				target = ErrorPage;
			}
			return new PublicHandler(target);
		}
		else if(PathSegments[0] == "api")
		{
			return new ApiHandler(prod);
		}
		else
		{
			logger.error("Undefined Request: %s%s", request.getHost(), request.getURI());
			return new PublicHandler(ErrorPage.toString(Path::PATH_UNIX));
		}
	}
private:
	Logger& logger;
	Prod* prod;
};

class VCP30: public ServerApplication
{
public:
	VCP30(): _helpRequested(false){}

	~VCP30(){}

protected:
	void initialize(Application& self)
	{
		loadConfiguration(); // load default configuration files, if present
		ServerApplication::initialize(self);
		if(_helpRequested)
		{
			terminate();
		}
		// create logger
		AutoPtr<SplitterChannel> DoubleChannel(new SplitterChannel());
		Path current(config().getString("application.dir"), Path::PATH_UNIX);
		current = current.pushDirectory("log");
		File log_folder(current);
		try
		{
			log_folder.createDirectory();
			current.append("log");
		}
		catch(Exception& e)
		{
			cerr << e.displayText() << endl;
		}
		//File Channel
		AutoPtr<FileChannel> FileConsole(new FileChannel(current.toString(Path::PATH_UNIX)));
		FileConsole ->setProperty("rotation", "daily");
		FileConsole ->setProperty("archive", "timestamp");
		FileConsole ->setProperty("compress", "true");
		FileConsole ->setProperty("rotateOnOpen", "false");
		FileConsole ->setProperty("times", "local");
		//Console Channel
		AutoPtr<ColorConsoleChannel> ColorConsole(new ColorConsoleChannel);
		ColorConsole->setProperty("traceColor", "blue");
		ColorConsole->setProperty("debugColor", "white");
		ColorConsole->setProperty("informationColor", "green");
		ColorConsole->setProperty("noticeColor", "cyan");
		ColorConsole->setProperty("warningColor", "yellow");
		ColorConsole->setProperty("errorColor", "red");
		ColorConsole->setProperty("criticalColor", "lightMagenta");
		ColorConsole->setProperty("fatalColor", "magenta");

		DoubleChannel -> addChannel(FileConsole);
		DoubleChannel -> addChannel(ColorConsole);

		AutoPtr<PatternFormatter> Channel_Formatter(new PatternFormatter);
		Channel_Formatter->setProperty("pattern", "[%Y-%m-%d %H:%M:%S][%s][%p]:%t");
		Channel_Formatter->setProperty("times", "local");

		Logger::root().setChannel(new FormattingChannel(Channel_Formatter, DoubleChannel));
		Logger::root().setLevel(Message::PRIO_TRACE);
		Logger& logger = Logger::get("Initialize");
		logger.information("Process Initialize");
		nc = new NotificationCenter();
		//啟動RFID
		logger.information("RFID Hardware: %s", config().getString("DEVICE.RFID", UART_PL3));
		RFID = new Rfid_ds(config().getString("DEVICE.RFID").c_str());
		network = new Network(&(config()));
		//連結資料庫
		mb = new MESBridge();
		myb = new MyBridge();
		rb = new RedisBridge("127.0.0.1", 6379);
		lb = new LocalBridge(&(config()));
		plc = new PLC(config().getString("PLC.ID"), &(config()), nc);
		plc->Connect(config().getString("PLC.IP").c_str(), 9600, "eth0", PROTOCOLTYPE_TCP);
		plc->Load();
//		plc->LoadAlarm(config().getString("PLC.ALARM_FILE"));
		ws = NULL;
		prod = new Prod(&(config()), RFID, mb, myb, rb, lb, plc, ws);
		/*加入監聽事件*/
		nc->addObserver(Observer<MyBridge, AlarmNotification>(*myb, &MyBridge::handleAlarm));
	}

	void uninitialize()
	{
		ServerApplication::uninitialize();
	}
	void restart()
	{
		ServerApplication::reinitialize(*this);
	}

	void defineOptions(OptionSet& options)
	{
		ServerApplication::defineOptions(options);

		options.addOption(
			Option("help", "h", "display help information on command line arguments")
				.required(false)
				.repeatable(false));
	}

	void handleOption(const std::string& name, const std::string& value)
	{
		ServerApplication::handleOption(name, value);

		if (name == "help")
		{
			_helpRequested = true;
			HelpFormatter helpFormatter(options());
			helpFormatter.setCommand(commandPath());
			helpFormatter.setUsage("OPTIONS");
			helpFormatter.setHeader("VCP-30 PPR Edge");
			helpFormatter.setFooter("( ° ͜ʖ͡°)╭∩╮");
			helpFormatter.format(std::cout); //Display on terminal
		}
	}

	int main(const std::vector<std::string>& args)
	{
		Logger& logger = Logger::get("Main");

		Timer NetworkTimer(0, 30000);
		logger.information("啟動網路斷線偵測背景程式");
		NetworkTimer.start(TimerCallback<Network>(*network, &Network::Ping));

		Timer DatabaseTimer(0, 1800000);
		if(config().getBool("PROGRAM.DATABASE", true))
		{
			logger.information("啟動RFID內碼轉換表同步背景程式");
			DatabaseTimer.start(TimerCallback<MESBridge>(*mb, &MESBridge::RFIDCloneTimer));
		}

		Timer PLCTimer(0, 1000);
		if(config().getBool("PROGRAM.PLC", true))
		{
			logger.information("啟動PLC資料紀錄功能");
			PLCTimer.start(TimerCallback<PLC>(*plc, &PLC::Base));
		}
		/* RFID觸發 流程 */
		RunnableAdapter<Prod> runnableReader(*prod, &Prod::Reader);
//		RunnableAdapter<Prod> runnableforPreMO(*prod, &Prod::forPreMO);
//		RunnableAdapter<Prod> runnableforWaitEvent(*prod, &Prod::WaitEvent);
		RunnableAdapter<Prod> runnableforPPRloop(*prod, &Prod::PPRloop);
		RunnableAdapter<Prod> runnableforBackgroundPPR(*prod, &Prod::BackgroundPPR);
		RunnableAdapter<Prod> runnableforActiveResponse(*prod, &Prod::ActiveResponse);
		ThreadPool::defaultPool().start(runnableforPPRloop);
		ThreadPool::defaultPool().start(runnableforActiveResponse);
		ThreadPool::defaultPool().start(runnableforBackgroundPPR);
		if(config().getBool("PROGRAM.READER", false))
		{
			logger.information("啟動RFID reader");
			ThreadPool::defaultPool().start(runnableReader);
//			ThreadPool::defaultPool().start(runnableforPreMO);
//			ThreadPool::defaultPool().start(runnableforWaitEvent);
		}
		// set-up a server socket
		ServerSocket svs(config().getInt("DEVICE.PORT", 9999));
		// set-up a HTTPServer instance
		HTTPServer srv(new RequestHandlerFactory(prod), svs, new HTTPServerParams);
		// start the HTTPServer
		srv.start();
		// wait for CTRL-C or kill
		waitForTerminationRequest();
		// Stop the HTTPServer
		srv.stop();
		return Application::EXIT_OK;
	}

private:
	bool _helpRequested;
	Prod *prod;
	Network* network;
	Rfid_ds* RFID;
	MESBridge* mb;
	MyBridge* myb;
	RedisBridge* rb;
	LocalBridge* lb;
	WebSocket* ws;
	PLC* plc;
	NotificationCenter* nc;
	PPRDEVICE* ppr;
};

POCO_SERVER_MAIN(VCP30)
