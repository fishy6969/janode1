#include <DHT22.h>
#include <PortsLCD.h>
#include <stdio.h>
#include <OneWire.h> // http://www.pjrc.com/teensy/arduino_libraries/OneWire.zip
#include <DallasTemperature.h> // http://download.milesburton.com/Arduino/MaximTemperature/DallasTemperature_371Beta.zi
#include <JeeLib.h> // https://github.com/jcw/jeelib
PortI2C myI2C (3);
LiquidCrystalI2C lcd (myI2C);

#define screen_width 16
#define screen_height 2
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

// Data wire is plugged into port 7 on the Arduino
// Connect a 4.7K resistor between VCC and the data pin (strong pullup)
#define DHT22_PIN1 7 //Jeenode Digital 4 - grn
#define DHT22_PIN2 4 //Jeenode Didital 1 - gdn
#define ONE_WIRE_BUS 17 //Jeenode Analog 4

#define screen_width 16
#define screen_height 2
#define myNodeID 18    // RF12 node ID in the range 1-30
#define network 212      // RF12 Network group
#define freq RF12_868MHZ // Frequency of RFM12B module
// Setup a DHT22 instance
DHT22 grnDHT22(DHT22_PIN1);
DHT22 gdnDHT22(DHT22_PIN2);
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance

DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature


 typedef struct {
   	  int grntemp;	// Temperature reading
     	  byte grnhumid;	// Humidity reading
       	  int gdntemp;
          byte gdnhumid;
          int soiltemp;        
 } Payload;

 Payload tinytx;

void setup(void)
{
  lcd.begin(screen_width, screen_height);
  lcd.print("Garden Node!");
  rf12_initialize(myNodeID,freq,network); // Initialize RFM12 with settings defined above 
  rf12_sleep(0);                          // Put the RFM12 to sleep
  delay(3000);
  pinMode(A0, OUTPUT); //jeenode analog port 1 for power

}

void loop(void)
{ tinytx.soiltemp = 0;
  digitalWrite(A0, HIGH);
  delay(3000);
  // The sensor can only be read from every 1-2s, and requires a minimum
  // 2s warm-up after power-on.
  
  
  grnDHT22.readData();
  tinytx.grntemp = grnDHT22.getTemperatureC(); // Get temperature reading and convert to integer, reversed at receiving end 
  tinytx.grnhumid = byte(grnDHT22.getHumidity()); // Get humidity reading and convert to integer, reversed at receiving end
  delay(20);
  gdnDHT22.readData();  
  tinytx.gdntemp = gdnDHT22.getTemperatureC();
  tinytx.gdnhumid = byte(gdnDHT22.getHumidity()); // Get humidity reading and convert to integer, reversed at receiving end
  delay(20);
  //sensors.begin(); //start up temp sensor
  //sensors.requestTemperatures(); // Get the temperature  
  //tinytx.soiltemp=(sensors.getTempCByIndex(0));
  digitalWrite(A0, LOW);
  rfwrite(); // Send data via RF 
  lcd.setCursor(0,0);
  lcd.print("Grn ");
  lcd.print(tinytx.grntemp);
  lcd.print("C ");
  lcd.print(tinytx.grnhumid);
  lcd.print("% Soil");
  lcd.setCursor(0,1); 
  lcd.print("Gar ");
  lcd.print(tinytx.gdntemp);
  lcd.print("C ");
  lcd.print(tinytx.gdnhumid);
  lcd.print("%  "); 
  lcd.print(tinytx.soiltemp);
  lcd.print("C"); 
  for (byte i = 0; i < 1; ++i)  // Change the large number for the number of minutes between readings
      Sleepy::loseSomeTime(10000);
      
  
}

static void rfwrite(){
   rf12_sleep(-1);     //wake up RF module
   while (!rf12_canSend())
   rf12_recvDone();
   rf12_sendStart(0, &tinytx, sizeof tinytx); 
   rf12_sendWait(2);    //wait for RF to finish sending while in standby mode
   rf12_sleep(0);    //put RF module to sleep
}
