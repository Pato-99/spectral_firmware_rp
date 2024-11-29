
#include "pico/stdlib.h"
#include "tmc_com.h"
#include <cstdio>


#define DIAG 9
#define EN 8
#define STEP 6
#define DIR 7

bool stall = false;

void stallCallback(uint gpio, uint32_t events)
{
    if (stall) {
        return;
    }
    printf("STALL\n");
    gpio_put(EN, true);
    stall = true;
}

void initDiag()
{
    gpio_init(DIAG);
    gpio_pull_down(DIAG);
    gpio_set_irq_enabled_with_callback(DIAG, GPIO_IRQ_EDGE_RISE, true, stallCallback);
}

void initStepDir()
{
    gpio_init(STEP);
    gpio_set_dir(STEP, GPIO_OUT);

    gpio_init(DIR);
    gpio_set_dir(DIR, GPIO_OUT);

    gpio_init(EN);
    gpio_set_dir(EN, GPIO_OUT);
    gpio_put(EN, false);  // enable the stepper
}

void step()
{
    gpio_put(STEP, true);
    sleep_ms(10);
    gpio_put(STEP, false);
    sleep_ms(10);
}

void steps(int n)
{
    int abs_n = n;
    if ( n < 0) {
        gpio_put(DIR, true);
        abs_n *= -1;
    } else {
        gpio_put(DIR, false);
    }
    for (int i = 0; i < abs_n; i++) {
        step();
    }
}

int main()
{
    stdio_init_all();
    initDiag();
    initStepDir();
    printf("Starting...\n");


    // initialize the uart for comunication with TMC drivers
    TmcUart tmcUart;
    TmcComm tmcComm(tmcUart, 1);
    tmcComm.defaultInit();

    tmcComm.setVactual(256 * 328);

    while (true) {
        if (stall) {
            continue;
        }
        uint32_t cnt = tmcUart.readRegister(1, TMC2209_IFCNT);
        uint32_t sgResult = tmcComm.getStallGuardResult();
        uint32_t tstep = tmcComm.getTstep();
        printf("SGRES=%lu, TSTEP=%lu, CNT=%lu\n", sgResult, tstep, cnt);
//        sleep_ms(300);

        if (sgResult > 220) {
            tmcComm.setVactual(0);
//            gpio_put(EN, true);
        }
    }


}
