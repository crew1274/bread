/**
 * @file        eth_socket.cpp
 *
 * @author      Matic Chiu <matic.chiu@cht-pt.com.tw>
 *
 * @date        2017/12/06
 */

#include "../dev/eth_socket.h"

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>    // for socket
#include <netdb.h>        // for gethostbyname()
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <net/if.h>

#include "../utility.h"


// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     DEFINITION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

#define CONNECT_RETRY           2       ///< Connect retry times
#define CONNECT_TIMEOUT         10      ///< Connect timeout (Unit: second)

#define DHCP_RETRY              10      ///< Retry times for DHCP getting IP address
#define DHCP_WAIT_SEC           5       ///< Waiting for DHCP getting IP address (Unit: Seconds)

#define RECV_TIMEOUT            3       ///< Timeout for receiving data (Unit: seconds)
#define SEND_TIMEOUT            3       ///< Timeout for sending data (Unit: seconds)


// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     IMPLEMENTATION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief       Constructor of this class
 */
EthSocket::EthSocket()
{
    socket_fd_ = -1;
}


/**
 * @brief       Will retry times to host if connection is failed
 *
 * @param[in]   host_addr       Host address
 * @param[in]   port            Port number of host
 * @param[in]   eth_name        The name of ethernet device
 * @param[in]   ptype           Protocol type of ethernet. Default is TCP
 *
 * @return      Return 0 if ok
 */
int EthSocket::ConnectWithRetry(const char *host_addr,
                                uint port,
                                const char *eth_name,
                                ProtocolTypeTable ptype)
{
    int socket_fd = -1;

    for(int retry_count = 0;
            retry_count < CONNECT_RETRY && socket_fd == -1;
            retry_count++)
    {
//        printf("CreateTcpIp Count = %d\r\n", retry_count);
        if(ptype != PROTOCOLTYPE_UDP)
            socket_fd = CreateTcpIp(host_addr, port, eth_name, ptype);
        else
            socket_fd = CreateUDP(host_addr, port, eth_name);
    }

    if(socket_fd == -1)
    {
        printf("ethSocket: Connect to %s:%d (%s) error.\r\n", host_addr, port, eth_name);
        return -1;
    }

    // Set send/recv timeout
    struct timeval tv;

    tv.tv_sec  = SEND_TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO , (const char*)&tv, sizeof(struct timeval));

    tv.tv_sec  = RECV_TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(struct timeval));


    socket_fd_ = socket_fd;
    strncpy(eth_name_, eth_name, sizeof(eth_name_));

//    printf("eth_name = %s, socket_fd_ = %d\r\n", eth_name, socket_fd_);

    return 0;
}


/**
 * @brief       To Create the connection with  TCP/IP to host
 *
 * @param[in]   host_addr       Host address
 * @param[in]   port            Port number of host
 * @param[in]   eth_name        The name of ethernet device
 * @param[in]   ptype           Protocol type of ethernet. Default is TCP
 *
 * @return      Return 0 if ok
 */
int EthSocket::CreateTcpIp(const char *host_addr,
                      uint port,
                      const char *eth_name,
                      ProtocolTypeTable ptype)
{
    int flags = 0, error = 0, ret = 0;
    fd_set  rset, wset;
    socklen_t len = sizeof(error);
    struct timeval  ts;

    struct hostent *he;
    struct sockaddr_in server_addr;
    int socket_fd;

//    printf("1\r\n");

    if((he = gethostbyname(host_addr))==NULL){
        return -1;
    }

//    printf("2\r\n");

    if((socket_fd = socket(AF_INET,SOCK_STREAM,ptype))==-1){
        return -1;
    }

//    printf("3\r\n");

    ts.tv_sec = CONNECT_TIMEOUT;
    ts.tv_usec = 0;

    //clear out descriptor sets for select
    //add socket to the descriptor sets
    FD_ZERO(&rset);
    FD_SET(socket_fd, &rset);
    wset = rset;    //structure assignment ok

    //set socket nonblocking flag
    if( (flags = fcntl(socket_fd, F_GETFL, 0)) < 0)
        return -1;

    if(fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) < 0)
        return -1;

