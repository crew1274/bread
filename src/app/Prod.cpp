/*
 * Prod.cpp
 *
 *  Created on: 2018年7月19日
 *      Author: 171104
 */

#include <app/Prod.h>

Prod::Prod(AutoPtr<AbstractConfiguration> _config, Rfid_ds* _RFID, MESBridge* _mb, MyBridge* _myb, RedisBridge* _rb, LocalBridge* _lb, ArangoBridge* _ab, PLC* _plc, WebSocket* _ws):
logger(Logger::get("Production")), config(_config), RFID(_RFID),
mb(_mb), myb(_myb), rb(_rb), lb(_lb), ab(_ab), plc(_plc), ws(_ws),
BreakPoint(false), _ActiveMethod(this, &Prod::toTSDB)
{
	// TODO Auto-generated constructor stub
	PPRstatusMap[1] = "入料中";
	PPRstatusMap[2] = "電鍍第一段";
	PPRstatusMap[3] = "電鍍第二段";
	PPRstatusMap[4] = "電鍍第三段";
	PPRstatusMap[5] = "出料中";
	PPRstatusMap[6] = "停止中";
	PPRstatusMap[10] = "異常";
	PPRstatus = plc->ReadPoint_PLC("DM", 20004, 0, false);
	logger.information("PPR起始狀態為: %d", PPRstatus);
	ActiveDetect = false; //回應給PLC
//	ppr = new PPRDEVICE(config->getString("DEVICE.PPR").c_str());

}

Prod::~Prod()
{
	// TODO Auto-generated destructor stub
}
/*----------------------------------------------------------------------------------------*/

