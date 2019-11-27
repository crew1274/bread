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
		else if(PathSegments[1] == "VCP_10" && PathSegments[2] == "prod_1" && request.getMethod() == HTTPRequest::HTTP_POST)
		{
			logger.information("Prod => Stage1");
			ReciveObject = parser.parse(s).extract<Object::Ptr>();
			if(!ReciveObject->has("LotNO") || !ReciveObject->has("Operator") || !ReciveObject->has("ProcSeq"))
			{
				throw ApplicationException("參數缺少");
			}
			std::string LotNO = ReciveObject->get("LotNO").convert<std::string>();
			std::string Operator = ReciveObject->get("Operator").convert<std::string>();
			std::string ProcSeq = ReciveObject->get("ProcSeq").convert<std::string>();
			logger.information("LotNO:%s, Operator:%s, ProcSeq:%s", LotNO, Operator, ProcSeq);
			//return
			std::string target = "鍍銅";
			std::string payload;
			if(!RecipeRequest(LotNO, ProcSeq, payload)) //取得參數
			{
				throw ApplicationException("請求製程參數錯誤");
			}
			logger.debug("請求製程參數完成!");
			if(!ParseXML(payload, MainObject)) //取得製程序以及批號
			{
				throw ApplicationException("無法確認為鍍銅站製程參數，請確認批號或是製程序是否正確");
			}
			logger.information("%s製程參數確認為鍍銅站參數", LotNO);

			float RD05M136;
			std::string itemno = MainObject.get("itemno").convert<std::string>();
			std::string itemver = MainObject.get("itemver").convert<std::string>();
			std::string mfver = MainObject.get("mfver").convert<std::string>();
			if(prod->mb->getRD05M136(itemno, itemver, mfver, RD05M136))
			{
				MainObject.set("RD05M136", RD05M136);
			}
			logger.information("確認板厚資訊:%hf", RD05M136);
		}
		else if(PathSegments[1] == "checkPPR" && request.getMethod() == HTTPRequest::HTTP_GET)
		{
			/*取得PPR.json資訊*/
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
		else if(PathSegments[1] == "error")
		{
			std::vector<ERRORmessage> msg;
			prod->lb->getERROR(msg);
			JSON::Array result;
			for(uint i=0; i<msg.size(); i++)
			{
				JSON::Object temp;
				temp.set("datetime", msg[i].datetime);
				temp.set("message", msg[i].message);
				temp.set("device", msg[i].device);
				temp.set("status", msg[i].status);
				result.add(temp);
			}
			MainObject.set("result", result);
		}
		else if(PathSegments[1] == "mo_all")
		{
			//查詢全資料
			std::vector<recipe> MOs;
			prod->lb->getMO(MOs);
			JSON::Array result;
			for(uint i=0; i<MOs.size(); i++)
			{
				JSON::Object temp;
				temp.set("STARTDATETIME", MOs[i].STARTDATETIME);
				temp.set("ENDDATETIME", MOs[i].ENDDATETIME);
				temp.set("LOTNO", MOs[i].LOTNO);
				temp.set("PARTNO", MOs[i].PARTNO);
				temp.set("RANDOMSTRING", MOs[i].RANDOMSTRING);
				temp.set("SOURCE", MOs[i].SOURCE);
				temp.set("detail", getDetail(MOs[i].RANDOMSTRING));
				result.add(temp);
			}
			MainObject.set("result", result);
		}
		else if(PathSegments[1] == "mo")
		{
			if(PathSegments.size() == 2)
			{
				std::vector<recipe> MOs;
				prod->lb->getMO(MOs);
				JSON::Array result;
				for(uint i=0; i<MOs.size(); i++)
				{
					JSON::Object temp;
					temp.set("STARTDATETIME", MOs[i].STARTDATETIME);
					temp.set("ENDDATETIME", MOs[i].ENDDATETIME);
					temp.set("LOTNO", MOs[i].LOTNO);
					temp.set("PARTNO", MOs[i].PARTNO);
					temp.set("RANDOMSTRING", MOs[i].RANDOMSTRING);
					temp.set("SOURCE", MOs[i].SOURCE);
					result.add(temp);
				}
				MainObject.set("result", result);
			}
			else if(PathSegments.size() == 3)
			{
				MainObject.set("result", getDetail(PathSegments[2]));
			}
		}
		else if(PathSegments[1] == "test" && PathSegments[2] == "insertHistory")
		{
			prod->insertHistory();
		}
		else if(PathSegments[1] == "test" && PathSegments[2] == "updateHistory")
		{
			prod->lb->updateMO();
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

JSON::Object::Ptr ApiHandler::getDetail(std::string RANDOMSTRING)
{
	JSON::Object::Ptr resultObject;
	JSON::Object temp;
	resultObject = prod->ab->Bridge(HTTPRequest::HTTP_GET,
			"/_db/VCP-30/_api/document/History/"+RANDOMSTRING, temp);
	if(resultObject->has("error"))
	{
		logger.error(resultObject->get("errorMessage").convert<std::string>());
		Path ProdFile(prod->config->getString("application.dir"), Path::PATH_UNIX);
		ProdFile.pushDirectory("history");
		ProdFile.setFileName(RANDOMSTRING+".json");
		File targetFile(ProdFile);
		std::ostringstream ostr;
		if(!targetFile.exists())
		{
			throw ApplicationException("無 "+RANDOMSTRING+".json");
		}
		FileInputStream fis(ProdFile.toString());
		StreamCopier::copyStream(fis, ostr);
		Parser file_parser;
		resultObject = file_parser.parse(ostr.str()).extract<JSON::Object::Ptr>();
	}
	return resultObject;
}

bool ApiHandler::RecipeRequest(std::string LotNO, std::string ProcSeq, std::string& response)
{
	try
    {
        URI url("http://10.11.0.139/mesws_chpt/wsmes/wsmes.asmx/GetParameter");
        HTTPClientSession session(url.getHost(), url.getPort());
        string path(url.getPathAndQuery());
        if (path.empty())
		{
        	path = "/";
		}
        HTTPRequest req(HTTPRequest::HTTP_POST, path, HTTPMessage::HTTP_1_1);
        req.setContentType("application/x-www-form-urlencoded;charset=utf-8");
        HTMLForm form;
        form.add("InXml", "<?xml version='1.0' encoding='utf-8'?><mfdata><lotdata><no>"
        		+ LotNO +"</no><itemno></itemno><itemver></itemver><mfver></mfver><procseq>"
				+ ProcSeq +"</procseq></lotdata><procdata></procdata><exception></exception></mfdata>");
        form.prepareSubmit(req);
        form.write(session.sendRequest(req));
        HTTPResponse res;
        istream &is = session.receiveResponse(res);
        StreamCopier::copyToString(is, response);
        return true;
    }
    catch (Exception &e)
    {
        logger.error(e.displayText());
    }
    return false;
}

bool ApiHandler::ParseXML(std::string payload, JSON::Object& rs)
{
	logger.debug("Ready for parse XML");
	try
	{
		DOMParser parser;
		AutoPtr<Document> pDoc = parser.parseString(payload);

		NodeIterator it(pDoc, NodeFilter::SHOW_ALL);
		Node* pNode = it.nextNode();
		pNode = it.nextNode();
		pNode = it.nextNode();

		//拆解第二層XML;
		bool header = true;
		int count = 0;
		DOMParser parser_1;
		AutoPtr<Document> pDoc_1 = parser_1.parseString(pNode->nodeValue());
		NodeIterator it_1(pDoc_1, NodeFilter::SHOW_ALL);
		Node* pNode_1 = it_1.nextNode();
		while (pNode_1)
		{
			if(pNode_1->nodeName() == "procdata")
			{
				header = false;
			}
			if(header)
			{
				if(pNode_1->nodeName() == "itemno") /*尋找料號*/
				{
					pNode_1 = it_1.nextNode();
					rs.set("itemno", pNode_1->nodeValue());
					logger.information("Find itemno = %s", rs.get("itemno").convert<std::string>());
				}
				else if(pNode_1->nodeName() == "itemver")
				{
					pNode_1 = it_1.nextNode();
					rs.set("itemver", pNode_1->nodeValue());
					logger.information("Find itemver = %s", rs.get("itemver").convert<std::string>());
				}
				else if(pNode_1->nodeName() == "mfver")
				{
					pNode_1 = it_1.nextNode();
					rs.set("mfver", pNode_1->nodeValue());
					logger.information("Find mfver = %s", rs.get("mfver").convert<std::string>());
				}
			}
			else
			{

				if(pNode_1->getNodeValue() == "RD05M47")
				{
					pNode_1 = it_1.nextNode();
					pNode_1 = it_1.nextNode();
					rs.set("RD05M47", pNode_1->nodeValue());
					logger.information("Extract RD05M47:%s", rs.get("RD05M47").convert<std::string>());
					count++;
				}
				else if(pNode_1->getNodeValue() == "RD05M134")
				{
					pNode_1 = it_1.nextNode();
					pNode_1 = it_1.nextNode();
					rs.set("RD05M134", pNode_1->nodeValue());
					logger.information("Extract RD05M47:%s", rs.get("RD05M134").convert<std::string>());
					count++;
				}
				else if(pNode_1->getNodeValue() == "RD05M145")
				{
					pNode_1 = it_1.nextNode();
					pNode_1 = it_1.nextNode();
					rs.set("RD05M145", pNode_1->nodeValue());
					logger.information("Extract RD05M145:%s", rs.get("RD05M145").convert<std::string>());
					count++;
				}
				else if(pNode_1->getNodeValue() == "RD05M146")
				{
					pNode_1 = it_1.nextNode();
					pNode_1 = it_1.nextNode();
					rs.set("RD05M146", pNode_1->nodeValue());
					logger.information("Extract RD05M146:%s", rs.get("RD05M146").convert<std::string>());
					count++;
				}
				else if(pNode_1->getNodeValue() == "RD05M49")
				{
					pNode_1 = it_1.nextNode();
					pNode_1 = it_1.nextNode();
					rs.set("RD05M49", pNode_1->nodeValue());
					logger.information("Extract RD05M49:%s", rs.get("RD05M49").convert<std::string>());
					count++;
				}
			}
			pNode_1 = it_1.nextNode();
		}
		if(count>=4)
		{
			return true;
		}
		else
		{
			throw ApplicationException("無法確認為鍍銅站製程參數");
		}
	}
	catch (Exception& e)
	{
		logger.error(e.displayText());
	}
	return false;
}

Object ApiHandler::ProdCheck()
{
	Object obj;
//	prod->RFID_ret != -1? obj.set("RFID", true): obj.set("RFID", false);
//	obj.set("PLC", prod->plc->ret);
	std::string res;
//	obj.set("MES", prod->RecipeRequest("", "", res));
	return obj;
}
