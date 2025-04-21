#ifndef MCP23017DRIVER_H
#define MCP23017DRIVER_H
#include <cstdint>

#include "MCP23017Registers.h"
#include <iostream>
#include <unistd.h>        //Needed for I2C port
#include <fcntl.h>         //Needed for I2C port
#include <sys/ioctl.h>     //Needed for I2C port
#include <linux/i2c-dev.h> //Needed for I2C port

#define MCP23017_I2C_ADDR 0x20
static int _file_i2c = open("/dev/i2c-1", O_RDWR);

/**
 * @class MCP23017Driver
 * @brief Driver for the MCP23017 I2C 16-bit I/O expander
 *
 * NB  - The new MCP23017 does not support inputs on GPIOA7 and GPIOB7, which really makes it
 * a 14-bit input expander (16-bit with outputs only).
 *
 * NB2 - This driver is tailored to input-only applications,
 * though implementing writes for outputs shouldn't be too bad.
 */
class MCP23017Driver
{
public:
    /**
     * @brief Begins the I2C communication and sets the I/Os.
     */
    bool begin();

    /**
     * @brief sets the Input or Output mode of a whole port.
     * @param port selects the port A or B (0 or 1).
     * @param bitMask sets the state of each input (0-out ; 1-in).
     */
    bool pinMode(bool port, uint8_t bitMask);

    /**
     * @brief reads the Input or Output state of a whole port.
     * @param port selects the port A or B (0 or 1).
     */
    uint8_t readPort(bool port);

    /**
     * @brief sets the Input or Output interrupt trigger of a whole port.
     * @param port selects the port A or B (0 or 1).
     * @param bitMask sets the state of each input (0-off ; 1-on).
     * @param intMode Falling, Rising, or Change.
     */
    bool enableInterrupts(bool port, uint8_t bitMask, uint8_t intMode);

    /**
     * @brief returns a 8-bit flag cluster for which I/O triggered the interrupt.
     * @param port selects the port A or B (0 or 1).
     */
    uint8_t getInterruptFlag(bool port);

    /**
     * @brief returns a 8-bit cluster for the state of each I/O.
     * @param port selects the port A or B (0 or 1).
     */
    uint8_t getInterruptRegister(bool port);

    /**
     * @brief sets interrupt A & B to either work for all I/O or just their port.
     * @param state selects whether the pins mirror or not.
     */
    bool mirrorInterrupts(bool state);

    /**
     * @brief sets interrupt A & B to either work for all I/O or nothing.
     * @param state selects whether the pins trigger the int or not.
     */
    bool enableAllInterrupt(bool state);

    /**
     * @brief I2C write command to setup or read the chip.
     * @param reg selects the register.
     * @param data data to be sent to the register.
     */
    bool i2c_write_8(uint8_t reg, uint8_t data);

    /**
     * @brief I2C read command to read the chip's registers.
     * @param reg selects the register.
     */
    uint8_t i2c_read_8(uint8_t reg);

    static const uint8_t INT_RISING = 0;
    static const uint8_t INT_FALLING = 1;
    static const uint8_t INT_CHANGE = 2;

    static constexpr uint8_t ENC_NUM[3] = {0, 1, 2};
    static constexpr uint8_t ENC_PUSH[3] = {1, 2, 4};
    static constexpr uint8_t ENC_A[3] = {1, 4, 16};
    static constexpr uint8_t ENC_B[3] = {2, 8, 32};
};

#endif // MCP23017DRIVER_H