/*
 * modbustcp.cpp
 *
 *  Created on: 2019年5月20日
 *      Author: 170302
 */

#include "modbustcp.h"


/**
 * Main Constructor of Modbus Connector Object
 * @param host IP Address of Host
 * @param port Port for the TCP Connection
 * @return     A Modbus Connector Object
 */
modbus::modbus(string host, uint16_t port, connect_mode mode):Uart("")
{
    HOST = host;
    PORT = port;
    _slaveid = 1;
    _msg_id = 1;
    _connected = false;
    _mode = _mode_modbus_tcp;

}

modbus::modbus(const char *uart_device, connect_mode mode):Uart(uart_device)
{
	Uart::SetOptions(8,1,'N');
	_mode = mode;
}

/**
 * Overloading Modbus Connector Constructor with Default Port Set at 502
 * @param host  IP Address of Host
 * @return      A Modbus Connector Object
 */
modbus::modbus(string host) :Uart("")
{
    modbus(host, 502, _mode_modbus_tcp);
}


/**
 * Destructor of Modbus Connector Object
 */
modbus::~modbus(void)
{

}

/**
 * Build up a Modbus/TCP Connection
 * @return   If A Connection Is Successfully Built
 */
bool modbus::modbus_connect()
{
	if(_mode == _mode_modbus_tcp){
		if(HOST == "" || PORT == 0) {
			std::cout << "Missing Host and Port" << std::endl;
			return false;
		} else {
			std::cout << "Found Proper Host "<< HOST << " and Port " <<PORT <<std::endl;
		}

		_socket = socket(AF_INET, SOCK_STREAM, 0);
		if(_socket == -1) {
			std::cout <<"Error Opening Socket" <<std::endl;
			return false;
		} else {
			std::cout <<"Socket Opened Successfully" << std::endl;
		}

		_server.sin_family = AF_INET;
		_server.sin_addr.s_addr = inet_addr(HOST.c_str());
		_server.sin_port = htons(PORT);

		if (connect(_socket, (struct sockaddr*)&_server, sizeof(_server)) < 0) {
			std::cout<< "Connection Error" << std::endl;
			return false;
		}

		 // Set send/recv timeout
		struct timeval tv;

		tv.tv_sec  = 3;
		tv.tv_usec = 0;
		setsockopt(_socket, SOL_SOCKET, SO_SNDTIMEO , (const char*)&tv, sizeof(struct timeval));

		tv.tv_sec  = 3;
		tv.tv_usec = 0;
		setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(struct timeval));


		std::cout<< "Connected" <<std::endl;
		_connected = true;
		return true;
	}

	return true;
}

/**
 * Modbus Slave ID Setter
 * @param id  ID of the Modbus Server Slave
 */
void modbus::modbus_set_slave_id(int id)
{
    _slaveid = id;
}

/**
 * Close the Modbus/TCP Connection
 */
void modbus::modbus_close() {
	if(_mode == _mode_modbus_tcp){
    close(_socket);
    std::cout <<"Socket Closed" <<std::endl;
	}
}


/**
 * Modbus Request Builder
 * @param to_send   Message Buffer to Be Sent
 * @param address   Reference Address
 * @param func      Modbus Functional Code
 */
void modbus::modbus_build_request(uint8_t *to_send, int address, int func) {
	if(_mode == _mode_modbus_tcp){
		to_send[0] = (uint8_t) _msg_id >> 8;
		to_send[1] = (uint8_t) (_msg_id & 0x00FF);
		to_send[2] = 0;
		to_send[3] = 0;
		to_send[4] = 0;
		to_send[6] = (uint8_t) _slaveid;
		to_send[7] = (uint8_t) func;
		to_send[8] = (uint8_t) (address >> 8);
		to_send[9] = (uint8_t) (address & 0x00FF);
	}else if(_mode == _mode_modbus_rtu){
//		to_send[0] = (uint8_t) _msg_id >> 8;
//		to_send[1] = (uint8_t) (_msg_id & 0x00FF);
//		to_send[2] = 0;
//		to_send[3] = 0;
//		to_send[4] = 0;
		to_send[0] = (uint8_t) _slaveid;
		to_send[1] = (uint8_t) func;
		to_send[2] = (uint8_t) (address >> 8);
		to_send[3] = (uint8_t) (address & 0x00FF);
	}else{
		std::cout <<"Connect mode error! " <<std::endl;
	}
}


