#include <cstdio>
#include "pico/stdlib.h"
#include "tmc_com.h"
#include "mg90s.h"
#include "controller.h"
#include "definitions.h"
#include "interrupts.h"


void init_blink()
{
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
}

void blink() {
    gpio_put(25, true);
    sleep_ms(100);
    gpio_put(25, false);
}

enum class InputState {LINE_RECEIVED, WAITING};

void receiveChar(char* buf, int bufSize, int* idx, InputState* inputState)
{
    if (*idx >= bufSize) {
        // wrap at end
        *idx = 0;
    }
    int c = stdio_getchar_timeout_us(0);
    if (c == PICO_ERROR_TIMEOUT) {
        return;
    }
    buf[*idx] = c;
    if (c != '\r' && c != '\n') {
        // ignore those chars
        *idx += 1;
    }
    buf[*idx] = '\0';
    if (c == '\n' || c == '\r') {
        *inputState = InputState::LINE_RECEIVED;
    }
}


int main()
{
    stdio_init_all();
    printf("Starting...\n");

    init_blink();
    blink();

    // initialize the uart for comunication with TMC drivers
    TmcUart tmcUart;

    // initialize M0
    MyAccelStepper m0(M0_ID, M0_STEP, M0_DIR, M0_EN, M0_DIAG, tmcUart);
    m0.setStallCallback(gpio_callback);
    MyAccelStepper m1(M1_ID, M1_STEP, M1_DIR, M1_EN, M1_DIAG, tmcUart);
    m1.setStallCallback(gpio_callback);
    MyAccelStepper m2(M2_ID, M2_STEP, M2_DIR, M2_EN, M2_DIAG, tmcUart);
    m2.setStallCallback(gpio_callback);

    // initialize SERVO
    MG90S servo(SERVO_PWM);

    // initialize RELAY
    Relay relay(RELAY_PIN);

    // create empty controller instance
    Controller controller;

    // populate the controller instance
    // order of the motor addition is essential
    // as it corresponds to their id...
    controller.addMotor(m0);
    controller.addMotor(m1);
    controller.addMotor(m2);

    controller.addServo(servo);
    controller.addRelay(relay);


    char inputBuffer[101];
    int currentIdx = 0;
    InputState currentState = InputState::WAITING;

    while (true) {
        receiveChar(inputBuffer, 101, &currentIdx, &currentState);
        if (currentState == InputState::LINE_RECEIVED) {
            currentState = InputState::WAITING;
            currentIdx = 0;
            controller.handleInput(inputBuffer);
        }

        // handle stall guard
        // here stallguard triggers outside
        // the homing sequence -> bad
        if (m0Stall) {
            m0Stall = false;
            m1.stepper.setCurrentPosition(0);
            m1.stepper.moveTo(0);
        }

        if (m1Stall) {
            m1Stall = false;
            m1.stepper.setCurrentPosition(0);
            m1.stepper.moveTo(0);
        }

        if (m2Stall) {
            m2Stall = false;
            m2.stepper.setCurrentPosition(0);
            m2.stepper.moveTo(0);
        }

        controller.run();
    }
}
