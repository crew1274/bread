/**
 * @file        omron_plc.cpp
 *
 * @author      Matic Chiu <matic.chiu@cht-pt.com.tw>
 *
 * @date        2017/12/06
 */


#include "../hal/omron_plc.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include "../utility.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     DEFINITION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

#define ETH_NAME        "eth1"      ///< The name of the ethernet device for Omron
#define F_PARAM         12          ///< first position of PARAM area in the fins command

#define FINS_RETRY      5           ///< Retry times. If SID doesn't match, retry to get data from ethernet

#define MAX_RESPONSE_FINS_RECEIVE_SIZE  1998
#define MAX_DATA_WITHOUT_DELAY          200 ///< If read/write data more than 200, enable delay while accessing

// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     GLOBAL PARAMETER
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

/// FINS command format
char g_cmdFins[1024] = {
    0x80,   // ICF
    0x00,   // RSV
    0x02,   // GCT
    0x00,   // DNA
    0x48,   // DA1  // PLC IP address
    0x00,   // DA2
    0x00,   // SNA
    0x7A,   // SA1  // PC IP address
    0x00,   // SA2
    0x01,   // SID
    0x01,   // MRC
    0x01,   // SRC
    0x82,   // Memory area code
    0x02,   // Start address of data: High byte
    0xD2,   // Start address of data: Low byte
    0x00,
    0x00,   // Data count: High byte
    0x01    // Data count: Low byte
};

/// FINS command length
unsigned int g_cmdFins_Len = 0;

/// FINS response data
char g_respFinsData[2048] = {0};
uint g_respFinsData_length = 0;

// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     IMPLEMENTATION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief       Constructor of Omron
 *              Set a random seed via time
 */
OmronPLC::OmronPLC()
{
    srand (time(NULL));
}


/**
 * @brief       Destructor of Omron
 *
 */
OmronPLC::~OmronPLC()
{

}


void OmronPLC::InitIpAddress(char *local_address, const char *plc_address)
{
    char sa1;   // Source Node Address for FINS command. Set local address.
    char da1;   // Destination Node Address for FINS command. Set PLC address.

    sa1 = Utility::GetIpItem(local_address, 3);
    da1 = Utility::GetIpItem((char *)plc_address, 3);

    g_cmdFins[4] = da1;
    g_cmdFins[7] = sa1;

    printf("Set local address to SA1 of FINS = 0x%X (%s)\r\n", sa1, local_address);
    printf("Set PLC address to DA1 of FINS = 0x%X (%s)\r\n", da1, plc_address);

}

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
int OmronPLC::Connect(const char *hostaddr,
                      int port,
                      const char *eth_name,
                      ProtocolTypeTable protocoltype)
{
    char eth_name_temp[16] = ETH_NAME;

    if(eth_name != NULL)
        strncpy(eth_name_temp, eth_name, sizeof(eth_name_temp));

    int ret = -1;

    //
    // Connect to PLC
    //
    if((ret = ConnectWithRetry(hostaddr, port, eth_name_temp, protocoltype)) == -1)
    {
        printf("Connect to PLC error: hostaddr = %s, port = %d, eth_name_temp = %s, protocoltype = %d\r\n",
                hostaddr, port, eth_name_temp, protocoltype);
        return -1;
    }

    //
    // If connected, do initialization
    //

    if(protocoltype == PROTOCOLTYPE_UDP)
        SendFrame = &OmronPLC::SendFrame_UDP;
    else
        SendFrame = &OmronPLC::SendFrame_TCP;

    printf("Connected to PLC at %s:%d, protocoltype = %d\r\n", hostaddr, port, protocoltype);

    char local_address[16];
    EthSocket::GetLocalAddr(eth_name, local_address, sizeof(local_address));

    InitIpAddress(local_address, hostaddr);


    //
    // If protocol type is not UDP, to get data from PLC and set into FINS command
    // Please keep this function at the last function running
    //
    if(protocoltype != PROTOCOLTYPE_UDP)
        SendNodeAddressData();


    return ret;

}


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
int OmronPLC::ReadPoint(MemoryAreaTable memory_area,
                        unsigned int node_addr,
                        char bit_position,
                        unsigned int point_count)
{
    int ret = 0;
    uint single_data_size = GetDataSize_MemoryArea(memory_area);

    if(single_data_size * point_count >= MAX_RESPONSE_FINS_RECEIVE_SIZE)
    {
        DBG("single_data_size(%d) * point_count(%d) is too big to store into array g_respFinsData", single_data_size, point_count);
        return -1;
    }

    SetCmdFins(OMRON_SRC_READ, memory_area, node_addr, bit_position, point_count);

    for(int retryindex = 0;
            retryindex < FINS_RETRY && ((ret = (this->*SendFrame)(NULL, 0, point_count > MAX_DATA_WITHOUT_DELAY)) == -1);
            retryindex++)
    {
        DBG("Read data from PLC fail - Retry(%d)", retryindex);
    }

    return ret;
}


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
int OmronPLC::WritePoint(MemoryAreaTable memory_area,
                         unsigned int node_addr,
                         char bit_position,
                         char *data,
                         unsigned int data_size)
{
    int ret = 0;

    SetCmdFins(OMRON_SRC_WRITE, memory_area, node_addr, bit_position, 1);

    for(int retryindex = 0;
            retryindex < FINS_RETRY && ((ret = (this->*SendFrame)(data, data_size, data_size >= MAX_DATA_WITHOUT_DELAY)) == -1);
            retryindex++)
    {
        DBG("Write data to PLC fail - Retry(%d)", retryindex);
    }

    return ret;
}