/**
 * Write Request Builder and Sender
 * @param address   Reference Address
 * @param amount    Amount of data to be Written
 * @param func      Modbus Functional Code
 * @param value     Data to Be Written
 */
void modbus::modbus_write(int address, int amount, int func, uint16_t *value) {
	if(_mode == _mode_modbus_tcp){
		if(func == WRITE_COIL || func == WRITE_REG) {
			uint8_t to_send[12];
			modbus_build_request(to_send, address, func);
			to_send[5] = 6;
			to_send[10] = (uint8_t) (value[0] >> 8);
			to_send[11] = (uint8_t) (value[0] & 0x00FF);
			modbus_send(to_send, 12);
		} else if(func == WRITE_REGS){
			uint8_t to_send[13 + 2 * amount];
			modbus_build_request(to_send, address, func);
			to_send[5] = (uint8_t) (5 + 2 * amount);
			to_send[10] = (uint8_t) (amount >> 8);
			to_send[11] = (uint8_t) (amount & 0x00FF);
			to_send[12] = (uint8_t) (2 * amount);
			for(int i = 0; i < amount; i++) {
				to_send[13 + 2 * i] = (uint8_t) (value[i] >> 8);
				to_send[14 + 2 * i] = (uint8_t) (value[i] & 0x00FF);
			}
			modbus_send(to_send, 13 + 2 * amount);
		} else if(func == WRITE_COILS) {
			uint8_t to_send[14 + (amount -1) / 8 ];
			modbus_build_request(to_send, address, func);
			to_send[5] = (uint8_t) (7 + (amount -1) / 8);
			to_send[10] = (uint8_t) (amount >> 8);
			to_send[11] = (uint8_t) (amount >> 8);
			to_send[12] = (uint8_t) ((amount + 7) / 8);
			for(int i = 0; i < amount; i++) {
				to_send[13 + (i - 1) / 8] += (uint8_t) (value[i] << (i % 8));
			}
			modbus_send(to_send, 14 + (amount - 1) / 8);
		}
	}else if(_mode == _mode_modbus_rtu){
		if(func == WRITE_COIL || func == WRITE_REG) {
			uint8_t to_send[6]; // 12
			modbus_build_request(to_send, address, func);
//			to_send[5] = 6;
			to_send[4] = (uint8_t) (value[0] >> 8);
			to_send[5] = (uint8_t) (value[0] & 0x00FF);
			modbus_send(to_send, 6);
		} else if(func == WRITE_REGS){
			uint8_t to_send[7 + 2 * amount];
			modbus_build_request(to_send, address, func);
//			to_send[5] = (uint8_t) (5 + 2 * amount);
			to_send[4] = (uint8_t) (amount >> 8);
			to_send[5] = (uint8_t) (amount & 0x00FF);
			to_send[6] = (uint8_t) (2 * amount);
			for(int i = 0; i < amount; i++) {
				to_send[7 + 2 * i] = (uint8_t) (value[i] >> 8);
				to_send[8 + 2 * i] = (uint8_t) (value[i] & 0x00FF);
			}
			modbus_send(to_send, 7 + 2 * amount);
		} else if(func == WRITE_COILS) {
			uint8_t to_send[8 + (amount -1) / 8 ];
			modbus_build_request(to_send, address, func);
//			to_send[5] = (uint8_t) (7 + (amount -1) / 8);
			to_send[4] = (uint8_t) (amount >> 8);
			to_send[5] = (uint8_t) (amount >> 8);
			to_send[6] = (uint8_t) ((amount + 7) / 8);
			for(int i = 0; i < amount; i++) {
				to_send[7 + (i - 1) / 8] += (uint8_t) (value[i] << (i % 8));
			}
			modbus_send(to_send, 8 + (amount - 1) / 8);
		}
	}else{
		cout << "mode error !" << endl ;
	}
}


/**
 * Read Request Builder and Sender
 * @param address   Reference Address
 * @param amount    Amount of Data to Read
 * @param func      Modbus Functional Code
 */
void modbus::modbus_read(int address, int amount, int func){
	if(_mode == _mode_modbus_tcp){
		uint8_t to_send[12];
		modbus_build_request(to_send, address, func);
		to_send[5] = 6;
		to_send[10] = (uint8_t) (amount >> 8);
		to_send[11] = (uint8_t) (amount & 0x00FF);
		modbus_send(to_send, 12);
	}else if(_mode == _mode_modbus_rtu){
		uint8_t to_send[6];
		modbus_build_request(to_send, address, func);
//		to_send[5] = 6;
		to_send[4] = (uint8_t) (amount >> 8);
		to_send[5] = (uint8_t) (amount & 0x00FF);
		modbus_send(to_send, 6);
	}else{
		cout << "mode error !" << endl ;
	}
}


