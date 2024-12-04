
#include <cstdio>
#include "hardware/uart.h"

#include "tmc_com.h"


TmcUart::TmcUart()
        : rxPin(1),
          txPin(0),
          uart(uart0),
          baudRate(115200)
{
    uart_init(this->uart, this->baudRate);
    gpio_set_function(this->rxPin, UART_FUNCSEL_NUM(this->uart, this->rxPin));
    gpio_set_function(this->txPin, UART_FUNCSEL_NUM(this->uart, this->txPin));

    // flush the uart
    this->flush();
}

void TmcUart::flush() const
{
    while (uart_is_readable(this->uart)) {
        uart_getc(this->uart);
    }
}

uint8_t TmcUart::readUart(uint8_t *buf, uint8_t len) const
{
    uint8_t count = 0;
    while (uart_is_readable(this->uart) && count < len) {
        buf[count] = uart_getc(this->uart);
        count++;
    }
    return count;
}

uint8_t TmcUart::writeUart(const uint8_t *data, uint8_t len) const
{
    uint8_t count = 0;
    while (uart_is_writable(this->uart) && count < len) {
        uart_write_blocking(this->uart, &data[count], 1);
        count++;
    }

    // read back check (RX TX oneline)
    char buf[10];
    for (int i = 0; i < count; i++) {
        char c = uart_getc(this->uart);
        buf[i] = c;
    }
    return count;
}

uint32_t TmcUart::readRegister(uint8_t motorId, uint8_t registerAddress) const
{
    this->flush();
    uint8_t datagram[4] = {0x55, motorId, registerAddress, 0};
    TmcUart::calcCRC(datagram, 4);
    this->writeUart(datagram, 4);

    // we need a little wait for the TMC driver reply
    sleep_ms(1);

    uint8_t buf[8];
    uint8_t readCount = this->readUart(buf, 8);  // read the reply
//    printf("bytes read: %d\n", readCount);

    if (readCount != 8) {
        printf("Read err.\n");
    }

    // crc check
    uint8_t recieved_crc = buf[7];
    TmcUart::calcCRC(buf, 8);

    if (recieved_crc != buf[7]) {
        printf("Bad CRC");
    }

    uint32_t value = buf[6] | buf[5] | buf[4] | buf[3];
//    uint32_t value = buf[10] | (buf[9] << 8) | (buf[8] << 16) | (buf[7] << 24);
    return value;
}

void TmcUart::writeRegister(uint8_t motorId, uint8_t registerAddress, uint32_t value) const
{
    uint8_t datagram[8] = {0x55, motorId, 0, 0, 0, 0, 0};
    datagram[2] = registerAddress | 0x80;
    datagram[3] = 0xFF & (value >> 24);
    datagram[4] = 0xFF & (value >> 16);
    datagram[5] = 0xFF & (value >> 8);
    datagram[6] = 0xFF & value;
    TmcUart::calcCRC(datagram, 8);

    this->writeUart(datagram, 8);
    uint8_t bufferek[8];
}

void TmcUart::calcCRC(uint8_t *datagram, uint8_t datagramLength)
{
    int i,j;
    uint8_t * crc = datagram + (datagramLength-1); // CRC located in last byte of message
    uint8_t currentByte;
    *crc = 0;
    for (i=0; i<(datagramLength-1); i++) { // Execute for all bytes of a message
        currentByte = datagram[i]; // Retrieve a byte to be sent from Array
        for (j=0; j<8; j++) {
            if ((*crc >> 7) ^ (currentByte&0x01)) // update CRC based result of XOR operation
            {
                *crc = (*crc << 1) ^ 0x07;
            }
            else
            {
                *crc = (*crc << 1);
            }
            currentByte = currentByte >> 1;
        } // for CRC bit
    } // for message byte
}


void TmcComm::defaultInit() const
{
    uint32_t gconf = tmcUart.readRegister(this->motorId, TMC2209_GCONF);

    gconf &= ~(1 << 2);  // stealthChop mode
//    gconf |= 1 << 3;  // direction 1
    gconf &= ~(1 << 3);  // direction 2
    gconf |= 1 << 6;  // disable pdn uart
    gconf |= 1 << 7;  // select microstepping with mres register
    tmcUart.writeRegister(this->motorId, TMC2209_GCONF, gconf);

    // set microstepping resolution to 256
    uint32_t chconf = tmcUart.readRegister(this->motorId, TMC2209_CHOPCONF);
    chconf &= (~(1 << 24) & ~(1 << 25) & ~(1 << 26) & ~(1 << 27));
    tmcUart.writeRegister(this->motorId, TMC2209_CHOPCONF, chconf);

    // configure stallguard
    tmcUart.writeRegister(this->motorId, TMC2209_SGTHRS, 100);
    tmcUart.writeRegister(this->motorId, TMC2209_TCOOLTHRS, 220);
    tmcUart.writeRegister(this->motorId, TMC2209_TPWMTHRS, 50);
}

void TmcComm::setStallGuardThreshold(uint8_t sgthrs) const
{
    this->tmcUart.writeRegister(this->motorId, TMC2209_SGTHRS, sgthrs);
}

uint32_t TmcComm::getStallGuardResult() const
{
    return this->tmcUart.readRegister(this->motorId, TMC2209_SG_RESULT);
}

uint32_t TmcComm::getTstep() const
{
    return this->tmcUart.readRegister(this->motorId, TMC2209_TSTEP);
}

void TmcComm::setVactual(uint32_t vactual) const
{
    this->tmcUart.writeRegister(this->motorId, TMC2209_VACTUAL, vactual);
}

void TmcComm::setCoolStepThreshold(uint32_t tcoolthrs) const
{
    this->tmcUart.writeRegister(this->motorId, TMC2209_TCOOLTHRS, tcoolthrs);
}