/**
 * @brief       Read point from PLC
 *              The received data can be got from GetData_ResponseFins() or GetValue_ResponseFins()
 *
 * @param[in]   memory_area     The point will be select
 * @param[in]   node_addr       The address in the point will be read
 *
 * @return      Return 0 if ok
 */
int OmronPLC::ReadDM(unsigned int node_addr)
{
    return ReadPoint(MEMORYAREA_DM, node_addr, 0, 1);
}


/**
 * @brief       Write DM point
 *
 * @param[in]   node_addr       The address of DM will be written
 * @param[in]   data            The data will be written
 *
 * @return      Return 0 if ok
 */
int OmronPLC::WriteDM(unsigned int node_addr,
                      char *data)
{
    return WritePoint(MEMORYAREA_DM, node_addr, 0, data, 2);
}


/**
 * @brief       Get data size according to memory area type
 *
 * @param[in]   memory_area     Set memory area type
 *
 * @return      Get data size (Unit: bytes)
 */
int OmronPLC::GetDataSize_MemoryArea(MemoryAreaTable memory_area)
{
    int data_size;

    switch(memory_area)
    {
#if 0
    /// MEMORYAREA_CIO_STATUS       = 0x00,
    /// MEMORYAREA_TR_STATUS        = 0x00,
    /// MEMORYAREA_CPU_BUS_STATUS   = 0x00,
    /// MEMORYAREA_AUXILIARY_STATUS = 0x00,
    case 0x00:
    /// MEMORYAREA_TIMER_STATUS     = 0x01,
    case 0x01:
    /// MEMORYAREA_TRANSITIOIN_STATUS= 0x03,
    case 0x03:
    /// MEMORYAREA_STEP_STATUS      = 0x04,
    case 0x04:
    /// MEMORYAREA_FORCED_STATUS    = 0x05,
    case 0x05:
    /// MEMORYAREA_ACTION_STATUS    = 0x1B
    case 0x1B:
//    case 0x40: - unused currently
//    case 0x41: - unused currently
//    case 0x43: - unused currently
//    case 0x44: - unused currently
        data_size = 1;
        break;

    /// MEMORYAREA_CIO              = 0x80,
    /// MEMORYAREA_TR               = 0x80,
    /// MEMORYAREA_CPU_BUS          = 0x80,
    /// MEMORYAREA_AUXILIARY        = 0x80,
    case 0x80:
    /// MEMORYAREA_TIMER_PV         = 0x81,
    case 0x81:
    /// MEMORYAREA_DM               = 0x82,
    case 0x82:
    /// MEMORYAREA_FORCED           = 0x85,
    case 0x85:
    /// MEMORYAREA_DM_EXP1          = 0x90,
    case 0x90:
    /// MEMORYAREA_DM_EXP2          = 0x91,
    case 0x91:
    /// MEMORYAREA_DM_EXP3          = 0x92,
    case 0x92:
    /// MEMORYAREA_DM_EXP4          = 0x93,
    case 0x93:
    /// MEMORYAREA_DM_EXP5          = 0x94,
    case 0x94:
    /// MEMORYAREA_DM_EXP6          = 0x95,
    case 0x95:
    /// MEMORYAREA_DM_EXP7          = 0x96,
    case 0x96:
    /// MEMORYAREA_DM_EXP8          = 0x97,
    case 0x97:
    /// MEMORYAREA_DM_EXP_CURRENT   = 0x98,
    case 0x98:
//    case 0x84: - unused currently
//    case 0x9C: - unused currently
        data_size = 2;
        break;

//    case 0xC0: - unused currently
//    case 0xDD: - unused currently
//        data_size = 4;
//        break;
#else

//        MEMORYAREA_CIO_BIT              = 0x30,     ///< Bit status: CIO area
//        MEMORYAREA_WR_BIT               = 0x31,     ///< Bit status: Work area
//        MEMORYAREA_HR_BIT               = 0x32,     ///< Bit status: Holding relay
//        MEMORYAREA_AR_BIT               = 0x33,     ///< Bit status: Auxiliary area
    case 0x30:
    case 0x31:
    case 0x32:
    case 0x33:
//        MEMORYAREA_CIO_BIT_FS           = 0x70,     ///< Bit with forced status: CIO status
//        MEMORYAREA_WR_BIT_FS            = 0x71,     ///< Bit with forced status: Work area status
//        MEMORYAREA_HR_BIT_FS            = 0x72,     ///< Bit with forced status: Holding relay status
    case 0x70:
    case 0x71:
    case 0x72:
//        MEMORYAREA_TIM                  = 0x09,     ///< Completion Flag: Timer area
//        MEMORYAREA_CNT                  = 0x09,     ///< Completion Flag: Counter area
//        MEMORYAREA_TIM_FS               = 0x49,     ///< Bit with forced status:
//        MEMORYAREA_CNT_FS               = 0x49,     ///< Bit with forced status:
    case 0x09:
    case 0x49:
//        MEMORYAREA_DM_BIT               = 0x02,     ///< Bit status: DM
    case 0x02:
//        MEMORYAREA_TK_BIT               = 0x06,     ///< Bit status: TK
//        MEMORYAREA_TK                   = 0x46      ///< Status: TK
    case 0x06:
    case 0x46:
        data_size = 1;
        break;

//        MEMORYAREA_CIO                  = 0xB0,     ///< Word status: CIO
//        MEMORYAREA_WR                   = 0xB1,     ///< Word status: Work area
//        MEMORYAREA_HR                   = 0xB2,     ///< Word status: Holding relay
//        MEMORYAREA_AR                   = 0xB3,     ///< Word status: Auxiliary area
    case 0xB0:
    case 0xB1:
    case 0xB2:
    case 0xB3:
//        MEMORYAREA_TIM_PV               = 0x89,     ///< PV: Timer area
//        MEMORYAREA_CNT_PV               = 0x89,     ///< PV: Counter area
    case 0x89:
//        MEMORYAREA_DM                   = 0x82,     ///< Word value: DM
    case 0x82:
//        MEMORYAREA_EM_0                 = 0x90,     ///< Word value: EM with bank 0
//        MEMORYAREA_EM_1                 = 0x91,     ///< Word value: EM with bank 1
//        MEMORYAREA_EM_2                 = 0x92,     ///< Word value: EM with bank 2
//        MEMORYAREA_EM_3                 = 0x93,     ///< Word value: EM with bank 3
//        MEMORYAREA_EM_4                 = 0x94,     ///< Word value: EM with bank 4
//        MEMORYAREA_EM_5                 = 0x95,     ///< Word value: EM with bank 5
//        MEMORYAREA_EM_6                 = 0x96,     ///< Word value: EM with bank 6
//        MEMORYAREA_EM_7                 = 0x97      ///< Word value: EM with bank 7
    case 0x90:
    case 0x91:
    case 0x92:
    case 0x93:
    case 0x94:
    case 0x95:
    case 0x96:
    case 0x97:
        data_size = 2;
        break;

//        MEMORYAREA_CIO_FS               = 0xF0,     ///< Bit with forced status: CIO
//        MEMORYAREA_WR_FS                = 0xF1,     ///< Bit with forced status: Work area
//        MEMORYAREA_HR_FS                = 0xF2,     ///< Bit with forced status: Holding relay area
    case 0xF0:
    case 0xF1:
    case 0xF2:
        data_size = 4;
        break;

#endif
    default:
        data_size = -1;
        break;
    }

    return data_size;
}


