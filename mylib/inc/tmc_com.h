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
#define TMC2209_TSTEP 0x12
#define TMC2209_TPWMTHRS 0x13
#define TMC2209_TCOOLTHRS 0x14
#define TMC2209_SGTHRS 0x40
#define TMC2209_SG_RESULT 0x41
#define TMC2209_VACTUAL 0x22
#define TMC2209_IFCNT 0x02


class TmcUart
{
private:
    const uint8_t rxPin;
    const uint8_t txPin;
    uart_inst_t *uart;
    const uint32_t baudRate;

public:
    TmcUart();
    void flush() const;
    uint8_t readUart(uint8_t *buf, uint8_t len) const;
    uint8_t writeUart(const uint8_t *data, uint8_t len) const;

    uint32_t readRegister(uint8_t motorId, uint8_t registerAddress) const;
    void writeRegister(uint8_t motorId, uint8_t registerAddress, uint32_t value) const;

private:
    static void calcCRC(uint8_t *datagram, uint8_t datagramLength);
};


class TmcComm
{
private:
    const TmcUart& tmcUart;
    const uint8_t motorId;

public:
    TmcComm(TmcUart& tmcUart, uint8_t motorId)
        : tmcUart(tmcUart),
          motorId(motorId)
    {}

    void defaultInit() const;
    void setStallGuardThreshold(uint8_t sgthrs) const;
    uint32_t getStallGuardResult() const;
    uint32_t getTstep() const;
    void setVactual(uint32_t vactual) const;
    void setCoolStepThreshold(uint32_t tcoolthrs) const;
};

#endif //AVCR_SPECTRAL_FW_TMC_COM_H
