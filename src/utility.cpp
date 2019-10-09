/**
 * @file        utility.cpp
 *
 * @author      Matic Chiu <matic.chiu@cht-pt.com.tw>
 *
 * @date        2017/12/06
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>     // for usleep()
#include <sys/time.h>
#include <math.h>



#include <iostream>
//POSIX Library
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <signal.h>
#include <getopt.h>
#include <sstream>
#include <fstream>

#include "utility.h"



// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     DEFINITION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

#define ADC_RESOLUTION 		4095
#define SERIAL_RESISTANCE	10E3
#define NOMINAL_RESISTANCE	10E3
#define BETA_COEF			3950
#define TEMPERATURE_NOMINAL	25
// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     STATIC VARIABLE INITIALIZATION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

timeval Utility::start_time_;

// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     IMPLEMENTATION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief		Get temperature via ADC value
 *
 * @param[in]	adc_value		ADC value from thermo-sensor
 *
 * @return		Return value = temperature(degree C) * 100
 */
int Utility::GetTemperature(unsigned int adc_value)
{

	if(!adc_value)
		return -65535;

	#ifdef VERBOSE_SENSOR_ENABLED
	Serial.print("adc_value analog reading ");
	Serial.println(adc_value);
	#endif

	// convert the value to resistance
	adc_value = ADC_RESOLUTION / adc_value - 1;
	adc_value = SERIAL_RESISTANCE * adc_value;

	#ifdef VERBOSE_SENSOR_ENABLED
	Serial.print("Thermistor resistance ");
	Serial.println(adc_value);
	#endif

	float steinhart;
	steinhart = adc_value / NOMINAL_RESISTANCE;     // (R/Ro)
	#ifdef PANSTAMP_NRG
	steinhart = logf(steinhart);                 // ln(R/Ro)
	#else
	steinhart = log(steinhart);                  // ln(R/Ro)
	#endif
	steinhart /= BETA_COEF;                   // 1/B * ln(R/Ro)
	steinhart += 1.0 / (TEMPERATURE_NOMINAL + 273.15); // + (1/To)
	steinhart = 1.0 / steinhart;                 // Invert
	steinhart -= 273.15;                         // convert to C

	#ifdef VERBOSE_SENSOR_ENABLED
	Serial.print("Temperature ");
	Serial.print(steinhart);
	Serial.println(" *C");
	#endif

	return (int)(steinhart * 10);
}


/**
 * @brief       Print hex data
 *
 * @param[in]   data_name       Data name which will be printed
 * @param[in]   data            Data which will be printed
 * @param[in]   data_length     Data length
 *
 * @return      None
 */
void Utility::ShowHexData(const char *data_name,
                          char *data,
                          int data_length)
{
    int data_show_length = data_length;


    if(data_show_length > 50)
        data_show_length = 50;

    printf("%s (%d):\r\n", data_name, data_length);
    for(int dataIdx = 0;
            dataIdx < data_show_length;
            dataIdx++)
    {
        printf("%02d : 0x%02X\t", dataIdx, data[dataIdx]);
        if(dataIdx % 4 == 3 && dataIdx != data_show_length - 1)
            printf("\r\n");
    }
    printf("\r\n");
}

uint Utility::HexStringToValue(char *data,
                               uint data_length)
{
//    uint data_value = 0;
//
//    for(uint data_index = 0; data_index < data_length; data_index++)
//    {
//        data_value = data_value * 16 +
//    }

    return strtol(data, NULL, 16);
}


/**
 * @brief       Get the specified item of IP address
 *
 * @param[in]   ipaddr          IP address
 * @param[in]   ip_item_index   Item index which the data will be returned
 *
 * @return      Return the specified item IP address
 */
