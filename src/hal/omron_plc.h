/**
 * @file        omron_plc.h
 *
 * @author      Matic Chiu <matic.chiu@cht-pt.com.tw>
 *
 * @date        2017/12/06
 */


#ifndef __HAL_OMRON_PLC_H__
#define __HAL_OMRON_PLC_H__

#include <vector>
#include <string>
#include <map>
#include "../dev/eth_socket.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     DATA TYPE DEFINITION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief    Memory area list of PLC
 */
typedef enum
{
#if 0   // CV mode
    MEMORYAREA_CIO_STATUS           = 0x00,
    MEMORYAREA_TR_STATUS            = 0x00,
    MEMORYAREA_CPU_BUS_STATUS       = 0x00,
    MEMORYAREA_AUXILIARY_STATUS     = 0x00,
    MEMORYAREA_TIMER_STATUS         = 0x01,
    MEMORYAREA_TRANSITIOIN_STATUS   = 0x03,
    MEMORYAREA_STEP_STATUS          = 0x04,
    MEMORYAREA_FORCED_STATUS        = 0x05,
    MEMORYAREA_CIO                  = 0x80,
    MEMORYAREA_TR                   = 0x80,
    MEMORYAREA_CPU_BUS              = 0x80,
    MEMORYAREA_AUXILIARY            = 0x80,
    MEMORYAREA_TIMER_PV             = 0x81,
    MEMORYAREA_DM                   = 0x82,
    MEMORYAREA_FORCED               = 0x85,
    MEMORYAREA_DM_EXP1              = 0x90,
    MEMORYAREA_DM_EXP2              = 0x91,
    MEMORYAREA_DM_EXP3              = 0x92,
    MEMORYAREA_DM_EXP4              = 0x93,
    MEMORYAREA_DM_EXP5              = 0x94,
    MEMORYAREA_DM_EXP6              = 0x95,
    MEMORYAREA_DM_EXP7              = 0x96,
    MEMORYAREA_DM_EXP8              = 0x97,
    MEMORYAREA_DM_EXP_CURRENT       = 0x98,
    MEMORYAREA_ACTION_STATUS        = 0x1B
#else   // CS mode
    MEMORYAREA_CIO_BIT              = 0x30,     ///< Bit status: CIO area
    MEMORYAREA_WR_BIT               = 0x31,     ///< Bit status: Work area
    MEMORYAREA_HR_BIT               = 0x32,     ///< Bit status: Holding relay
    MEMORYAREA_AR_BIT               = 0x33,     ///< Bit status: Auxiliary area
    MEMORYAREA_CIO_BIT_FS           = 0x70,     ///< Bit with forced status: CIO status
    MEMORYAREA_WR_BIT_FS            = 0x71,     ///< Bit with forced status: Work area status
    MEMORYAREA_HR_BIT_FS            = 0x72,     ///< Bit with forced status: Holding relay status
    MEMORYAREA_CIO                  = 0xB0,     ///< Word status: CIO
    MEMORYAREA_WR                   = 0xB1,     ///< Word status: Work area
    MEMORYAREA_HR                   = 0xB2,     ///< Word status: Holding relay
    MEMORYAREA_AR                   = 0xB3,     ///< Word status: Auxiliary area
    MEMORYAREA_CIO_FS               = 0xF0,     ///< Bit with forced status: CIO
    MEMORYAREA_WR_FS                = 0xF1,     ///< Bit with forced status: Work area
    MEMORYAREA_HR_FS                = 0xF2,     ///< Bit with forced status: Holding relay area
    MEMORYAREA_TIM                  = 0x09,     ///< Completion Flag: Timer area
    MEMORYAREA_CNT                  = 0x09,     ///< Completion Flag: Counter area
    MEMORYAREA_TIM_FS               = 0x49,     ///< Bit with forced status:
    MEMORYAREA_CNT_FS               = 0x49,     ///< Bit with forced status:
    MEMORYAREA_TIM_PV               = 0x89,     ///< PV: Timer area
    MEMORYAREA_CNT_PV               = 0x89,     ///< PV: Counter area
    MEMORYAREA_DM_BIT               = 0x02,     ///< Bit status: DM
    MEMORYAREA_DM                   = 0x82,     ///< Word value: DM
    MEMORYAREA_TK_BIT               = 0x06,     ///< Bit status: TK
    MEMORYAREA_TK                   = 0x46,     ///< Status: TK
    MEMORYAREA_EM_0                 = 0x90,     ///< Word value: EM with bank 0
    MEMORYAREA_EM_1                 = 0x91,     ///< Word value: EM with bank 1
    MEMORYAREA_EM_2                 = 0x92,     ///< Word value: EM with bank 2
    MEMORYAREA_EM_3                 = 0x93,     ///< Word value: EM with bank 3
    MEMORYAREA_EM_4                 = 0x94,     ///< Word value: EM with bank 4
    MEMORYAREA_EM_5                 = 0x95,     ///< Word value: EM with bank 5
    MEMORYAREA_EM_6                 = 0x96,     ///< Word value: EM with bank 6
    MEMORYAREA_EM_7                 = 0x97      ///< Word value: EM with bank 7
#endif
}MemoryAreaTable;


