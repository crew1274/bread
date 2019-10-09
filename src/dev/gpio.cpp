/**
 * @file        gpio.cpp
 *
 * @author      Matic Chiu <matic.chiu@cht-pt.com.tw>
 *
 * @date        2018/03/31
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "gpio.h"


// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     DEFINITION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------



// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     IMPLEMENTATION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief       Constructor of GPIO
 */
Gpio::Gpio()
{

}

/**
 * @brief       Constructor of GPIO with initial parameters
 *
 * @param[in]   dev_path        GPIO device path in linux
 */
Gpio::Gpio(unsigned int gpio_num,
           GpioDir gpio_dir,
           GpioLevel gpio_level)
{
    Init(gpio_num,
         gpio_dir,
         gpio_level);
}

/**
 * @brief       GPIO initialization
 *
 * @param[in]   gpio_num        GPIO number
 * @param[in]   gpio_dir        GPIO direction: IN/OUT
 * @param[in]   gpio_level      GPIO level:     HIGH/LOW
 */
void Gpio::Init(unsigned int gpio_num,
                GpioDir gpio_dir,
                GpioLevel gpio_level)
{
    // Save GPIO path
    sprintf(device_path_, "/sys/class/gpio/gpio%d/", gpio_num);

    // Set export of GPIO
    SetExport(gpio_num);

    // Set direction of GPIO
    SetDir(gpio_dir);

    // Set level of GPIO
    if(gpio_dir == GPIO_DIR_OUTPUT)
        SetLevel(gpio_level);
}

/**
 * @brief        Destructor of GPIO
 */
Gpio::~Gpio()
{
    close(gpio_fd_);
}


/**
 * @brief       Set level of GPIO
 *
 * @param[in]   gpio_level      GPIO level
 */
void Gpio::SetLevel(GpioLevel gpio_level)
{
    char cmd[64];

    sprintf(cmd, "echo %d > %s/value", gpio_level, device_path_);
    system(cmd);
}


/**
 * @brief       Set export of GPIO
 *
 * @param[in]   gpio_num        GPIO number
 */
void Gpio::SetExport(unsigned int gpio_num)
{
    char cmd[64];

    sprintf(cmd, "echo %d > /sys/class/gpio/export", gpio_num);
    system(cmd);
}


/**
 * @brief       Set direction of GPIO
 *
 * @param[in]   gpio_dir        GPIO direction
 */
void Gpio::SetDir(GpioDir gpio_dir)
{
    char cmd[64];

    sprintf(cmd, "echo %s > %s/direction", gpio_dir == GPIO_DIR_INPUT ? "in" : "out", device_path_);
    system(cmd);
}


