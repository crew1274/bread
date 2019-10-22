/*
 * ppr_device.cpp
 *
 *  Created on: 2019年6月26日
 *      Author: 170302
 */

#include "../hal/ppr_device.h"

using namespace std;

PPRDEVICE::PPRDEVICE(const char *uart_device)
				:modbus(uart_device,_mode_modbus_rtu){
//	modbus::modbus_set_slave_id(slave_id);
}

/**
 * @brief       Destructor of DeltaPLC
 *
 */
PPRDEVICE::~PPRDEVICE()
{

}

/**
 * @brief       Disconnect to PLC
 *
 * @return      None
 */
void PPRDEVICE::Shutdown()
{
	modbus::modbus_close();
}


int PPRDEVICE::ppr_actual_status(int slave_id){
// 整合所有狀態

	uint16_t buffer[1];
	memset(buffer,0,sizeof(buffer));
	if(modbus::modbus_read_input_registers(slave_id,2, 1, buffer)==false){
		cout << "ppr_actual_status (modbus error)" << endl;
		return 65535;
	}
	if((buffer[0] & 0x01) == 0x01){
		cout << "Operating status = ON" << endl;
	}
	if(((buffer[0] >> 1) & 0x01) == 0x01){
		cout << " Range error. " << endl;
	}
	if(((buffer[0] >> 2) & 0x01) == 0x01){
		cout << "Watch dog / Reset." << endl;
	}
	if(((buffer[0] >> 3) & 0x01) == 0x01){
		cout << "Operation mode = Automatic." << endl;
	}
	if(((buffer[0] >> 4) & 0x01) == 0x01){
		cout << "As Counter overflow." << endl;
	}
	if(((buffer[0] >> 5) & 0x01) == 0x01){
		cout << "Actual Warning." << endl;
	}
	if(((buffer[0] >> 6) & 0x01) == 0x01){
		cout << "Actual Error." << endl;
	}
	if(((buffer[0] >> 7) & 0x01) == 0x01){
		cout << "Warning from the past." << endl;
	}
	if(((buffer[0] >> 8) & 0x01) == 0x01){
		cout << "Error from the past." << endl;
	}

	return buffer[0];
}

int PPRDEVICE::ppr_actual_messages1(int slave_id){
// 整合所有狀態

	uint16_t buffer[1];
	memset(buffer,0,sizeof(buffer));
	if(modbus::modbus_read_input_registers(slave_id, 5, 1, buffer)==false){
		cout << "ppr_actual_messages1 (modbus error)" << endl;
		return 65535;
	}

	if((buffer[0] & 0x01) == 0x01){
		cout << "Warning over current due to effective." << endl;
	}
	if(((buffer[0] >> 1) & 0x01) == 0x01){
		cout << "Warning over voltage due to effective." << endl;
	}
	if(((buffer[0] >> 2) & 0x01) == 0x01){
		cout << "Warning over power due to average." << endl;
	}
	if(((buffer[0] >> 3) & 0x01) == 0x01){
		cout << "Warning temperature." << endl;
	}
	if(((buffer[0] >> 4) & 0x01) == 0x01){
		cout << "Error:Over current switch off due to effective or peak(with switch off)." << endl;
	}
	if(((buffer[0] >> 5) & 0x01) == 0x01){
		cout << "Error:Over voltage switch off due to effective value." << endl;
	}
	if(((buffer[0] >> 6) & 0x01) == 0x01){
		cout << "Error:Over power switch off due to average value." << endl;
	}
	if(((buffer[0] >> 7) & 0x01) == 0x01){
		cout << "Error:Temperature switch off due to high temperature." << endl;
	}
	if(((buffer[0] >> 8) & 0x01) == 0x01){
		cout << "Intermediate circuit voltage too low." << endl;
	}
	if(((buffer[0] >> 9) & 0x01) == 0x01){
		cout << "Intermediate circuit voltage too high." << endl;
	}
	if(((buffer[0] >> 10) & 0x01) == 0x01){
		cout << "Error voltage supply DSP board." << endl;
	}
	if(((buffer[0] >> 11) & 0x01) == 0x01){
		cout << "At least 1 MPP defective." << endl;
	}
	if(((buffer[0] >> 12) & 0x01) == 0x01){
		cout << "Error period start." << endl;
	}
	if(((buffer[0] >> 13) & 0x01) == 0x01){
		cout << "Temperature measurement unit is defective." << endl;
	}
	if(((buffer[0] >> 14) & 0x01) == 0x01){
		cout << "Magnetic valve off." << endl;
	}
	if(((buffer[0] >> 15) & 0x01) == 0x01){
		cout << "Parallel PRPS off." << endl;
	}
	return buffer[0];
}

