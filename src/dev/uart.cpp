/**
 * @file        uart.cpp
 *
 * @author      Matic Chiu <matic.chiu@cht-pt.com.tw>
 *
 * @date        2017/12/06
 */

#include "../dev/uart.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>

#include <string.h>

#include "../utility.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     GLOBAL PARAMETER
//
// ----------------------------------------------------------------------------------------------------------------------------------------------


// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     IMPLEMENTATION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief       Constructor of UART
 *
 * @param[in]   dev_path        UART device path in linux
 */
Uart::Uart(const char *devPath)
{
    uart_fd_ = open(devPath, O_RDWR | O_NOCTTY);
    if(uart_fd_ < 0)
    {
        printf("Can not opne uart: %s\r\n", devPath);
        return;
    }

    struct termios  oldtio;
    tcgetattr(uart_fd_, &oldtio);

    oldtio.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                        | INLCR | IGNCR | ICRNL | IXON);
    oldtio.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

    oldtio.c_oflag &= ~OPOST; /*Output*/
        
    tcflush(uart_fd_, TCIFLUSH);
    tcsetattr(uart_fd_, TCSANOW, &oldtio);

//    SetBaudRate(BAUDRATE_57600);
//    SetOptions(8, 1, 'N');
}


/**
 * @brief        Destructor of UART
 */
Uart::~Uart()
{
    close(uart_fd_);
}


/**
 * @brief       Set baud rate
 *
 * @param[in]   baudrate    Set the baud rate into UART
 *
 * @return      Return 0 if baud rate setting is ok
 */
int Uart::SetBaudRate(BaudRateTable baudrate)
{
    int baud_rate_table[BAUDRATE_TOTAL] = {
            B9600, B19200, B38400, B57600,
            B115200
    };

    int   status;
    struct termios   opt;


    if(uart_fd_ < 0)
    {
        printf("Can not opne uart: %d\r\n", uart_fd_);
        return uart_fd_;
    }

    tcgetattr(uart_fd_, &opt);

    tcflush(uart_fd_, TCIOFLUSH);
    cfsetispeed(&opt, baud_rate_table[baudrate]);
    cfsetospeed(&opt, baud_rate_table[baudrate]);
    status = tcsetattr(uart_fd_, TCSANOW, &opt);
    if  (status != 0) {
        perror("tcsetattr fd1");
        return -1;
    }
    tcflush(uart_fd_,TCIOFLUSH);

    return 0;
}


/**
 * @brief       Set databits, stopbits, and parity
 *
 * @param[in]   databits    data bits, value = 5 ~ 9    (Recommend: 8)
 * @param[in]   stopbits    Stop bits, value = 1 or 2    (Recommend: 1)
 * @param[in]   parity      Parity, value = 'N', 'E', 'O', 'S'    (Recommend: ''N)
 *
 * @return      Return 0 if setting is ok
 */
int Uart::SetOptions(int databits,
                     int stopbits,
                     int parity)
{
    struct termios options;

    if  ( tcgetattr(uart_fd_, &options)  !=  0) {
        perror("SetupSerial 1");
        return -1;
    }

    options.c_cflag &= ~CSIZE;

    // Set data bits
    switch (databits)
    {
        case 7:
            options.c_cflag |= CS7;
        break;
        case 8:
            options.c_cflag |= CS8;
        break;
        default:
            fprintf(stderr,"Unsupported data size\n");
            return -1;
    }

    // Set parity
    switch (parity)
    {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;       /* Clear parity enable */
            options.c_iflag &= ~INPCK;         /* Enable parity checking */
            break;
        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB);     /* Odd parity*/
            options.c_iflag |= INPCK;                 /* Disnable parity checking */
            break;
        case 'e':
        case 'E':
            options.c_cflag |= PARENB;         /* Enable parity */
            options.c_cflag &= ~PARODD;       /* Even parity*/
            options.c_iflag |= INPCK;       /* Disnable parity checking */
            break;
        case 'S':
        case 's':  /*as no parity*/
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;break;
        default:
            fprintf(stderr,"Unsupported parity\n");
            return -1;
    }

    // Set stop bits
    switch (stopbits)
    {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            fprintf(stderr,"Unsupported stop bits\n");
            return -1;
    }

    /* Set input parity option */
    if (parity != 'n')
        options.c_iflag |= INPCK;
    tcflush(uart_fd_, TCIFLUSH);
    options.c_cc[VTIME] = 1;     /* timeout = 0.1 seconds*/
    options.c_cc[VMIN]  = 0;     /* Update the options and do it NOW */
//    options.c_cc[VEOF]  = 4;
    if (tcsetattr(uart_fd_,TCSANOW,&options) != 0)
    {
        perror("SetupSerial 3");
        return -1;
    }
    return 0;
}


/**
 * @brief       Send data to slave device
 *
 * @param[in]   data        Data will be sent to slave
 * @param[in]   data_len    Data length
 *
 * @return      Return 0 if ok
 */
int Uart::Send(const char *data,
               int data_len)
{
    write(uart_fd_, data, data_len);
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
int Uart::Get(char *data,
              int &data_len)
{
    int nread;
    char buff[512];
    int rcv_data_len = 0;

    int bytes_available = 0;
    int timeout_max = 1000;    // unit: 1000ms

    // Waiting for the first byte data from UART
    while(bytes_available == 0 && timeout_max-- > 0)
    {
        ioctl(uart_fd_, FIONREAD, &bytes_available);
        Utility::Sleep_us(1E3);
    }

    if(timeout_max <= 0)
    {
        //DBG("UART Rx timeout");
        data_len = 0;
        return -1;
    }

    while(bytes_available > 0)  // Read data until no data in FIFO of UART any more
    {
        nread = read(uart_fd_, &buff[rcv_data_len], sizeof(buff) - rcv_data_len);
        rcv_data_len += nread;

        if(rcv_data_len >= (int)sizeof(buff))
            break;

        if(rcv_data_len >= data_len)
        {
            rcv_data_len = data_len;
            break;
        }

        Utility::Sleep_us(1E5);                         // Waiting for the next byte into FIFO of UART

        ioctl(uart_fd_, FIONREAD, &bytes_available);    // Check if any data in FIFO of UART
    }

    memcpy(data, buff, rcv_data_len);
    data_len = rcv_data_len;

    return 0;
}

int Uart::ClearBuffer()
{
    int nread;
    char buff[512];
//    int rcv_data_len = 0;

    int bytes_available = 0;

    ioctl(uart_fd_, FIONREAD, &bytes_available);

    if(bytes_available > 0)
    	nread = read(uart_fd_, buff, sizeof(buff));

    return bytes_available;

}
