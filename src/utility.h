/**
 * @file        utility.h
 *
 * @author      Matic Chiu <matic.chiu@cht-pt.com.tw>
 *
 * @date        2017/12/06
 */


#ifndef SRC_UTILITY_H_
#define SRC_UTILITY_H_

//#include <sys/time.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <ctime>
#include <cmath>
#include <cerrno>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
#include <map>
#include <sstream>
#include <fstream>
#include <sys/time.h>
using namespace std;
struct timeval;

// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     DEFINITION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

/// Print message with function name and line number
//#define DBG(msg, arg...) printf("%s:%s(%d): " msg "\r\n",  __FILE__, __FUNCTION__, __LINE__, ##arg)
#define DBG(msg, arg...) printf("%s(%d): " msg "\r\n",  __FUNCTION__, __LINE__, ##arg)

#define DEBUG_PRINT(fmt, args...) fprintf(stderr, "%d:%s(): " fmt, __LINE__, __func__, ##args)

#define LOG "elf_log.txt"

#define max_file_size 1048576

typedef enum
{
    Debug = 1,
    Info  = 2,        ///< Must be 0x01 for read
	Warning = 3,        ///< Must be 0x02 for write
	Error = 4,
	Fatal = 5
}LogLevel;

// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     CLASS
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief       Utility. Collect general tools and can be used easily and everywhere.
 */
class Utility {
public:

	/**
	 * @brief		Get temperature via ADC value
	 *
	 * @param[in]	adc_value		ADC value from thermo-sensor
	 *
	 * @return		Return value = real temperature(degree C) * 10
	 */
	static int GetTemperature(unsigned int adc_value);

    /**
     * @brief       Print hex data
     *
     * @param[in]   data_name       Data name which will be printed
     * @param[in]   data            Data which will be printed
     * @param[in]   data_length     Data length
     *
     * @return      None
     */
    static void ShowHexData(const char *data_name,
                            char *data,
                            int data_length);


    static uint HexStringToValue(char *data,
                                 uint data_length);

    /**
     * @brief       Get the specified item of IP address
     *
     * @param[in]   ipaddr          IP address
     * @param[in]   ip_item_index   Item index which the data will be returned
     *
     * @return      Return the specified item IP address
     */
    static int GetIpItem(char *ipaddr,
                         unsigned int ip_item_index);


    /**
     * @brief       sleep in micro-seconds
     *
     * @param[in]   usec        Micro-seconds
     *
     * @return      None
     */
    static void Sleep_us(unsigned int usec);


    /**
     * @brief       Start to record time
     *
     * @return      None
     */
    static void StartTime();


    /**
     * @brief       Get time difference from starting
     *
     * @return      Return time difference (Unit: Seconds)
     */
    static double GetTime();

    static unsigned int GetCRC16(unsigned char *data, int data_length);

    static unsigned int GetCRC32(unsigned char *data, int data_length);

    /**
     * @brief       Check file exist or not
     *
     * @param[in]   file_path
     *
     * @return      Return 1 if file exist
     */
    static int file_exist(const char *file_path);


    /**
     * @brief       Convert int to string
     *
     * @param[in]   int
     *
     * @return      Return string
     */
    static std::string IntToString(int value);


    /**
     * @brief       Convert float to string
     *
     * @param[in]   float
     *
     * @return      Return string
     */
    static std::string FloatToString(float value);


    /**
     * @brief       Open new named pipe
     *
     * @param[in]   pipe     path of named pipe
     *
     * @return      Return new named pipe file descriptor
     */
    static int FIFO(const char *pipe);


    /**
     * @brief       Get now time stamp
     *
     * @param[in]   precision
     *
     * @return      Return now time stamp of string
     */
    static std::string NowTime(bool precision);

    /**
     * @brief       Write log to LOG stream
     *
     * @param[in]   level
     * @param[in]   message
     *
     * @return      Return 0 if write and sync success
     */
    static int WriteLog(std::string path, std::string message);


    /**
     * @brief       Check file size
     *
     * @param[in]   file name
     * @param[in]   create new ofstream
     *
     * @return
     */
    static int CheckFileSize(std::string file_header, std::ofstream* output);

    static int RemoveFile(std::string file_name);

    static int CreateFile(std::string file_name, std::ofstream* output);

    template <typename T>
    static bool fromHex(const std::string& hexValue, T& result);


private:

    static void GenerateCRC32Table(uint *table);

    /// To record starting time
    static timeval start_time_;

};



#endif /* SRC_UTILITY_H_ */