/**
 * @brief       Get byte data from FINS response
 *              This function will return raw byte data
 *
 * @param[out]  data        Store result data
 * @param[in]   data_len    Data length
 *
 * @return      Return 0 if ok
 */
int OmronPLC::GetData_ResponseFins(char *data,
                                   unsigned int data_len)
{
    if(data_len > sizeof(g_respFinsData))
    {
        printf("data_len is too big = %d\r\n", data_len);
        return -1;
    }

    memcpy(data, g_respFinsData, data_len);
    return 0;
}


/**
 * @brief       Get value from specified data index of FINS response
 *              This function will transfer byte data into value
 *
 * @param[in]   data_index      Data index at FINS response
 * @param[in]   data_len        How many bytes of raw data will be transfer to value
 *
 * @return      Return data value
 */
int OmronPLC::GetValue_ResponseFins(unsigned int data_index,
                                    unsigned int data_len)
{
    int value = 0;

    for(unsigned int current_data_index = data_index*data_len;
                     current_data_index < (data_index+1)*data_len;
                     current_data_index++)
    {
        value = value * 100 + ((g_respFinsData[current_data_index] >> 4) * 10 + (g_respFinsData[current_data_index] & 0x0F));
//        printf("respFinsData[%d] = %d, value = %d\r\n", dataindex, respFinsData[dataindex], value);
    }

    return value;
}