/**
 * Read Holding Registers
 * MODBUS FUNCTION 0x03
 * @param address    Reference Address
 * @param amount     Amount of Registers to Read
 * @param buffer     Buffer to Store Data Read from Registers
 */
bool modbus::modbus_read_holding_registers(int address, int amount, uint16_t *buffer) {
	if(_mode == _mode_modbus_tcp){
		if(_connected) {
			if(amount > 65535 || address > 65535) {
				return false;
			}
			modbus_read(address, amount, READ_REGS);
			uint8_t to_rec[MAX_MSG_LENGTH];
			ssize_t k = modbus_receive(to_rec);
			try {
				modbus_error_handle(to_rec, READ_REGS);
				for(int i = 0; i < amount; i++) {
					buffer[i] = ((uint16_t)to_rec[9 + 2 * i]) << 8;
					buffer[i] += (uint16_t) to_rec[10 + 2 * i];
				}
			} catch (exception &e) {
				cout<<e.what()<<endl;
				return false;
			}
		} else {
			return false;
		}
	}else if(_mode == _mode_modbus_rtu){
//		if(_connected) {
			if(amount > 65535 || address > 65535) {
				return false;
			}
			modbus_read(address, amount, READ_REGS);
			uint8_t to_rec[MAX_MSG_LENGTH];
			ssize_t k = modbus_receive(to_rec);
			try {
				modbus_error_handle(to_rec, READ_REGS);
				for(int i = 0; i < amount; i++) {
					buffer[i] = ((uint16_t)to_rec[3 + 2 * i]) << 8;
					buffer[i] += (uint16_t) to_rec[4 + 2 * i];
				}
			} catch (exception &e) {
				cout<<e.what()<<endl;
				return false;
			}
//		} else {
//			throw modbus_connect_exception();
//		}
	}else{
		cout << "mode error !" << endl ;
	}
}


/**
 * Read Input Registers
 * MODBUS FUNCTION 0x04
 * @param address     Reference Address
 * @param amount      Amount of Registers to Read
 * @param buffer      Buffer to Store Data Read from Registers
 */
bool modbus::modbus_read_input_registers(int address, int amount, uint16_t *buffer) {
	if(_mode == _mode_modbus_tcp){
		if(_connected){
			if(amount > 65535 || address > 65535) {
				return false;
			}
			modbus_read(address, amount, READ_INPUT_REGS);
			uint8_t to_rec[MAX_MSG_LENGTH];
			ssize_t k = modbus_receive(to_rec);
			try {
				modbus_error_handle(to_rec, READ_INPUT_REGS);
				for(int i = 0; i < amount; i++) {
					buffer[i] = ((uint16_t)to_rec[9 + 2 * i]) << 8;
					buffer[i] += (uint16_t) to_rec[10 + 2 * i];
				}
			} catch (exception &e) {
				cout<<e.what()<<endl;
				return false;
			}
		} else {
			return false;
		}
	}else if(_mode == _mode_modbus_rtu){
//		if(_connected){
			if(amount > 65535 || address > 65535) {
				return false;
			}
			modbus_read(address, amount, READ_INPUT_REGS);
			uint8_t to_rec[MAX_MSG_LENGTH];
			ssize_t k = modbus_receive(to_rec);
			try {
				modbus_error_handle(to_rec, READ_INPUT_REGS);
				for(int i = 0; i < amount; i++) {
					buffer[i] = ((uint16_t)to_rec[3 + 2 * i]) << 8;
					buffer[i] += (uint16_t) to_rec[4 + 2 * i];
				}
			} catch (exception &e) {
				cout<<e.what()<<endl;
				return false;
			}
//		} else {
//			throw modbus_connect_exception();
//		}
	}else{
		cout << "mode error !" << endl ;
	}
}


/**
 * Read Coils
 * MODBUS FUNCTION 0x01
 * @param address     Reference Address
 * @param amount      Amount of Coils to Read
 * @param buffer      Buffer to Store Data Read from Coils
 */
