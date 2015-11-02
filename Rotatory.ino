#define IN_USE 0
#define HANDLE_LIFTED 1

#define SAMPLE_RATE 8000
#define BUFF_SIZE 400

#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "sounddata.h"

int speakerPin = 3;
volatile uint16_t sample = 0;

volatile unsigned char  sound_buffer[400];
volatile uint16_t sound_offset = 0;

int pulsarPort = 11;
int switchPort = 12;
int phonePort = 10;

int prevPulsar;
bool prevInUse; 
bool prevHandle;

int pulseCount = 0;

void stopPlayback()
{
    // Disable playback per-sample interrupt.
    TIMSK1 &= ~_BV(OCIE1A);

    // Disable the per-sample timer completely.
    TCCR1B &= ~_BV(CS10);

    // Disable the PWM timer.
    TCCR2B &= ~_BV(CS10);

    digitalWrite(speakerPin, LOW);
}

// This is called at 8000 Hz to load the next sample.
ISR(TIMER1_COMPA_vect) {
  
    if(sample >= BUFF_SIZE) {
      if(sound_offset >= 10000) { sound_offset = 0; }
      
        copyArray(sound_buffer, 400);
        sound_offset += 400;
        sample = 0;
        //Serial.print("offset = ");
        //Serial.println(sound_buffer[0]);
    } else {
            OCR2B = sound_buffer[sample];     
//            OCR2B = pgm_read_byte(&sounddata_data[sound_offset + sample]);
//            //Serial.println(sound_buffer[sample]);  
    }

    ++sample;
}

void startPlayback()
{
    pinMode(speakerPin, OUTPUT);

    // Set up Timer 2 to do pulse width modulation on the speaker
    // pin.

    // Use internal clock (datasheet p.160)
    ASSR &= ~(_BV(EXCLK) | _BV(AS2));

    // Set fast PWM mode  (p.157)
    TCCR2A |= _BV(WGM21) | _BV(WGM20);
    TCCR2B &= ~_BV(WGM22);

  
    // Do non-inverting PWM on pin OC2B (p.155)
    // On the Arduino this is pin 3.
    TCCR2A = (TCCR2A | _BV(COM2B1)) & ~_BV(COM2B0);
    TCCR2A &= ~(_BV(COM2A1) | _BV(COM2A0));
    // No prescaler (p.158)
    TCCR2B = (TCCR2B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);

    // Set initial pulse width to the first sample.
    OCR2B = pgm_read_byte(&sounddata_data[0]);
   
    // Set up Timer 1 to send a sample every interrupt.

    cli();

    // Set CTC mode (Clear Timer on Compare Match) (p.133)
    // Have to set OCR1A *after*, otherwise it gets reset to 0!
    TCCR1B = (TCCR1B & ~_BV(WGM13)) | _BV(WGM12);
    TCCR1A = TCCR1A & ~(_BV(WGM11) | _BV(WGM10));

    // No prescaler (p.134)
    TCCR1B = (TCCR1B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);

    // Set the compare register (OCR1A).
    // OCR1A is a 16-bit register, so we have to do this with
    // interrupts disabled to be safe.
    OCR1A = F_CPU / SAMPLE_RATE;    // 16e6 / 8000 = 2000

    // Enable interrupt when TCNT1 == OCR1A (p.136)
    TIMSK1 |= _BV(OCIE1A);
    
    sample = 0;
    sei();

    copyArray(sound_buffer, 400);
}


void setup() {
  // initialize digital pin 13 as an output.
  Serial.begin(230400);
  
  pinMode(pulsarPort, INPUT);
  digitalWrite(pulsarPort, HIGH);
  
  pinMode(switchPort, INPUT);
  digitalWrite(switchPort, HIGH);
  
  pinMode(phonePort, INPUT);
  digitalWrite(phonePort, HIGH);

  // sonund staff

  startPlayback();
  //end
  
  delay(1);

  prevPulsar = digitalRead(pulsarPort);
  prevInUse = digitalRead(switchPort);
  prevHandle = digitalRead(phonePort);

  
}

// the loop function runs over and over again forever
void loop() {
  int pulsar = digitalRead(pulsarPort);
  bool inUse = digitalRead(switchPort);
  bool handle = digitalRead(phonePort);

  if (handle != prevHandle) {
    prevHandle = handle;
    handleStateChanged(handle);
  }

  if (inUse != prevInUse || pulsar != prevPulsar) {
    prevPulsar = pulsar;
    prevInUse = inUse;

    if (handle == HANDLE_LIFTED) {
      dialStateChanged(pulsar, inUse);
    }
  }
  
  delay(10);
}

void dialStateChanged(int pulsar, bool inUse) {
//  Serial.print("in use: ");
//  Serial.print(!inUse);
//  Serial.print(", puls: ");
//  Serial.println(pulsar);

  if(pulsar == 1) {
    pulseCount++;
//    Serial.print("Licznik pulsow: ");
//    Serial.println(pulseCount);
  }

  if(inUse == !IN_USE) {
//    Serial.print("Koniec: ");
    Serial.println(pulseCount);
    pulseCount = 0;
  }
}

void handleStateChanged(int handle) {
  if (handle == HANDLE_LIFTED) {
    Serial.println("Handle was lifted");
  } else {
    Serial.println("Handle was put down");
    pulseCount = 0;
  }
}

void copyArray(volatile unsigned char * dest, uint16_t n) {
     for(uint16_t i = 0; i < n; i++){
//        Serial.println(pgm_read_byte(&sound_buffer[sample]));
        dest[i] = pgm_read_byte(&sounddata_data[sound_offset + i]);
     }
}