int PPRDEVICE::ppr_actual_messages2(int slave_id){
// 整合所有狀態

	uint16_t buffer[1];
	memset(buffer,0,sizeof(buffer));
	if(modbus::modbus_read_input_registers(slave_id, 6, 1, buffer)==false){
		cout << "ppr_actual_messages2 (modbus error)" << endl;
		return 65535;
	}
	if((buffer[0] & 0x01) == 0x01){
		cout << "Interrupt-calculating time too high." << endl;
	}
	if(((buffer[0] >> 1) & 0x01) == 0x01){
		cout << "Error fault mains supply." << endl;
	}
	if(((buffer[0] >> 2) & 0x01) == 0x01){
		cout << "SPI-Error DSP side." << endl;
	}
	if(((buffer[0] >> 3) & 0x01) == 0x01){
		cout << "Error DSP parameter." << endl;
	}
	if(((buffer[0] >> 4) & 0x01) == 0x01){
		cout << "Regulator limit / pulse duty factor DSP." << endl;
	}
	if(((buffer[0] >> 5) & 0x01) == 0x01){
		cout << "DSP reset event." << endl;
	}
	if(((buffer[0] >> 6) & 0x01) == 0x01){
		cout << "Error reading dataset." << endl;
	}
	if(((buffer[0] >> 7) & 0x01) == 0x01){
		cout << "Error parameter reading." << endl;
	}
	if(((buffer[0] >> 8) & 0x01) == 0x01){
		cout << "Range error." << endl;
	}
	if(((buffer[0] >> 9) & 0x01) == 0x01){
		cout << "Error SPI communicator." << endl;
	}
	if(((buffer[0] >> 10) & 0x01) == 0x01){
		cout << "Error EEPROM set points lost." << endl;
	}
	if(((buffer[0] >> 11) & 0x01) == 0x01){
		cout << "Error EEPROM configuration lost." << endl;
	}
	if(((buffer[0] >> 12) & 0x01) == 0x01){
		cout << "Error EEPROM device parameter lost." << endl;
	}
	if(((buffer[0] >> 13) & 0x01) == 0x01){
		cout << "Error EEPROM DSP parameter lost." << endl;
	}
	if(((buffer[0] >> 14) & 0x01) == 0x01){
		cout << "Error EEPROM actual values lost." << endl;
	}
	if(((buffer[0] >> 15) & 0x01) == 0x01){
		cout << "Error EEPROM actual Ah counter value lost." << endl;
	}
	return buffer[0];
}

int PPRDEVICE::ppr_actual_messages3(int slave_id){
// 整合所有狀態

	uint16_t buffer[1];
	memset(buffer,0,sizeof(buffer));
	if(modbus::modbus_read_input_registers(slave_id, 7, 1, buffer)==false){
		cout << "ppr_actual_messages3 (modbus error)" << endl;
		return 65535;
	}
	if((buffer[0] & 0x01) == 0x01){
		cout << "Error DSP parameter transmission." << endl;
	}
	if(((buffer[0] >> 1) & 0x01) == 0x01){
		cout << "Warning power supply PK temperature." << endl;
	}
	if(((buffer[0] >> 2) & 0x01) == 0x01){
		cout << "Warning PK temperature." << endl;
	}
	if(((buffer[0] >> 3) & 0x01) == 0x01){
		cout << "Error PK temperature." << endl;
	}
	if(((buffer[0] >> 4) & 0x01) == 0x01){
		cout << "Watchdog / Reset." << endl;
	}
	if(((buffer[0] >> 5) & 0x01) == 0x01){
		cout << "Error EEPROM type settings." << endl;
	}
	if(((buffer[0] >> 6) & 0x01) == 0x01){
		cout << "Error EEPROM dosage value lost." << endl;
	}
	if(((buffer[0] >> 7) & 0x01) == 0x01){
		cout << "Warning power fail." << endl;
	}
	if(((buffer[0] >> 8) & 0x01) == 0x01){
		cout << "Ready for operation." << endl;
	}
	if(((buffer[0] >> 9) & 0x01) == 0x01){
		cout << "Operation:PRPS in operation (ON)." << endl;
	}
	if(((buffer[0] >> 10) & 0x01) == 0x01){
		cout << "Dosage active." << endl;
	}
	if(((buffer[0] >> 11) & 0x01) == 0x01){
		cout << "Status Profibus (PB active)." << endl;
	}
	if(((buffer[0] >> 12) & 0x01) == 0x01){
		cout << "Error Profibus (PB inactive)." << endl;
	}
	if(((buffer[0] >> 13) & 0x01) == 0x01){
		cout << "Warning high voltage occurred." << endl;
	}
	if(((buffer[0] >> 14) & 0x01) == 0x01){
		cout << "Warning voltage limiter." << endl;
	}
	if(((buffer[0] >> 15) & 0x01) == 0x01){
		cout << "Warning TimeOut RS485 (CU, Display)." << endl;
	}
	return buffer[0];
}