/**
 * @brief       Show responseFins data
 *
 * @return None
 */
void OmronPLC::ShowData_ResponseFins()
{
    printf(" ----- ShowResponseFinsData (%d) ----- \r\n", g_respFinsData_length);
    for(uint data_index = 0; data_index < g_respFinsData_length; data_index++)
    {
        printf("%3d:0x%02X\t", data_index, g_respFinsData[data_index]);
        if((data_index+1) % 4 == 0)
            printf("\r\n");
    }
    printf("\r\n");
}


/**
 * @brief       Read multiple area data
 *
 * @param[in]   area_list       All the data of multiple areas. Data number can more than 128.
 * @param[in]   data_number     Data number. Can more than 128.
 *
 * @return      Return 0 if ok
 */
int OmronPLC::ReadData_MultipleArea(MultipleAreaRead *area_list,
                                    uint data_number)
{
    uint send_data_length;

    for(uint data_index = 0; data_index < data_number; data_index+=128)
    {
        send_data_length = ((data_index + 128) <= data_number) ? 128 : (data_number - data_index);
        SetCmdFins(&area_list[data_index], send_data_length);
        (this->*SendFrame)(NULL, 0, false);
        GetValue_MultipleArea(&area_list[data_index],
                              send_data_length);
    }

    return 0;
}

int OmronPLC::ReadData_MultipleArea_V2(std::vector<MultipleAreaRead> &MultipleAreaReads)
{
    size_t data_number =  MultipleAreaReads.size();
    uint send_data_length;
    //a batch responsive 128 PLC points
    for(uint data_index = 0; data_index < data_number; data_index+=128)
    {
        send_data_length = ((data_index + 128) <= data_number) ? 128 : (data_number - data_index);

        SetCmdFins_V2(MultipleAreaReads, data_index, send_data_length);

        (this->*SendFrame)(NULL, 0, false);

        if(GetValue_MultipleArea_V2(MultipleAreaReads, data_index, send_data_length) == -1)
        {
        	return -1;
        }
    }

    return 0;
}

/**
 * @brief       Get value of multiple area data
 *
 * @param[in]   area_list       All the data of multiple areas
 * @param[in]   data_number     Data number
 *
 * @return      The value of the multiple area data
 */
int OmronPLC::GetValue_MultipleArea(MultipleAreaRead *area_list,
                                    uint data_number)
{
    uint respFinsData_index = 0;
    uint data_size;

    for(uint data_index = 0; data_index < data_number; data_index++)
    {
        if(g_respFinsData[respFinsData_index++] != area_list[data_index].area)
        {
            DBG("Memory area code error!!!(%d/%d) 0x%X -> 0x%X",
                    data_index,
                    data_number,
                    area_list[data_index].area,
                    g_respFinsData[respFinsData_index]);

            return -1;
        }
        data_size = GetDataSize_MemoryArea(area_list[data_index].area);
        area_list[data_index].return_value = TransferDataToValue(&g_respFinsData[respFinsData_index], data_size, area_list[data_index].hex_as_dec);
        respFinsData_index += data_size;
    }

    return 0;
}

