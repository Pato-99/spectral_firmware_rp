
#include <cstdio>
#include "hardware/uart.h"

#include "tmc_com.h"


TmcComm::TmcComm()
    : rxPin(1),
      txPin(0),
      uart(uart0),
      baudRate(115200)
{
    uart_init(this->uart, this->baudRate);
    gpio_set_function(this->rxPin, UART_FUNCSEL_NUM(this->uart, this->rxPin));
    gpio_set_function(this->txPin, UART_FUNCSEL_NUM(this->uart, this->txPin));
}

uint8_t TmcComm::readUart(uint8_t *buf, uint8_t len)
{
    uint8_t count = 0;
    while (uart_is_readable(this->uart) && count != len) {
        uart_read_blocking(this->uart, buf + count, 1);
        count++;
    }
    return count;
}

uint8_t TmcComm::writeUart(const uint8_t *data, uint8_t len)
{
    uint8_t count = 0;
    while (uart_is_writable(this->uart) && count != len) {
        uart_write_blocking(this->uart, data + count, 1);
        count++;
    }
    return count;
}

uint32_t TmcComm::readRegister(uint8_t motorId, uint8_t registerAddress, uint8_t *buf)
{
    uint8_t datagram[4] = {0x55, motorId, registerAddress, 0};
    calcCRC(datagram, 4);
    writeUart(datagram, 4);

//    uint8_t buf[8];
    readUart(buf, 4);  // read the trash (RX and TX one line)

    uint8_t readCount = readUart(buf, 8);  // read the reply
    printf("bytes read: %d\n", readCount);

//    uint32_t value = buf[3] | (buf[4] << 8) | (buf[5] << 16) | (buf[6] << 24);
    uint32_t value = buf[6] | (buf[5] << 8) | (buf[4] << 16) | (buf[3] << 24);
    return value;
}

void TmcComm::writeRegister(uint8_t motorId, uint8_t registerAddress, uint32_t value)
{
    uint8_t datagram[8] = {0x55, motorId, registerAddress, 0, 0, 0, 0, 0};
    datagram[3] = 0xFF & (value >> 24);
    datagram[4] = 0xFF & (value >> 16);
    datagram[5] = 0xFF & (value >> 8);
    datagram[6] = 0xFF & value;
    calcCRC(datagram, 8);

    writeUart(datagram, 8);
    uint8_t buf[8];
    readUart(buf, 8); // read the trash (RX and TX one line)
}


void TmcComm::calcCRC(uint8_t *datagram, uint8_t datagramLength)
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
