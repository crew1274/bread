/*
 * PLC.cpp
 *
 *  Created on: 2018年9月6日
 *      Author: 171104
 */

#include "PLC.h"

PLC::PLC(std::string EngName, AutoPtr<AbstractConfiguration> _config, NotificationCenter* _nc):
EngName(EngName), logger(Logger::get(EngName)), config(_config), nc(_nc), np({EngName, ("/tmp/"+EngName+".pipe").c_str(), Utility::FIFO(("/tmp/"+EngName+".pipe").c_str())})
{
	// TODO Auto-generated constructor stub
}

PLC::~PLC() {
	// TODO Auto-generated destructor stub
}

void PLC::Load()
{
	MultipleAreaRead MultipleAreaRead_temp;
	MultipleAreaRead_temp.area = MemoryTypeIndex("EM");
	MultipleAreaRead_temp.hex_as_dec = false;
	MultipleAreaRead_temp.bit_position = 0;
	MultipleAreaRead_temp.return_value = 0;
	MultipleAreaRead_temp.address = 31001;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(0.1);
	PLC_DATA.OPC_TAGNAME.push_back("E31001");
	//電鍍槽溫度

	MultipleAreaRead_temp.address = 31011;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(0.1);
	PLC_DATA.OPC_TAGNAME.push_back("E31011");
	//溶解槽溫度

	MultipleAreaRead_temp.hex_as_dec = true;
	MultipleAreaRead_temp.area = MemoryTypeIndex("DM");
	MultipleAreaRead_temp.address = 3108;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D3108");
	//電鍍時間設定

	MultipleAreaRead_temp.area = MemoryTypeIndex("DM");
	MultipleAreaRead_temp.address = 3118;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D3118");
	//電鍍時間實際

	MultipleAreaRead_temp.area = MemoryTypeIndex("DM");
	MultipleAreaRead_temp.address = 6108;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D6108");
	//推桿位置設定

	MultipleAreaRead_temp.area = MemoryTypeIndex("DM");
	MultipleAreaRead_temp.address = 6100;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D6100");
	//推桿位置實際

	MultipleAreaRead_temp.area = MemoryTypeIndex("DM");
	MultipleAreaRead_temp.address = 6208;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D6208");
	//遮板位置設定

	MultipleAreaRead_temp.area = MemoryTypeIndex("DM");
	MultipleAreaRead_temp.address = 6200;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D6200");
	//遮板位置實際

	MultipleAreaRead_temp.address = 601;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D601");
	//電鍍電流

	MultipleAreaRead_temp.hex_as_dec = false;
	MultipleAreaRead_temp.address = 1301;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D1301");
	//整流器01+

	MultipleAreaRead_temp.address = 1302;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D1302");
	//整流器01-

	MultipleAreaRead_temp.address = 1303;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D1303");
	//整流器02+

	MultipleAreaRead_temp.address = 1304;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D1304");
	//整流器02-

	MultipleAreaRead_temp.area = MemoryTypeIndex("H");
	MultipleAreaRead_temp.address = 0;
	MultipleAreaRead_temp.bit_position = 0;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("AutoModeSwitch");
	//自動開關

	MultipleAreaRead_temp.area = MemoryTypeIndex("H");
	MultipleAreaRead_temp.address = 0;
	MultipleAreaRead_temp.bit_position = 2;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("ManualModeSwitch");
	//手動開關

	MultipleAreaRead_temp.hex_as_dec = true;
	MultipleAreaRead_temp.area = MemoryTypeIndex("DM");
	MultipleAreaRead_temp.address = 4121;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D4121");
	//設定1

	MultipleAreaRead_temp.address = 4122;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D4122");
	//設定2

	MultipleAreaRead_temp.address = 4123;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D4123");
	//設定3

	MultipleAreaRead_temp.address = 4124;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D4124");
	//設定4

	MultipleAreaRead_temp.address = 4125;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D4125");
	//設定5

	MultipleAreaRead_temp.address = 4126;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D4126");
	//設定6

	MultipleAreaRead_temp.address = 4127;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D4127");
	//設定7

	MultipleAreaRead_temp.address = 4128;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D4128");
	//設定8

	MultipleAreaRead_temp.address = 4131;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D4131");
	//實際1

	MultipleAreaRead_temp.address = 4132;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D4132");
	//實際2

	MultipleAreaRead_temp.address = 4133;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D4133");
	//實際3

	MultipleAreaRead_temp.address = 4134;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D4134");
	//實際4

	MultipleAreaRead_temp.address = 4135;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D4135");
	//實際5

	MultipleAreaRead_temp.address = 4136;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D4136");
	//實際6

	MultipleAreaRead_temp.address = 4137;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D4137");
	//實際7

	MultipleAreaRead_temp.address = 4138;
	PLC_DATA.MultipleAreaReads.push_back(MultipleAreaRead_temp);
	PLC_DATA.DATA_RATIO.push_back(1);
	PLC_DATA.OPC_TAGNAME.push_back("D4138");
	//實際8
}

