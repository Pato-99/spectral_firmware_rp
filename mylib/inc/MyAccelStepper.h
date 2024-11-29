#ifndef AVCR_SPECTRAL_FW_MYACCELSTEPPER_H
#define AVCR_SPECTRAL_FW_MYACCELSTEPPER_H

#include "AccelStepper.h"
#include "tmc_com.h"

class MyAccelStepper
{
public:
    AccelStepper stepper;
    const TmcComm tmcComm;
private:
    const uint id;
    const uint en;
    const uint diag;

public:
    MyAccelStepper(uint id, uint step, uint dir, uint en, uint diag, TmcUart& tmcUart);
    void setStallCallback(gpio_irq_callback_t callback) const;
};


#endif //AVCR_SPECTRAL_FW_MYACCELSTEPPER_H
