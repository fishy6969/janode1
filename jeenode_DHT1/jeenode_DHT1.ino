#include <DHT22.h>
// Only used for sprintf
#include <stdio.h>
#include <JeeLib.h> // https://github.com/jcw/jeelib

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

// Data wire is plugged into port 7 on the Arduino
// Connect a 4.7K resistor between VCC and the data pin (strong pullup)
#define DHT22_PIN 7
#define DHT22_POWER 4

#define myNodeID 18    // RF12 node ID in the range 1-30
#define network 212      // RF12 Network group
#define freq RF12_868MHZ // Frequency of RFM12B module
// Setup a DHT22 instance
DHT22 myDHT22(DHT22_PIN);


 typedef struct {
   	  int temp;	// Temperature reading
     	  byte humidity;	// Humidity reading
       	  byte lobat :1;	// Supply voltage
 } Payload;

 Payload tinytx;

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("DHT22 Library Demo");
  rf12_initialize(myNodeID,freq,network); // Initialize RFM12 with settings defined above 
  rf12_sleep(0);                          // Put the RFM12 to sleep
  pinMode(DHT22_POWER, OUTPUT); // set power pin for DHT to output

}

void loop(void)
{ 
  digitalWrite(DHT22_POWER, HIGH); // turn DHT sensor on
  DHT22_ERROR_t errorCode;
  
  // The sensor can only be read from every 1-2s, and requires a minimum
  // 2s warm-up after power-on.
  delay(2000);
  
  Serial.print("Requesting data...");
  errorCode = myDHT22.readData();
  switch(errorCode)
  {
    case DHT_ERROR_NONE:
    tinytx.temp = myDHT22.getTemperatureC(); // Get temperature reading and convert to integer, reversed at receiving end
    
    tinytx.humidity = byte(myDHT22.getHumidity()); // Get humidity reading and convert to integer, reversed at receiving end

    //tinytx.supplyV = readVcc(); // Get supply voltage
    tinytx.lobat = rf12_lowbat();
    rfwrite(); // Send data via RF 

      Serial.print("Got Data ");
      Serial.print(tinytx.temp);
      Serial.print("C ");
      Serial.print(tinytx.humidity);
      Serial.println("%");
      // Alternately, with integer formatting which is clumsier but more compact to store and
	  // can be compared reliably for equality:
	  //	  
      //char buf[128];
      //sprintf(buf, "Integer-only reading: Temperature %hi.%01hi C, Humidity %i.%01i %% RH",
                   //myDHT22.getTemperatureCInt()/10, abs(myDHT22.getTemperatureCInt()%10),
                  // myDHT22.getHumidityInt()/10, myDHT22.getHumidityInt()%10);
      //Serial.println(buf);
      break;
    case DHT_ERROR_CHECKSUM:
      Serial.print("check sum error ");
      Serial.print(myDHT22.getTemperatureC());
      Serial.print("C ");
      Serial.print(myDHT22.getHumidity());
      Serial.println("%");
      break;
    case DHT_BUS_HUNG:
      Serial.println("BUS Hung ");
      break;
    case DHT_ERROR_NOT_PRESENT:
      Serial.println("Not Present ");
      break;
    case DHT_ERROR_ACK_TOO_LONG:
      Serial.println("ACK time out ");
      break;
    case DHT_ERROR_SYNC_TIMEOUT:
      Serial.println("Sync Timeout ");
      break;
    case DHT_ERROR_DATA_TIMEOUT:
      Serial.println("Data Timeout ");
      break;
    case DHT_ERROR_TOOQUICK:
      Serial.println("Polled to quick ");
      break;
  }
  digitalWrite(DHT22_POWER, LOW); // turn DS18B20 off
  Sleepy::loseSomeTime(60000);
}

static void rfwrite(){
   rf12_sleep(-1);     //wake up RF module
   while (!rf12_canSend())
   rf12_recvDone();
   rf12_sendStart(0, &tinytx, sizeof tinytx); 
   rf12_sendWait(2);    //wait for RF to finish sending while in standby mode
   rf12_sleep(0);    //put RF module to sleep
}
