#define F_CPU 16000000UL    // 16Mhz
#define TIMER_TARGET_OFF 61UL   // 61 * 16.32ms = 1000ms
#define TIMER_TARGET_ON 64UL   // 64 * 16.32ms = 1050ms
#define IN_PIN PB1
#define OUT_PIN PB3

#ifndef __AVR_ATtiny45__ 
#define __AVR_ATtiny45__ 
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned char timer_count=0;

// Timer0 interrupt vector, called when the 8 bit counter TCNT0 overflows (every 16.32ms | 1 / (F_CPU / 1024) * 255)
// Simulates a button press/depress after TIMER_TARGET's are reached
ISR(TIMER0_OVF_vect) { 
  if (timer_count == TIMER_TARGET_OFF) {                    // 1000ms have passed
    PORTB = (PORTB & ~_BV(OUT_PIN)) |                       // Clear OUT_PIN
            (((PINB & _BV(IN_PIN)) >> IN_PIN) << OUT_PIN);  // Set OUT_PIN if IN_PIN is set
  } else if (timer_count == TIMER_TARGET_ON) {              // 1050ms have passed
    PORTB |= _BV(OUT_PIN);                                  // Set OUTPUT pin high
    TIMSK &= ~(_BV(TOIE0));                                 // Disable timer0 interrupt
    return;
  } 
  timer_count++;
}

int main(void) {
  // Configure input / output pins
  DDRB &= ~_BV(IN_PIN);                                     // Set IN_PIN as input
  DDRB |= _BV(OUT_PIN);                                     // Set OUT_PIN as output
  PORTB |= _BV(OUT_PIN);                                    // Set OUT_PIN high

  // Configure interrupts
  TCCR0A = 0;                                               // Enable timer interrupt in normal mode
  TCCR0B = 0;
  TCCR0B |= _BV(CS00) | _BV(CS02);                          // Prescale timer (Divide clock) by 1024
  TCNT0 = 0;                                                // Reset counter to 0
  TIMSK |= _BV(TOIE0);                                      // Enable timer0 interrupt

  sei();                                                    // Enable interrupts

  while(1){}

  return 0;
}