bool modbus::modbus_read_coils(int address, int amount, bool *buffer) {
	if(_mode == _mode_modbus_tcp){
		if(_connected) {
			if(amount > 2040 || address > 65535) {
				return false;
			}
			modbus_read(address, amount, READ_COILS);
			uint8_t to_rec[MAX_MSG_LENGTH];
			ssize_t k = modbus_receive(to_rec);
			try {
				modbus_error_handle(to_rec, READ_COILS);
				for(int i = 0; i < amount; i++) {
					buffer[i] = (bool) ((to_rec[9 + i / 8] >> (i % 8)) & 1);
				}
			} catch (exception &e) {
				cout<<e.what()<<endl;
				cout << "modbus_read_coils return false" <<endl;
				return false;
			}
		} else {
			return false;
		}
	}else if(_mode == _mode_modbus_rtu){
//		if(_connected) {
			if(amount > 2040 || address > 65535) {
				return false;
			}
			modbus_read(address, amount, READ_COILS);
			uint8_t to_rec[MAX_MSG_LENGTH];
			ssize_t k = modbus_receive(to_rec);
			try {
				modbus_error_handle(to_rec, READ_COILS);
				for(int i = 0; i < amount; i++) {
					buffer[i] = (bool) ((to_rec[3 + i / 8] >> (i % 8)) & 1);
				}
			} catch (exception &e) {
				cout<<e.what()<<endl;
				return false;
			}
//		} else {
//			throw modbus_connect_exception();
//		}
	}else{
		cout << "modbus_read_coils mode error !" << endl ;
	}
}


/**
 * Read Input Bits(Discrete Data)
 * MODBUS FUNCITON 0x02
 * @param address   Reference Address
 * @param amount    Amount of Bits to Read
 * @param buffer    Buffer to store Data Read from Input Bits
 */
bool modbus::modbus_read_input_bits(int address, int amount, bool* buffer) {
	if(_mode == _mode_modbus_tcp){
		if(_connected) {
			if(amount > 2040 || address > 65535) {
				return false;
			}
			modbus_read(address, amount, READ_INPUT_BITS);
			uint8_t to_rec[MAX_MSG_LENGTH];
			ssize_t k = modbus_receive(to_rec);
			try {
				modbus_error_handle(to_rec, READ_INPUT_BITS);
				for(int i = 0; i < amount; i++) {
					buffer[i] = (bool) ((to_rec[9 + i / 8] >> (i % 8)) & 1);
				}
			} catch (exception &e) {
				cout<<e.what()<<endl;
				return false;
			}
		} else {
			return false;
		}
	}else if(_mode == _mode_modbus_rtu){
//		if(_connected) {
			if(amount > 2040 || address > 65535) {
				return false;
			}
			modbus_read(address, amount, READ_INPUT_BITS);
			uint8_t to_rec[MAX_MSG_LENGTH];
			ssize_t k = modbus_receive(to_rec);
			try {
				modbus_error_handle(to_rec, READ_INPUT_BITS);
				for(int i = 0; i < amount; i++) {
					buffer[i] = (bool) ((to_rec[3 + i / 8] >> (i % 8)) & 1);
				}
			} catch (exception &e) {
				cout<<e.what()<<endl;
				return false;
			}
//		} else {
//			throw modbus_connect_exception();
//		}
	}else{
		cout << "modbus_read_input_bits mode error !" << endl ;
	}
}


/**
 * Write Single Coils
 * MODBUS FUNCTION 0x05
 * @param address    Reference Address
 * @param to_write   Value to be Written to Coil
 */
bool modbus::modbus_write_coil(int address, bool to_write) {
	if(_mode == _mode_modbus_tcp){
		if(_connected) {
			if(address > 65535) {
				return false;
			}
			int value = to_write * 0xFF00;
			modbus_write(address, 1, WRITE_COIL, (uint16_t *)&value);
			uint8_t to_rec[MAX_MSG_LENGTH];
			ssize_t k = modbus_receive(to_rec);
			try{
				modbus_error_handle(to_rec, WRITE_COIL);
			} catch (exception &e) {
				cout<<e.what()<<endl;
				return false;
			}
		} else {
			return false;
		}
	}else if(_mode == _mode_modbus_rtu){
//		if(_connected) {
			if(address > 65535) {
				return false;
			}
			int value = to_write * 0xFF00;
			modbus_write(address, 1, WRITE_COIL, (uint16_t *)&value);
			uint8_t to_rec[MAX_MSG_LENGTH];
			ssize_t k = modbus_receive(to_rec);
			try{
				modbus_error_handle(to_rec, WRITE_COIL);
			} catch (exception &e) {
				cout<<e.what()<<endl;
				return false;
			}
//		} else {
//			throw modbus_connect_exception();
//		}
	}else{
		cout << "mode error !" << endl ;
	}
}


