/**
 * @file        gpio.h
 *
 * @author      Matic Chiu <matic.chiu@cht-pt.com.tw>
 *
 * @date        2018/03/31
 */

#ifndef __DEV_GPIO_H__
#define __DEV_GPIO_H__

#include "../def.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     DEFINITION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------



// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     DATA TYPE DEFINITION
//
// ----------------------------------------------------------------------------------------------------------------------------------------------



// ----------------------------------------------------------------------------------------------------------------------------------------------
//
//                                                                     CLASS
//
// ----------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief    GPIO device
 */
class Gpio{
public:

    /**
     * @brief       Constructor of GPIO
     */
    Gpio();

    /**
     * @brief       Constructor of GPIO with initial parameters
     *
     * @param[in]   dev_path        GPIO device path in linux
     */
    Gpio(unsigned int gpio_num,
            GpioDir gpio_dir,
            GpioLevel gpio_level = GPIO_LEVEL_LOW);


    /**
     * @brief        Destructor of GPIO
     */
    ~Gpio();

    /**
     * @brief       GPIO initialization
     *
     * @param[in]   gpio_num        GPIO number
     * @param[in]   gpio_dir        GPIO direction: IN/OUT
     * @param[in]   gpio_level      GPIO level:     HIGH/LOW
     */
    void Init(unsigned int gpio_num,
              GpioDir gpio_dir,
              GpioLevel gpio_level);

    /**
     * @brief       Set level of GPIO
     *
     * @param[in]   gpio_level      GPIO level
     */
    void SetLevel(GpioLevel gpio_level);

private:

    /**
     * @brief       Set export of GPIO
     *
     * @param[in]   gpio_num        GPIO number
     */
    void SetExport(unsigned int gpio_num);


    /**
     * @brief       Set direction of GPIO
     *
     * @param[in]   gpio_dir        GPIO direction
     */
    void SetDir(GpioDir gpio_dir);


    /// GPIO device path
    char device_path_[32];

    /// UART header
    int gpio_fd_;

};


#endif /* __DEV_GPIO_H__ */
