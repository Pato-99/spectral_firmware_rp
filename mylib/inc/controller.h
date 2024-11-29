#ifndef AVCR_SPECTRAL_FW_CONTROLLER_H
#define AVCR_SPECTRAL_FW_CONTROLLER_H

#include <AccelStepper.h>
#include "tmc_com.h"
#include "mg90s.h"
#include "MyAccelStepper.h"
#include "relay.h"


#define MAX_STEPPERS 3

class Controller
{
private:
    MyAccelStepper* motors[MAX_STEPPERS] = {};
    uint numMotors = 0;
    MG90S* servo = nullptr;
    bool servoStatus = false;
    Relay* relay = nullptr;

public:
    Controller() = default;
    bool addMotor(MyAccelStepper& motor);
    bool addServo(MG90S& serv);
    bool addRelay(Relay& rela);
    void handleInput(char* line);
    void run();

private:
    void handleInputMotors();
    void handleInputMotor(MyAccelStepper* m);
    void handleInputMotorSpd(MyAccelStepper* m);
    void handleInputMotorAcc(MyAccelStepper* m);
    void handleInputMotorAbs(MyAccelStepper* m);
    void handleInputMotorRel(MyAccelStepper* m);
    void handleInputMotorSg(MyAccelStepper* m);
    void handleInputMotorHom(MyAccelStepper* m);

    void handleInputServo();
    void handleInputRelay();
    void handleInputStatus();
};

#endif //AVCR_SPECTRAL_FW_CONTROLLER_H
