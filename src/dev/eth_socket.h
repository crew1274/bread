/**
 * @file        eth_socket.h
 *
 * @author      Matic Chiu <matic.chiu@cht-pt.com.tw>
 *
 * @date        2017/12/06
 */

#ifndef __DEV_ETH_SOCKET_H__
#define __DEV_ETH_SOCKET_H__

#include <stdlib.h>

// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     DATA TYPE DEFINITION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief    Protocol type for ethernet
 */
typedef enum
{
    PROTOCOLTYPE_UNKNOWN                            = -1,   ///< Unknown protocol.
    PROTOCOLTYPE_IP                                 = 0,    ///< Internet Protocol.
    PROTOCOLTYPE_IPv6_HOP_BY_HOP_OPTIONS            = 0,    ///< IPv6 Hop by Hop Options header.
    PROTOCOLTYPE_UNSPECIFIED                        = 0,    ///< Unspecified protocol.
    PROTOCOLTYPE_ICMP                               = 1,    ///< Internet Control Message Protocol.
    PROTOCOLTYPE_IGMP                               = 2,    ///< Internet Group Management Protocol.
    PROTOCOLTYPE_GGP                                = 3,    ///< Gateway To Gateway Protocol.
    PROTOCOLTYPE_IPv4                               = 4,    ///< Internet Protocol version 4.
    PROTOCOLTYPE_TCP                                = 6,    ///< Transmission Control Protocol.
    PROTOCOLTYPE_PUP                                = 12,   ///< PARC Universal Packet Protocol.
    PROTOCOLTYPE_UDP                                = 17,   ///< User Datagram Protocol.
    PROTOCOLTYPE_IDP                                = 22,   ///< Internet Datagram Protocol.
    PROTOCOLTYPE_IPv6                               = 41,   ///< Internet Protocol version 6 (IPv6).
    PROTOCOLTYPE_IPv6_ROUTING_HEADER                = 43,   ///< IPv6 Routing header.
    PROTOCOLTYPE_IPv6_FRAGMENT_HEADER               = 44,   ///< IPv6 Fragment header.
    PROTOCOLTYPE_IP_SEC_ENCAPSULATING_SECURITY_PAYLOAD = 50, ///< IPv6 Encapsulating Security Payload header.
    PROTOCOLTYPE_IP_SEC_AUTHENTICATION_HEADER       = 51,   ///< IPv6 Authentication header. For details, see RFC 2292 section 2.2.1, available at http:///< www.ietf.org.
    PROTOCOLTYPE_ICMPV6                             = 58,   ///< Internet Control Message Protocol for IPv6.
    PROTOCOLTYPE_IPv6_NO_NEXT_HEADER                = 59,   ///< IPv6 No next header.
    PROTOCOLTYPE_IPv6_DESTINATION_OPTIONS           = 60,   ///< IPv6 Destination Options header.
    PROTOCOLTYPE_ND                                 = 77,   ///< Net Disk Protocol (unofficial).
    PROTOCOLTYPE_RAW                                = 255,  ///< Raw IP packet protocol.
    PROTOCOLTYPE_IPX                                = 1000, ///< Internet Packet Exchange Protocol.
    PROTOCOLTYPE_SPX                                = 1256, ///< Sequenced Packet Exchange protocol.
    PROTOCOLTYPE_SPXII                              = 1257  ///< Sequenced Packet Exchange version 2 protocol.
}ProtocolTypeTable;


// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     CLASS
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief    Ethernet socket.
 */
class EthSocket {
public:

    /**
     * @brief         Constructor of this class
     */
    EthSocket();


    /**
     * @brief       Bind ethernet device
     *
     * @param[in]   eth_name        Ethernet name (ex: "eth1")
     *
     * @return      Return 0 if ok
     */
    int BindEth(const char *eth_name);


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
    int ConnectWithRetry(const char *host_addr,
                         uint port,
                         const char *eth_name,
                         ProtocolTypeTable ptype = PROTOCOLTYPE_TCP);


    /**
     * @brief       Close connection
     *
     * @return      None
     */
    void Close();


    /**
     * @brief       Send data to host
     *
     * @param[in]   databuff        The data will be sent to host
     * @param[in]   databuff_len    The length of databuff
     *
     * @return      Return 0 if ok
     */
    int SendData(char *databuff,
                 uint databuff_len);


    /**
     * @brief       Receive data from host
     *
     * @param[in]   databuff        The received data from host
     * @param[in]   databuff_len    The length of databuff
     *
     * @return         Return 0 if ok
     */
    int ReceiveData(char *databuff,
                    uint databuff_len);


    /*
     * @brief       Get socket header
     *
     * @return      Socket header
     */
    int socket_fd();


    /**
     * @brief       Get the name of ethernet device
     *
     * @param[out]  eth_buff        To store the name of ethernet device
     * @param[in]   eth_buff_len    The size of eth_buff
     *
     * @return      None
     */
    void GetEthName(char *eth_buff,
                    uint eth_buff_len);


    /**
     * @brief       Get IP address of the ethernet device
     *
     * @param[in]   eth_name        The ethernet device which will be detected IP address
     * @param[out]  databuff        IP address
     * @param[out]  databuff_len    The databuff size
     *
     * @return      Return 0 if IP address got
     */
    static int GetLocalAddr(const char *eth_name,
                            char *databuff = NULL,
                            uint databuff_len = 0);


    /**
     * @brief       Wait for DHCP to assign IP address to the specified ethernet device
     *
     * @param[in]   eth_name        To check if this ethernet device has been assigned IP address via DHCP
     *
     * @return      Return 0 if ok
     */
    static int WaitLocalAddrGetFromDHCP(const char *eth_name);

private:

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
    int CreateTcpIp(const char *host_addr,
               uint port,
               const char *eth_name,
               ProtocolTypeTable ptype);

    /**
     * @brief       To Create the connection with  TCP/IP to host
     *
     * @param[in]   host_addr       Host address
     * @param[in]   port            Port number of host
     * @param[in]   eth_name        The name of ethernet device
     *
     * @return      Return 0 if ok
     */
    int CreateUDP(const char *host_addr,
                  uint port,
                  const char *eth_name);

    /// Socket header
    int socket_fd_;


    /// The name of ethernet device
    char eth_name_[32];

};


#endif /* __DEV_ETH_SOCKET_H__ */