/**
 * @brief    Sub-request code list
 */
typedef enum
{
    OMRON_SRC_NONE,
    OMRON_SRC_READ  = 0x01,        ///< Must be 0x01 for read
    OMRON_SRC_WRITE = 0x02,        ///< Must be 0x02 for write
    OMRON_SRC_TOTAL
}OmronSRCTable;


/**
 * @brief   Structure for multiple memory area read
 */
struct MultipleAreaRead{
	MultipleAreaRead() : area(), address(), bit_position(), return_value(), hex_as_dec() {}
	MultipleAreaRead(MemoryAreaTable area, uint address, char bit_position,uint return_value, bool hex_as_dec): area(area), address(address), bit_position(bit_position), return_value(return_value), hex_as_dec(hex_as_dec)  {}
    MemoryAreaTable area;           ///< Memory area
    uint            address;        ///< Data address
    char            bit_position;   ///< The bit position of data
    uint            return_value;   ///< The value returned from PLC
    bool            hex_as_dec;     ///< If true, transfer hex string as decimal.
                                    ///< Ex: 0x1234(hex) -> 1234(dec)
                                    ///< Or transfer hex value to decimal value
                                    ///< Ex: 0x1234(hex) -> 4660(dec)
};

// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     CLASS
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief        Access data to PLC
 */
class OmronPLC : private EthSocket {
public:

    /**
     * @brief       Constructor of Omron
     *              Set a random seed via time
     */
    OmronPLC();


    /**
     * @brief       Destructor of Omron
     *
     */
    ~OmronPLC();


    void InitIpAddress(char *local_address, const char *plc_address);


    /**
     * @brief       Connect to OMRON PLC
     *
     * @param[in]   hostaddr        PLC IP address
     * @param[in]   port            PLC port number
     * @param[in]   eth_name        The name of the specified ethernet device will be used
     * @param[in]   protocoltype    Protocol type for ethernet
     *
     * @return      Return 0 if connected
     */
    int Connect(const char *hostaddr,
                int port,
                const char *eth_name,
                ProtocolTypeTable protocoltype);


    /**
     * @brief       Read lots of points from PLC at the same time
     *              The received data can be got from GetData_ResponseFins() or GetValue_ResponseFins()
     *
     * @param[in]   memory_area     The point will be select
     * @param[in]   node_addr       The address in the point will be read
     * @param[in]   bit_position    The bit position of data
     * @param[in]   point_count     Get number of point
     *                              Notice that the size of received data can not bigger or equal than size of array g_respFinsData
     *
     * @return      Return 0 if ok
     */
    int ReadPoint(MemoryAreaTable memory_area,
                  unsigned int node_addr,
                  char bit_position,
                  unsigned int point_count);


    /**
     * @brief       Write point to PLC
     *
     * @param[in]   memory_area     The point will be select
     * @param[in]   node_addr       The address in the point will be written to
     * @param[in]   bit_position    The bit position of data
     * @param[in]   data            The data will be written to PLC
     * @param[in]   data_size       Data size
     *
     * @return      Return 0 if ok
     */
    int WritePoint(MemoryAreaTable memory_area,
                   unsigned int node_addr,
                   char bit_position,
                   char *data,
                   unsigned int data_size);


    /**
     * @brief       Read DM point
     *
     * @param[in]   node_addr       The address of DM will be read
     *
     * @return      Return 0 if ok
     */
    int ReadDM(unsigned int node_addr);


    /**
     * @brief       Write DM point
     *
     * @param[in]   node_addr       The address of DM will be written
     * @param[in]   data            The data will be written
     *
     * @return      Return 0 if ok
     */
    int WriteDM(unsigned int node_addr,
                char *data);


    /**
     * @brief       Get data size according to memory area type
     *
     * @param[in]   memory_area     Set memory area type
     *
     * @return      Get data size (Unit: bytes)
     */
    int GetDataSize_MemoryArea(MemoryAreaTable memory_area);


    /**
     * @brief       Get byte data from FINS response
     *              This function will return raw byte data
     *
     * @param[out]  data        Store result data
     * @param[in]   data_len    Data length
     *
     * @return      Return 0 if ok
     */
    int GetData_ResponseFins(char *data,
                             unsigned int data_len);


    /**
     * @brief       Get value from specified data index of FINS response
     *              This function will transfer byte data into value
     *
     * @param[in]   data_index      Data index at FINS response
     * @param[in]   data_len        How many bytes of raw data will be transfer to value
     *
     * @return      Return data value
     */
    int GetValue_ResponseFins(unsigned int data_index,
                              unsigned int data_len);


