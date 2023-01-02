/*
 * Timers.c
 *
 *  Creates a pool of A timers. When a Timer is needed, it is requested, used, and killed.
 *
 */

// STD C
#include <stdbool.h>
#include <stdlib.h>

// TI Libraries
#include <msp430.h>
#include "timer_a.h"
#include "timer_b.h"

// Custom Libraries
#include <Timers.h>

uint16_t timeSinceMaintInt = 0, maintPeriodSec = 300; //this is waiting for setup // 5 min // this changes to 900 after setup happens



//array of all available timer A modules
msTimerA TimerList[5];


/* - - - - Timer Pool - - - - */

void timer_initTimerpool(void)
{
	int16_t i;

	for(i=0; i<5; i++)
	{
		TimerList[i].Callback = NULL;
		TimerList[i].inUse = false;
	}

	TimerList[0].timerName = TIMER_A0_BASE;
	TimerList[1].timerName = TIMER_A1_BASE;
	TimerList[2].timerName = TIMER_A2_BASE;
	TimerList[3].timerName = TIMER_A3_BASE;
	TimerList[4].timerName = TIMER_A4_BASE;

}

msTimerA * timer_requestTimer(bool (*Callback)())
{
    int16_t i;
	msTimerA * tPtr = NULL;

	for(i=0; i<5; i++)
	{
		if(TimerList[i].inUse == false)
		{
			tPtr = &TimerList[i];
			TimerList[i].inUse = true;
			TimerList[i].Callback = Callback;
			break;
		}

	}
	return tPtr;
}

void timer_startTimer(uint16_t Period_ms, msTimerA * timer)
{
    /*	using Aclk, at 1khz.. or actually about 1024counts per second
     * TIMER_A0_BASE	TIMER0_A1_VECTOR
     * TIMER_A1_BASE	TIMER1_A1_VECTOR
     * TIMER_A2_BASE	TIMER2_A1_VECTOR
     * TIMER_A3_BASE	TIMER3_A1_VECTOR
     * TIMER_A4_BASE	TIMER4_A1_VECTOR
     */

	if(timer !=0)
	{
		Timer_A_initUpModeParam tParams;
		tParams.clockSource = TIMER_A_CLOCKSOURCE_ACLK;  //in init clock, we set ACLK to the XTLF
		tParams.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_32; //puts timer at around 1024 counts per second
		tParams.timerPeriod = Period_ms;
		tParams.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE; //dont enable interrup
		tParams.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE; //disable Capture an compare interrupt {this is default}
		tParams.timerClear = TIMER_A_DO_CLEAR;
		tParams.startTimer = true;

		//Timer_A_clear(uint16_t baseAddress);  //done in the init params
		Timer_A_initUpMode(timer->timerName,&tParams);
		//Timer_A_enableInterrupt(uint16_t baseAddress); //done in the init params
		//Timer_A_startCounter(uint16_t baseAddress, TIMER_A_UP_MODE); //done in init params
		Timer_A_clearTimerInterrupt(timer->timerName); //added because Im not sure this is done in initupMode
	}
}

void timer_stopTimer(msTimerA * timer)
{
	if(timer !=0)
	{
		Timer_A_stop(timer->timerName);
	}
}

void timer_resetTimer(msTimerA * timer)
{
	if(timer !=0)
	{
		Timer_A_clearTimerInterrupt(timer->timerName);
	}
}

void timer_freeTimer(msTimerA * timer)
{
	if(timer !=0)
	{
		timer->inUse = false;
	}
}

void timer_killTimer(msTimerA** ptrToTimer){

	if(*ptrToTimer !=0)
	{
		timer_stopTimer(*ptrToTimer);
		timer_freeTimer(*ptrToTimer);
		*ptrToTimer = 0;
	}

}

//DONT EDIT THESE
#pragma vector=	TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
{
	Timer_A_clearTimerInterrupt(TIMER_A0_BASE);
	if(TimerList[0].Callback != NULL)
		if(TimerList[0].Callback())
			__bic_SR_register_on_exit(LPM4_bits);     // Exit LPM
}

#pragma vector=	TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR(void)
{
	Timer_A_clearTimerInterrupt(TIMER_A1_BASE);
	if(TimerList[1].Callback != NULL)
		if(TimerList[1].Callback())
			__bic_SR_register_on_exit(LPM4_bits);     // Exit LPM
}

#pragma vector=	TIMER2_A1_VECTOR
__interrupt void TIMER2_A1_ISR(void)
{
	Timer_A_clearTimerInterrupt(TIMER_A2_BASE);
	if(TimerList[2].Callback != NULL)
		if(TimerList[2].Callback())
			__bic_SR_register_on_exit(LPM4_bits);     // Exit LPM
}

#pragma vector=	TIMER3_A1_VECTOR
__interrupt void TIMER3_A1_ISR(void)
{
	Timer_A_clearTimerInterrupt(TIMER_A3_BASE);
	if(TimerList[3].Callback != NULL)
		if(TimerList[3].Callback())
			__bic_SR_register_on_exit(LPM4_bits);     // Exit LPM
}

#pragma vector=	TIMER4_A1_VECTOR
__interrupt void TIMER4_A1_ISR(void)
{
	Timer_A_clearTimerInterrupt(TIMER_A4_BASE);
	if(TimerList[4].Callback != NULL)
		if(TimerList[4].Callback())
			__bic_SR_register_on_exit(LPM4_bits);     // Exit LPM
}



/* - - - - Maintenance Timer - - - - */

void timer_startMaintenanceTimer(void)
{
	Timer_B_initUpModeParam tParams;

	//startup timerb0 to interrupt at about 1 sec to use as heartbeat
	tParams.clockSource = TIMER_B_CLOCKSOURCE_ACLK;  //in init clock, we set ACLK to the XTLF
	tParams.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_32; //puts timer at around 1024 counts per second
	tParams.timerPeriod = 1024;	//makes it about 1 sec between interrupts
	tParams.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_ENABLE; //dont enable interrup
	tParams.timerClear = TIMER_B_DO_CLEAR;
	tParams.startTimer = true;

	Timer_B_initUpMode(TIMER_B0_BASE, &tParams);
	Timer_B_clearTimerInterrupt(TIMER_B0_BASE); //added because Im not sure this is done in initupMode
}

void timer_stopMaintenanceTimer(void)
{
	Timer_B_stop(TIMER_B0_BASE);
}

void timer_setMaintenancePeriod(uint8_t seconds)
{
	maintPeriodSec = seconds;
}


//for maintenance timer
#pragma vector=	TIMER0_B1_VECTOR
__interrupt void TIMER0_B1_ISR(void)
{
	Timer_B_clearTimerInterrupt(TIMER_B0_BASE);
	timeSinceMaintInt++;

	if (timeSinceMaintInt >= maintPeriodSec)
	{
			timeSinceMaintInt = 0;
//			addNewEvent(MaintenanceTimer);    Uncomment this if you are using event handling paradigm http://github.com/ TODO add repo

			__bic_SR_register_on_exit(LPM4_bits);     // Exit LPM
	}
}