//bool Prod::getProcSeq(std::string payload, std::string& ProcSeq)
//{
//	logger.debug("Ready for getProcSeq");
//	try
//	{
//		DOMParser parser;
//		AutoPtr<Document> pDoc = parser.parseString(payload);
//
//		NodeIterator it(pDoc, NodeFilter::SHOW_ALL);
//		Node* pNode = it.nextNode();
//		pNode = it.nextNode();
//		pNode = it.nextNode();
//
//		//拆解第二層XML;
//		bool header = true;
//		DOMParser parser_1;
//		AutoPtr<Document> pDoc_1 = parser_1.parseString(pNode->nodeValue());
//		NodeIterator it_1(pDoc_1, NodeFilter::SHOW_ALL);
//		Node* pNode_1 = it_1.nextNode();
//		while (pNode_1)
//		{
//			if(pNode_1->nodeName() == "procdata")
//			{
//				header = false;
//			}
//			if(header)
//			{
//				if(pNode_1->nodeName() == "procseq") /*尋找製程序*/
//				{
//					pNode_1 = it_1.nextNode();
//					ProcSeq = pNode_1->nodeValue();
//					logger.information("Find ProcSeq = %s", ProcSeq);
//					return true;
//				}
//			}
//			pNode_1 = it_1.nextNode();
//		}
//	}
//	catch (Exception& e)
//	{
//		logger.error(e.displayText());
//	}
//	return false;
//}
//
//bool Prod::ParseXML(std::string payload, std::string target, uint length)
//{
//	logger.debug("Ready for parse XML");
//	try
//	{
//		DOMParser parser;
//		AutoPtr<Document> pDoc = parser.parseString(payload);
//
//		NodeIterator it(pDoc, NodeFilter::SHOW_ALL);
//		Node* pNode = it.nextNode();
//		pNode = it.nextNode();
//		pNode = it.nextNode();
//
//		//拆解第二層XML;
//		bool header = true;
//		DOMParser parser_1;
//		AutoPtr<Document> pDoc_1 = parser_1.parseString(pNode->nodeValue());
//		NodeIterator it_1(pDoc_1, NodeFilter::SHOW_ALL);
//		Node* pNode_1 = it_1.nextNode();
//		while (pNode_1)
//		{
//			if(pNode_1->nodeName() == "procdata")
//			{
//				header = false;
//			}
//			if(header)
//			{
//				if(pNode_1->nodeName() == "itemno") /*尋找料號*/
//				{
//					pNode_1 = it_1.nextNode();
//					Recipes["PartNO"] = pNode_1->nodeValue();
//					logger.information("Find PartNO = %s", Recipes["PartNO"] );
//				}
//				if(pNode_1->nodeName() == "procseq") /*尋找製程序*/
//				{
//					pNode_1 = it_1.nextNode();
//					Recipes["ProcSeq"] = pNode_1->nodeValue();
//					logger.information("Find ProcSeq = %s", Recipes["ProcSeq"]);
//				}
//			}
//			else
//			{
//				//	procdata
//				if(pNode_1->nodeName() == "procname")
//				{
//					pNode_1 = it_1.nextNode();
//					std::string procname = pNode_1->nodeValue();
//					logger.information("Detect procname = %s", procname);
//					if(procname.size() > length)
//					{
//						logger.information("procname find %s", procname.substr(0, length));
//						if(procname.substr(0, length) == target)
//						{
//							return true;
//						}
//					}
//				}
//			}
//			pNode_1 = it_1.nextNode();
//		}
//
//	}
//	catch (Exception& e)
//	{
//		logger.error(e.displayText());
//	}
//	return false;
//}
//
//bool Prod::ExtractXML(std::string payload)
//{
//	logger.debug("Ready for Extract XML");
//	int count = 0;
//	try
//	{
//		DOMParser parser;
//		AutoPtr<Document> pDoc = parser.parseString(payload);
//		NodeIterator it(pDoc, NodeFilter::SHOW_TEXT);
//		Node* pNode = it.nextNode();
//		while (pNode)
//		{
//			if(pNode->getNodeValue() == "RD05M134") //成品孔銅
//			{
//				pNode = it.nextNode();
//				pNode = it.nextNode();
//				MO["Spec"] = pNode->getNodeValue();
//				logger.information("Extract RD05M134(成品孔銅):%s", MO["Spec"] );
//				count++;
//			}
//			if(pNode->getNodeValue() == "RD05M47")
//			{
//				pNode = it.nextNode();
//				pNode = it.nextNode();
//				Recipes["Height"] = pNode->getNodeValue();
//				logger.information("Extract Height:%s", Recipes["Height"]);
//				count++;
//			}
//			else if(pNode->getNodeValue() == "RD05M48")
//			{
//				pNode = it.nextNode();
//				pNode = it.nextNode();
//				Recipes["Width"] = pNode->getNodeValue();
//				logger.information("Extract Width:%s", Recipes["Width"]);
//				count++;
//			}
//			else if(pNode->getNodeValue() == "PlatingTime")
//			{
//				pNode = it.nextNode();
//				pNode = it.nextNode();
//				Recipes["PlatingTime"] = pNode->getNodeValue();
//				logger.information("Extract PlatingTime:%s", Recipes["PlatingTime"]);
//				count++;
//			}
//			else if(pNode->getNodeValue() == "PlatingAmp")
//			{
//				pNode = it.nextNode();
//				pNode = it.nextNode();
//				Recipes["PlatingAmp"] = pNode->getNodeValue();
//				logger.information("Extract PlatingAmp:%s", Recipes["PlatingAmp"]);
//				count++;
//			}
//			else if(pNode->getNodeValue() == "PlatingPnl")
//			{
//				pNode = it.nextNode();
//				pNode = it.nextNode();
//				Recipes["PlatingPnl"] = pNode->getNodeValue();
//				logger.information("Extract PlatingPnl:%s", Recipes["PlatingPnl"]);
//				count++;
//			}
//			else if(pNode->getNodeValue() == "FinalAmpTime")
//			{
//				pNode = it.nextNode();
//				pNode = it.nextNode();
//				Recipes["FinalAmpTime"] = pNode->getNodeValue();
//				logger.information("Extract FinalAmpTime:%s", Recipes["FinalAmpTime"]);
//				count++;
//			}
//			else if(pNode->getNodeValue() == "FinalAmpPercentage")
//			{
//				pNode = it.nextNode();
//				pNode = it.nextNode();
//				Recipes["FinalAmpPercentage"] = pNode->getNodeValue();
//				logger.information("Extract FinalAmpPercentage:%s", Recipes["FinalAmpPercentage"]);
//				count++;
//			}
//			else if(pNode->getNodeValue() == "WithDUMMY")
//			{
//				pNode = it.nextNode();
//				pNode = it.nextNode();
//				Recipes["withDummy"] = pNode->getNodeValue();
//				if(pNode->getNodeValue() == "0")
//				{
//					logger.information("Extract withDummy:false");
//				}
//				else
//				{
//					logger.information("Extract withDummy:true");
//				}
//				count++;
//			}
//			else if(pNode->getNodeValue() == "RD05M335")	//AR
//			{
//				pNode = it.nextNode();
//				pNode = it.nextNode();
//				MO["AR"] = pNode->getNodeValue();
//				logger.information("Extract AR:%s", MO["AR"]);
//				count++;
//			}
//			else if(pNode->getNodeValue() == "RD05M336")	//MIN_DRILL_PATH
//			{
//				pNode = it.nextNode();
//				pNode = it.nextNode();
//				MO["MIN_DRILL_PATH"] = pNode->getNodeValue();
//				logger.information("Extract MIN_DRILL_PATH:%s", MO["MIN_DRILL_PATH"]);
//				count++;
//			}
//			else if(pNode->getNodeValue() == "RD05M49") //PLATING_AREA
//			{
//				pNode = it.nextNode();
//				pNode = it.nextNode();
//				MO["PLATING_AREA"] = pNode->getNodeValue();
//				logger.information("Extract MIN_DRILL_PATH:%s", MO["PLATING_AREA"]);
//				count++;
//			}
//			pNode = it.nextNode();
//		}
//		return true;
//	}
//	catch (Exception& e)
//	{
//		logger.error(e.displayText());
//	}
//	return false;
//}
//
//bool Prod::RecipeRequest(std::string LotNO, std::string ProcSeq, std::string& response)
//{
//	try
//    {
//    	// setup url
//        URI url("http://mesap/mesws_chpt/wsmes/wsmes.asmx/GetParameter");
//        HTTPClientSession session(url.getHost());
//
//        // prepare path
//        string path(url.getPathAndQuery());
//        if (path.empty())
//		{
//        	path = "/";
//		}
//
//        // send request
//        HTTPRequest req(HTTPRequest::HTTP_POST, path, HTTPMessage::HTTP_1_1);
//        req.setContentType("application/x-www-form-urlencoded;charset=utf-8");
//
//        // Set the request body
//        HTMLForm form;
//        form.add("InXml", "<?xml version='1.0' encoding='utf-8'?><mfdata><lotdata><no>"
//        		+ LotNO +"</no><itemno></itemno><itemver></itemver><mfver></mfver><procseq>"
//				+ ProcSeq +"</procseq></lotdata><procdata></procdata><exception></exception></mfdata>");
//        form.prepareSubmit(req);
//        form.write(session.sendRequest(req));
//
//        // get response
//        HTTPResponse res;
//        logger.trace("MES web service response: %s", res.getReason());
//        istream &is = session.receiveResponse(res);
//        StreamCopier::copyToString(is, response);
//        return true;
//    }
//    catch (Exception &e)
//    {
//        logger.error(e.displayText());
//    }
//    return false;
//}
//
//bool Prod::Rebuild(std::string &payload)
//{
//	DOMParser parser;
//	NodeIterator it_1(parser.parseString(payload), NodeFilter::SHOW_TEXT);
//	AutoPtr<Document> pDoc = parser.parseString(it_1.nextNode()->nodeValue());
//	NodeIterator it(pDoc, NodeFilter::SHOW_ALL);
//	Node* pNode = it.nextNode();
//	std::string parameter;
//	while (pNode)
//	{
//		if(pNode->nodeName() == "procprams")
//		{
//			try
//			{
//				AutoPtr<Element> Operator = pDoc->createElement("procpram"); //Create procpram Element
//				Operator->setAttribute("ErpName", "人員工號");
//				AutoPtr<Element> Operator_procprammes_Element = pDoc->createElement("procprammes");
//				AutoPtr<Text> Operator_procprammes_Text = pDoc->createTextNode("Operator");
//				Operator_procprammes_Element->appendChild(Operator_procprammes_Text);
//				AutoPtr<Element> Operator_procvalue_Element = pDoc->createElement("procvalue");
//				AutoPtr<Text> Operator_procvalue_Text = pDoc->createTextNode(Recipes["Operator"]);
//				Operator_procvalue_Element->appendChild(Operator_procvalue_Text);
//				Operator->appendChild(Operator_procprammes_Element);
//				Operator->appendChild(Operator_procvalue_Element);
//				pNode->appendChild(Operator);
//
//				AutoPtr<Element> CurrentDensity = pDoc->createElement("procpram"); //Create procpram Element
//				CurrentDensity->setAttribute("ErpName", "電流密度");
//				AutoPtr<Element> CurrentDensity_procprammes_Element = pDoc->createElement("procprammes");
//				AutoPtr<Text> CurrentDensity_procprammes_Text = pDoc->createTextNode("CurrentDensity");
//				AutoPtr<Element> CurrentDensity_procvalue_Element = pDoc->createElement("procvalue");
//				AutoPtr<Text> CurrentDensity_procvalue_Text = pDoc->createTextNode("0");
//				CurrentDensity_procprammes_Element->appendChild(CurrentDensity_procprammes_Text);
//				CurrentDensity_procvalue_Element->appendChild(CurrentDensity_procvalue_Text);
//				CurrentDensity->appendChild(CurrentDensity_procprammes_Element);
//				CurrentDensity->appendChild(CurrentDensity_procvalue_Element);
//				pNode->appendChild(CurrentDensity);
//
//				AutoPtr<Element> ProtectAmp = pDoc->createElement("procpram"); //Create procpram Element
//				ProtectAmp->setAttribute("ErpName", "電鍍前電流");
//				AutoPtr<Element> ProtectAmp_procprammes_Element = pDoc->createElement("procprammes");
//				AutoPtr<Text> ProtectAmp_procprammes_Text = pDoc->createTextNode("ProtectAmp");
//				AutoPtr<Element> ProtectAmp_procvalue_Element = pDoc->createElement("procvalue");
//				AutoPtr<Text> ProtectAmp_procvalue_Text = pDoc->createTextNode("10");
//				ProtectAmp_procprammes_Element->appendChild(ProtectAmp_procprammes_Text);
//				ProtectAmp_procvalue_Element->appendChild(ProtectAmp_procvalue_Text);
//				ProtectAmp->appendChild(ProtectAmp_procprammes_Element);
//				ProtectAmp->appendChild(ProtectAmp_procvalue_Element);
//				pNode->appendChild(ProtectAmp);
//
//				AutoPtr<Element> FinalAmpPercentage = pDoc->createElement("procpram"); //Create procpram Element
//				FinalAmpPercentage->setAttribute("ErpName", "末端電流百分比");
//				AutoPtr<Element> FinalAmpPercentage_procprammes_Element = pDoc->createElement("procprammes");
//				AutoPtr<Text> FinalAmpPercentage_procprammes_Text = pDoc->createTextNode("FinalAmpPercentage");
//				AutoPtr<Element> FinalAmpPercentage_procvalue_Element = pDoc->createElement("procvalue");
//				AutoPtr<Text> FinalAmpPercentage_procvalue_Text = pDoc->createTextNode("10");
//				FinalAmpPercentage_procprammes_Element->appendChild(FinalAmpPercentage_procprammes_Text);
//				FinalAmpPercentage_procvalue_Element->appendChild(FinalAmpPercentage_procvalue_Text);
//				FinalAmpPercentage->appendChild(FinalAmpPercentage_procprammes_Element);
//				FinalAmpPercentage->appendChild(FinalAmpPercentage_procvalue_Element);
//				pNode->appendChild(FinalAmpPercentage);
//
//				AutoPtr<Element> FinalAmpTime = pDoc->createElement("procpram"); //Create procpram Element
//				FinalAmpTime->setAttribute("ErpName", "末端電流時間");
//				AutoPtr<Element> FinalAmpTime_procprammes_Element = pDoc->createElement("procprammes");
//				AutoPtr<Text> FinalAmpTime_procprammes_Text = pDoc->createTextNode("FinalAmpTime");
//				AutoPtr<Element> FinalAmpTime_procvalue_Element = pDoc->createElement("procvalue");
//				AutoPtr<Text> FinalAmpTime_procvalue_Text = pDoc->createTextNode("1");
//				FinalAmpTime_procprammes_Element->appendChild(FinalAmpTime_procprammes_Text);
//				FinalAmpTime_procvalue_Element->appendChild(FinalAmpTime_procvalue_Text);
//				FinalAmpTime->appendChild(FinalAmpTime_procprammes_Element);
//				FinalAmpTime->appendChild(FinalAmpTime_procvalue_Element);
//				pNode->appendChild(FinalAmpTime);
//
//				AutoPtr<Element> PlatingPnl = pDoc->createElement("procpram"); //Create procpram Element
//				PlatingPnl->setAttribute("ErpName", "投入片數");
//				AutoPtr<Element> PlatingPnl_procprammes_Element = pDoc->createElement("procprammes");
//				AutoPtr<Text> PlatingPnl_procprammes_Text = pDoc->createTextNode("PlatingPnl");
//				AutoPtr<Element> PlatingPnl_procvalue_Element = pDoc->createElement("procvalue");
//				AutoPtr<Text> PlatingPnl_procvalue_Text = pDoc->createTextNode("0");
//				PlatingPnl_procprammes_Element->appendChild(PlatingPnl_procprammes_Text);
//				PlatingPnl_procvalue_Element->appendChild(PlatingPnl_procvalue_Text);
//				PlatingPnl->appendChild(PlatingPnl_procprammes_Element);
//				PlatingPnl->appendChild(PlatingPnl_procvalue_Element);
//				pNode->appendChild(PlatingPnl);
//
//				AutoPtr<Element> WithDUMMY = pDoc->createElement("procpram"); //Create procpram Element
//				WithDUMMY->setAttribute("ErpName", "是否有DUMMY");
//				AutoPtr<Element> WithDUMMY_procprammes_Element = pDoc->createElement("procprammes");
//				AutoPtr<Text> WithDUMMY_procprammes_Text = pDoc->createTextNode("WithDUMMY");
//				AutoPtr<Element> WithDUMMY_procvalue_Element = pDoc->createElement("procvalue");
//				AutoPtr<Text> WithDUMMY_procvalue_Text = pDoc->createTextNode("1");
//				WithDUMMY_procprammes_Element->appendChild(WithDUMMY_procprammes_Text);
//				WithDUMMY_procvalue_Element->appendChild(WithDUMMY_procvalue_Text);
//				WithDUMMY->appendChild(WithDUMMY_procprammes_Element);
//				WithDUMMY->appendChild(WithDUMMY_procvalue_Element);
//				pNode->appendChild(WithDUMMY);
//
//				AutoPtr<Element> DUMMYAmp = pDoc->createElement("procpram"); //Create procpram Element
//				DUMMYAmp->setAttribute("ErpName", "Dummy電流");
//				AutoPtr<Element> DUMMYAmp_procprammes_Element = pDoc->createElement("procprammes");
//				AutoPtr<Text> DUMMYAmp_procprammes_Text = pDoc->createTextNode("DUMMYAmp");
//				AutoPtr<Element> DUMMYAmp_procvalue_Element = pDoc->createElement("procvalue");
//				AutoPtr<Text> DUMMYAmp_procvalue_Text = pDoc->createTextNode("10");	//Dummy電流
//				DUMMYAmp_procprammes_Element->appendChild(DUMMYAmp_procprammes_Text);
//				DUMMYAmp_procvalue_Element->appendChild(DUMMYAmp_procvalue_Text);
//				DUMMYAmp->appendChild(DUMMYAmp_procprammes_Element);
//				DUMMYAmp->appendChild(DUMMYAmp_procvalue_Element);
//				pNode->appendChild(DUMMYAmp);
//
//				AutoPtr<Element> PlatingMode = pDoc->createElement("procpram"); //Create procpram Element
//				PlatingMode->setAttribute("ErpName", "電鍍時間模式");
//				AutoPtr<Element> PlatingMode_procprammes_Element = pDoc->createElement("procprammes");
//				AutoPtr<Text> PlatingMode_procprammes_Text = pDoc->createTextNode("PlatingMode");
//				AutoPtr<Element> PlatingMode_procvalue_Element = pDoc->createElement("procvalue");
//				AutoPtr<Text> PlatingMode_procvalue_Text = pDoc->createTextNode("3"); //電鍍時間模式
//				PlatingMode_procprammes_Element->appendChild(PlatingMode_procprammes_Text);
//				PlatingMode_procvalue_Element->appendChild(PlatingMode_procvalue_Text);
//				PlatingMode->appendChild(PlatingMode_procprammes_Element);
//				PlatingMode->appendChild(PlatingMode_procvalue_Element);
//				pNode->appendChild(PlatingMode);
//			}
//		    catch (Exception &e)
//		    {
//		        logger.error(e.displayText());
//		        return false;
//		    }
//		}
//		pNode = it.nextNode();
//	}
//	DOMWriter writer;
//	std::ofstream output((config->getString("application.dir")+"output.xml").c_str(), std::ofstream::out);
//	writer.setNewLine(XMLWriter::NEWLINE_DEFAULT);
//	writer.setOptions(XMLWriter::PRETTY_PRINT);
//	writer.writeNode(output, pDoc);
//	std::ostringstream  temp;
//	writer.writeNode(temp, pDoc);
//	payload = temp.str();
//	return true;
//}

