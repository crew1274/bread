/*
 * modbustcp.h
 *
 *  Created on: 2019年5月20日
 *      Author: 170302
 */

#ifndef SRC_HAL_MODBUSTCP_H_
#define SRC_HAL_MODBUSTCP_H_

#include <cstdio>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "modbustcp_exception.h"
#include "dev/uart.h"
#include "utility.h"

using namespace std;

#define MAX_MSG_LENGTH 260

typedef enum{
	_mode_modbus_tcp = 0,
	_mode_modbus_rtu = 1
}connect_mode;

///Function Code
enum{
    READ_COILS       = 0x01,
    READ_INPUT_BITS  = 0x02,
    READ_REGS        = 0x03,
    READ_INPUT_REGS  = 0x04,
    WRITE_COIL       = 0x05,
    WRITE_REG        = 0x06,
    WRITE_COILS      = 0x0F,
    WRITE_REGS       = 0x10,
};

///Exception Codes
enum {
    EX_ILLEGAL_FUNCTION = 0x01, // Function Code not Supported
    EX_ILLEGAL_ADDRESS  = 0x02, // Output Address not exists
    EX_ILLEGAL_VALUE    = 0x03, // Output Value not in Range
    EX_SERVER_FAILURE   = 0x04, // Slave Deive Fails to process request
    EX_ACKNOWLEDGE      = 0x05, // Service Need Long Time to Execute
    EX_SERVER_BUSY      = 0x06, // Server Was Unable to Accept MB Request PDU
    EX_GATEWAY_PROBLEMP = 0x0A, // Gateway Path not Available
    EX_GATEWYA_PROBLEMF = 0x0B, // Target Device Failed to Response
};


/// Modbus Operator Class
/**
 * Modbus Operator Class
 * Providing networking support and mobus operation support.
 */
class modbus: public Uart
{
public:
    modbus(string host, uint16_t port, connect_mode mode);
    modbus(const char *uart_device, connect_mode mode);
    modbus(string host);
    virtual ~modbus();

    bool modbus_connect();
    void modbus_close();

    void modbus_set_slave_id(int id);

    bool modbus_read_coils(int slave_id,int address, int amount, bool* buffer);
    bool modbus_read_input_bits(int slave_id,int address, int amount, bool* buffer);
    bool modbus_read_holding_registers(int slave_id,int address, int amount, uint16_t *buffer);
    bool modbus_read_input_registers(int slave_id,int address, int amount, uint16_t *buffer);

    bool modbus_write_coil(int slave_id,int address, bool to_write);
    bool modbus_write_register(int slave_id,int address, uint16_t value);
    bool modbus_write_coils(int slave_id,int address, int amount, bool* value );
    bool modbus_write_registers(int slave_id,int address, int amount, uint16_t *value);

private:
    bool _connected;
    uint16_t PORT;
    int _socket;
    int _msg_id;
    int _slaveid ;
    string HOST;
    connect_mode _mode;
    struct sockaddr_in _server;
    Uart *uart_modbus_rtu_;

    void modbus_build_request(int slave_id,uint8_t *to_send,int address, int func);

    void modbus_read(int slave_id,int address, int amount, int func);
    void modbus_write(int slave_id,int address, int amount, int func, uint16_t *value);

    ssize_t modbus_send(int slave_id,uint8_t *to_send, int length);
    ssize_t modbus_receive(int slave_id,uint8_t *buffer);

    void modbus_error_handle(uint8_t *msg, int func);
};



#endif /* SRC_HAL_MODBUSTCP_H_ */