int OmronPLC::GetValue_MultipleArea_V2(std::vector<MultipleAreaRead> &MultipleAreaReads, uint index, uint data_number)
{
    uint respFinsData_index = 0;
    uint data_size;

    for(uint data_index = 0; data_index < data_number; data_index++)
    {
        if(g_respFinsData[respFinsData_index++] != MultipleAreaReads[data_index+index].area)
        {
            /*
        	DBG("Memory area code error!!!(%d/%d) 0x%X -> 0x%X",
                    data_index,
                    data_number,
                    area_list[data_index].area,
                    g_respFinsData[respFinsData_index]);
			*/
            return -1;
        }
        data_size = GetDataSize_MemoryArea(MultipleAreaReads[data_index+index].area);
        MultipleAreaReads[data_index+index].return_value = TransferDataToValue(&g_respFinsData[respFinsData_index], data_size, MultipleAreaReads[data_index+index].hex_as_dec);
        respFinsData_index += data_size;
    }

    return 0;
}

/**
 * @brief       Show all the multiple area data
 *
 * @param[in]   area_list       All the data of multiple areas
 * @param[in]   data_number     Data number
 *
 * @return      None
 */
void OmronPLC::ShowData_MultipleArea(MultipleAreaRead *area_list,
                                     uint data_number)
{
    printf("----- ShowMultipleAreaData (%d) -----\r\n", data_number);

    for(uint data_index = 0; data_index < data_number; data_index++)
    {
        printf("area(0x%X)\taddress(%d)\tbit_position(%d)\treturn_value(%d)\r\n",
                area_list[data_index].area,
                area_list[data_index].address,
                area_list[data_index].bit_position,
                area_list[data_index].return_value
                );
    }
}



MemoryAreaTable OmronPLC::MemoryTypeIndex(std::string type)
{
	std::map<std::string, MemoryAreaTable> MemoryAreaTable_Index;
	MemoryAreaTable_Index["DM"] = MEMORYAREA_DM;
	MemoryAreaTable_Index["dm"] = MEMORYAREA_DM;
	MemoryAreaTable_Index["D"] = MEMORYAREA_DM;
	MemoryAreaTable_Index["d"] = MEMORYAREA_DM;

	MemoryAreaTable_Index["EM"] = MEMORYAREA_EM_0;
	MemoryAreaTable_Index["em"] = MEMORYAREA_EM_0;
	MemoryAreaTable_Index["E"] = MEMORYAREA_EM_0;
	MemoryAreaTable_Index["e"] = MEMORYAREA_EM_0;

	MemoryAreaTable_Index["EM_3"] = MEMORYAREA_EM_3;
	MemoryAreaTable_Index["em_3"] = MEMORYAREA_EM_3;
	MemoryAreaTable_Index["EM3"] = MEMORYAREA_EM_3;
	MemoryAreaTable_Index["em3"] = MEMORYAREA_EM_3;

	MemoryAreaTable_Index["DO"] = MEMORYAREA_CIO_BIT;
	MemoryAreaTable_Index["do"] = MEMORYAREA_CIO_BIT;
	MemoryAreaTable_Index["DI"] = MEMORYAREA_CIO_BIT;
	MemoryAreaTable_Index["di"] = MEMORYAREA_CIO_BIT;
	MemoryAreaTable_Index["CIO"] = MEMORYAREA_CIO_BIT;
	MemoryAreaTable_Index["cio"] = MEMORYAREA_CIO_BIT;
	MemoryAreaTable_Index["C"] = MEMORYAREA_CIO_BIT;
	MemoryAreaTable_Index["c"] = MEMORYAREA_CIO_BIT;

	MemoryAreaTable_Index["WR"] = MEMORYAREA_WR_BIT;
	MemoryAreaTable_Index["wr"] = MEMORYAREA_WR_BIT;
	MemoryAreaTable_Index["W"] = MEMORYAREA_WR_BIT;
	MemoryAreaTable_Index["w"] = MEMORYAREA_WR_BIT;

	MemoryAreaTable_Index["ww"] = MEMORYAREA_WR;
	MemoryAreaTable_Index["WW"] = MEMORYAREA_WR;

	MemoryAreaTable_Index["HR"] = MEMORYAREA_HR_BIT;
	MemoryAreaTable_Index["hr"] = MEMORYAREA_HR_BIT;
	MemoryAreaTable_Index["H"] = MEMORYAREA_HR_BIT;
	MemoryAreaTable_Index["h"] = MEMORYAREA_HR_BIT;

	MemoryAreaTable_Index["TIM"] = MEMORYAREA_TIM;
	MemoryAreaTable_Index["tim"] = MEMORYAREA_TIM;
	MemoryAreaTable_Index["T"] = MEMORYAREA_TIM;
	MemoryAreaTable_Index["t"] = MEMORYAREA_TIM;

	return MemoryAreaTable_Index.find(type)->second;
}


