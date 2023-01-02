# MSP430-Timer-Pool-Paradigm
This project creates a pool of dynamically allocated A timers in MSP430 Microcontrollers. Timers are requested, used, and freed/killed as needed.

1) Create a Timer pointer. msTimerA is the struct used for timer pointers:
      msTimerA* myTimerPtr;

2) Request a Timer to point to. You must specify a callback ( a function that gets called when timer expires ) :
      myTimerPtr = timer_requestTimer(myCallbackFunction);
      
      Your callback function must return true to exit LPM4 or false to not wake CPU. 
      Theoretically you can just pass a bool if you don't have a callback function.
      When passing callback, do not include () and do not pass arguments.
      
3) Use Timer as needed:
      timer_startTimer(numberOfMilliseconds, myTimerPtr);
      timer_stopTimer(myTimerPtr);
      timer_resetTimer(myTimerPtr);
      
      Resetting a timer clears the interrupt for that timer. This is done automatically upon exiting actual ISR which occurs after callback ( SWI -> HWI/ISR ).
      Compared to RTOS, callback is similar to Software Interrupt (SWI) and Interrupt Subroutine (ISR) is similar to Hardware Interrupt (HWI).
      
4) Kill or Reuse Timer when done:
      timer_killTimer(&myTimerPtr);
      
      Killing a Timer sets the Timer Pointer to 0 and frees the actual Timer being used.
      
      
When a timer expires, the callback specified is called. Your callback must return a bool ( true or false ). If you decide to return true, the actual ISR will exit Low Power Mode. If your callback returns false, the ISR will not attempt to exit Low Power Mode.

This Timer Pool paradigm is designed to mimic the way a Real Time Operating System (RTOS) would work. We are mimicing one because we cannot use RTOS on 16-bit microcontrollers:

    Timer is requested -> Timer is Started/Stopped/Used -> Timer Times Out -> Callback Fired (SWI) -> ISR Fires (HWI) -> Timer is Reused or Killed 
