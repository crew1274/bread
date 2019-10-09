/**
 * @file        max13433.h
 *
 * @author      Matic Chiu <matic.chiu@cht-pt.com.tw>
 *
 * @date        2018/05/17
 */


#ifndef __HAL_MAX13433_H__
#define __HAL_MAX13433_H__

#include "../dev/uart.h"
#include "../dev/gpio.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     CLASS
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief       MAX13433: UART to RS485
 */
class Max13433 : public Uart {
public:

    /**
     * @brief       Constructor of MAX13433
     *
     * @param[in]   uart_path           UART device path
     * @param[in]   gpio_num_tx_en      Enable pin for Tx (Active high)
     * @param[in]   gpio_num_rx_en_n    Enable pin for Rx (Active low)
     */
    Max13433(const char *uart_path, int gpio_num_tx_en, int gpio_num_rx_en_n);

    /**
     * @brief       Destructor of MAX13433
     */
    ~Max13433();

    /**
     * @brief       Send data to slave device
     *
     * @param[in]   data        Data will be sent to slave
     * @param[in]   data_len    Data length
     *
     * @return      Return 0 if ok
     */
    int Send(char *data,
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
    int Get(char *data,
            int &data_len);
private:

    /// Tx enable pin (Active high)
    Gpio *gpio_tx_en_;

    /// Rx enable pin (Active low)
    Gpio *gpio_rx_ex_n;
};



#endif /* __HAL_MAX13433_H__ */
