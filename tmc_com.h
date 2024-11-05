//
// Created by patrik on 11/5/24.
//

#ifndef AVCR_SPECTRAL_FW_TMC_COM_H
#define AVCR_SPECTRAL_FW_TMC_COM_H

#include "pico/stdlib.h"  // for types


// TMC2209 Registers
#define TMC2209_WRITE 0x05
#define TMC2209_READ  0x06
#define TMC2209_GCONF 0x00
#define TMC2209_CHOPCONF 0x6C


class TmcComm
{
private:
    uint8_t rxPin;
    uint8_t txPin;
    uart_inst_t *uart;
    uint32_t baudRate;

public:
    TmcComm();
    uint8_t readUart(uint8_t *buf, uint8_t len);
    uint8_t writeUart(const uint8_t *data, uint8_t len);

    uint32_t readRegister(uint8_t motorId, uint8_t registerAddress, uint8_t *buf);
    void writeRegister(uint8_t motorId, uint8_t registerAddress, uint32_t value);

private:
    static void calcCRC(uint8_t *datagram, uint8_t datagramLength);
};


#endif //AVCR_SPECTRAL_FW_TMC_COM_H
