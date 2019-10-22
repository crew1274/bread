/*
 * ApiHandler.cpp
 *
 *  Created on: 2019年9月5日
 *      Author: 171104
 */

#include <app/Handler/ApiHandler.h>

ApiHandler::ApiHandler(Prod* _prod): logger(Logger::get("ApiHandler")), prod(_prod)
{
	// TODO Auto-generated constructor stub

}

ApiHandler::~ApiHandler()
{
	// TODO Auto-generated destructor stub
}

bool ApiHandler::checkPPR(JSON::Array::Ptr& ReciveArray)
{
	Path pwd(prod->config->getString("application.dir"), Path::PATH_UNIX);
	pwd.setFileName("PPR.json");
	File inputFile(pwd);
	std::ostringstream ostr;
	if(inputFile.exists())
	{
		FileInputStream fis(pwd.toString());
		StreamCopier::copyStream(fis, ostr);
		Parser parser;
		ReciveArray = parser.parse(ostr.str()).extract<JSON::Array::Ptr>();
		return true;
	}
	else
	{
		return false;
	}
}

void ApiHandler::storePPR(std::string recipes)
{
	//覆蓋 PPR.json
	Path pwd(prod->config->getString("application.dir"), Path::PATH_UNIX);
	pwd.setFileName("PPR.json");
	std::ofstream ofs(pwd.toString().c_str(), std::ios::trunc);
	ofs << recipes;
	ofs.close();
	//寫入PLC
	Parser parser;
	float current;
	JSON::Array::Ptr ReciveArray = parser.parse(recipes).extract<JSON::Array::Ptr>();
	if(ReciveArray->size() != 5)
	{
		throw ApplicationException("PPR參數異常");
	}
	//寫入暫存區
	int total_time = 0;
	for(uint i=0; i<ReciveArray->size(); i++)
	{
		int address = 30020 + (i*5);
		JSON::Object::Ptr item = ReciveArray->getObject(i);
		prod->plc->WritePoint_PLC("EM_3", address, item->get("current_time").convert<int>(), true);
		if(i>=1 and i<=3)	//1~3
		{
			total_time = total_time + item->get("current_time").convert<int>();
		}
		current = item->get("forward_current").convert<float>();
		prod->plc->WritePoint_PLC("EM_3", address+1, static_cast<int>(current * 10), false);
		prod->plc->WritePoint_PLC("EM_3", address+3, item->get("forward_current_time").convert<int>(), false);
		if(item->get("type").convert<std::string>() == "AC")
		{
			current = item->get("reverse_current").convert<float>();
			prod->plc->WritePoint_PLC("EM_3", address+2, static_cast<int>(current * 10), false);
			prod->plc->WritePoint_PLC("EM_3", address+4, item->get("reverse_current_time").convert<int>(), false);
		}
		else
		{
			prod->plc->WritePoint_PLC("EM_3", address+2, 0, false);
			prod->plc->WritePoint_PLC("EM_3", address+4, 0, false);
		}
	}
	prod->plc->WritePoint_PLC("EM_3", 30009, total_time, true);	//電鍍時間(min)
	prod->plc->WritePoint_PLC("EM_3", 30015, (total_time*60)/10000, true);	//寫入電鍍時間 HighBit
	prod->plc->WritePoint_PLC("EM_3", 30014, (total_time*60)%10000, true);	//寫入電鍍時間 LowBit
}

void ApiHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
//	logger.debug("Request from: %s to %s%s", request.clientAddress().toString(), request.getHost(), request.getURI());
//	logger.debug("Request Method: %s", request.getMethod());
	URI RequestPath(request.getURI());
	std::vector<std::string> PathSegments;
	RequestPath.getPathSegments(PathSegments);
	response.setVersion(HTTPMessage::HTTP_1_1);
	response.setChunkedTransferEncoding(true);
	response.setContentType("application/json");
	response.set("Access-Control-Allow-Origin", "*");
	response.set("Access-Control-Allow-Credentials", "true");
	response.set("Access-Control-Allow-Methods", "POST, GET, OPTIONS, DELETE");
	response.setStatusAndReason(HTTPResponse::HTTP_OK);
	std::ostream& ostr = response.send();
	std::istream &i = request.stream();
	string s((istreambuf_iterator<char>(i)), istreambuf_iterator<char>());
	JSON::Object MainObject;
	JSON::Object::Ptr ReciveObject;
	JSON::Array::Ptr ReciveArray;
	Parser parser;
	try
	{
		if(PathSegments.size()<2) //檢查PathSegments長度
		{
			throw ApplicationException("PathSegments長度過短");
		}
		if(PathSegments[1] == "now" && request.getMethod() == HTTPRequest::HTTP_GET)
		{
			MainObject.set("DM3500", 1);
		}
		else if(PathSegments[1] == "getRD05M136" && request.getMethod() == HTTPRequest::HTTP_POST)
		{
			cout << s << endl;
			ReciveObject = parser.parse(s).extract<JSON::Object::Ptr>();
			float RD05M136;
			std::string itemno = ReciveObject->get("itemno").convert<std::string>();
			std::string itemver = ReciveObject->get("itemver").convert<std::string>();
			std::string mfver = ReciveObject->get("mfver").convert<std::string>();
			if(prod->mb->getRD05M136(itemno, itemver, mfver, RD05M136))
			{
				MainObject.set("result", RD05M136);
			}
			else
			{
				throw ApplicationException("查無["+ReciveObject->get("no").convert<std::string>()+"]板厚資訊");
			}
		}
		else if(PathSegments[1] == "checkPPR" && request.getMethod() == HTTPRequest::HTTP_GET)
		{
			checkPPR(ReciveArray);
			MainObject.set("result", ReciveArray);
		}
		else if(PathSegments[1] == "startPPR" && request.getMethod() == HTTPRequest::HTTP_POST)
		{

			if(!prod->plc->OverWrite("EM_3", 11, 0, 30099, 31099, 1, true)) //寫入運轉靶數
			{
				throw ApplicationException("觸發啟動點位錯誤");
			}
			MainObject.set("response", true);
		}
		else if(PathSegments[1] == "stopPPR" && request.getMethod() == HTTPRequest::HTTP_POST)
		{
			MainObject.set("response", true);
		}
		else if(PathSegments[1] == "startPPR" && request.getMethod() == HTTPRequest::HTTP_POST)
		{
			if(!prod->plc->OverWrite("EM_3", 11900, 0, 30099, 31099, 1, true)) //寫入運轉靶數
			{
				throw ApplicationException("修改觸發點位錯誤");
			}
			MainObject.set("response", true);
		}
		else if(PathSegments[1] == "storePPR" && request.getMethod() == HTTPRequest::HTTP_POST)
		{
			/*保存參數 */
			storePPR(s);
			if(!prod->plc->OverWrite("EM_3", 11900, 0, 30000, 31000, 1, true)) //寫入運轉靶數
			{
				throw ApplicationException("修改運轉靶數錯誤");
			}
			MainObject.set("response", true);
		}
		else if(PathSegments[1] == "prod")
		{
			if(PathSegments[2] == "check" && request.getMethod() == HTTPRequest::HTTP_GET)
			{
				MainObject.set("check", ProdCheck());
			}
		}
		else if(PathSegments[1] == "PLC")
		{
			if(PathSegments[2] == "temp" && request.getMethod() == HTTPRequest::HTTP_POST) //寫入參數
			{
				ReciveObject = parser.parse(s).extract<Object::Ptr>();
				if(ReciveObject->has("ppr_result") and ReciveObject->has("ppr_data"))
				{
					logger.information("請求寫入暫存區");
					MainObject.set("response", prod->ReadyProd(ReciveObject));
				}

			}
			else if(PathSegments[2] == "prod" && request.getMethod() == HTTPRequest::HTTP_POST)	//啟動自動模式
			{
				logger.information("啟動自動模式");
				MainObject.set("response", prod->confirmProd());
			}
		}
		else if(PathSegments[1] == "CallAGV")
		{
			logger.information("呼叫AGV");
			if(prod->config->getBool("DEVICE.ENV"))
			{
				MainObject.set("response", prod->CallAGV());
			}
			else
			{
				MainObject.set("response", true);
			}
		}
		else if(PathSegments[1] == "RFID" && request.getMethod() == HTTPRequest::HTTP_GET)
		{
			Object inner;
			inner.set("Operator", "171104");
			prod->theEvent(this, inner);
		}
		else if(PathSegments[1] == "RFID" && request.getMethod() == HTTPRequest::HTTP_POST)
		{
			StringTokenizer st(s, "=", StringTokenizer::TOK_TRIM | StringTokenizer::TOK_IGNORE_EMPTY);
			std::string payload = st[1];
//			logger.trace("RFID API payload: %s", payload);
			std::string target;
			Object inner;
			std::ostringstream oss;
			if(payload != "NULL" && payload.size()>3)
			{
				std::string tail = payload.substr(payload.size()-2, 2);
				if(tail == "a1" || tail == "A1")
				{
					target = prod->mb->GotTargetNo(payload);
					logger.information("RFID Reader API偵測到人員卡: %s", target);
					inner.set("Operator", target);
					prod->theEvent(this, inner);
				}
				else if(tail == "d4" || tail == "D4")
				{
					target = prod->mb->GotTargetNo(payload);
					logger.information("RFID Reader API偵測到工單: %s", target);
					inner.set("LotNO", target);
					prod->theEvent(this, inner);
				}
			}
		}
		else
		{
			logger.debug("Request from: %s to %s%s", request.clientAddress().toString(), request.getHost(), request.getURI());
			logger.debug("Request Method: %s", request.getMethod());
			throw ApplicationException("無效路徑請求");
		}
	}
    catch (Exception &e)
    {
        logger.error(e.displayText());
		MainObject.set("Exception", e.displayText());
		MainObject.set("RequestPath", RequestPath.toString());
    }
	LocalDateTime now;
	MainObject.set("Timestamp", DateTimeFormatter::format(now, "%Y-%m-%d %H:%M:%S"));
	MainObject.stringify(ostr);
    std::ostringstream ss;
    ss << ostr.rdbuf();
	return;
}

Object ApiHandler::ProdCheck()
{
	Object obj;
	obj.set("EDGE", prod->isFine);
//	prod->RFID_ret != -1? obj.set("RFID", true): obj.set("RFID", false);
//	obj.set("PLC", prod->plc->ret);
	std::string res;
	obj.set("MES", prod->RecipeRequest("", "", res));
	return obj;
}
