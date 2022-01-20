#define BLYNK_TEMPLATE_ID "template id from blynk.io"
#define BLYNK_DEVICE_NAME "template name from blynk.io"
#define BLYNK_AUTH_TOKEN "copied from blynk.io"
#define BLYNK_PRINT Serial
#define relayPin1 23 //P23 PC817 Optocoupler for door button
#define switchPin1 19 // P19 Magentic reed switch for door 1

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "MyWifi";
char pass[] = "MyPassword";

int buttonState1 = 0; // set inital button state to keep optocoupler Off
int stickyDisplay = 0; // Prevent the default message on the display to remove flashing
int snoozeAlert = 0; // Snooze the door mobile app alert
unsigned long doorTime; // start time of door open alert
unsigned long alertTime = 9000000; // time to alert mobile app door open in ms, eg 10 minutes = 600000, 15 minutes = 9000000

BlynkTimer timer;

BLYNK_WRITE(V1) {
  buttonState1 = param.asInt();
  if (buttonState1 == 1){
    digitalWrite(relayPin1, 1);
    updateDisplay("Door Button Pressed!");
    digitalWrite(relayPin1, 0);
  } 
}

BLYNK_WRITE(V23) {
    snoozeAlert = 1; // set value from snooze push button
}

BLYNK_CONNECTED() {
}

void doorSwitch() {
  if (digitalRead(switchPin1) == LOW){
    if (digitalRead(switchPin1) == LOW){
      stickyDisplay = 1;
      updateDisplay("Door is Open");
    }     
    if (doorTime == 0){
          doorTime = millis(); // record the for the beginning of door alarm
    }
    if ((millis()-doorTime) > alertTime){
      if (snoozeAlert != 1){
        Blynk.logEvent("dooralarm", "Garage door is open!");
      }
      doorTime = 0; // reset timer for next alert
    }
  } else {
    if (stickyDisplay == 1) {
      updateDisplay("Ready...");
    }
    stickyDisplay = 0;
    snoozeAlert = 0;
    doorTime = 0;
  }
}

void updateDisplay(String message){
    Blynk.virtualWrite(V21, message);        // display the message on the virtual LCD
    delayMod(2000);                          // pause for 2 seconds to keep message up
    if (stickyDisplay != 1){
      Blynk.virtualWrite(V21, "Ready...");     // return to default message
    }
}

void delayMod(unsigned long delayTime) {
  unsigned long startTime = millis();
  while (millis() - startTime < delayTime)
  {
    continue;
  }
  yield();
}

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);

  pinMode(relayPin1, OUTPUT);             // set gpio mode
  pinMode(switchPin1, INPUT_PULLUP);      // set gpio mode with pullup option to use passive switch
  
  digitalWrite(relayPin1, buttonState1);

  Blynk.virtualWrite(V21, "System Ready...");
  timer.setInterval(100, doorSwitch);
}

void loop() {
  Blynk.run();
  timer.run();
}
