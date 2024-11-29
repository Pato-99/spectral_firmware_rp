
#include <cstring>
#include <cstdio>

#include "controller.h"


bool Controller::addMotor(MyAccelStepper& motor)
{
    if (this->numMotors >= MAX_STEPPERS) {
        // can not add more motors
        return false;
    }
    this->motors[this->numMotors++] = &motor;
    return true;
}

bool Controller::addServo(MG90S& serv)
{
    if (this->servo != nullptr) {
        // servo already added
        return false;
    }
    this->servo = &serv;
    return true;
}

bool Controller::addRelay(Relay& rela)
{
    if (this->relay != nullptr) {
        // relay already added
        return false;
    }
    this->relay = &rela;
    return true;
}

void Controller::handleInput(char *line)
{
    const char* delimiters = " \n";
    char* token = std::strtok(line, delimiters);

    if (!token) {
        // no token found or end of line/string
        return;
    }

    if (strcmp(token, "M") == 0) {
        handleInputMotors();
    } else if (strcmp(token, "S") == 0) {
        handleInputServo();
    } else if (strcmp(token, "R") == 0) {
        handleInputRelay();
    } else if (strcmp(token, "STATUS") == 0) {
        handleInputStatus();
    } else if (strcmp(token, "ID") == 0) {
        // for device identification -> autoconnect
        printf("RPI_PICO_SPECTRAL_AVCR\n");
    }
}

void Controller::run()
{
    for (int i = 0; i < this->numMotors; i++) {
        this->motors[i]->stepper.run();
    }
}

void Controller::handleInputMotors()
{
    char* token = std::strtok(nullptr, " ");

    if (!token) {
        // no token found or end of line/string
        return;
    }
    uint motorId = atol(token);
    if (motorId >= MAX_STEPPERS) {
        // invalid motor ID
        // TODO: when conversion fails then ID -> 0 -> BAD
        return;
    }
    handleInputMotor(this->motors[motorId]);
}

void Controller::handleInputMotor(MyAccelStepper* m)
{
    char* token = std::strtok(nullptr, " ");

    if (!token) {
        // no token found or end of line/string
        return;
    }

    if (strcmp(token, "SPD") == 0) {
        // set max speed
        handleInputMotorSpd(m);
    } else if (strcmp(token, "ACC") == 0) {
        // set max acceleration
        handleInputMotorAcc(m);
    } else if (strcmp(token, "ABS") == 0) {
        // absolute move
        handleInputMotorAbs(m);
    } else if (strcmp(token, "REL") == 0) {
        // relative move
        handleInputMotorRel(m);
    } else if (strcmp(token, "HOM") == 0) {
        // perform homing sequence
        handleInputMotorHom(m);
    } else if (strcmp(token, "ON") == 0) {
        m->stepper.enableOutputs();
        // turn on the driver/motor
    } else if (strcmp(token, "OFF") == 0) {
        m->stepper.disableOutputs();
        // turn off the driver/motor
    } else if (strcmp(token, "SG") == 0) {
        // set sg threshold
        handleInputMotorSg(m);
    } else if (strcmp(token, "TCO")) {
        // set tco.. TODO
    }
}

void Controller::handleInputMotorSpd(MyAccelStepper* m)
{
    char* token = std::strtok(nullptr, " ");

    if (!token) {
        // no token found or end of line/string
        return;
    }
    float maxSpeed = atof(token);
    m->stepper.setMaxSpeed(maxSpeed);
}

void Controller::handleInputMotorAcc(MyAccelStepper* m)
{
    char* token = std::strtok(nullptr, " ");

    if (!token) {
        // no token found or end of line/string
        return;
    }
    float acceleration = atof(token);
    m->stepper.setAcceleration(acceleration);
}

void Controller::handleInputMotorAbs(MyAccelStepper* m)
{
    char* token = std::strtok(nullptr, " ");

    if (!token) {
        // no token found or end of line/string
        return;
    }
    long absPosition = atol(token);
    m->stepper.moveTo(absPosition);
}

void Controller::handleInputMotorRel(MyAccelStepper* m)
{
    char* token = std::strtok(nullptr, " ");

    if (!token) {
        // no token found or end of line/string
        return;
    }
    long relPosition = atol(token);
    m->stepper.move(relPosition);
}

void Controller::handleInputMotorSg(MyAccelStepper* m)
{
    char* token = std::strtok(nullptr, " ");

    if (!token) {
        // no token found or end of line/string
        return;
    }
    uint8_t sgthrs = atoi(token);
    m->tmcComm.setStallGuardThreshold(sgthrs);
}

void Controller::handleInputMotorHom(MyAccelStepper* m)
{
    m->stepper.enableOutputs();
    m->tmcComm.setVactual(256 * 328);
    sleep_ms(30);
    while (true) {
        uint32_t sgres = m->tmcComm.getStallGuardResult();
//        printf("%lu\n", sgres);
        if (sgres > 200) {
            m->tmcComm.setVactual(0);
            break;
        }
    }
    m->stepper.setCurrentPosition(0);
    printf("OK\n");
}


void Controller::handleInputServo()
{
    char* token = std::strtok(nullptr, " ");

    if (!token) {
        // no token found or end of line/string
        return;
    }
    if (strcmp(token, "IN") == 0) {
        // turn the servo in
        this->servo->setLevel((MG90S_PWM_LEVEL_MID + MG90S_PWM_LEVEL_MIN) / 2);
        this->servoStatus = false;
//        printf("Turning the servo in\n");
    } else if (strcmp(token, "OUT") == 0) {
        // turn the servo out
        this->servo->setLevel((MG90S_PWM_LEVEL_MID + MG90S_PWM_LEVEL_MAX) / 2);
        this->servoStatus = true;
//        printf("Turning the servo out\n");
    } else if (strcmp(token, "ON") == 0) {
        // turn on the driver/motor
        this->servo->enable();
//        printf("Turning the servo ON\n");
    } else if (strcmp(token, "OFF") == 0) {
        // turn off the driver/motor
        this->servo->disable();
//        printf("Turning the servo OFF\n");
    }
}

void Controller::handleInputRelay()
{
    char* token = std::strtok(nullptr, " ");

    if (!token) {
        // no token found or end of line/string
        return;
    }

    if (strcmp(token, "ON") == 0) {
        // turn the relay on
        this->relay->on();
    } else if (strcmp(token, "OFF") == 0) {
        this->relay->off();
    }
}

void Controller::handleInputStatus()
{
    for (int i = 0; i < MAX_STEPPERS; i++) {
        // M {id} {position} {running} {max speed} {acceleration}
        long position = this->motors[i]->stepper.currentPosition();
        bool running = this->motors[i]->stepper.isRunning();
        float maxSpeed = this->motors[i]->stepper.maxSpeed();
        float acceleration = this->motors[i]->stepper.acceleration();
        printf("M %d %ld %d %f %f\n", i, position, running, maxSpeed, acceleration);
    }
    printf("S %d\n", this->servoStatus);
    printf("R %d\n", this->relay->status());
}