int Utility::GetIpItem(char *ipaddr,
                       unsigned int ip_item_index)
{
    if(ip_item_index >= 4 || strlen(ipaddr) > 15)
    {
        printf("Utl::GetIpItem(%s, %d): Please check input parameters\r\n", ipaddr, ip_item_index);
        return -1;
    }

#if 1    // Easy way to get value

    int ip_temp[4];

    sscanf(ipaddr, "%d.%d.%d.%d", &ip_temp[0], &ip_temp[1], &ip_temp[2], &ip_temp[3]);
//    printf("%s(%d.%d.%d.%d)\r\n", ipaddr, ip_temp[0], ip_temp[1], ip_temp[2], ip_temp[3]);

//    DBG("(%s, %d) is %d\r\n", ipaddr, ip_item_index, ip_temp[ip_item_index]);

    return ip_temp[ip_item_index];

#else
    if(ip_item_index >= 4 || strlen(ipaddr) > 15)
    {
        printf("Utl::GetIpItem(%s, %d): Please check input parameters\r\n", ipaddr, ip_item_index);
        return -1;
    }

    char ipaddrtemp[16];

    strncpy(ipaddrtemp, ipaddr, sizeof(ipaddrtemp));

    char *ip_start = ipaddrtemp;
    char *ip_end = strchr(ipaddrtemp, '.');


    for(int ipidx = 0; ipidx < ip_item_index && ip_end != NULL; ipidx++)
    {
        ip_start = ip_end;
        ip_end = strchr(ipaddr, '.');
    }

    if(ip_end)
        *ip_end = '\0';

    int ret; // = atoi(ip_start);

    sscanf(ip_start, "%d", &ret);

    printf("Utl::GetIpItem: (%s, %d) is %d\r\n", ipaddr, ip_item_index, ret);

    return ret;

#endif
}


/**
 * @brief       sleep in micro-seconds
 *
 * @param[in]   usec        Micro-seconds
 *
 * @return      None
 */
void Utility::Sleep_us(unsigned int usec)
{
//    DBG("usec = %d\r\n", usec);

    if(usec >= 1E6)
        usleep(1E6);
    else
        usleep(usec);
}


/**
 * @brief       Start to record time
 *
 * @return      None
 */
void Utility::StartTime()
{
//    start_time_ = GetTickCount() ;
    gettimeofday(&start_time_,0);
}


/**
 * @brief       Get time difference from starting
 *
 * @return      Return time difference (Unit: Seconds)
 */
double Utility::GetTime()
{
    static timeval current_time;

    gettimeofday(&current_time,0);

    double timeuse = 1000000*(current_time.tv_sec - start_time_.tv_sec) + current_time.tv_usec - start_time_.tv_usec;

    timeuse /=1E6;

    return timeuse;
}


unsigned int Utility::GetCRC16(unsigned char *data, int data_length)
{
    unsigned short crc = 0xFFFF;
    int i,j;
    unsigned char LSB;
    for (i = 0; i < data_length; i++)
    {
        crc ^= data[i];
        for (j = 0; j < 8; j++)
        {
            LSB = crc & 1;
            crc = crc >> 1;
            if (LSB)
            {
                crc ^= 0xA001;
            }
        }
    }

    return ((crc & 0xFF00) >> 8)|((crc & 0x0FF) << 8 );
}

unsigned int Utility::GetCRC32(unsigned char *data, int data_length)
{
    uint c = 0xFFFFFFFF;     // initial ^ 0xFFFFFFFF;
    static uint table[256];
    static bool table_done = false;
    int i;

    if(table_done == false)
    {
        GenerateCRC32Table(table);
        table_done = true;
    }

    for (i = 0; i < data_length; i++)
    {
        c = table[(c ^ data[i]) & 0xFF] ^ (c >> 8);
    }
    return c ^ 0xFFFFFFFF;
}

int Utility::file_exist(const char *file_path)
{
  struct stat buffer;
  return (stat(file_path, &buffer) == 0);
}

std::string Utility::IntToString(int value)
{
    std::ostringstream temp;
    temp << value;
    return temp.str();
}

