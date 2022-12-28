/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "rtos.h"

// Blinking rate in milliseconds
#define BLINKING_RATE     1000ms
#define STEPPER_DELAY     100ms
#define STEPS_PER_REV     400
#define PULSE_WIDTH       1000

struct SevenSegmentLED {
    DigitalOut *seg_A;
    DigitalOut *seg_B;
    DigitalOut *seg_C;
    DigitalOut *seg_D;
    DigitalOut *seg_E;
    DigitalOut *seg_F;
    DigitalOut *seg_G;
    DigitalOut *seg_decimal = 0;

    int display(int value) {
        switch (value) {
            case 0:
                *seg_A = true;
                *seg_B = true;
                *seg_C = true;
                *seg_D = true;
                *seg_E = true;
                *seg_F = true;
                *seg_G = false;
                return 0;
            case 1:
                *seg_A = false;
                *seg_B = true;
                *seg_C = true;
                *seg_D = false;
                *seg_E = false;
                *seg_F = false;
                *seg_G = false;
                return 0;
            case 2:
                *seg_A = true;
                *seg_B = true;
                *seg_C = false;
                *seg_D = true;
                *seg_E = true;
                *seg_F = false;
                *seg_G = true;
                return 0;
            case 3:
                *seg_A = true;
                *seg_B = true;
                *seg_C = true;
                *seg_D = true;
                *seg_E = false;
                *seg_F = false;
                *seg_G = true;
                return 0;
            case 4:
                *seg_A = false;
                *seg_B = true;
                *seg_C = true;
                *seg_D = false;
                *seg_E = false;
                *seg_F = true;
                *seg_G = true;
                return 0;
            case 5:
                *seg_A = true;
                *seg_B = false;
                *seg_C = true;
                *seg_D = true;
                *seg_E = false;
                *seg_F = true;
                *seg_G = true;
                return 0;
            case 6:
                *seg_A = true;
                *seg_B = false;
                *seg_C = true;
                *seg_D = true;
                *seg_E = true;
                *seg_F = true;
                *seg_G = true;
                return 0;
            case 7:
                *seg_A = true;
                *seg_B = true;
                *seg_C = true;
                *seg_D = false;
                *seg_E = false;
                *seg_F = false;
                *seg_G = false;
                return 0;
            case 8:
                *seg_A = true;
                *seg_B = true;
                *seg_C = true;
                *seg_D = true;
                *seg_E = true;
                *seg_F = true;
                *seg_G = true;
                return 0;
            case 9:
                *seg_A = true;
                *seg_B = true;
                *seg_C = true;
                *seg_D = true;
                *seg_E = false;
                *seg_F = true;
                *seg_G = true;
                return 0;
            case 10:
                *seg_A = true;
                *seg_B = true;
                *seg_C = true;
                *seg_D = false;
                *seg_E = true;
                *seg_F = true;
                *seg_G = true;
                return 0;
            case 11:
                *seg_A = false;
                *seg_B = false;
                *seg_C = true;
                *seg_D = true;
                *seg_E = true;
                *seg_F = true;
                *seg_G = true;
                return 0;
            case 12:
                *seg_A = true;
                *seg_B = false;
                *seg_C = false;
                *seg_D = true;
                *seg_E = true;
                *seg_F = true;
                *seg_G = false;
                return 0;
            case 13:
                *seg_A = false;
                *seg_B = true;
                *seg_C = true;
                *seg_D = true;
                *seg_E = true;
                *seg_F = false;
                *seg_G = true;
                return 0;
            case 14:
                *seg_A = true;
                *seg_B = false;
                *seg_C = false;
                *seg_D = true;
                *seg_E = true;
                *seg_F = true;
                *seg_G = true;
                return 0;
            case 15:
                *seg_A = true;
                *seg_B = false;
                *seg_C = false;
                *seg_D = false;
                *seg_E = true;
                *seg_F = true;
                *seg_G = true;
                return 0;
            default:
                return -1;
        }
    }
};

void doBlinkyCounting(SevenSegmentLED *sevenSegLED) {
    int i = 0;
    while (true) {
        sevenSegLED->display(i++);

        if (i > 15)
            i = 0;

        ThisThread::sleep_for(BLINKING_RATE);
    }
}

int main()
{
    // Initialize the onboard LED
    DigitalOut led(LED1);

    // Initialize the output pins for the 7-seg LED
    DigitalOut led_A(PA_0);
    DigitalOut led_B(PA_1);
    DigitalOut led_C(PA_4);
    DigitalOut led_D(PA_10);
    DigitalOut led_E(PB_5);
    DigitalOut led_F(PB_0);
    DigitalOut led_G(PB_4);
    SevenSegmentLED ssled;
    ssled.seg_A = &led_A;
    ssled.seg_B = &led_B;
    ssled.seg_C = &led_C;
    ssled.seg_D = &led_D;
    ssled.seg_E = &led_E;
    ssled.seg_F = &led_F;
    ssled.seg_G = &led_G;

    // Initialize the output pins for the A4988
    DigitalOut a4988_STEP(PB_10);
    DigitalOut a4988_DIR(PA_8);
    DigitalOut a4988_MS3(PA_9);
    DigitalOut a4988_MS2(PC_7);
    DigitalOut a4988_MS1(PB_6);

    a4988_DIR = false;
    int i = 0;

    Thread blinkyThread;
    blinkyThread.start(callback(doBlinkyCounting, &ssled));

    while (true) {
        led = !led;
        a4988_DIR = !a4988_DIR;

        for (int q = 0; q < STEPS_PER_REV; q++) {
            a4988_STEP = true;
            wait_us(PULSE_WIDTH);
            a4988_STEP = false;
            //wait_us(PULSE_WIDTH);
        }

        ThisThread::sleep_for(STEPPER_DELAY);
    }
}