//bool Prod::StartPLC(int random)
//{
//	Object inner;
//	std::ostringstream oss;
//	int TotalPnl = 0;
//	bool withDummy;
//	try
//	{
//		Recipe.clear();
//		for (std::map<std::string, std::string>::iterator it=Recipes_extend.begin(); it!=Recipes_extend.end(); ++it)
//		{
//			MO[it->first] = it->second;
//		}
//		MO["LotNO"] = Recipes["LotNO"];
//		MO["Operator"] = Recipes["Operator"];
//		MO["ProcSeq"] = Recipes["ProcSeq"];
//		MO["Mode"] = Recipes["Mode"];
//		MO["PartNO"] = Recipes["PartNO"];
//
//		Recipe.set("LotNO", Recipes["LotNO"]);
//		Recipe.set("PartNO", Recipes["PartNO"]);
//		Recipe.set("Operator", Recipes["Operator"]);
//		Recipe.set("ProcSeq", Recipes["ProcSeq"]);
//		plc->WritePoint_PLC("EM_3", 30019, random, true); //寫入隨機亂數
//		if(Recipes["withDummy"] == "1")
//		{
//			MO["withDummy"] = "TRUE";
//			withDummy = true;
//		}
//		else if(Recipes["withDummy"] == "0")
//		{
//			MO["withDummy"] = "FALSE";
//			withDummy = false;
//		}
//		Recipe.set("withDummy", withDummy);
//		if(TotalPnl > 8 || NumberParser::parse(Recipes["PlatingPnl"]) < 1)
//		{
//			logger.error("投入片數%d數量錯誤", TotalPnl);
//			return false;
//		}
//		int TotalPnl = NumberParser::parse(Recipes["PlatingPnl"]); //片數
//		int TotalPlatingAmp;
//		TotalPlatingAmp = TotalPnl * NumberParser::parse(Recipes["PlatingAmp"]); //片數 * 單片電流
//		rb->set("LastPlatingAmp", Recipes["PlatingAmp"]);
//		if(withDummy)
//		{
//			TotalPlatingAmp = TotalPlatingAmp + 10;
//		}
//		wsSend(inner, oss);
//		plc->WritePoint_PLC("EM_3", 30018, TotalPlatingAmp, true); //寫入反面電流
//		plc->WritePoint_PLC("EM_3", 30017, TotalPlatingAmp, true); //寫入正面電流
//		rb->set("LastTotalPlatingAmp", NumberFormatter::format(TotalPlatingAmp));
//		Recipe.set("PlatingAmp", NumberFormatter::format(TotalPlatingAmp));
//		MO["PlatingAmp"] = NumberFormatter::format(TotalPlatingAmp);
//		logger.information("總電鍍電流:%d", TotalPlatingAmp);
//		inner.set("MSG", "總電鍍電流:"+ MO["PlatingAmp"]);
//		inner.set("toast", "總電鍍電流計算為:"+MO["PlatingAmp"]);
//		wsSend(inner, oss);
//
//		int FinalPlatingAmp = TotalPlatingAmp * NumberParser::parse(Recipes["FinalAmpPercentage"]) /100 ;
//		logger.information("末端電鍍電流:%d", FinalPlatingAmp);
//		if(FinalPlatingAmp < 10)
//		{
//			logger.warning("末端電鍍電流:%d小於10, 更換為10", FinalPlatingAmp);
//			inner.set("MSG", "原始末端電鍍電流小於10, 更新為10");
//			inner.set("toast", "末端電鍍電流更新為10");
//			wsSend(inner, oss);
//			FinalPlatingAmp = 10;
//		}
//		if(FinalPlatingAmp > 15)
//		{
//			logger.warning("末端電鍍電流:%d大於15, 更換為15", FinalPlatingAmp);
//			inner.set("MSG", "原始末端電鍍電流大於15, 更新為15");
//			inner.set("toast", "末端電鍍電流更新為15");
//			wsSend(inner, oss);
//			FinalPlatingAmp = 15;
//		}
//		plc->WritePoint_PLC("EM_3", 30012, FinalPlatingAmp, true); //寫入末端電鍍電流
//		Recipe.set("FinalPlatingAmp", NumberFormatter::format(FinalPlatingAmp));
//
//		MO["FinalPlatingAmp"] = NumberFormatter::format(FinalPlatingAmp);
//
//		if(withDummy)
//		{
//			TotalPnl = TotalPnl + 2;
//		}
//		logger.information("總片數:%d", TotalPnl);
//		MO["PlatingPnl"] = NumberFormatter::format(TotalPnl);
//
//		plc->WritePoint_PLC("EM_3", 30016, NumberParser::parse(Recipes["Height"]), true); //寫入遮板位置
//		Recipe.set("Height", Recipes["Height"]);
//
//		MO["Height"] = Recipes["Height"];
//		MO["Width"] = Recipes["Width"];
//
//		plc->WritePoint_PLC("EM_3", 30015, (NumberParser::parse(Recipes["PlatingTime"])*60)/10000, true); //寫入電鍍時間 HighBit
//		plc->WritePoint_PLC("EM_3", 30014, (NumberParser::parse(Recipes["PlatingTime"])*60)%10000, true); //寫入電鍍時間 LowBit
//		MO["PlatingTime"] = Recipes["PlatingTime"];
//		Recipe.set("PlatingTime", Recipes["PlatingTime"]);
//
//		plc->WritePoint_PLC("EM_3", 30013, NumberParser::parse(Recipes["PlatingPnl"]), true); //寫入投入片數(不包含Dummy)
//		plc->WritePoint_PLC("EM_3", 30011, (NumberParser::parse(Recipes["FinalAmpTime"])*60)/10000, true); //寫入末端電鍍時間HighBit
//		plc->WritePoint_PLC("EM_3", 30010, (NumberParser::parse(Recipes["FinalAmpTime"])*60)%10000, true); //寫入末端電鍍時間LowBit
//		MO["FinalAmpTime"] = Recipes["FinalAmpTime"];
//		Recipe.set("FinalAmpTime", Recipes["FinalAmpTime"]);
//
//		plc->WritePoint_PLC("EM_3", 30009, NumberParser::parse(Recipes["PlatingTime"]) + NumberParser::parse(Recipes["FinalAmpTime"]), true); //寫入總電鍍時間
////		plc->WritePoint_PLC("EM_3", 30009, NumberParser::parse(Recipes["PlatingTime"]), true); //寫入總電鍍時間
//
//		/*寫入放置位置*/
//		std::list<int> DistanceList = ConvertDistance(NumberParser::parse(Recipes["PlatingPnl"]), static_cast<int>(NumberParser::parseFloat(Recipes["Width"])), withDummy);
//		for(int i=0; i<8; i++)
//		{
//			if(!DistanceList.empty())
//			{
//				cout << DistanceList.front() << endl;
//				plc->WritePoint_PLC("EM_3", 30001+i, DistanceList.front(), true);
//				DistanceList.pop_front();
//			}
//			else
//			{
//				plc->WritePoint_PLC("EM_3", 30001+i, 0, true);
//			}
//		}
//
//		Recipe.set("TotalPnl", NumberFormatter::format(TotalPnl));
//
//		if(!plc->OverWrite("EM_3", 11900, 0, 30000, 31000, TotalPnl, true)) //寫入運轉靶數
//		{
//			logger.error("修改運轉靶數及觸發點位錯誤");
//			return false;
//		}
//		if(!plc->OverWrite("EM_3", 11, 0, 30020, 31020, 1, false))
//		{
//			logger.error("寫入完成[30020]錯誤");
//			return false;
//		}
//
//		return true;
//	}
//    catch (Exception &e)
//    {
//        logger.error(e.displayText());
//    }
//	return false;
//}