int PPRDEVICE::ppr_actual_messages4(int slave_id){
// 整合所有狀態

	uint16_t buffer[1];
	memset(buffer,0,sizeof(buffer));
	if(modbus::modbus_read_input_registers(slave_id, 8, 1, buffer)==false){
		cout << "ppr_actual_messages4 (modbus error)" << endl;
		return 65535;
	}
	if((buffer[0] & 0x01) == 0x01){
		cout << "Ah counter 1." << endl;
	}
	if(((buffer[0] >> 1) & 0x01) == 0x01){
		cout << "Ah counter 2." << endl;
	}
	if(((buffer[0] >> 2) & 0x01) == 0x01){
		cout << "Ah counter 3." << endl;
	}
	if(((buffer[0] >> 3) & 0x01) == 0x01){
		cout << "Ah counter 4." << endl;
	}
	if(((buffer[0] >> 4) & 0x01) == 0x01){
		cout << "Error MMC logging failed." << endl;
	}
	if(((buffer[0] >> 5) & 0x01) == 0x01){
		cout << "TimeOut SPI-serial telegram.(error)" << endl;
	}
	if(((buffer[0] >> 6) & 0x01) == 0x01){
		cout << "Emergency Off (error)." << endl;
	}
	if(((buffer[0] >> 7) & 0x01) == 0x01){
		cout << "Switch on safety contactor." << endl;
	}
	if(((buffer[0] >> 8) & 0x01) == 0x01){
		cout << "Error EEPROM timer lost." << endl;
	}
	if(((buffer[0] >> 9) & 0x01) == 0x01){
		cout << "Warning charge too high." << endl;
	}
	if(((buffer[0] >> 10) & 0x01) == 0x01){
		cout << "PROFIBUS time out(error)." << endl;
	}
	if(((buffer[0] >> 11) & 0x01) == 0x01){
		cout << "Error charge too high(error)." << endl;
	}
	if(((buffer[0] >> 12) & 0x01) == 0x01){
		cout << "Overload(error)." << endl;
	}
	if(((buffer[0] >> 13) & 0x01) == 0x01){
		cout << "Current error." << endl;
	}
	if(((buffer[0] >> 14) & 0x01) == 0x01){
		cout << "Voltage error." << endl;
	}
	if(((buffer[0] >> 15) & 0x01) == 0x01){
		cout << "Voltage error user." << endl;
	}
	return buffer[0];
}

int PPRDEVICE::ppr_actual_messages5(int slave_id){
// 整合所有狀態

	uint16_t buffer[1];
	memset(buffer,0,sizeof(buffer));
	if(modbus::modbus_read_input_registers(slave_id, 9, 1, buffer)==false){
		cout << "ppr_actual_messages5 (modbus error)" << endl;
		return 65535;
	}
	if((buffer[0] & 0x01) == 0x01){
		cout << "Power error" << endl;
	}
	if(((buffer[0] >> 1) & 0x01) == 0x01){
		cout << "Amplifier error." << endl;
	}
	if(((buffer[0] >> 2) & 0x01) == 0x01){
		cout << "Fan error." << endl;
	}
	if(((buffer[0] >> 3) & 0x01) == 0x01){
		cout << "Error EEPROM batch data lost." << endl;
	}
	if(((buffer[0] >> 4) & 0x01) == 0x01){
		cout << "Error EEPROM correction factor lost." << endl;
	}
	if(((buffer[0] >> 5) & 0x01) == 0x01){
		cout << "U_Trigger error." << endl;
	}
	if(((buffer[0] >> 6) & 0x01) == 0x01){
		cout << "Switch off safety contactor." << endl;
	}
	if(((buffer[0] >> 7) & 0x01) == 0x01){
		cout << "External release missing." << endl;
	}
	if(((buffer[0] >> 8) & 0x01) == 0x01){
		cout << "External release Ok." << endl;
	}
	if(((buffer[0] >> 9) & 0x01) == 0x01){
		cout << "Batch file loaded." << endl;
	}
	if(((buffer[0] >> 10) & 0x01) == 0x01){
		cout << "General warning." << endl;
	}
	if(((buffer[0] >> 11) & 0x01) == 0x01){
		cout << "General error." << endl;
	}
	if(((buffer[0] >> 12) & 0x01) == 0x01){
		cout << "DSP type unknown." << endl;
	}

	return buffer[0];
}