void PLC::LoadAlarm(std::string fileName)
{
	MultipleAreaRead TP;
	TP.return_value = 0;
	TP.hex_as_dec = false;
	logger.information("Load %s alarm address", fileName);
	Path filePath(Path::current(), fileName);
	std::ostringstream ostr;
	try
	{
		File inputFile(filePath);
		if(inputFile.exists())
		{
			FileInputStream fis(filePath.toString());
			StreamCopier::copyStream(fis, ostr);
			logger.information("Load file %s to stream successfully",filePath.toString());
			Parser str_parser;
			Array::Ptr main = str_parser.parse(ostr.str()).extract<Array::Ptr>(); //Array Ptr
			Object::Ptr object_temp;
			std::string address;
			std::string type;
			RegularExpression letterRegularExpression("[A-Za-z]+");
			RegularExpression numberRegularExpression("[0-9]+");
			for(uint i=0; i<main->size(); i++)
			{
				object_temp = main->getObject(i);
				if(object_temp->getValue<std::string>("Message") != "")
				{
					address = object_temp->getValue<std::string>("Bit Address");
//					cout << "Bit Address: " << address << endl;
					StringTokenizer temp(address, "[].", StringTokenizer::TOK_TRIM | StringTokenizer::TOK_IGNORE_EMPTY);
//					cout << "StringTokenizer1: " << temp[1] << endl;
//					cout << "StringTokenizer2: " << NumberParser::parse(temp[2]) << endl;
					letterRegularExpression.extract(temp[1], type);
					if(type == "")
					{
						logger.information("CIO");
						type = "CIO";
					}
//					cout << "type: " << type << endl;
					numberRegularExpression.extract(temp[1], address);
//					cout << "address: " << NumberParser::parse(address) << endl;
					TP.area = MemoryTypeIndex(type);
					TP.address = NumberParser::parse(address);
					TP.bit_position = NumberParser::parse(temp[2]); //bit_position
					ALARM_DATA.address.push_back(TP);
					ALARM_DATA.pre.push_back(0);
					ALARM_DATA.msg.push_back(object_temp->getValue<std::string>("Message"));
//					cout << "Message: " << ALARM_DATA.msg.back() << endl;
				}
			}
			if(ALARM_DATA.msg.size() == ALARM_DATA.address.size())
			{
				logger.information("警報點位總共載入%u", ALARM_DATA.msg.size());
			}
			else
			{
				throw ApplicationException("警報點位數量錯誤");
			}
		}
		else
		{
			throw ApplicationException(filePath.toString()+" not exist!");
		}
	}
	catch(Exception& e)
	{
		logger.error(e.displayText());
	}

}

