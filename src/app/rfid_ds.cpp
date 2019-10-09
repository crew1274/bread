/**
 * @file        Rfid_ds_ds.cpp
 *
 * @author      Matic Chiu <matic.chiu@cht-pt.com.tw>
 *
 * @date        2018/09/05
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../dev/uart.h"
#include "../utility.h"
#include "rfid_ds.h"
//#include "rfid.h"

#include "Poco/StringTokenizer.h"
#include "Poco/String.h" // for cat
#include "Poco/NumberParser.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     DEFINITION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

#define RFID_STRING_MIN     20


enum{
    TAGPART_IP,
    TAGPART_TIMESTAMP,
    TAGPART_TAGNUMBER,
    TAGPART_CRC32,
    TAGPART_TOTAL
};


// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     IMPLEMENTATION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

Rfid_ds::Rfid_ds(const char *uart_path,
           uint gpio_num_tx_en,
           uint gpio_num_rx_en_n)
        : Rfid(uart_path,
                gpio_num_tx_en,
                gpio_num_rx_en_n)
{
    memset(rcv_data_, 0, sizeof(rcv_data_));
    rcv_data_len_ = 0;

    uart_object = new Max13433( uart_path,
                                gpio_num_tx_en,
                                gpio_num_rx_en_n);
}

Rfid_ds::Rfid_ds(const char *uart_path)
        : Rfid(uart_path)
{
    uart_object = new Uart(uart_path);
    uart_object->SetBaudRate(BAUDRATE_9600);
    uart_object->SetOptions(8, 1, 'N');
}

Rfid_ds::~Rfid_ds()
{
    if(uart_object)
    {
        delete uart_object;
        uart_object = NULL;
    }
}


void Rfid_ds::Get()
{
#if 1
    rcv_data_len_ = sizeof(rcv_data_);
    memset(rcv_data_, 0, sizeof(rcv_data_));

    uart_object->Get(( char *)rcv_data_, rcv_data_len_);
#else
    strcpy(rcv_data_, "[192.168.6.81,20180813193019,2018081411351217500000d4,1377529415]");
    rcv_data_len_ = strlen(rcv_data_);

#endif
}

int Rfid_ds::Get(char *data,
              int &data_len)
{
    int ret = 0;

    rcv_data_len_ = sizeof(rcv_data_);
    memset(rcv_data_, 0, sizeof(rcv_data_));

    ret = uart_object->Get(( char *)data, data_len);

    if(ret < 0)
        return -1;

    strncpy(rcv_data_, data, data_len);
    rcv_data_len_ = data_len;

    return ret;
}

int Rfid_ds::GetTagNumber(char *tag_number)
{
    return GetPartInfo(TAGPART_TAGNUMBER, tag_number);
}

int Rfid_ds::GetIP(char *ip)
{
    return GetPartInfo(TAGPART_IP, ip);
}

int Rfid_ds::GetTimeStamp(char *time_stamp)
{
    return GetPartInfo(TAGPART_TIMESTAMP, time_stamp);
}

bool Rfid_ds::CRC_Check()
{
    char crc_input[128];

    memset(crc_input, 0, sizeof(crc_input));

    std::string st(rcv_data_);

    //
    // Get position of the last dot
    //
    int last_dot = st.find_last_of(",");

    if(last_dot < 1 || last_dot > (int)(sizeof(crc_input) - 1))
    {
        DBG("Can not find dot or tag message is wrong (%d)", last_dot);
        return false;
    }

    //
    // Copy input string for CRC32
    //
    strncpy(crc_input, &rcv_data_[1], last_dot);

    printf("CRC input = %s\r\n", crc_input);

    //
    // Calculate CRC32
    //
    unsigned int crc32_calculate = Utility::GetCRC32((unsigned char *)crc_input, strlen(crc_input));

    //
    // Get CRC32 value from tag message
    //
    string crc32_input = st.substr(last_dot + 1, rcv_data_len_ - 1 - (last_dot + 1));

    unsigned int crc32_str_value = 0;

    if(!Poco::NumberParser::tryParseUnsigned(crc32_input, crc32_str_value))
    {
        DBG("tryParseUnsigned error, %s", crc32_input.c_str());
        return false;
    }

    //
    // Check CRC32
    //
    if(crc32_calculate != crc32_str_value)
    {
        DBG("crc32_calculate = 0x%x, crc32_str_value = 0x%x", crc32_calculate, crc32_str_value);
        return false;
    }

    return true;
}

void Rfid_ds::ClearBuffer()
{
    int buffer_size = 0;
    do{
        buffer_size = uart_object->ClearBuffer();
    }while(buffer_size > 0);
}

int Rfid_ds::GetPartInfo(unsigned int tag_part_index, char *data)
{
    char rcv_data_tmp[128];

    memset(rcv_data_tmp, 0, sizeof(rcv_data_tmp));
    strncpy(rcv_data_tmp, rcv_data_, sizeof(rcv_data_tmp));

    Poco::StringTokenizer t1(rcv_data_tmp, ",[]"",;",
            Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);

    if(t1.count() != 4)
    {
        DBG("rcv_data_ error, %s", rcv_data_);
        return -1;
    }

    strcpy(data, t1[tag_part_index].c_str());

    return 0;
}
