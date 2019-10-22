/*
 * ppr_device.h
 *
 *  Created on: 2019年6月26日
 *      Author: 170302
 */

#ifndef SRC_HAL_PPR_DEVICE_H_
#define SRC_HAL_PPR_DEVICE_H_

#include <vector>
#include <string>
#include <map>
#include <cstdio>
#include <cstring>
#include <cstdlib>     /* srand, rand */
#include <ctime>       /* time */
#include <iostream>
#include <string>
#include <sstream>
#include "Poco/Mutex.h"

#include "utility.h"
#include "hal/modbustcp_slaveid.h"

using namespace std;

struct PPR_Waveform_Table
{
	PPR_Waveform_Table() : Ix1(), Ix2(), Ux1(), Ux2(), Tx1(), Tx2(), Cycle() {}
	PPR_Waveform_Table(float Ix1, float Ix2, float Ux1, float Ux2, float Tx1, float Tx2, int Cycle): Ix1(Ix1), Ix2(Ix2), Ux1(Ux1), Ux2(Ux2), Tx1(Tx1), Tx2(Tx2), Cycle(Cycle)  {}
	float Ix1; // Current1 step
	float Ix2; // Current2 step
	float Ux1; // voltage1 step
	float Ux2; // voltage2 step
	float Tx1; // Time current1 step (ms)
	float Tx2; // Time current2 step (ms)
	int Cycle; // Cycle for step.
};

class PPRDEVICE : private modbus {
public:

	/**
	 * @brief       Constructor of PPR DEVICE
	 * @param[in]   uart_device         PPR RS485 / Uart Device
	 * @param[in]   slave_id            PPR Slave ID for modbus RTU
	 *
	 */
	//PPRDEVICE(const char *uart_device, int slave_id);
	PPRDEVICE(const char *uart_device);

	/**
	 * @brief       Destructor of PPR
	 *
	 */
    ~PPRDEVICE();


	/**
	* @brief       Read status from PPR Device.
	*
	* @param[in]   type            DataTypeTable_Delta
	* @param[in]   address         The start address in the point will be read
	* @param[in]   amount 	       Amount of bits to Read
	* @param[in]   buffer          Buffer to Store Data Read from PLC
	*
	*/
    int ppr_actual_status(int slave_id);
	int ppr_actual_messages1(int slave_id);
	int ppr_actual_messages2(int slave_id);
	int ppr_actual_messages3(int slave_id);
	int ppr_actual_messages4(int slave_id);
	int ppr_actual_messages5(int slave_id);

	int operation_start(int slave_id,bool mode);

	int waveform_set(int slave_id,int regulator,const std::vector<PPR_Waveform_Table> PPR_Table);

	int actual_counter_value(int slave_id); // return counter

	int actual_average_current(int slave_id); // return current

	int actual_average_voltage(int slave_id); // return voltage


	int waveform_check(int slave_id);

	/**
	 * @brief       Disconnect to PLC
	 *
	 * @return      None
	 */
	void Shutdown();
private:
	template<typename T>
	std::string int_to_hex(T i);
	Mutex mutex;
};


#endif /* SRC_HAL_PPR_DEVICE_H_ */