    /**
     * @brief       Show responseFins data
     *
     * @return      None
     */
    void ShowData_ResponseFins();


    /**
     * @brief       Read multiple area data
     *
     * @param[in]   area_list       All the data of multiple areas. Data number can more than 128.
     * @param[in]   data_number     Data number. Can more than 128.
     *
     * @return      Return 0 if ok
     */
    int ReadData_MultipleArea(MultipleAreaRead *area_list,
                              uint data_number);

    int ReadData_MultipleArea_V2(std::vector<MultipleAreaRead> &MultipleAreaReads);

    static MemoryAreaTable MemoryTypeIndex(std::string type);


    /**
     * @brief       Show all the multiple area data
     *
     * @param[in]   area_list       All the data of multiple areas
     * @param[in]   data_number     Data number
     *
     * @return      None
     */
    void ShowData_MultipleArea(MultipleAreaRead *area_list,
                               uint data_number);

    /**
     * @brief       Disconnect to PLC
     *
     * @return      None
     */
    void Shutdown();

    int reConnect();

private:

    /**
     * @brief       Send node address data
     *              Need to be sent once while the first time connecting to PLC
     *
     * @return      Return 0 if ok
     */
    int SendNodeAddressData();


    /**
     * @brief       Combine the FINS command
     *
     * @param[in]   eomron_src      Sub-request code. It will decide the FINS command is read or write function
     * @param[in]   area            Memory area.
     * @param[in]   address         The data address of memory area
     * @param[in]   bit_position    The bit position of data
     * @param[in]   count           How many data will be read/written
     *
     * @return      None
     */
    void SetCmdFins(OmronSRCTable eomron_src,
                    MemoryAreaTable area,
                    unsigned int address,
                    char bit_position,
                    unsigned int count);


    /**
     * @brief       Combine the FINS command of multiple memory area
     *
     * @param[in]   area_list       Multiple memory area data
     * @param[in]   area_number     area_list number
     *
     *
     * @return      None
     */
    void SetCmdFins(MultipleAreaRead *area_list,
                    uint area_number);

    void SetCmdFins_V2(std::vector<MultipleAreaRead> &MultipleAreaReads, uint index, uint area_number);

    /**
     * @brief       Function pointer to save sending commands to PLC
     *
     * @param[in]   data            Append this data into FINS command. (If necessary)
     * @param[in]   data_size       Data size
     * @param[in]   enable_delay    If read/write data more than 200, enable this delay
     *
     * @return      Return 0 if ok
     */
    int (OmronPLC::*SendFrame)(char *data,
                     unsigned int data_size,
                     bool enable_delay);


    /**
     * @brief       Send commands(FS command and FINS command) to PLC via TCP
     *
     * @param[in]   data            Append this data into FINS command. (If necessary)
     * @param[in]   data_size       Data size
     * @param[in]   enable_delay    If read/write data more than 200, enable this delay
     *
     * @return      Return 0 if ok
     */
    int SendFrame_TCP(char *data,
                      unsigned int data_size,
                      bool enable_delay = false);


    /**
     * @brief       Send commands(FS command and FINS command) to PLC via UDP
     *
     * @param[in]   data            Append this data into FINS command. (If necessary)
     * @param[in]   data_size       Data size
     * @param[in]   enable_delay    If read/write data more than 200, enable this delay
     *
     * @return      Return 0 if ok
     */
    int SendFrame_UDP(char *data,
                      unsigned int data_size,
                      bool enable_delay = false);


    /**
     * @brief       Get value of multiple area data
     *
     * @param[in]   area_list       All the data of multiple areas
     * @param[in]   data_number     Data number
     *
     * @return      The value of the multiple area data
     */
    int GetValue_MultipleArea(MultipleAreaRead *area_list,
                              uint data_number);

    int GetValue_MultipleArea_V2(std::vector<MultipleAreaRead> &MultipleAreaReads, uint index, uint data_number);

    /**
     * @brief       Transfer byte data into value
     *
     * @param[in]   data            Byte data will be transferred into value
     * @param[in]   data_size       Data size
     * @param[in]   hex_as_dec      If true, transfer hex string as decimal.
     *                              Ex: 0x1234(hex) -> 1234(dec)
     *                              Or transfer hex value to decimal value
     *                              Ex: 0x1234(hex) -> 4660(dec)
     *
     * @return      The value of data
     */
    uint TransferDataToValue(char *data,
                             uint data_size,
                             bool hex_as_dec);
protected:

	const char *hostaddr_b;
	int port_b;
	const char *eth_name_b;
	ProtocolTypeTable protocoltype_b;

};


#endif /* __HAL_OMRON_PLC_H__ */