void PLC::Base(Timer& timer)
{
	if(!PLCmutex.tryLock())
	{
		logger.warning("PLC mutex locked");
		return;
	}
	else
	{
//		_stopwatch.start();
		if(ReadData_MultipleArea_V2(PLC_DATA.MultipleAreaReads) == -1)
		{
			logger.warning("PLC_DATA.MultipleAreaReads return -1");
		}
		PLCmutex.unlock();
//		_stopwatch.stop();
//		logger.trace("elapsed: %?d", _stopwatch.elapsed());
//		_stopwatch.reset();
		if(pre <=10 && PLC_DATA.MultipleAreaReads[8].return_value > 10)
		{
			std::stringstream output;
			logger.warning("電鍍電流不為0, D601:%z", PLC_DATA.MultipleAreaReads[8].return_value);
			logger.warning("電鍍電流不為0, D1301:%z", PLC_DATA.MultipleAreaReads[9].return_value);
			logger.warning("電鍍電流不為0, D1302:%z", PLC_DATA.MultipleAreaReads[10].return_value);
			logger.warning("電鍍電流不為0, D1303:%z", PLC_DATA.MultipleAreaReads[11].return_value);
			logger.warning("電鍍電流不為0, D1304:%z", PLC_DATA.MultipleAreaReads[12].return_value);
			output << PLC_DATA.MultipleAreaReads[8].return_value << ",";
			output << PLC_DATA.MultipleAreaReads[9].return_value << ",";
			output << PLC_DATA.MultipleAreaReads[10].return_value << ",";
			output << PLC_DATA.MultipleAreaReads[11].return_value << ",";
			output << PLC_DATA.MultipleAreaReads[12].return_value << ",";
			Utility::WriteLog("D601.txt", output.str());
		}
		pre = PLC_DATA.MultipleAreaReads[8].return_value;

		if(ReadData_MultipleArea_V2(ALARM_DATA.address) == -1)
		{
			logger.warning("ALARM_DATA.address return -1");
		}
		for(uint i=0; i<ALARM_DATA.address.size(); i++)
		{
			if(ALARM_DATA.address[i].return_value != ALARM_DATA.pre[i])
			{
				logger.error("警報:%s", ALARM_DATA.msg[i]);
				//建立警報
				nc->postNotification(new AlarmNotification(ALARM_DATA.msg[i],
						ALARM_DATA.address[i].return_value == 1? true: false));
				ALARM_DATA.pre[i] = ALARM_DATA.address[i].return_value;
			}
		}
	}
}

bool PLC::WritePoint_PLC(std::string CS_mode, int address, int write_value, bool dec_as_hex, int bit_position)
{
	MemoryAreaTable memory_area = MemoryTypeIndex(CS_mode);
	char write_data[2];
	if(dec_as_hex)
	{
		int sum = 0;
		int counter = 0;
		while(write_value > 0)
		{
			sum = sum + pow(16, counter) * (write_value % 10);
			counter++;
			write_value = write_value / 10;
		}
		write_data[0] = static_cast<char>(sum/256);
		write_data[1] = static_cast<char>(sum%256);
	}
	else
	{
		if(write_value >= 0 )
		{
			write_data[0] = static_cast<char>(write_value/256);
			write_data[1] = static_cast<char>(write_value%256);
		}
		else
		{
			write_data[0] = static_cast<char>(write_value/256 - 1 ) ;
			write_data[1] = static_cast<char>(write_value%256);
		}
	}
	PLCmutex.lock();
	if(WritePoint(memory_area, address, bit_position, write_data, GetDataSize_MemoryArea(memory_area)))
	{
		//DEBUG_PRINT("Write value:%d to PLC point[%d] failure. \n", write_value, address);
		logger.error("Write value:%d to PLC point[%d] failure", write_value, address);
		PLCmutex.unlock();
		return false;
	}
	else
	{
		//DEBUG_PRINT("Write value:%d to PLC point[%d] successful. \n", write_value, address);
		PLCmutex.unlock();
		return true;
	}
}

