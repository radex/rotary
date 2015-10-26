#define IN_USE 0
#define HANDLE_LIFTED 1

int pulsarPort = 11;
int switchPort = 12;
int phonePort = 10;

int prevPulsar;
bool prevInUse; 
bool prevHandle;

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