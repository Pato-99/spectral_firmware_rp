/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "pico/stdlib.h"
#include "tmc_com.h"
#include <AccelStepper.h>


enum TYPE {ONE, TWO};

struct motor_command {
    TYPE type;
    int steps;
};

enum class MOVEMENT_TYPE
{
    CONTINUOUS,
    ABSOLUTE,
};

void blink() {
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, true);
    sleep_ms(100);
    gpio_put(25, false);
}

void handle_input_motor(AccelStepper& stepper, MOVEMENT_TYPE& mt)
{
    const char* delimiters = " \n\r\t";
    char *token = std::strtok(nullptr, delimiters);
    if (!token) {
        return;
    }
    if (strcmp(token, "MOVE") == 0) {
        token = std::strtok(nullptr, delimiters);
        if (!token) {
            return;
        }
        long value = std::strtol(token, nullptr, 10);
        mt = MOVEMENT_TYPE::ABSOLUTE;
        stepper.moveTo(value);
        printf("M1 moving to: %ld\n", value);
    } else if (strcmp(token, "CMOVE") == 0) {
        token = std::strtok(nullptr, delimiters);
        if (!token) {
            return;
        }
        float value = std::strtof(token, nullptr);
        mt = MOVEMENT_TYPE::CONTINUOUS;
        stepper.setSpeed(value);
    } else if (strcmp(token, "RESET") == 0) {
        stepper.setCurrentPosition(0);
//        blink();
    } else if (strcmp(token, "STATUS") == 0) {
        printf("%ld\n", stepper.currentPosition());
    }
}

void handle_input(AccelStepper& s1, AccelStepper& s2, MOVEMENT_TYPE& mt1, MOVEMENT_TYPE& mt2)
{
    char buffer[101];
    const char* delimiters = " \n";

    while (true) {
        int num_read = stdio_get_until(buffer, 100, 0);
        if (num_read == PICO_ERROR_TIMEOUT) {
//            printf("Timeout\n");
//            sleep_ms(1000);
            return;
        }

        if (num_read > 0) {
            buffer[num_read] = '\0';  // add the NULL termination
            char* token = std::strtok(buffer, delimiters);
//            printf("OK %s\n", token);
            if (!token) {
                return;
            }
            if (strcmp(token, "M1") == 0) {
                handle_input_motor(s1, mt1);
            } else if (strcmp(token, "M2") == 0) {
                handle_input_motor(s2, mt2);
            } else if (strcmp(token, "INFO") == 0) {
                printf(R"({"device": "Pico", "motors": ["M1", "M2"]})");
                printf("\n");
            } else if (strcmp(token, "ID") == 0) {
                printf("RPI_PICO_MOTOR_CONTROLLER_AVCR\n");
            }

        }
    }
}

MOVEMENT_TYPE motor1_move_type = MOVEMENT_TYPE::ABSOLUTE;
MOVEMENT_TYPE motor2_move_type = MOVEMENT_TYPE::ABSOLUTE;



void swuart_calcCRC(uint8_t * datagram, uint8_t datagramLength)
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



void tmcEchoTest(TmcComm& tmc)
{
    uint8_t data = 0xAA;
    printf("Sending data: %b\n", data);
    uint8_t sentCount = tmc.writeUart(&data, 1);
    printf("Sent %d bytes.\n", sentCount);

    sleep_ms(10);

    uint8_t buf;
    uint8_t readCount = tmc.readUart(&buf, 1);
    printf("Recieved data: %b\n", buf);
    printf("Received %d bytes.\n", readCount);
}

void printBuffer(const uint8_t *buf, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++) {
        printf(" %b", *(buf + i));
    }
    printf("\n");
}


void tmcEchoTest2(TmcComm& tmc)
{
    uint8_t data[4] = {0xAA, 0xFF, 0xFF, 0xFF};
    printf("Sending data:");
    printBuffer(data, 4);

    uint8_t writeCount = tmc.writeUart(data, 4);
    printf("Sent %d bytes.\n", writeCount);

    sleep_ms(10);

    uint8_t buf[7];
    uint8_t readCount = tmc.readUart(buf, 7);
    printf("Recieved data:");
    printBuffer(buf, 4);
    printf("Received %d bytes.\n", readCount);
}


void tmcGconfTest(TmcComm& tmc) {
    uint8_t datagram[4] = {0, 0, 0, 0};  // read datagram

    datagram[0] = 0x55;
    datagram[1] = 1;  // mtr id
    datagram[2] = 0;  // GCONF address
    swuart_calcCRC(datagram, 4);
    printf("Sending data:");
    printBuffer(datagram, 4);


    uint8_t writeCount = tmc.writeUart(datagram, 4);
    printf("Sent %d bytes.\n", writeCount);
    sleep_ms(1);

    uint8_t buf[12];
    uint8_t rc = tmc.readUart(buf, 4);  // read the trash (RX and TX one line)
    printf("Recieved data:");
    printBuffer(buf, 4);
    printf("Bytes read: %d\n", rc);

    sleep_ms(1);
    uint8_t readCount = tmc.readUart(buf, 12);
    printf("Recieved data:");
    printBuffer(buf, 12);
    printf("Bytes read: %d\n", readCount);
}

void readRegTest(TmcComm& tmc, uint8_t *buf)
{
    uint32_t value = tmc.readRegister(1, 0x41, buf);

    printf("Received data:");
    printBuffer(buf, 8);

    printf("Value: %ld\n", value);

//    tmc.writeRegister(1, 0x40, 200);
}




int main()
{
    stdio_init_all();
    printf("Starting...\n");
    blink();


    AccelStepper stepper1(AccelStepper::MotorInterfaceType::DRIVER, 6, 7);
    stepper1.setPinsInverted(false, false, true);
    stepper1.setEnablePin(8);
////    AccelStepper stepper2(AccelStepper::MotorInterfaceType::DRIVER, 1, 0);
////    stepper2.setPinsInverted(false, false, true);
////    stepper2.setEnablePin(4);
////
    stepper1.setMaxSpeed(12800.0);
    stepper1.setAcceleration(6400.0 * 2 / 2);
////
////    stepper2.setMaxSpeed(12800.0);
////    stepper2.setAcceleration(6400.0 * 2);
////
    stepper1.moveTo(1200000000);
////    stepper2.moveTo(12000);

    stepper1.disableOutputs();


    // UART things
    TmcComm tmc;


    uint32_t s = 0;
    uint32_t n = 0;

    uint8_t buf[8];
    while (true) {
        if ( n % 1000000 == 0) {
//            printf("SG result: %lu\n", s++);
//            tmcGconfTest(tmc);
            readRegTest(tmc, buf);
        // 10111011
        }
        stepper1.run();
        n++;
        continue;

//
//        handle_input(stepper1, stepper2, motor1_move_type, motor2_move_type);
//
//        if (motor1_move_type == MOVEMENT_TYPE::ABSOLUTE) {
//            stepper1.run();
//        } else if (motor1_move_type == MOVEMENT_TYPE::CONTINUOUS) {
//            stepper1.runSpeed();
//        }
//
//        if (motor2_move_type == MOVEMENT_TYPE::ABSOLUTE) {
//            stepper2.run();
//        } else if (motor2_move_type == MOVEMENT_TYPE::CONTINUOUS) {
//            stepper2.runSpeed();
//        }
    }
}
