/**
 * @file        def.h
 *
 * @author      Matic Chiu <matic.chiu@cht-pt.com.tw>
 *
 * @date        2018/05/17
 */

#ifndef __DEF_H__
#define __DEF_H__


#include "Poco/NotificationCenter.h"
#include "Poco/Notification.h"
#include "Poco/Observer.h"
#include "Poco/NObserver.h"
#include "Poco/AutoPtr.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>

using namespace std;
using namespace Poco;
// ------------------------------------------------------------------------------------
//                                      I2C
// ------------------------------------------------------------------------------------


// ------------------------------------------------------------------------------------
//                                      UART
// ------------------------------------------------------------------------------------
#define UART_PS0    "/dev/ttyPS0"        ///< UART device path: PS0
#define UART_PS1    "/dev/ttyPS1"        ///< UART device path: PS1
#define UART_PL1    "/dev/ttyUL1"        ///< UART device path: PL1
#define UART_PL2    "/dev/ttyUL2"        ///< UART device path: PL2
#define UART_PL3    "/dev/ttyUL3"        ///< UART device path: PL3
#define UART_PL4    "/dev/ttyUL4"        ///< UART device path: PL4
#define UART_PL5    "/dev/ttyUL5"        ///< UART device path: PL5
#define UART_PL6    "/dev/ttyUL6"        ///< UART device path: PL6


typedef float float_2046[2046];
typedef unsigned long u32;

/**
 * @brief    Baud rate table
 */
typedef enum{
    BAUDRATE_9600,          ///< Baud rate: 9600
    BAUDRATE_19200,         ///< Baud rate: 19200
    BAUDRATE_38400,         ///< Baud rate: 38400
    BAUDRATE_57600,         ///< Baud rate: 57600
    BAUDRATE_115200,        ///< Baud rate: 115200
    BAUDRATE_TOTAL          ///< Baud rate: Total
}BaudRateTable;


// ------------------------------------------------------------------------------------
//                                      SPI
// ------------------------------------------------------------------------------------
#define SPI_PS_0    "/dev/spidev32766.0"        ///< SPI device path: PS0
//#define SPI_PL_0    "/dev/spidev32764.0"        ///< SPI device path: PL0
#define SPI_PL_0_0    "/dev/spidev0.0"        ///< SPI device path: PL0.0
#define SPI_PL_0_1    "/dev/spidev0.1"        ///< SPI device path: PL0.1
#define SPI_PL_0_2    "/dev/spidev0.2"        ///< SPI device path: PL0.2

#define SPI_PL_1_0    "/dev/spidev1.0"        ///< SPI device path: PL1.0

#define SPI_PL_2_0    "/dev/spidev2.0"        ///< SPI device path: PL2.0
#define SPI_PL_3_0    "/dev/spidev3.0"        ///< SPI device path: PL2.0
#define SPI_PL_4_0    "/dev/spidev4.0"        ///< SPI device path: PL2.0


// ------------------------------------------------------------------------------------
//                                      GPIO
// ------------------------------------------------------------------------------------

/**
 * @brief    GPIO direction
 */
typedef enum{
    GPIO_DIR_OUTPUT,
    GPIO_DIR_INPUT
}GpioDir;

/**
 * @brief    GPIO level
 */
typedef enum{
    GPIO_LEVEL_LOW,
    GPIO_LEVEL_HIGH
}GpioLevel;

/**
 * @brief   ON/OFF
 */
enum{
    ONOFF_OFF = 0,
    ONOFF_ON
};

enum class Role
{
	unknown,
	watcher,
	producer,
	admin,
};

struct pipe
{
	std::string id;
	const char * pipe_path;
	int pipe_fd;
};

struct ERRORmessage
{
	std::string datetime;
	std::string message;
	std::string device;
	bool status;
};

struct POTIONmessage
{
	std::string datetime;
	std::string potion;
	std::string action;
};

struct recipe
{
	std::string LOTNO;
	std::string PARTNO;
	std::string STARTDATETIME;
	std::string ENDDATETIME;
	std::string RANDOMSTRING;
	std::string SOURCE;
	std::string PPR_or_DC;
};

class AlarmNotification: public Notification
{
public:
	AlarmNotification(std::string _msg, std::string _device, bool _status):msg(_msg), device(_device), status(_status)
{}
	~AlarmNotification(){};
	std::string msg;
	std::string device;
	bool status;
};

#endif /* __DEF_H__ */