/**
 * @brief       Send node address data
 *              Need to be sent once while the first time connecting to PLC
 *
 * @return      Return 0 if ok
 */
int OmronPLC::SendNodeAddressData()
{
    /// NODE ADDRESS DATA SEND buffer
    char cmdNADS[] = {
        0x46, 0x49, 0x4E, 0x53,     // 'F' 'I' 'N' 'S'
        0x00, 0x00, 0x00, 0x0C,     // 12 chars expected
        0x00, 0x00, 0x00, 0x00,     // NADS Command (0 Client to server, 1 server to client)
        0x00, 0x00, 0x00, 0x00,     // Error code (Not used)
        0x00, 0x00, 0x00, 0x00      // Client node address, 0 = auto assigned
    };
    char respNADS[24] = {0};

    // send NADS command
    //
    SendData(cmdNADS, sizeof(cmdNADS));

    // wait for a plc response
    //
    ReceiveData(respNADS, sizeof(respNADS));

    // checks response error
    //
    if (respNADS[15] != 0)
    {
        printf("NASD command error: %d\r\n", respNADS[15]);

        // no more actions
        //
        Close();
        return -1;
    }

    // checking header error
    //
    if (respNADS[8] != 0 || respNADS[9] != 0 || respNADS[10] != 0 || respNADS[11] != 1)
    {
        printf("Error sending NADS command = %d, %d, %d, %d\r\n",
                respNADS[8],
                respNADS[9],
                respNADS[10],
                respNADS[11]);

        // no more actions
        //
        Close();
        return -1;
    }

    // save the client & server node in the FINS command for all next conversations
    //
    g_cmdFins[4] = respNADS[23];
    g_cmdFins[7] = respNADS[19];

    return 0;
}


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
void OmronPLC::SetCmdFins(OmronSRCTable eomron_src,
                          MemoryAreaTable area,
                          unsigned int address,
                          char bit_position,
                          unsigned int count)
{
    // command & subcomand
    //
    g_cmdFins[10] = 0x01;                // MC Main command
    g_cmdFins[11] = eomron_src;        // SC Subcommand

    // memory area
    //
    g_cmdFins[F_PARAM] = area;

    // address
    //
    g_cmdFins[F_PARAM + 1] = (address >> 8) & 0xFF;
    g_cmdFins[F_PARAM + 2] = (address & 0xFF);

    // special flag
    //
    g_cmdFins[F_PARAM + 3] = bit_position;

    // count items
    //
    g_cmdFins[F_PARAM + 4] = (count >> 8) & 0xFF;
    g_cmdFins[F_PARAM + 5] = count & 0xFF;

    // set command lenght (12 + additional params)
    //
    g_cmdFins_Len = 18;
}


/**
 * @brief       Combine the FINS command of multiple memory area
 *
 * @param[in]   area_list       Multiple memory area data
 * @param[in]   area_number     area_list number
 *
 *
 * @return      None
 */
void OmronPLC::SetCmdFins(MultipleAreaRead *area_list,
                          uint area_number)
{
    uint command_start_index = F_PARAM;

    g_cmdFins[10] = 0x01;           // MC Main command
    g_cmdFins[11] = 0x04;           // SC Subcommand - Multiple memory area read

    for(uint data_index = 0; data_index < area_number; data_index++)
    {
        g_cmdFins[command_start_index++] = area_list[data_index].area;                          // Memory area
        g_cmdFins[command_start_index++] = (area_list[data_index].address >> 8) & 0xFF;         // Address - high byte
        g_cmdFins[command_start_index++] = (area_list[data_index].address & 0xFF);              // Address - low byte
        g_cmdFins[command_start_index++] = area_list[data_index].bit_position;                  // Bit position
    }

    g_cmdFins_Len = command_start_index;
}

