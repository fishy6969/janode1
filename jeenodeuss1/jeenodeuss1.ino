#include <DallasTemperature.h>

#include <OneWire.h>

#include <Ports.h>
#include <JeeLib.h>
#include <RF12.h>
#define ONE_WIRE_BUS 5


// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
struct {
   int temp; // temperature: -500..+500 (tenths)
   byte distance; 
} payload;

Port pingPin (1);

void setup() {
  // initialize serial communication:
  Serial.begin(57600);
  Serial.print("Starting Up");
  Serial.println();
  rf12_initialize(6, RF12_868MHZ, 212);
  sensors.begin();
}

void loop()
{
  // establish variables for duration of the ping,
  // and the distance result in inches and centimeters:
  
  
  long duration, cm;

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pingPin.mode(OUTPUT);
  pingPin.digiWrite(LOW);
  delayMicroseconds(2);
  pingPin.digiWrite(HIGH);
  delayMicroseconds(5);
  pingPin.digiWrite(LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pingPin.mode(INPUT);
  duration = pingPin.pulse(1);

  // convert the time into a distance
  //sensors.requestTemperatures();
  cm = microsecondsToCentimeters(duration);
  payload.distance = byte(cm);
  //payload.temp = sensors.getTempCByIndex(0);
  payload.temp = 0;
  Serial.print(payload.distance);
  Serial.print("cm");
  Serial.println();
  Serial.print(payload.temp);
  Serial.println();
  //rf12_sleep(RF12_WAKEUP);
  while (!rf12_canSend())
       rf12_recvDone();
  rf12_sendStart(0, &payload, sizeof payload);
  //rf12_sleep(RF12_SLEEP);
  delay(10000);
}


long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}
