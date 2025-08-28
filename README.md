# BLTBlinker
BLT Challenge blinker project

//
// Purpose:        BLT Technical Challenge
//
// Description:    Using the timer count, toggle an LED within 30ms of each
//                  time the count reaches 65000. Using the same timer, but to
//                  a separate LED, blink 5 times at a 3s interval. The second
//                  LED blinks should be spaced enough to be counted by the
//                  human eye. The timer should be free running.
//
// Microprocessor: TI MSP430F2272
//

This project was created using TI Code Composer Studio (CCS) version 11.2.0 for an MSP430F2272 embedded processor. It builds with the compiler included with that version of CCS. It is best to install CCS 11.2.0 and select Project->Build All. An output file BLTChallenge.out will be created in the Debug folder that can be installed to an MSP430F2272 processor. A makefile is also created in the Debug folder that can be executed from the command line, but it will only compile using the compiler installed with CCS.