void OmronPLC::SetCmdFins_V2(std::vector<MultipleAreaRead> &MultipleAreaReads, uint index, uint area_number)
{
    uint command_start_index = F_PARAM;

    g_cmdFins[10] = 0x01;           // MC Main command
    g_cmdFins[11] = 0x04;           // SC Subcommand - Multiple memory area read

    for(uint data_index = 0; data_index < area_number; data_index++)
    {
        g_cmdFins[command_start_index++] = MultipleAreaReads[data_index+index].area;                          // Memory area
        g_cmdFins[command_start_index++] = (MultipleAreaReads[data_index+index].address >> 8) & 0xFF;         // Address - high byte
        g_cmdFins[command_start_index++] = (MultipleAreaReads[data_index+index].address & 0xFF);              // Address - low byte
        g_cmdFins[command_start_index++] = MultipleAreaReads[data_index+index].bit_position;                  // Bit position
    }

    g_cmdFins_Len = command_start_index;
}


/**
 * @brief       Send commands(FS command and FINS command) to PLC via TCP
 *
 * @param[in]   data            Append this data into FINS command. (If necessary)
 * @param[in]   data_size       Data size
 * @param[in]   enable_delay    If read/write data more than 200, enable this delay
 *
 * @return      Return 0 if ok
 */
int OmronPLC::SendFrame_TCP(char *data,
                            unsigned int data_size,
                            bool enable_delay)
{
    char cmdFS[] = {
        0x46, 0x49, 0x4E, 0x53,         // 'F' 'I' 'N' 'S'
        0x00, 0x00, 0x00, 0x00,         // Expected number of chars for response
        0x00, 0x00, 0x00, 0x02,         // Command FS  Sending=2 / Receiving=3
        0x00, 0x00, 0x00, 0x00          // Error code
    };

    char respFS[16];
    unsigned char sid = 0;
    int ret = 0;

    // clear FS response buffer
    //
    memset(respFS, 0, sizeof(respFS));

    // data lenght plus 8 bytes (4 bytes for command & 4 bytes for error)
    //
    int fsLen = g_cmdFins_Len + 8;

    if (data != NULL && data_size)
    {
        fsLen += data_size;
    }

    // set length [6]+[7]
    //
    cmdFS[6] = (fsLen >> 8) & 0xFF;
    cmdFS[7] = fsLen & 0xFF;

    // send frame header
    //
    ret = SendData(cmdFS, sizeof(cmdFS));
    if(ret < 0)
    {
        printf("FS Header failure = %d\r\n", ret);
        return -1;
    }

    // Set SID for received data checking
    sid = rand() % 0x100;
    g_cmdFins[9] = sid;
//    printf("cmdFins[9](SID) = 0x%X\r\n", cmdFins[9]);

    // send FINS command
    //
    ret = SendData(g_cmdFins, g_cmdFins_Len);
    if(ret < 0)
    {
        printf("FINS command failure = %d\r\n", ret);
        return -1;
    }

    // send additional data
    //
    if (data != NULL && data_size)
    {
        ret = SendData(data, data_size);
        if(ret < 0)
        {
            printf("Additional data failure = %d\r\n", ret);
            return -1;
        }
    }

    if(enable_delay)
        Utility::Sleep_us(50E3);

    int retrytimes = 0;
    while(retrytimes++ < FINS_RETRY)
    {

        // frame response
        //
        ret = ReceiveData(respFS, sizeof(respFS));

        if(ret <= 0)
        {
            printf("respFS failure = %d\r\n", ret);
            return -1;
        }
        if(enable_delay)
            Utility::Sleep_us(50E3);

        // check frame error [8]+[9]+[10]+[11]
        //
        if(respFS[8] != 0 || respFS[9] != 0 || respFS[10] != 0 || respFS[11] != 2)
        {
            printf("FRAME SEND error: %d-%d-%d-%d\r\n", respFS[8], respFS[9], respFS[10], respFS[11]);
            return -1;
        }


        // checks response error
        //
        if (respFS[15] != 0)
        {
            printf("Error receving FS command: %d\r\n", respFS[15]);
            return -1;
        }

        // calculate the expedted response lenght
        //
        // 16 bits word ([6] + [7])
        // substract the additional 8 bytes
        //
        int respFins_Len = (respFS[6] << 8) + respFS[7];
        respFins_Len -= 8;


        // fins command response
        //
        char respFins[2048] = {0};

        ReceiveData(respFins, 14);


        if (respFins_Len > 14)
        {
            if(enable_delay)
                Utility::Sleep_us(50E3);

            // fins command response data
            //
            g_respFinsData_length = respFins_Len - 14;
            ReceiveData(g_respFinsData, g_respFinsData_length);
        }

        // check response code
        //
        if (respFins[12] != 0 || respFins[13] != 0)
        {
            printf("Response Code error: (Codi: {%d}  Subcodi: {%d})\r\n", respFins[12], respFins[13]);

            return -1;
        }

        if (respFins[9] != sid)
        {
            printf("SID error: 0x%X -> 0x%X\r\n", sid, respFins[9]);
            if(retrytimes >= FINS_RETRY)
            {
                DBG("SID error and has retried %d times!!!", retrytimes);
                return -1;
            }
            continue;
        }
        else
        {
//            printf("SID check pass(0x%X)\r\n", respFins[9]);
            break;
        }
    }

    return 0;
}


