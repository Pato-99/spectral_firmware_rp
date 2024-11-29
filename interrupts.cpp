
#include "pico/stdlib.h"

#include "definitions.h"
#include "interrupts.h"


bool m0Stall = false;
bool m1Stall = false;
bool m2Stall = false;

void gpio_callback(uint gpio, uint32_t events)
{
    // handle only EDGE_RISE events, for now...
    if (!(events & GPIO_IRQ_EDGE_RISE)) {
        return;
    }

    switch (gpio) {
        case M0_DIAG:
            gpio_put(M0_EN, true);  // power off the motor
            m0Stall = true;
            break;
        case M1_DIAG:
//            gpio_put(M1_EN, true);  // power off the motor
            m1Stall = true;
            break;
        case M2_DIAG:
            gpio_put(M2_EN, true);  // power off the motor
            m2Stall = true;
            break;
        default:
            break;
    }
}