//    printf("4\r\n");

    printf("Bind to device: %s\r\n", eth_name);
    if(setsockopt(socket_fd, SOL_SOCKET, SO_BINDTODEVICE, (char*)eth_name, strlen(eth_name)))
    {
        printf("ethSocket: Bind to device %s error.\r\n", eth_name);
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)he->h_addr);

    //initiate non-blocking connect
    if( (ret = connect(socket_fd, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) ) < 0 )
    {
//        printf("errno = %d\r\n", errno);
        if (errno != EINPROGRESS)
            return -1;
    }

    if(ret == 0)    //then connect succeeded right away
        goto done;

//    printf("5\r\n");

    //we are waiting for connect to complete now
    if( (ret = select(socket_fd + 1, &rset, &wset, NULL, (CONNECT_TIMEOUT) ? &ts : NULL)) < 0)
        return -1;
    if(ret == 0){   //we had a timeout
        errno = ETIMEDOUT;
        return -1;
    }

    //we had a positivite return so a descriptor is ready
    if (FD_ISSET(socket_fd, &rset) || FD_ISSET(socket_fd, &wset)){
        if(getsockopt(socket_fd,  SOL_SOCKET, SO_ERROR, &error, &len) < 0)
            return -1;
    }else
        return -1;

//    printf("6\r\n");

    if(error){  //check if we had a socket error
        errno = error;
        return -1;
    }

done:

//    printf("7\r\n");

//put socket back in blocking mode
    if(fcntl(socket_fd, F_SETFL, flags) < 0)
        return -1;
//    printf("8\r\n");
    return socket_fd;
}


/**
 * @brief       To Create the connection with  TCP/IP to host
 *
 * @param[in]   host_addr       Host address
 * @param[in]   port            Port number of host
 * @param[in]   eth_name        The name of ethernet device
 *
 * @return      Return 0 if ok
 */
int EthSocket::CreateUDP(const char *host_addr,
                         uint port,
                         const char *eth_name)
{
    int flags = 0, error = 0, ret = 0;
    fd_set  rset, wset;
    socklen_t len = sizeof(error);
    struct timeval  ts;

    struct hostent *he;
    struct sockaddr_in server_addr;
    int socket_fd;

//    printf("1\r\n");

    if((he = gethostbyname(host_addr))==NULL){
        return -1;
    }

//    printf("2\r\n");

    if((socket_fd = socket(AF_INET,SOCK_DGRAM,PROTOCOLTYPE_UDP))==-1){
        return -1;
    }

//    printf("3\r\n");

    ts.tv_sec = CONNECT_TIMEOUT;
    ts.tv_usec = 0;

    //clear out descriptor sets for select
    //add socket to the descriptor sets
    FD_ZERO(&rset);
    FD_SET(socket_fd, &rset);
    wset = rset;    //structure assignment ok

    //set socket nonblocking flag
    if( (flags = fcntl(socket_fd, F_GETFL, 0)) < 0)
        return -1;

    if(fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) < 0)
        return -1;

//    printf("4\r\n");

    printf("Bind to device: %s\r\n", eth_name);
    if(setsockopt(socket_fd, SOL_SOCKET, SO_BINDTODEVICE, (char*)eth_name, strlen(eth_name)))
    {
        printf("ethSocket: Bind to device %s error.\r\n", eth_name);
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)he->h_addr);

    //initiate non-blocking connect
    if( (ret = connect(socket_fd, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) ) < 0 )
    {
//        printf("errno = %d\r\n", errno);
        if (errno != EINPROGRESS)
            return -1;
    }

    if(ret == 0)    //then connect succeeded right away
        goto done;

//    printf("5\r\n");

    //we are waiting for connect to complete now
    if( (ret = select(socket_fd + 1, &rset, &wset, NULL, (CONNECT_TIMEOUT) ? &ts : NULL)) < 0)
        return -1;
    if(ret == 0){   //we had a timeout
        errno = ETIMEDOUT;
        return -1;
    }

    //we had a positivite return so a descriptor is ready
    if (FD_ISSET(socket_fd, &rset) || FD_ISSET(socket_fd, &wset)){
        if(getsockopt(socket_fd,  SOL_SOCKET, SO_ERROR, &error, &len) < 0)
            return -1;
    }else
        return -1;

//    printf("6\r\n");

    if(error){  //check if we had a socket error
        errno = error;
        return -1;
    }

done:

//    printf("7\r\n");

//put socket back in blocking mode
    if(fcntl(socket_fd, F_SETFL, flags) < 0)
        return -1;