std::list<int> Prod::ConvertDistance(int number, int width, bool withDummy)
{
	std::list<int> DistanceList;
	uint amount = number;
	int mid = config->getInt("PLC.MID", 2835);
	int dummy = config->getInt("PLC.DUMMY", 240);
	if(amount%2)
	{
		//odd
		DistanceList.push_back(mid);
		--amount;
	}
	else
	{
		//even
		DistanceList.push_front(mid+(width/2)+10);
		DistanceList.push_back(mid-(width/2)-10);
		amount = amount-2;
	}

	for(uint i=0; i<amount/2; i++)
	{
		DistanceList.push_front(DistanceList.front()+width+10);
		DistanceList.push_back(DistanceList.back()-width-10);
	}
	if(withDummy)
	{
		if(number == 2)
		{
			DistanceList.push_front(ceil(DistanceList.front()+dummy+(width/2)));
			DistanceList.push_back(ceil(DistanceList.back()-dummy-(width/2)));
		}
		else
		{
			DistanceList.push_front(ceil(DistanceList.front()+dummy+(width/2) + 10));
			DistanceList.push_back(ceil(DistanceList.back()-dummy-(width/2) - 10));
		}
	}
	Poco::JSON::Array list;
	for (std::list<int>::iterator it=DistanceList.begin(); it != DistanceList.end(); ++it)
	{
		list.add(NumberFormatter::format(*it));
	}
	while(list.size() < 8)
	{
		list.add(NumberFormatter::format(0));
	}
	Recipe.set("Positions", list);
	return DistanceList;
}

