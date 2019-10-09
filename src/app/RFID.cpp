/**
 * @file        rfid.h
 *
 * @author      Matic Chiu <matic.chiu@cht-pt.com.tw>
 *
 * @date        2018/05/21
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../dev/uart.h"
#include "../utility.h"
#include "rfid.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     DEFINITION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

#define RFID_STRING_MIN     20


// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     IMPLEMENTATION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

Rfid::Rfid(const char *uart_path,
           uint gpio_num_tx_en,
           uint gpio_num_rx_en_n)
/*    : Max13433(uart_path,
               gpio_num_tx_en,
               gpio_num_rx_en_n)*/
{
    memset(rcv_data_, 0, sizeof(rcv_data_));
    rcv_data_len_ = 0;

    uart_object = new Max13433( uart_path,
                                gpio_num_tx_en,
                                gpio_num_rx_en_n);
}

Rfid::Rfid(const char *uart_path)
/*        : Max13433("",
                   0,
                   0)*/
{
    uart_object = new Uart(uart_path);
    uart_object->SetBaudRate(BAUDRATE_9600);
    uart_object->SetOptions(8, 1, 'N');
}

Rfid::~Rfid()
{
    delete uart_object;
}


void Rfid::Get()
{
    rcv_data_len_ = sizeof(rcv_data_);
    memset(rcv_data_, 0, sizeof(rcv_data_));

    uart_object->Get(( char *)rcv_data_, rcv_data_len_);
}

int Rfid::Get(char *data,
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

int Rfid::GetTagNumber(char *tag_number)
{
    //try
    //{
    	if(strlen(rcv_data_) < RFID_STRING_MIN)
            return -1;

        char *start_pointer = strstr(rcv_data_, "Tag:");
        if(start_pointer == NULL)
            return -1;

        char *end_pointer = strchr(start_pointer, ',');
        if(end_pointer == NULL)
            return -1;

        std::cout << "start_pointer = " << start_pointer << std::endl;
        std::cout << "end_pointer = " << end_pointer << std::endl;

        start_pointer += strlen("Tag:");
        strncpy(tag_number, start_pointer, end_pointer - start_pointer);
    //}
    //catch(...)
    //{
    //	printf("Rfid::GetTagNumber error");
    //}

    return 0;
}

int Rfid::GetLotNumber(char *lot_number)
{
    if(strlen(rcv_data_) < RFID_STRING_MIN)
        return -1;

    char *start_pointer = strchr(rcv_data_, ',');

    if(start_pointer == NULL)
        return -1;

    start_pointer += 1;

    strncpy(lot_number, start_pointer, rcv_data_len_ - (start_pointer - rcv_data_) - 1);

    return 0;

}

void Rfid::ClearBuffer()
{
	uart_object->ClearBuffer();
}

bool Rfid::is_IC(std::string tag_number)
{
	if(tag_number.size()>2)
	{
		return (tag_number.substr(tag_number.size()-2, 2) == "A1") ? true : false ;
	}
	else
	{
		return false;
	}
}

bool Rfid::is_RC(std::string tag_number)
{
	if(tag_number.size()>2)
	{
		return (tag_number.substr(tag_number.size()-2, 2) == "D4") ? true : false ;
	}
	else
	{
		return false;
	}
}
