/**
 * @file
 * As the interrupts have to be handled globally,
 * I specify the handlers and global flags here.
 */

#ifndef AVCR_SPECTRAL_FW_INTERRUPTS_H
#define AVCR_SPECTRAL_FW_INTERRUPTS_H


// global flag variables

extern bool m0Stall;
extern bool m1Stall;
extern bool m2Stall;

/**
 * GPIO interrupt handler
 *
 * @param gpio pin that caused the interrupt
 * @param events events that happened GPIO_IRQ_EDGE_RISE...
 */
void gpio_callback(uint gpio, uint32_t events);

#endif //AVCR_SPECTRAL_FW_INTERRUPTS_H