/**
 * @brief       Send commands(FS command and FINS command) to PLC via UDP
 *
 * @param[in]   data            Append this data into FINS command. (If necessary)
 * @param[in]   data_size       Data size
 * @param[in]   enable_delay    If read/write data more than 200, enable this delay
 *
 * @return      Return 0 if ok
 */
int OmronPLC::SendFrame_UDP(char *data,
                            unsigned int data_size,
                            bool enable_delay)
{
    unsigned char sid = 0;
    int ret = 0;

    // Set SID for received data checking
    sid = rand() % 0x100;
    g_cmdFins[9] = sid;
    //    printf("cmdFins[9](SID) = 0x%X\r\n", cmdFins[9]);

    if (data != NULL && data_size)
    {
        memcpy(&g_cmdFins[g_cmdFins_Len], data, data_size);
        g_cmdFins_Len += data_size;
    }

    // send FINS command
    //
    ret = SendData(g_cmdFins, g_cmdFins_Len);
    if(ret < 0)
    {
        printf("FINS command failure = %d\r\n", ret);
        return -1;
    }


    int retrytimes = 0;
    char respFins[2010] = {0};
    int recv_data_num = 0;
    uint respFins_length = 0;
//    int read_fail_count = 0;

    memset(g_respFinsData, 0, sizeof(g_respFinsData));

    while(1)
    {
//        if(enable_delay)
//            Utility::Sleep_us(100E3);

        recv_data_num = ReceiveData(&respFins[respFins_length], sizeof(respFins) - respFins_length);

        if(recv_data_num < 0)
        {
//            read_fail_count++;
            printf("Received all the data from PLC (%d)\r\n", respFins_length);
//            printf("read_fail_count = %d\r\n", read_fail_count);
            break;
//            continue;
//            return -1;
        }

        respFins_length += recv_data_num;
        if(respFins_length < 14)
        {
            printf("Received data length error (%d). Keep receiving...\r\n", recv_data_num);
            continue;
        }

        // check response code
        //
        if (respFins[12] != 0 || respFins[13] != 0)
        {
            printf("Response Code error: (Codi: {%d}  Subcodi: {%d})\r\n", respFins[12], respFins[13]);

            return -1;
        }

        if (respFins[9] != sid)
        {
            printf("SID error: 0x%X -> 0x%X\r\n", sid, respFins[9]);
            if(retrytimes >= FINS_RETRY)
            {
                DBG("SID error and has retried %d times!!!", retrytimes);
                return -1;
            }
            continue;
        }
        else
        {
//            printf("SID check pass(0x%X)\r\n", respFins[9]);
//            printf("Exit RecvData function (%d)\r\n", respFins_length);
            g_respFinsData_length = respFins_length - 14;
            memcpy(g_respFinsData, &respFins[14], g_respFinsData_length);

            break;
        }
    }

    return 0;
}


/**
 * @brief       Disconnect to PLC
 *
 * @return      None
 */
void OmronPLC::Shutdown()
{
    Close();
}


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
uint OmronPLC::TransferDataToValue(char *data,
                                   uint data_size,
                                   bool hex_as_dec)
{
    int value = 0;

    for(unsigned int data_index = 0;
                     data_index < data_size;
                     data_index++)
    {
        if(hex_as_dec)
            value = value * 100 + ((data[data_index] >> 4) * 10 + (data[data_index] & 0x0F));
        else
            value = (value << 8) + data[data_index];
    }

    return value;
}
