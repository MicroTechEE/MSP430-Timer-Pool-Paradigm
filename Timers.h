/*
 * Timers.h
 *
 *  Creates a pool of A timers. When a Timer is needed, it is requested, used, and killed.
 *
 */

#ifndef TIMERS_H_
#define TIMERS_H_

#include <stdint.h>
#include <stdbool.h>

/*      NOTES
 *
 * How the timers work:
 * -> All A timers are thrown into a pool --> timerpool_init()
 * -> Suggest declaring one dedicated global timer per library ( uart, spi, BLE, Whatever, etc...) --> msTimerA* globalTimer;
 * -> When timer is needed, request one --> globalTimer = timer_requestTimer(callbackFunction);
 * -> Callback is the function you want to trigger on Timeout ( Don't include () when passing function as argument )
 * -> If Timer* doesn't equal zero, it worked. The value of Timer* is now the memory location of the actual Timer Struct you're using
 * -> When Timeout occurs, your callback function will fire. In RTOS, this would be considered a SWI (Software Interrupt)
 * -> Return callback as True if you want to wake up MCU out of LPM (Low Power Mode) or false if you don't
 * -> When the callback function is done, the ACTUAL Interrupt occurs in Hardware. In RTOS, this would be a HWI (Hardware Interrupt)
 *
 * TIMER_A0_BASE
 * TIMER_A1_BASE
 * TIMER_A2_BASE	// All these are used dynamically in a pool
 * TIMER_A3_BASE
 * TIMER_A4_BASE
 *
 * TIMER_B0_BASE	// Some people use this as a maintenance timer which constantly runs and assists with whatever you're doing
 * TIMER_B1_BASE	// Some chips have this, MSP430FR5994 doesn't
 */

typedef struct {
	bool (*Callback)();    // function pointer to timer callback
	bool inUse;
	uint16_t timerName;
} msTimerA;

/* - - - - Timer A Pool - - - - */
void timer_initTimerpool(void);
msTimerA* timer_requestTimer(bool (*Callback)());
void timer_startTimer(unsigned int Period_ms, msTimerA* timer);
void timer_stopTimer(msTimerA* timer);
void timer_resetTimer(msTimerA* timer);
void timer_freeTimer(msTimerA* timer);
void timer_killTimer(msTimerA** ptrToTimer);


/* - - - - Maintenance Timer - - - - */
// This timer acts as a heartbeat. The maintenance callback will fire every x number of seconds Up To 60
void timer_startMaintenanceTimer(void);
void timer_stopMaintenanceTimer(void);
void timer_setMaintenancePeriod(uint8_t seconds);

#endif /* TIMERS_H_ */