/**
 * Write Single Register
 * FUCTION 0x06
 * @param address   Reference Address
 * @param value     Value to Be Written to Register
 */
bool modbus::modbus_write_register(int address, uint16_t value) {
	if(_mode == _mode_modbus_tcp){
		if(_connected) {
			if(address > 65535) {
				return false;
			}
			modbus_write(address, 1, WRITE_REG, &value);
			uint8_t to_rec[MAX_MSG_LENGTH];
			ssize_t k = modbus_receive(to_rec);
			try{
				modbus_error_handle(to_rec, WRITE_COIL);
			} catch (exception &e) {
				cout << e.what() << endl;
				return false;
			}
		} else {
			return false;
		}
	}
	else if(_mode == _mode_modbus_rtu)
	{
		if(address > 65535)
		{
			return false;
		}
		modbus_write(address, 1, WRITE_REG, &value);
		uint8_t to_rec[MAX_MSG_LENGTH];
		ssize_t k = modbus_receive(to_rec);
		try
		{
			modbus_error_handle(to_rec, WRITE_COIL);
		}
		catch (exception &e)
		{
			cout << e.what() << endl;
			return false;
		}
	}
	else
	{
		cout << "mode error !" << endl ;
	}
	return true;
}


/**
 * Write Multiple Coils
 * MODBUS FUNCTION 0x0F
 * @param address  Reference Address
 * @param amount   Amount of Coils to Write
 * @param value    Values to Be Written to Coils
 */
bool modbus::modbus_write_coils(int address, int amount, bool *value) {
	if(_mode == _mode_modbus_tcp){
		if(_connected) {
			if(address > 65535 || amount > 65535) {
				return false;
			}
			uint16_t temp[amount];
			for(int i = 0; i < 4; i++) {
				temp[i] = (uint16_t)value[i];
			}
			modbus_write(address, amount, WRITE_COILS,  temp);
			uint8_t to_rec[MAX_MSG_LENGTH];
			ssize_t k = modbus_receive(to_rec);
			try{
				modbus_error_handle(to_rec, WRITE_COILS);
			} catch (exception &e) {
				cout << e.what() << endl;
				return false;
			}
		} else {
			return false;
		}
	}else if(_mode == _mode_modbus_rtu){
//		if(_connected) {
			if(address > 65535 || amount > 65535) {
				return false;
			}
			uint16_t temp[amount];
			for(int i = 0; i < 4; i++) {
				temp[i] = (uint16_t)value[i];
			}
			modbus_write(address, amount, WRITE_COILS,  temp);
			uint8_t to_rec[MAX_MSG_LENGTH];
			ssize_t k = modbus_receive(to_rec);
			try{
				modbus_error_handle(to_rec, WRITE_COILS);
			} catch (exception &e) {
				cout << e.what() << endl;
				return false;
			}
//		} else {
//			throw modbus_connect_exception();
//		}
	}else{
		cout << "mode error !" << endl ;
	}

}


/**
 * Write Multiple Registers
 * MODBUS FUNCION 0x10
 * @param address Reference Address
 * @param amount  Amount of Value to Write
 * @param value   Values to Be Written to the Registers
 */
bool modbus::modbus_write_registers(int address, int amount, uint16_t *value) {
	if(_mode == _mode_modbus_tcp){
		if(_connected) {
			if(address > 65535 || amount > 65535) {
				return false;
			}
			modbus_write(address, amount, WRITE_REGS, value);
			uint8_t to_rec[MAX_MSG_LENGTH];
			ssize_t k = modbus_receive(to_rec);
	//        for(int ii=0;ii<20;ii++)
	//        	printf("[%d , %x]",ii,to_rec[ii]);

			try{
				modbus_error_handle(to_rec, WRITE_REGS);
			} catch (exception &e) {
				cout << e.what() << endl;
				return false;
			}
		} else {
			return false;
		}
	}else if(_mode == _mode_modbus_rtu){
//		if(_connected) {
			if(address > 65535 || amount > 65535) {
				return false;
			}
			modbus_write(address, amount, WRITE_REGS, value);
			uint8_t to_rec[MAX_MSG_LENGTH];
			ssize_t k = modbus_receive(to_rec);
	//        for(int ii=0;ii<20;ii++)
	//        	printf("[%d , %x]",ii,to_rec[ii]);

			try{
				modbus_error_handle(to_rec, WRITE_REGS);
			} catch (exception &e) {
				cout << e.what() << endl;
				return false;
			}
//		} else {
//			throw modbus_connect_exception();
//		}
	}else{
		cout << "mode error !" << endl ;
	}
}