bool Prod::CallAGV()
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
	server.sin_addr.s_addr = inet_addr(config->getString("AGV.IP", "192.168.2.13").c_str());
	server.sin_family = AF_INET;
	server.sin_port = htons(config->getInt("AGV.PORT", 504));

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

	if(res == "ExCallCarResponse$")
	{
		return true;
	}
	else
	{
		throw ApplicationException("AGV回應:"+res);
	}
	return false;
}

void Prod::ActiveResponse()
{
	while(1)
	{
		if(ActiveDetect)
		{
			plc->WritePoint_PLC("W", 503, 0, false, 0);
			ActiveDetect = false;
		}
		else
		{
			plc->WritePoint_PLC("W", 503, 257, false, 0);
			ActiveDetect = true;
		}
		Thread::sleep(3000);
	}
}

//void Prod::PPRloop()
//{
//	while(1)
//	{
//		for(int i=1; i<7; i++) /* 1 2 3 4 5 6*/
//		{
//			if(plc->ReadPoint_PLC("W", 500, i-1, false) && PPRstatus != i)
//			{
//				logger.information("偵測到觸發訊號:%d", i);
//				if( i == 6)
//				{
//					if(StopPPR())
//					{
//						//正常關閉
//						plc->WritePoint_PLC("WW", 501, i, false);
//					}
//					else
//					{
//						//關閉失敗
//						plc->WritePoint_PLC("WW", 501, 10, false);
//						StopPPR();
//					}
//				}
//				else
//				{
//					//啟動PPR
//					if(startPPR(i-1))
//					{
//						//正常啟動
//						plc->WritePoint_PLC("WW", 501, i, false);
//					}
//					else
//					{
//						//啟動失敗
//						plc->WritePoint_PLC("WW", 501, 10, false);
//						startPPR(i-1);
//					}
//				}
//			}
//		}
//		PPRstatus = plc->ReadPoint_PLC("DM", 20004, 0, false);
//		Thread::sleep(1000);
//	}
//}

