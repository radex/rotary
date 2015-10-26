/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://www.arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */

 
#define IN_USE 0
#define HANDLE_PRESSED 0

int pulsarPort = 11;
int switchPort = 12;
int phonePort = 10;

int prevPulsar = 0; // store previous value od puls
bool prevInUse = !IN_USE; 
bool prevHandle = !HANDLE_PRESSED;

int pulseCount = 0;

void setup() {
  // initialize digital pin 13 as an output.
  Serial.begin(9600);
  
  pinMode(pulsarPort, INPUT);
  digitalWrite(pulsarPort, HIGH);
  
  pinMode(switchPort, INPUT);
  digitalWrite(switchPort, HIGH);
  
  pinMode(phonePort, INPUT);
  digitalWrite(phonePort, HIGH);
  
  delay(1);
}

// the loop function runs over and over again forever
void loop() {
  int pulsar = digitalRead(pulsarPort);
  bool inUse = digitalRead(switchPort);
  bool handle = digitalRead(phonePort);
  

  if(inUse != prevInUse || pulsar != prevPulsar || handle != prevHandle){
    stateChanged(pulsar, inUse, handle);
    prevHandle = handle;
    prevPulsar = pulsar;
    prevInUse = inUse;
  }


  delay(10);
}

void stateChanged(int pulsar, bool inUse, bool handle) {
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


  if(handle == HANDLE_PRESSED) {
    Serial.println("Handle is not in use");
  } else {
    Serial.println("Handle is in use");
  }
}