std::string Utility::FloatToString(float value)
{
    std::ostringstream temp;
    temp << value;
    return temp.str();
}

int Utility::FIFO(const char *pipe)
{
	int pipe_fd = -1;
	if(access(pipe, F_OK) == -1)
	{
		if(mkfifo(pipe, 0777) != 0)
		{
			printf("Do not access %s. But could not create FIFO. \n", pipe);
			exit(EXIT_FAILURE);
		}
		else
		{
			printf("Do not access %s. Create FIFO. \n", pipe);
		}
	}
	else
	{
		printf("FIFO %s is exist. \n", pipe);
	}

	pipe_fd = open(pipe, O_RDWR|O_NONBLOCK);
	if(pipe_fd == -1)
	{
		printf("Could not open FIFO %s\n", pipe);
	}
	printf("Process %d create named pipe result: %d \n", getpid(), pipe_fd);
	return pipe_fd;
}


std::string Utility::NowTime(bool precision)
{
    time_t now = time(NULL);
    struct tm tstruct;
    char buf[40];
    tstruct = *localtime(&now);
    //strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tstruct);
    //strftime(buf, sizeof(buf), "%Y%m%d", &tstruct);
    precision?strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%s", &tstruct) : strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tstruct);
    return buf;
}

int Utility::WriteLog(std::string path, std::string message)
{
	FILE* log_stream;
	try
	{
		log_stream = fopen(path.c_str(), "a");
		//DEBUG_PRINT("%s\n", message.c_str());
	}
	catch(...)
	{
		DBG("Can not open log stream \n");
		return 1;
	}
	fprintf(log_stream, "[%s] %s \n", NowTime(false).c_str(), message.c_str());
	fflush(log_stream);
	fdatasync(fileno(log_stream));
	fclose(log_stream);
	return 0;
}

int Utility::CheckFileSize(std::string file_header, std::ofstream* output)
{
	std::ifstream file((file_header+".csv").c_str(), std::ifstream::in | std::ifstream::binary);
    if(!file.is_open())
    {
    	DBG("No file: %s \n",(file_header+".csv").c_str());
    	return -1;
    }
    file.seekg(0, std::ios::end);
    int file_size = file.tellg();
    file.close();
	DBG("%s size is %d \n",(file_header+".csv").c_str(), file_size);
	if(file_size > max_file_size) //1MB
	{
		//start backup file
		//FTPBackup FTPBackup;

		//compression file
		//FTPBackup.Compresison(file_header);
		//upload file
		std::ifstream in ((file_header+".csv").c_str());
		std::ofstream out (("/mnt/data/auto_cg/adc/"+file_header+"/"+Utility::NowTime(true)+".csv").c_str());
		out << in.rdbuf();
		out.close();
		in.close();
		//remove file
		Utility::RemoveFile(file_header+".csv");
		//create file
		//Utility::CreateFile(file_name, output);
		return 1;
	}
	return 0;
}

int Utility::RemoveFile(std::string file_name)
{
	if( remove( file_name.c_str() ) != 0 )
	{
		DBG( "Error deleting file \n" );
		return -1;
	}
	else
	{
		DBG( "File successfully deleted \n" );
	}
	return 0;
}

int Utility::CreateFile(std::string file_name, std::ofstream* output)
{
	output = new std::ofstream(file_name.c_str(), std::ofstream::app);
	return 0;
}

template <typename T>
bool Utility::fromHex(const std::string& hexValue, T& result)
{
    std::stringstream ss;
    ss << std::hex << hexValue;
    ss >> result;

    return !ss.fail();
}

void Utility::GenerateCRC32Table(uint *table)
{
    uint polynomial = 0xEDB88320;
    uint c;

    for (uint i = 0; i < 256; i++)
    {
        c = i;
        for (size_t j = 0; j < 8; j++)
        {
            if (c & 1) {
                c = polynomial ^ (c >> 1);
            }
            else {
                c >>= 1;
            }
        }
        table[i] = c;
    }
}