//bool Prod::StopPPR()
//{
//	try
//	{
//		if(config->getBool("DEVICE.ENV"))
//		{
//			bool result_1 = ppr->operation_start(1, false);
//			bool result_2 = ppr->operation_start(2, false);
//			if(result_1 != 0 || result_2 != 0)
//			{
//				logger.error("關閉PPR失敗");
//				return false;
//			}
//			else
//			{
//				logger.information("關閉PPR成功");
//				return true;
//			}
//		}
//		else
//		{
//			logger.information("關閉PPR成功");
//			return true;
//		}
//	}
//    catch (Exception &e)
//    {
//        logger.error(e.displayText());
//    }
//    return false;
//}

//bool Prod::startPPR(int index)
//{
//	try
//	{
//		Path pwd(config->getString("application.dir"), Path::PATH_UNIX);
//		pwd.setFileName("PPR.json");
//		File inputFile(pwd);
//		std::ostringstream ostr;
//		if(!inputFile.exists())
//		{
//			throw ApplicationException("無 PPR.json");
//		}
//		FileInputStream fis(pwd.toString());
//		StreamCopier::copyStream(fis, ostr);
//		Parser parser;
//		JSON::Array::Ptr ReciveArray = parser.parse(ostr.str()).extract<JSON::Array::Ptr>();
//		JSON::Object::Ptr item = ReciveArray->getObject(index);
//		PPR_Waveform_Table tempTable;
//		tempTable.Cycle = 1;
//		tempTable.Ux1 = 0;
//		tempTable.Ux2 = 0;
//		tempTable.Ix1 = item->get("forward_current").convert<float>();
//		tempTable.Tx1 = item->get("forward_current_time").convert<float>();
//		tempTable.Ix2 = 0;
//		tempTable.Tx2 = 0;
//		if(item->get("type").convert<std::string>() == "AC") //交流
//		{
//			tempTable.Ix2 = item->get("reverse_current").convert<float>();
//			tempTable.Tx2 = item->get("reverse_current_time").convert<float>();
//		}
//		std::vector<PPR_Waveform_Table> ppr_recipe;
//		ppr_recipe.push_back(tempTable);
//		if(ppr->waveform_set(1, 100, ppr_recipe) != 0)
//		{
//			throw ApplicationException("PPR 1 waveform_set error");
//		}
//		if(ppr->waveform_set(2, 100, ppr_recipe) != 0)
//		{
//			throw ApplicationException("PPR 2 waveform_set error");
//		}
//		if(config->getBool("DEVICE.ENV"))
//		{
//			if(ppr->operation_start(1, true) != 0)
//			{
//				throw ApplicationException("PPR 1 operation_start error");
//			}
//			if(ppr->operation_start(2, true) != 0)
//			{
//				throw ApplicationException("PPR 2 operation_start error");
//			}
//		}
//		logger.information("啟動PPR成功");
//		return true;
//	}
//    catch (Exception &e)
//    {
//        logger.error(e.displayText());
//    }
//    return false;
//}

