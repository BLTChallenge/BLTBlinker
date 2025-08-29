#include <msp430.h> 

//
// Purpose:        BLT Technical Challenge
//
// Description:    Using the timer count, toggle an LED within 30ms of each
//                  time the count reaches 65000. Using the same timer, but to
//                  a separate LED, blink 5 times at a 3s interval. The second
//                  LED blinks should be spaced enough to be counted by the
//                  human eye. The timer should be free running.
//
// Microprocessor: MSP430F2272
//
// Notes:          - Since this is simple test code to handle the tasks in the description,
//                    certain liberties were taken to complete the challenge quickly
//                 - All ports have not been initialized and would be for release code
//                 - Other unused processor peripherals/IO are not initialized but should be
//                 - Use of certain constructs, such as modulo operator, would probably
//                    be refactored in release code for efficiency
//                 - Use of an infinite while loop in main() would be replaced with code
//                    that placed the processor in Low Power Mode using LPM0, which disables
//                    MCLK, but keeps SMCLK running, allowing the timers to continue to operate
//

// Configure LED pins
#define LED1    BIT0    // P1.0
#define LED2    BIT7    // P4.7

// Certain defs to clarify code workflow
#define LED1_TOGGLE_COUNT   65000
#define BLINK_OVERFLOWS     46
#define BLINK_COUNT         5
#define BLINK_ON_COUNT      4
#define IS_BLINKING         1
#define TAIV_OVERFLOW       2 // TAIV will be 2 upon overflow

// Variables that are updated within the interrupt handler
volatile char led2IsBlinking = 0;
unsigned int overflowCount = 0;
unsigned int blinkPhase = 0;
unsigned int blinkHold = 0;

/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop watch dog timer
    DCOCTL = 0;               // Set DCO to lowest tap
    BCSCTL1 = CALBC1_1MHZ;    // Set range
    DCOCTL = CALDCO_1MHZ;     // Set DCO step and modulation

    P1DIR |= LED1;  // P1.0 LED1 output
    P1OUT &= ~LED1; // LED1 off

    P4DIR |= LED2;  // P4.7 LED2 output
    P4OUT &= ~LED2; // LED2 off

    // Timer_A setup: Continuous, 1MHz SMCLK, Timer_A Interrupt set flag on overflow
    TACTL = TASSEL_2 | MC_2 | TAIE;

    _enable_interrupt();

    // The infinite loop is not a preferred way to sit idle because it
    //  uses power resource unnecessarily. This would be modified in
    //  a real world scenario.
    while (1) {
        // Wait in infinite loop here since all logic is in the interrupt handler
    }
}

// Timer_A interrupt vector
#pragma vector = TIMERA1_VECTOR
__interrupt void Timer_A(void)
{
    unsigned int tar = TAR; // Timer A counter register

    // Check if count hits the toggle count for LED1
    //  This is supposed to toggle within 30 mSecs of timer reaching toggle count (65000)
    //  Since the clock is 1MHz this should be equivalent to 65 mSecs
    //  P1OUT should toggle within a couple mSecs after P1OUT is set with SMCLK set to 1MHz
    if (tar >= LED1_TOGGLE_COUNT) {
        P1OUT ^= LED1;
        TACTL |= TACLR;     // Reset TAR to trigger on next toggle count (65000)
    }

    // Check for timer overflow
    // TAIV is the Timer A Interrupt Vector word, it should overflow every 65535
    if (TAIV == TAIV_OVERFLOW) {
        // Blinking logic
        //
        // Assume 1MHz, overflow=~65mSecs, count=~46 overflows each 3secs)
        // Every ~3 seconds (actual 46*65=2990 msecs)
        // Blink 5 times at about ~250mSecs each to aid visual confirmation
        overflowCount++;
        if (overflowCount >= BLINK_OVERFLOWS) {
            led2IsBlinking = IS_BLINKING;
            overflowCount = 0;  // reset for next blink cycle in 3 seconds
            blinkPhase = 0;
            blinkHold = 0;

            // Toggle LED2 (ON)
            P4OUT ^= LED2;
        }

        // Provide for LED2 to blink X=5 times at ~250mSec intervals
        if (led2IsBlinking == IS_BLINKING) {
            blinkHold++;

            // Blink hold for ~250ms each
            if (blinkHold > BLINK_ON_COUNT) {
                blinkHold = 0;
                blinkPhase++;

                // Toggle LED2
                P4OUT ^= LED2;

                // Blinks X=5 times (on and off)
                if (blinkPhase >= 2*BLINK_COUNT) {
                    led2IsBlinking = 0;

                    // Make sure that LED2 off upon completion even though
                    //  it should already be off on the last toggle
                    P4OUT &= ~LED2;
                }
            }
        }
    }
}