int PPRDEVICE::operation_start(int slave_id, bool mode){
 // mode = 1 開啟  / mode = 0 關閉
	mutex.lock();
	if (mode) {
		if(modbus::modbus_write_register(slave_id, 0, 1)==false){
			cout << "operation_start -- start (modbus error)" << endl;
			mutex.unlock();
			return 65535;
		}
		cout << " operation <START> " << endl;
	} else {
		if(modbus::modbus_write_register(slave_id, 0, 0)==false){
			cout << "operation_start -- stop (modbus error)" << endl;
			mutex.unlock();
			return 65535;
		}
		cout << " operation <STOP> " << endl;
	}
	mutex.unlock();
	return 0;
}

int PPRDEVICE::waveform_set(int slave_id, int regulator, std::vector<PPR_Waveform_Table> PPR_Table)
{
// 檢查長度、寫連續點位
	mutex.lock();
	regulator = regulator * 100;
	if(modbus::modbus_write_register(slave_id, 1, unsigned(regulator)) == false)
	{
		cout << "waveform_set (modbus error)" << endl;
		mutex.unlock();
		return 65535;
	}
	cout << "regulator = "  << unsigned(regulator) << endl;
	for (uint ii = 0; ii < PPR_Table.size(); ii++)
	{
		if(modbus::modbus_write_register(slave_id, ii*7 + 2, (uint16_t)((int)(PPR_Table[ii].Ix1*10)))==false){
			cout << "waveform_set --Ix1 (modbus error)" << endl;
			mutex.unlock();
			return 65535;
		}
		if(modbus::modbus_write_register(slave_id, ii*7 + 3, (uint16_t)((int)(PPR_Table[ii].Ix2*10)))==false){
			cout << "waveform_set --Ix2 (modbus error)" << endl;
			mutex.unlock();
			return 65535;
		}
		if(modbus::modbus_write_register(slave_id, ii*7 + 4, (uint16_t)((int)(PPR_Table[ii].Ux1*10)))==false){
			cout << "waveform_set --Ux1 (modbus error)" << endl;
			mutex.unlock();
			return 65535;
		}
		if(modbus::modbus_write_register(slave_id, ii*7 + 5, (uint16_t)((int)(PPR_Table[ii].Ux2*10)))==false){
			cout << "waveform_set --Ux2 (modbus error)" << endl;
			mutex.unlock();
			return 65535;
		}
		if(modbus::modbus_write_register(slave_id, ii*7 + 6, (uint16_t)((int)(PPR_Table[ii].Tx1*50)))==false){
			cout << "waveform_set --Tx1 (modbus error)" << endl;
			mutex.unlock();
			return 65535;
		}
		if(modbus::modbus_write_register(slave_id, ii*7 + 7, (uint16_t)((int)(PPR_Table[ii].Tx2*50)))==false){
			cout << "waveform_set --Tx2 (modbus error)" << endl;
			mutex.unlock();
			return 65535;
		}
		if(modbus::modbus_write_register(slave_id, ii*7 + 8, unsigned(PPR_Table[ii].Cycle)*2)==false){
			cout << "waveform_set --Cycle (modbus error)" << endl;
			mutex.unlock();
			return 65535;
		}

		cout << "Ix1 = "  << unsigned(PPR_Table[ii].Ix1) << endl;
		cout << "Ix2 = "  << unsigned(PPR_Table[ii].Ix2) << endl;
		cout << "Ux1 = "  << unsigned(PPR_Table[ii].Ux1) << endl;
		cout << "Ux2 = "  << unsigned(PPR_Table[ii].Ux2) << endl;
		cout << "Tx1 = "  << unsigned(PPR_Table[ii].Tx1) << endl;
		cout << "Tx2 = "  << unsigned(PPR_Table[ii].Tx2) << endl;
		cout << "Cycle = "  << unsigned(PPR_Table[ii].Cycle) << endl;

	}

	for (uint ii = PPR_Table.size(); ii < PPR_Table.size()+1; ii++)
	{
		if(modbus::modbus_write_register(slave_id, ii*7 + 2, 0)==false){
			cout << "waveform_set (modbus error)" << endl;
			mutex.unlock();
			return 65535;
		}
		if(modbus::modbus_write_register(slave_id, ii*7 + 3, 0)==false){
			cout << "waveform_set (modbus error)" << endl;
			mutex.unlock();
			return 65535;
		}
		if(modbus::modbus_write_register(slave_id, ii*7 + 4, 0)==false){
			cout << "waveform_set (modbus error)" << endl;
			mutex.unlock();
			return 65535;
		}
		if(modbus::modbus_write_register(slave_id, ii*7 + 5, 0)==false){
			cout << "waveform_set (modbus error)" << endl;
			mutex.unlock();
			return 65535;
		}
		if(modbus::modbus_write_register(slave_id, ii*7 + 6, 0)==false){
			cout << "waveform_set (modbus error)" << endl;
			mutex.unlock();
			return 65535;
		}
		if(modbus::modbus_write_register(slave_id, ii*7 + 7, 0)==false){
			cout << "waveform_set (modbus error)" << endl;
			mutex.unlock();
			return 65535;
		}
		if(modbus::modbus_write_register(slave_id, ii*7 + 8, 0)==false){
			cout << "waveform_set (modbus error)" << endl;
			mutex.unlock();
			return 65535;
		}

//		cout << "Ix1 = "  << unsigned(PPR_Table[ii].Ix1) << endl;
//		cout << "Ix2 = "  << unsigned(PPR_Table[ii].Ix2) << endl;
//		cout << "Ux1 = "  << unsigned(PPR_Table[ii].Ux1) << endl;
//		cout << "Ux2 = "  << unsigned(PPR_Table[ii].Ux2) << endl;
//		cout << "Tx1 = "  << unsigned(PPR_Table[ii].Tx1) << endl;
//		cout << "Tx2 = "  << unsigned(PPR_Table[ii].Tx2) << endl;
//		cout << "Cycle = "  << unsigned(PPR_Table[ii].Cycle) << endl;

	}
	mutex.unlock();
	return 0;
}