void Prod::BackgroundPPR()
{
	int value = 0;
	std::stringstream payload;
	while(1)
	{
		payload.str("");
		payload << "PPR"; //目標資料表
		payload << ",status="<< PPRstatusMap[PPRstatus] << " "; //狀態描述
		value = plc->ReadPoint_PLC("DM", 12303, 0, false);
		payload << "average_current_1=" << value << ",";


		value = plc->ReadPoint_PLC("DM", 12403, 0, false);
		payload << "average_current_2=" << value << ",";


		value = plc->ReadPoint_PLC("DM", 12304, 0, false);
		payload << "average_voltage_1=" << value << ",";


		value = plc->ReadPoint_PLC("DM", 12404, 0, false);
		payload << "average_voltage_2=" << value;

		if(config->getBool("TSDB.RUN", false))
		{
//			this->_ActiveMethod(payload.str().substr(0, payload.str().size()-1));
			this->_ActiveMethod(payload.str());
		}

		PPRstatus = plc->ReadPoint_PLC("DM", 20004, 0, false);
		if(PRE_PPRstatus != 1 && PPRstatus == 1)
		{
			//入料
			insertHistory();
		}
		else if(PRE_PPRstatus == 5 && PPRstatus == 6)
		{
			//出料
			lb->updateMO();

			JSON::Object ReciveObject;
			LocalDateTime now;
			ReciveObject.set("ENDDATETIME", DateTimeFormatter::format(now, "%Y-%m-%d %H:%M:%S"));
			JSON::Object::Ptr result = ab->Bridge(HTTPRequest::HTTP_PUT, "/_db/VCP-30/_api/document/History", ReciveObject);
			if(result->has("error"))
			{
				logger.error("e");
				logger.error(result->get("errorMessage").convert<std::string>());
			}
		}
		PRE_PPRstatus = PPRstatus;
		Thread::sleep(500);
	}
}

