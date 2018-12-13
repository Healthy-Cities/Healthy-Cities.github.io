#include <SoftwareSerial.h>
#include "ArduinoJson.h"
#include "MQ7.h"

/* Set Bluetooth and co scanner addresses */
SoftwareSerial Bluetooth(10, 9); // RX, TX
MQ7 mq7(A0, 5.0);

/* Define pin addresses */
int co2Pin = 3;
int coPin  = 5;
int buzz   = 2;

/* Timer vars */
int uptime = 0;
int last = 0;

/* Data Storage */
int co2PPM = 0;
float coPPM = 0.0f;
String lastScan = "";

void setup() {
  /* Begin Serial and Bluetooth */
  Bluetooth.begin(9600);
  Serial.begin(9600);

  /* Set pin modes */
  pinMode(coPin, INPUT);
  pinMode(co2Pin, INPUT);
}

void loop() {
  /* Check if its time to scan (three seconds after the last one) */
  if(last >= 3) {
    /* Scan */
    scan();

    /* Log data */
    logData(); 

    /* Reset last timer */
    last = 0;
  }

  /* Update timers */
  uptime++;
  last++;
  
  /* Tiemr Delay */
  delay(1000);
}


void scan() {
  /* Resets last counter */
  last = 0;

  /* Updates PPm data */
  coPPM = mq7.getPPM();
  co2PPM = analogRead(co2Pin);

  /* Generates JSON */
  const size_t bufferSize = JSON_OBJECT_SIZE(4);
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.createObject();
  root["co2"] = co2PPM;
  root["co"] = coPPM;
  root["uptime"] = uptime;
  root["last"] = last;
  
  /* Outputs JSON */
  String output;
  root.printTo(output);
  lastScan = output;
}

/* Log last scan to Bluetooth and Serial */
void logData() {
  Serial.println(lastScan);
  Bluetooth.println(lastScan);
}
