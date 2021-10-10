#include <Arduino.h>

// variables
unsigned int digit, sec, ms, value, percentage;

// constants
const char full = 6;
const char bottom = 40;
const char ADC3 = 3;

void init_timer() {
  SREG |= (1<<7);  // global interrup enable
  TCCR0A = 0b00000010;  // CTC mode enabled
  TCCR0B = 0b00000011;  // 64 prescaler
  TCNT0 = 0;
  OCR0A = 50;
  TIMSK0 |= (1 << 1);  // compare match interrupt enable
}

void init_PWM() {
  DDRB |= (1 << 1);  // PB1 ouput
  TCCR1A = 0b10000010;  // fast PWM mode, non-inverting mode
  TCCR1B = 0b00011101;  // 128 prescaler
  ICR1 = 312;
  OCR1A = bottom;
  //TCCR1A |= 0b10000000;  
}

void init_ADC() {
  ADMUX = 0b01000000;  // AVCC referece
  ADCSRA = 0b10001111;  // 128 prescaler
}

void read_ADC(char ch) {
  ADMUX &= 0b11100000;
  ADMUX |= ch;  // configure ADCch
  ADCSRA |= (1 << 6);  // start ADC
}

ISR(TIMER0_COMPA_vect) { 
  unsigned int position = 0;

  read_ADC(ADC3);
  position = bottom - (percentage/100.0)*34; 
  if (OCR1A <= position - 2 || OCR1A >= position + 2) {
    OCR1A = position;
  }
  
  if (ms >= 999) {
    ms = 0;
  } else ms++;
}

ISR(ADC_vect) {
  unsigned int adc_l = 0, adc_h = 0;
  adc_l = ADCL;
  adc_h = ADCH;
  value = (adc_h << 8) | adc_l;
  percentage = (value/1023.0)*100;
}

void setup() {
  // put your setup code here, to run once:
  DDRC &= ~(1 << ADC3);  // PC3 input
  init_timer();
  init_ADC();
  init_PWM();
  Serial.begin(9600);
  Serial.print("Initializare...");
}

void loop() {
  // put your main code here, to run repeatedly:
}