/**
 * Data Sender
 * @param to_send Request to Be Sent to Server
 * @param length  Length of the Request
 * @return        Size of the request
 */
ssize_t modbus::modbus_send(uint8_t *to_send, int length) {

	uint8_t Send_buffer[length +2];

	if(_mode == _mode_modbus_tcp){
		_msg_id++;
	//    for(int i=0;i<length;i++) printf("[%d] %x ",i,to_send[i]);
	//    cout << endl;
		return send(_socket, to_send, (size_t)length, 0);
	}else if(_mode == _mode_modbus_rtu){
		//TODO
		memcpy(Send_buffer, to_send, length);

		Send_buffer[length] = Utility::GetCRC16(to_send,
				length) >> 8;
		Send_buffer[length + 1] = Utility::GetCRC16(to_send,
				length) & 0x00FF;

		Uart::Send((const char*) Send_buffer, length+2);

	}
}


/**
 * Data Receiver
 * @param buffer Buffer to Store the Data Retrieved
 * @return       Size of Incoming Data
 */
ssize_t modbus::modbus_receive(uint8_t *buffer) {

	uint8_t Get_buffer[100];
	int len_o = 100;

	if(_mode == _mode_modbus_tcp){
		int kk;
		kk=-1;
		kk=recv(_socket, (char *) buffer, 1024, 0);
//		printf("kk test (modbus_receive)=%d \r\n ",kk);
		// if =-1 replace buffer = disconnect
		if(kk<0)
		{
			memset(Get_buffer,0,len_o);
			memcpy(buffer, Get_buffer, len_o-2);
		}
		return kk;
	}else if(_mode == _mode_modbus_rtu){
		//TODO
		memset(Get_buffer,0,len_o);
		int ret = Uart::Get(( char*) Get_buffer, len_o);
		memcpy(buffer, Get_buffer, len_o-2);
		return ret;
	}
}


/**
 * Error Code Handler
 * @param msg   Message Received from the Server
 * @param func  Modbus Functional Code
 */
void modbus::modbus_error_handle(uint8_t *msg, int func) {
	if(_mode == _mode_modbus_tcp){
		if(msg[7] == 0 & msg[8] == 0)
			throw modbus_disconnect_or_not_response_exception();
		if(msg[7] == func + 0x80) {
			switch(msg[8]){
				case EX_ILLEGAL_FUNCTION:
					throw modbus_illegal_function_exception();
				case EX_ILLEGAL_ADDRESS:
					throw modbus_illegal_address_exception();
				case EX_ILLEGAL_VALUE:
					throw modbus_illegal_data_value_exception();
				case EX_SERVER_FAILURE:
					throw modbus_server_failure_exception();
				case EX_ACKNOWLEDGE:
					throw modbus_acknowledge_exception();
				case EX_SERVER_BUSY:
					throw modbus_server_busy_exception();
				case EX_GATEWAY_PROBLEMP:
				case EX_GATEWYA_PROBLEMF:
					throw modbus_gateway_exception();
				default:
					break;
			}
		}
	}else if(_mode == _mode_modbus_rtu){
		if(msg[0] == 0)
			throw modbus_rtu_exception();

		if(msg[1] == func + 0x80) {
			switch(msg[8]){
				case EX_ILLEGAL_FUNCTION:
					throw modbus_illegal_function_exception();
				case EX_ILLEGAL_ADDRESS:
					throw modbus_illegal_address_exception();
				case EX_ILLEGAL_VALUE:
					throw modbus_illegal_data_value_exception();
				case EX_SERVER_FAILURE:
					throw modbus_server_failure_exception();
				case EX_ACKNOWLEDGE:
					throw modbus_acknowledge_exception();
				case EX_SERVER_BUSY:
					throw modbus_server_busy_exception();
				case EX_GATEWAY_PROBLEMP:
				case EX_GATEWYA_PROBLEMF:
					throw modbus_gateway_exception();
				default:
					break;
			}
		}
	}
}
