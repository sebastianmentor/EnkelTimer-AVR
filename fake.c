#include <avr/io.h>
// AVR Timer CTC Interrupts Calculator
// v. 8
// http://www.arduinoslovakia.eu/application/timer-calculator
// Microcontroller: ATmega328P
// Created: 2025-05-08T11:53:51.589Z

#define ledPin 13

void setupTimer1() {
  noInterrupts();
  // Clear registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // 250 Hz (16000000/((249+1)*256))
  OCR1A = 249;

  // 50 Hz (16000000/((1249+1)*256))
  // OCR1A = 1249

  // 20ms --> 50Hz
  // 100ms --> 10Hz
  // 500ms --> 2Hz
  // 1 sec --> 1Hz
  // 2 sec --> 0.5Hz
  
  // CTC
  TCCR1B |= (1 << WGM12);
  // Prescaler 256
  TCCR1B |= (1 << CS12);
  // Output Compare Match A Interrupt Enable
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
}

void setup() {
  pinMode(ledPin, 0);
  setupTimer1();
}

void loop() {
}

ISR(TIMER1_COMPA_vect) {
  digitalWrite(ledPin, digitalRead(ledPin) ^ 1);
}