/**
 * @file        max13433.cpp
 *
 * @author      Matic Chiu <matic.chiu@cht-pt.com.tw>
 *
 * @date        2018/05/17
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../def.h"

#include "max13433.h"
#include "../utility.h"


// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     IMPLEMENTATION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief       Constructor of MAX13433
 *
 * @param[in]   uart_path           UART device path
 * @param[in]   gpio_num_tx_en      Enable pin for Tx (Active high)
 * @param[in]   gpio_num_rx_en_n    Enable pin for Rx (Active low)
 */
Max13433::Max13433(const char *uart_path,
                   int gpio_num_tx_en,
                   int gpio_num_rx_en_n)
    : Uart(uart_path)
{
    gpio_tx_en_  = new Gpio(gpio_num_tx_en,
                            GPIO_DIR_OUTPUT, GPIO_LEVEL_LOW);
    gpio_rx_ex_n = new Gpio(gpio_num_rx_en_n,
                            GPIO_DIR_OUTPUT, GPIO_LEVEL_LOW);
    SetBaudRate(BAUDRATE_9600);
    SetOptions(8, 1, 'N');
}

/**
 * @brief       Destructor of MAX13433
 */
Max13433::~Max13433()
{

}

/**
 * @brief       Send data to slave device
 *
 * @param[in]   data        Data will be sent to slave
 * @param[in]   data_len    Data length
 *
 * @return      Return 0 if ok
 */
int Max13433::Send(char *data,
                   int data_len)
{
    // Enable Tx
    gpio_tx_en_->SetLevel(GPIO_LEVEL_HIGH);
    // Disable Rx first before Tx is running
    gpio_rx_ex_n->SetLevel(GPIO_LEVEL_HIGH);

    Uart::Send(data, data_len);

    // Worse case is baud rate = 9600bps -> 960 bytes per second ~= 1 byte per 1 ms
    // Sleep until send all data out
    Utility::Sleep_us(data_len * 1E3);

    // Set to Rx mode
    gpio_tx_en_->SetLevel(GPIO_LEVEL_LOW);
    gpio_rx_ex_n->SetLevel(GPIO_LEVEL_LOW);

    return 0;
}

/**
 * @brief           Get data from slave device
 *
 * @param[out]      data        Received data from slave device
 * @param[in,out]   data_len    1. In:  Received data buffer size
 *                              2. Out: Received data length
 *
 * @return       Return 0 if ok
 */
int Max13433::Get(char *data,
                  int &data_len)
{
    gpio_tx_en_->SetLevel(GPIO_LEVEL_LOW);
    gpio_rx_ex_n->SetLevel(GPIO_LEVEL_LOW);
    return Uart::Get(data, data_len);
}


