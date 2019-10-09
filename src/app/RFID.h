/**
 * @file        rfid.h
 *
 * @author      Matic Chiu <matic.chiu@cht-pt.com.tw>
 *
 * @date        2018/05/21
 */


#ifndef __APP_RFID_H__
#define __APP_RFID_H__

#include "../hal/max13433.h"



class Uart;

// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     DEFINITION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------


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
 * @brief       Read lot info via RS485
 */
class Rfid /*: public Max13433*/
{
public:
    /**
     * @brief       Constructor of Rfid
     *
     * @param[in]   uart_path           Uart path
     * @param[in]   gpio_num_tx_en      GPIO number of Tx enable pin of Max13433 (Active: high)
     * @param[in]   gpio_num_rx_en_n    GPIO number of Rx enable pin of Max13433 (Active: low)
     */
    Rfid(const char *uart_path,
         uint gpio_num_tx_en,
         uint gpio_num_rx_en_n);

    Rfid(const char *uart_path);

    /**
     * @brief       Destructor of Rfid
     */
    ~Rfid();

    /**
     * @brief       Get RFID id number
     *
     * @param[out]  tag_number           Get ID card number string
     */
    int GetTagNumber(char *tag_number);

    /**
     * @brief       Get lot number
     *
     * @param[in]   lot_number          Get lot number string
     */
    int GetLotNumber(char *lot_number);

    /**
     * @brief       Get ID number and lot number from RFID
     */
    void Get();

    /**
     * @brief       Get ID number and lot number from RFID
     *              Can return string and string length
     *
     * @param[out]      data        Received data from RFID
     * @param[in,out]   data_len    1. In:  Received data buffer size
     *                              2. Out: Received data length
     *
     * @return      0: ok;  1: fail
     */
    int Get(char *data,
            int &data_len);

    void ClearBuffer();

    bool is_IC(std::string tag_number);

    bool is_RC(std::string tag_number);

private:

protected:
    /// Received data from RFID
    char rcv_data_[128];

    /// Received data length
    int rcv_data_len_;

    Uart *uart_object;

};



#endif /* __APP_RFID_H__ */
