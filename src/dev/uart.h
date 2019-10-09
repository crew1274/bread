/**
 * @file        uart.h
 *
 * @author      Matic Chiu <matic.chiu@cht-pt.com.tw>
 *
 * @date        2017/12/06
 */

#ifndef __DEV_UART_H__
#define __DEV_UART_H__

#include "../def.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     DEFINITION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//#define UART_PS0    "/dev/ttyPS0"        ///< UART device path: PS0
//#define UART_PS1    "/dev/ttyPS1"        ///< UART device path: PS1
//#define UART_PL1    "/dev/ttyUL1"        ///< UART device path: PL1
//#define UART_PL2    "/dev/ttyUL2"        ///< UART device path: PL2
//#define UART_PL3    "/dev/ttyUL3"        ///< UART device path: PL3
//#define UART_PL4    "/dev/ttyUL4"        ///< UART device path: PL4

// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     DATA TYPE DEFINITION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------



// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     CLASS
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief    UART device
 */
class Uart{
public:

    /**
     * @brief       Constructor of UART
     *
     * @param[in]   dev_path        UART device path in linux
     */
    Uart(const char *dev_path);


    /**
     * @brief        Destructor of UART
     */
    virtual ~Uart();


    /**
     * @brief       Set baud rate
     *
     * @param[in]   baudrate    Set the baud rate into UART
     *
     * @return      Return 0 if baud rate setting is ok
     */
    int SetBaudRate(BaudRateTable baudrate);


    /**
     * @brief       Set databits, stopbits, and parity
     *
     * @param[in]   databits    Data bits, value = 5 ~ 9    (Recommend: 8)
     * @param[in]   stopbits    Stop bits, value = 1 or 2    (Recommend: 1)
     * @param[in]   parity      Parity, value = 'N', 'E', 'O', 'S'    (Recommend: ''N)
     *
     * @return      Return 0 if setting is ok
     */
    int SetOptions(int databits,
                   int stopbits,
                   int parity);


    /**
     * @brief       Send data to slave device
     *
     * @param[in]   data        Data will be sent to slave
     * @param[in]   data_len    Data length
     *
     * @return      Return 0 if ok
     */
    virtual int Send(const char *data,
                     int data_len);


    /**
     * @brief           Get data from slave device
     *
     * @param[out]      data        Received data from slave device
     * @param[in,out]   data_len    1. In:  Received data buffer size
     *                              2. Out: Received data length
     *
     * @return       Return 0 if ok
     */
    virtual int Get(char *data,
                    int &data_len);

    int ClearBuffer();

private:

    /// UART device path
    char *device_path_;


    /// UART header
    int uart_fd_;

};


#endif /* __DEV_UART_H__ */
