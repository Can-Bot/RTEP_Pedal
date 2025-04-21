#include "MCP23017Driver.h"

// Initialise I2C and set all the pins to INPUT_PULLUP except for GPA7 & GPB7
bool MCP23017Driver::begin()
{
    ioctl(_file_i2c, I2C_SLAVE, MCP23017_I2C_ADDR);

    // set the control register (mirror interrupt, int active-low, no sequential operation)
    this->i2c_write_8(MCP23x17_IOCR, 0x60);

    // set all as inputs except the forbidden ones
    this->i2c_write_8(MCP23x17_DDR_A, 0x7F);
    this->i2c_write_8(MCP23x17_DDR_B, 0x7F);

    // set the polarity to invert on all
    this->i2c_write_8(MCP23x17_POL_A, 0xFF);
    this->i2c_write_8(MCP23x17_POL_B, 0xFF);

    // set the interrupt on change to all buttons and A pins (see schematic for reference)
    this->enableAllInterrupt(true);

    // set the interrupt trigger comparison to -> previous state
    this->i2c_write_8(MCP23x17_INTCON_A, 0x00);
    this->i2c_write_8(MCP23x17_INTCON_B, 0x00);

    // read the interrupt capture to clear the interrupt
    this->i2c_read_8(MCP23x17_INTCAP_A);
    this->i2c_read_8(MCP23x17_INTCAP_B);

    return true;
}

bool MCP23017Driver::enableAllInterrupt(bool state)
{
    if (state)
    {
        this->i2c_write_8(MCP23x17_GPINTEN_A, (ENC_A[0] + ENC_A[1] + ENC_A[2]));
        this->i2c_write_8(MCP23x17_GPINTEN_B, (ENC_PUSH[0] + ENC_PUSH[1] + ENC_PUSH[2]));
        this->i2c_read_8(MCP23x17_INTCAP_A);
        this->i2c_read_8(MCP23x17_INTCAP_B);
    }
    else
    {
        this->i2c_write_8(MCP23x17_GPINTEN_A, 0x00);
        this->i2c_write_8(MCP23x17_GPINTEN_B, 0x00);
    }

    return true;
}

// Set all pins of one port to either Input or Output depending on the bit mask
bool MCP23017Driver::pinMode(bool port, uint8_t bitMask)
{
    this->i2c_write_8(port == 0 ? MCP23x17_PUR_A : MCP23x17_PUR_B, bitMask);
    this->i2c_write_8(port == 0 ? MCP23x17_DDR_A : MCP23x17_DDR_B, bitMask);
    this->i2c_write_8(port == 0 ? MCP23x17_POL_A : MCP23x17_POL_B, 0x00);

    return true;
}

// Read the state of all I/O of a port when requested
uint8_t MCP23017Driver::readPort(bool port)
{
    return this->i2c_read_8(port == 0 ? MCP23x17_GPIO_A : MCP23x17_GPIO_B);
}

// Enable the interrupt trigger capability for specific I/Os
bool MCP23017Driver::enableInterrupts(bool port, uint8_t bitMask, uint8_t intMode)
{
    uint8_t intcon = 0, defval = 0;

    if (intMode == INT_CHANGE)
    {
        intcon = ~bitMask;
    }
    else
    {
        if (intMode == INT_RISING)
        {
            intcon = bitMask;
            defval = ~bitMask;
        }
        else if (intMode == INT_FALLING)
        {
            intcon = bitMask;
            defval = bitMask;
        }
        this->i2c_write_8(port == 0 ? MCP23x17_DEFVAL_A : MCP23x17_DEFVAL_B, defval);
    }

    this->i2c_write_8(port == 0 ? MCP23x17_INTCON_A : MCP23x17_INTCON_B, intcon);

    this->i2c_write_8(port == 0 ? MCP23x17_GPINTEN_A : MCP23x17_GPINTEN_B, bitMask);
    return true;
}

// Get an 8-bit flag cluster of which pin triggered the interrupt
uint8_t MCP23017Driver::getInterruptFlag(bool port)
{
    return i2c_read_8(port == 0 ? MCP23x17_INTF_A : MCP23x17_INTF_B);
}

// Get an 8-bit pin state cluster of the state of the pins at the interrupt
uint8_t MCP23017Driver::getInterruptRegister(bool port)
{
    return i2c_read_8(port == 0 ? MCP23x17_INTCAP_A : MCP23x17_INTCAP_A);
}

// Make interrupts A & B work for any input or just the respective port's
bool MCP23017Driver::mirrorInterrupts(bool state)
{
    uint8_t reg = i2c_read_8(MCP23x17_IOCR);

    if (state)
    {
        reg |= MCP23x17_IOCR_MIRROR;
    }
    else
    {
        reg &= ~MCP23x17_IOCR_MIRROR;
    }

    i2c_write_8(MCP23x17_IOCR, reg);

    return true;
}

// Write a register with given data over I2C
bool MCP23017Driver::i2c_write_8(uint8_t reg, uint8_t data)
{
    unsigned char buffer[2] = {reg, data};
    if (write(_file_i2c, buffer, 2) != 2)
    {
        std::cerr << "Failed to write over I2C" << std::endl;
        return false;
    }

    return true;
}

// Read an 8-bit register over I2C
uint8_t MCP23017Driver::i2c_read_8(uint8_t reg)
{
    uint8_t buffer[1] = {reg};

    if (write(_file_i2c, buffer, 1) != 1)
    {
        std::cerr << "Failed to write over I2C" << std::endl;
        return 0x00;
    }

    if (read(_file_i2c, buffer, 1) != 1)
    {
        std::cerr << "Failed to read over I2C" << std::endl;
        return 0x00;
    }

    return buffer[0];
}