int PLC::ReadPoint_PLC(std::string CS_mode, uint address, uint bit_partition, bool dec_as_hex)
{
	MultipleAreaRead area_data_list[1];
	area_data_list[0] = { MemoryTypeIndex(CS_mode), address, static_cast<char>(bit_partition), 0, dec_as_hex};

	PLCmutex.lock();
	if(ReadData_MultipleArea(area_data_list, sizeof(area_data_list)/sizeof(MultipleAreaRead)) == -1)
	{
		logger.error("Read PLC multiple area data from PLC failure");
	}
	PLCmutex.unlock();
	return area_data_list[0].return_value;
}
/*
bool PLC::OverWrite(int TargetAddress, int TempAddress, int StatusAddress, int Value)
{
	int stage = 0;
	PLCmutex.lock();
	_stopwatch.start();
	while(stage<3)
	{
		switch(stage)
		{
			case 0:
				//cout<< 0 << endl;
				if(ReadPoint_PLC("EM", StatusAddress, 0) == 0x0001)
				{
					stage++;
				}
				break;
			case 1:
				//cout<< 1 << endl;
				WritePoint_PLC("EM", TempAddress, Value, false);
				WritePoint_PLC("EM", StatusAddress, 0x0011, false);
				stage++;
				break;
//			case 2:
//				//cout<< 2 << endl;
//				if(ReadPoint_PLC("DM", TargetAddress, 0) == Value && ReadPoint_PLC("EM", StatusAddress, 0) == 0x0111)
//				{
//					stage++;
//				}
//				break;
			case 2:
				//cout<< 3 << endl;
				WritePoint_PLC("EM", StatusAddress, 0x0101, false);
				stage++;
				break;
		}
		if(_stopwatch.elapsedSeconds() > 10)
		{
			logger.error("PLC寫入點位[%d]時等待過久,取消生產", TargetAddress);
			_stopwatch.stop();
			_stopwatch.reset();
			return false;
		}
		Poco::Thread::sleep(100);
	}
	PLCmutex.unlock();
	logger.information("PLC寫入點位[%d]值[%d]成功", TargetAddress, Value);
	_stopwatch.stop();
	_stopwatch.reset();
	return true;
}
*/
bool PLC::OverWrite(std::string CS_mode, int TargetAddress, int bit_partition, int TempAddress, int StatusAddress, int Value, bool dec_as_hex)
{
	int stage = 0;
	PLCmutex.lock();
	_stopwatch.start();
	while(stage<4)
	{
		switch(stage)
		{
			case 0:
//				cout<< 0 << endl;
				if(ReadPoint_PLC(CS_mode, StatusAddress, 0, false) == 0x0001) //確認PLC
				{
					stage++;
				}
				break;
			case 1:
//				cout<< 1 << endl;
				WritePoint_PLC(CS_mode, TempAddress, Value, dec_as_hex);
				WritePoint_PLC(CS_mode, StatusAddress, 0x0011, false);
				stage++;
				break;
			case 2:
//				cout<< 2 << endl;
//				if(ReadPoint_PLC(CS_mode, TargetAddress, bit_partition, false) == Value && ReadPoint_PLC("EM", StatusAddress, 0, false) == 0x0111)
				if(ReadPoint_PLC(CS_mode, StatusAddress, 0, false) == 0x0111)
				{
					stage++;
				}
				break;
			case 3:
//				cout<< 3 << endl;
				WritePoint_PLC(CS_mode, StatusAddress, 0x0101, false);
				stage++;
				break;
		}
		if(_stopwatch.elapsedSeconds() > 10)
		{
			PLCmutex.unlock();
			_stopwatch.stop();
			_stopwatch.reset();
			logger.error("PLC寫入點位[%d]時等待過久", TargetAddress);
			if(stage == 0)
			{
				logger.error("狀態點位%d 尚未為 0x0001", StatusAddress);
			}
			else if(stage == 1)
			{
				logger.error("寫入暫存點位%d 及更改狀態點位%d 為0x0011時發生錯誤", TempAddress, StatusAddress);
			}
			else if(stage == 2)
			{
				logger.error("狀態點位%d 尚未為 0x0111", StatusAddress);
			}
			else if(stage == 3)
			{
				logger.error("更改狀態點位%d 為 0x0101時發生錯誤", StatusAddress);
			}
			return false;
		}
		Poco::Thread::sleep(100);
	}
	PLCmutex.unlock();
	logger.information("PLC寫入點位[%d]值[%d]成功", TargetAddress, Value);
	_stopwatch.stop();
	logger.trace("Elapsed %d seconds", _stopwatch.elapsedSeconds());
	_stopwatch.reset();
	return true;
}
