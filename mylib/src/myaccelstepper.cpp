
#include "MyAccelStepper.h"


MyAccelStepper::MyAccelStepper(uint id, uint step, uint dir, uint en, uint diag, TmcUart& tmcUart)
    : stepper(AccelStepper(AccelStepper::MotorInterfaceType::DRIVER, step, dir)),
      tmcComm(TmcComm(tmcUart, id)),
      id(id),
      en(en),
      diag(diag)
{
    this->stepper.setPinsInverted(false, false, true);
    this->stepper.setEnablePin(en);
    this->stepper.disableOutputs();  // off by default

    this->tmcComm.defaultInit();
    this->stepper.setAcceleration(100000);
    this->stepper.setMaxSpeed(100000);

    // diag pin recieves impulse on motor stall
//    gpio_init(this->diag);
//    gpio_set_irq_enabled(this->diag, GPIO_IRQ_EDGE_RISE, true);
}

void MyAccelStepper::setStallCallback(gpio_irq_callback_t callback) const
{
    gpio_init(this->diag);
    gpio_pull_down(this->diag);
    gpio_set_irq_enabled_with_callback(this->diag, GPIO_IRQ_EDGE_RISE, true, callback);
}