//    printf("8\r\n");
    return socket_fd;
}


/**
 * @brief       Close connection
 *
 * @return      None
 */
void EthSocket::Close()
{
    if(socket_fd_ == -1)
    {
        printf("socket_fd_ == -1\r\n");
        return;
    }
    shutdown(socket_fd_, 2);
}


/**
 * @brief       Send data to host
 *
 * @param[in]   databuff        The data will be sent to host
 * @param[in]   databuff_len    The length of databuff
 *
 * @return      Return 0 if ok
 */
int EthSocket::SendData(char *databuff,
                        uint databuff_len)
{
    int sent=0,tmpres=0;

    if(socket_fd_ == -1)
    {
        printf("EthSocket::SendData: socket_fd_ == -1\r\n");
        return -1;
    }

//    printf("SendData: %s (%d)\r\n", databuff, socket_fd_);

//    Utility::ShowHexData("Send Data", databuff, databuff_len);

    while(sent < (int)databuff_len)
    {
        tmpres = send(socket_fd_,databuff+sent,databuff_len-sent,0);

        if(tmpres == -1)
        {
            DBG("Ethernet send data error! (%d)", sent);
            return -1;
        }
        sent += tmpres;
    }
    return sent;
}


/**
 * @brief       Receive data from host
 *
 * @param[in]   databuff        The received data from host
 * @param[in]   databuff_len    The length of databuff
 *
 * @return      Return 0 if ok
 */
int EthSocket::ReceiveData(char *databuff,
                           uint databuff_len)
{
    int recvnum = 0;

//    printf("socket_fd_ = %d\r\n", socket_fd_);


    if(socket_fd_ == -1)
    {
        printf("EthSocket::RcvData: socket_fd_ == -1\r\n");
        return -1;
    }
    recvnum = recv(socket_fd_, databuff, databuff_len, 0);

//    Utility::ShowHexData("Rcv Data", databuff, (recvnum > 0) ? recvnum : 0);
//    printf("RcvData: %s (%d)\r\n", databuff, recvnum);
//    printf("Rx data size = %d\r\n", recvnum);

    return recvnum;
}


/*
 * @brief       Get socket header
 *
 * @return      Socket header
 */
int EthSocket::socket_fd()
{
    return socket_fd_;
}


/**
 * @brief       Get the name of ethernet device
 *
 * @param[out]  eth_buff        To store the name of ethernet device
 * @param[in]   eth_buff_len    The size of eth_buff
 *
 * @return      None
 */
void EthSocket::GetEthName(char *eth_buff,
                           uint eth_buff_len)
{
    strncpy(eth_buff, eth_name_, eth_buff_len);
}


/**
 * @brief        Get IP address of the ethernet device
 *
 * @param[in]    eth_name        The ethernet device which will be detected IP address
 * @param[out]   databuff        IP address
 * @param[out]   databuff_len    The databuff size
 *
 * @return       Return 0 if IP address got
 */
int EthSocket::GetLocalAddr(const char *eth_name,
                            char *databuff,
                            uint databuff_len)
{
    int s;
    struct ifreq ifr = {};

    s = socket(PF_INET, SOCK_DGRAM, 0);

    strncpy(ifr.ifr_name, eth_name, sizeof(ifr.ifr_name));

    if (ioctl(s, SIOCGIFADDR, &ifr) < 0)
    {
        printf("Get local address fail\r\n");
        return -1;
    }

    if(databuff_len)
    {
        strncpy(databuff, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr), databuff_len);
        printf("Local address: %s\r\n", databuff);
    }

    return 0;
}


/**
 * @brief       Wait for DHCP to assign IP address to the specified ethernet device
 *
 * @param[in]   eth_name        To check if this ethernet device has been assigned IP address via DHCP
 *
 * @return      Return 0 if ok
 */
int EthSocket::WaitLocalAddrGetFromDHCP(const char *eth_name)
{
    for(int retryindex = 0;
            retryindex < DHCP_RETRY;
            retryindex++)
    {
        if(GetLocalAddr(eth_name) == 0)
        {
            DBG("IP of %s has detected", eth_name);
            return 0;
        }

        DBG("(%s) Sleep %d seconds", eth_name, DHCP_WAIT_SEC);
        Utility::Sleep_us(DHCP_WAIT_SEC * 1E6);
    }

    return -1;
}

