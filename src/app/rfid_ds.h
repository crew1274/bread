/**
 * @file        rfid_ds.h
 *
 * @author      Matic Chiu <matic.chiu@cht-pt.com.tw>
 *
 * @date        2018/09/05
 */


#ifndef __APP_RFID_DS_H__
#define __APP_RFID_DS_H__

#include "../hal/max13433.h"
#include "RFID.h"

class Uart;
//class Rfid;

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
class Rfid_ds : public Rfid
{
public:
    /**
     * @brief       Constructor of Rfid
     *
     * @param[in]   uart_path           Uart path
     * @param[in]   gpio_num_tx_en      GPIO number of Tx enable pin of Max13433 (Active: high)
     * @param[in]   gpio_num_rx_en_n    GPIO number of Rx enable pin of Max13433 (Active: low)
     */
    Rfid_ds(const char *uart_path,
         uint gpio_num_tx_en,
         uint gpio_num_rx_en_n);

    Rfid_ds(const char *uart_path);

    /**
     * @brief       Destructor of Rfid
     */
    ~Rfid_ds();

    /**
     * @brief       Get RFID id number
     *
     * @param[out]  tag_number           Get ID card number string
     */
    int GetTagNumber(char *tag_number);


    int GetIP(char *ip);

    int GetTimeStamp(char *time_stamp);

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

    bool CRC_Check();

    void ClearBuffer();

private:

    int GetPartInfo(unsigned int tag_part_index, char *data);

};



#endif /* __APP_RFID_DS_H__ */