bool Prod::ReadyProd(JSON::Object::Ptr data)
{
	/*
	 * 生產判斷*/
	try
	{
		std::ostringstream Prod_oss;
		data->stringify(Prod_oss);
		Path pwd(config->getString("application.dir"), Path::PATH_UNIX);
		pwd.setFileName("Prod.json");
		std::ofstream Prod_ofs(pwd.toString().c_str(), std::ios::trunc);
		Prod_ofs << Prod_oss.str();
		Prod_ofs.close();

		JSON::Array::Ptr ReciveArray = data->getArray("ppr_result");
		std::ostringstream PPR_oss;
		ReciveArray->stringify(PPR_oss);
		pwd.setFileName("PPR.json");
		std::ofstream PPR_ofs(pwd.toString().c_str(), std::ios::trunc);
		PPR_ofs << PPR_oss.str();
		PPR_ofs.close();

		/*ppr參數*/
		float current;
		int total_time = 0;
		if(ReciveArray->size() != 5)
		{
			throw ApplicationException("PPR參數異常");
		}
		for(uint i=0; i<ReciveArray->size(); i++)
		{
			int address = 30020 + (i*5);
			JSON::Object::Ptr item = ReciveArray->getObject(i);
			plc->WritePoint_PLC("EM_3", address, item->get("current_time").convert<int>(), true);
			if(i>=1 and i<=3)	//1~3
			{
				total_time = total_time + item->get("current_time").convert<int>();
			}
			current = item->get("forward_current").convert<float>();
			plc->WritePoint_PLC("EM_3", address+1, static_cast<int>(current * 10), false);
			plc->WritePoint_PLC("EM_3", address+3, item->get("forward_current_time").convert<int>(), false);
			if(item->get("type").convert<std::string>() == "AC")
			{
				current = item->get("reverse_current").convert<float>();
				plc->WritePoint_PLC("EM_3", address+2, static_cast<int>(current * 10), false);
				plc->WritePoint_PLC("EM_3", address+4, item->get("reverse_current_time").convert<int>(), false);
			}
			else
			{
				plc->WritePoint_PLC("EM_3", address+2, 0, false);
				plc->WritePoint_PLC("EM_3", address+4, 0, false);
			}
		}

		/*舊有參數*/
		std::string erpname = "";
		JSON::Object::Ptr ppr_data = data->getObject("ppr_data");
//		JSON::Array::Ptr procpram = data->getObject("procdata")->getObject("procprams")->getArray("procpram");
		float RD05M47 = ppr_data->get("RD05M47").convert<float>();//長
		logger.information("版高: %hf", RD05M47);
		float RD05M48 = ppr_data->get("RD05M48").convert<float>();//寬
		logger.information("版寬: %hf", RD05M48);
		int pnl = ppr_data->get("PlatingPnl").convert<int>();
		logger.information("運轉靶數(扣除dummy): %d", pnl);
		plc->WritePoint_PLC("EM_3", 30010, 0, true);	//末端 固定給0
		plc->WritePoint_PLC("EM_3", 30011, 0, true);	//末端 固定給0
		plc->WritePoint_PLC("EM_3", 30012, 0, true);	//末端 固定給0
		plc->WritePoint_PLC("EM_3", 30013, pnl, true);	//運轉靶數(扣除dummy)
		plc->WritePoint_PLC("EM_3", 30009, total_time, true);	//電鍍時間(min)
		plc->WritePoint_PLC("EM_3", 30015, (total_time*60)/10000, true);	//寫入電鍍時間 HighBit
		plc->WritePoint_PLC("EM_3", 30014, (total_time*60)%10000, true);	//寫入電鍍時間 LowBit
		plc->WritePoint_PLC("EM_3", 30016, static_cast<int>(RD05M47), true); //寫入遮板位置
		plc->WritePoint_PLC("EM_3", 30019, rand() % 3000 + 1, true); //寫入隨機亂數
		std::list<int> DistanceList = ConvertDistance(pnl, static_cast<int>(RD05M48), true);
		for(int i=0; i<8; i++)	//寫入推桿位置
		{
			if(!DistanceList.empty())
			{
				if(DistanceList.front() < 60)
				{
					throw ApplicationException("推桿位置小於60");
				}
				plc->WritePoint_PLC("EM_3", 30001+i, DistanceList.front(), true);
				DistanceList.pop_front();
			}
			else
			{
				plc->WritePoint_PLC("EM_3", 30001+i, 0, true);
			}
		}
		return plc->OverWrite("EM_3", 11900, 0, 30000, 31000, pnl + 2, true) and plc->OverWrite("EM_3", 11999, 0, 30099, 31099, 1, false);
	}
    catch (Exception &e)
    {
        logger.error(e.displayText());
    }
    return false;
}

bool Prod::confirmProd()
{
	return plc->WritePoint_PLC("H", 0, 256, false, 0);
}

void Prod::handleAlarm(AlarmNotification* pNf)
{
	//異常處理機制
	lb->insertERROR(pNf->msg, pNf->device, pNf->status);
	pNf->release();
}

void Prod::toTSDB(const std::string& payload)
{
	URI target;
	target.setScheme("http");
	target.setHost(config->getString("TSDB.HOST", "10.11.0.156"));
	target.setPort(8086);
	target.setPath("write");
	target.addQueryParameter("db", config->getString("TSDB.DB_NAME", "VCP_30")); //選擇db
	HTTPClientSession session(target.getHost(), target.getPort());
	HTTPRequest request(HTTPRequest::HTTP_POST, target.toString(), Net::HTTPMessage::HTTP_1_1);
	HTTPResponse res;
	request.setContentType("application/x-www-form-urlencoded");
	request.setContentLength(payload.length());
	try
	{
		std::ostream& BodyOstream = session.sendRequest(request); // sends request, returns open stream
		BodyOstream << payload;  // sends the body
	    istream &is = session.receiveResponse(res);
	    StreamCopier::copyStream(is, cout);
	}
	catch (Exception& e)
	{
		logger.error(e.displayText());
	}
}

void Prod::insertHistory()
{
	Path ProdFile(config->getString("application.dir"), Path::PATH_UNIX);
	ProdFile.setFileName("Prod.json");
	File generateFile(ProdFile);
	recipe rcp;
	std::ostringstream ostr;
//	if(!generateFile.exists())
//	{
//		throw ApplicationException("無 Prod.json");
//	}
	FileInputStream fis(ProdFile.toString());
	StreamCopier::copyStream(fis, ostr);
	Parser parser;
	JSON::Object::Ptr ReciveObject = parser.parse(ostr.str()).extract<JSON::Object::Ptr>();
	LocalDateTime now;
	ReciveObject->set("STARTDATETIME", DateTimeFormatter::format(now, "%Y-%m-%d %H:%M:%S"));
	JSON::Object::Ptr lotdataObject = ReciveObject->getObject("lotdata");
	rcp.LOTNO = lotdataObject->get("no").convert<std::string>();
	rcp.PARTNO = lotdataObject->get("itemno").convert<std::string>();
	rcp.SOURCE = lotdataObject->get("source").convert<std::string>();
	ProdFile.pushDirectory("history");

	JSON::Object::Ptr result = ab->Bridge(HTTPRequest::HTTP_POST, "/_db/VCP-30/_api/document/History", *ReciveObject);
	if(result->has("error"))
	{
		logger.error(result->get("errorMessage").convert<std::string>());
		rcp.RANDOMSTRING = random_string(5);
	}
	else
	{
		rcp.RANDOMSTRING = result->get("_key").convert<std::string>();
	}
	ProdFile.setFileName(rcp.RANDOMSTRING+".json");
	File targetFile(ProdFile);
	targetFile.createFile();
	generateFile.copyTo(ProdFile.toString());
	lb->insertMO(rcp);
}

std::string Prod::random_string( size_t length )
{
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}
