#include <stdint.h>
#include "sleep.h"
#include <stdio.h>

#define SEG_CTL   (*(volatile uint32_t *)0x43C10000)
#define SEG_DATA  (*(volatile uint32_t *)0x43C10004)
#define Button_Data (*(volatile uint32_t *)0x41200000)

static inline void display_num(uint16_t number) {
    // Enable 7-seg "hex mode" in the IP core
    SEG_CTL = 1;

    // Split 16-bit number into 4 hex nibbles (d0 = least significant)
    uint8_t d0 =  number        & 0xF;
    uint8_t d1 = (number >> 4)  & 0xF;
    uint8_t d2 = (number >> 8)  & 0xF;
    uint8_t d3 = (number >> 12) & 0xF;

    // Turn OFF all decimal points by setting bit7=1 on each digit
    uint32_t temp =
        ((uint32_t)(0x80 | d0)      ) |
        ((uint32_t)(0x80 | d1) <<  8) |
        ((uint32_t)(0x80 | d2) << 16) |
        ((uint32_t)(0x80 | d3) << 24);

    SEG_DATA = temp;
}

int main(void) {
    uint16_t counter = 0;
    int stopwatchRunning = 0;

    // For crude edge-detection
    uint32_t prevButtons = 0;

    while (1) {
        // Read buttons once per loop
        uint32_t buttons = Button_Data & 0xF;
        uint32_t rising  = (~prevButtons) & buttons; // edges

        // BTN0: toggle run/stop
        if (rising & 0x1) {
            stopwatchRunning = !stopwatchRunning;
        }
        // BTN1: reset counter
        if (rising & 0x2) {
            counter = 0;
        }

        // Update counter + pace
        if (stopwatchRunning) {
            counter++;        // wraps at 0xFFFF naturally
            sleep(1);         // 1 second (depends on your BSP)
        }

        // Update display every loop
        display_num(counter);

        // Save for next edge detect
        prevButtons = buttons;
    }

    return 0;
}