int PPRDEVICE::waveform_check(int slave_id){
	uint16_t buffer[200];
	if(modbus::modbus_read_holding_registers(slave_id, 1,110,buffer)==false){
		cout << "waveform_check (modbus error)" << endl;
		return 65535;
	}
	for(int ii=0;ii<110;ii++)
		cout << "read ("<<ii+1 <<") "  << buffer[ii] << endl;
	return 0;
}
int PPRDEVICE::actual_counter_value(int slave_id){

	uint16_t buffer[2];
	memset(buffer,0,sizeof(buffer));
	if(modbus::modbus_read_input_registers(slave_id, 0, 2, buffer)==false){
		cout << "actual_counter_value (modbus error)" << endl;
		return 65535;
	}
//	cout << " actual_counter_value " << ((buffer[0]<<16)+buffer[1]) << endl;
	return int((buffer[0]<<16)+buffer[1]) ;
}

int PPRDEVICE::actual_average_current(int slave_id)
{
	if(!mutex.tryLock())
	{
		return 65535;
	}
	else
	{
		uint16_t buffer[1];
		memset(buffer,0,sizeof(buffer));
		if(modbus::modbus_read_input_registers(slave_id, 3, 1, buffer)==false)
		{
			cout << "actual_average_current (modbus error)" << endl;
			mutex.unlock();
			return 65535;
		}
		mutex.unlock();
	//	cout << "actual_average_current: " << buffer[0] << endl;
		return int(buffer[0]) ;
	}
}

int PPRDEVICE::actual_average_voltage(int slave_id)
{
	if(!mutex.tryLock())
	{
		return 65535;
	}
	uint16_t buffer[1];
	memset(buffer,0,sizeof(buffer));
	if(modbus::modbus_read_input_registers(slave_id, 4, 1, buffer)==false){
		cout << "actual_average_voltage (modbus error)" << endl;
		mutex.unlock();
		return 65535;
	}
	mutex.unlock();
//	cout << "actual_average_voltage: " << buffer[0] << endl;
	return int(buffer[0]) ;
}



template< typename T >
std::string PPRDEVICE::int_to_hex( T i )
{
  std::stringstream stream;
  stream << std::setfill ('0') << std::setw(sizeof(T)*1) // *2
         << std::hex << i;
  return stream.str();
}

