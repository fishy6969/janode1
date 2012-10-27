// Demo sketch for an LCD connected to I2C port via MCP23008 I/O expander
// 2009-10-31 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

#include <PortsLCD.h>
#include <JeeLib.h>

PortI2C myI2C (4);
LiquidCrystalI2C lcd (myI2C);

#define screen_width 16
#define screen_height 2
#define myNodeID 8      // RF12 node ID in the range 1-30
#define network 212      // RF12 Network group
#define freq RF12_868MHZ // Frequency of RFM12B module
int nodeID; 
typedef struct {
   	  int temp;	// Temperature reading
     	  byte humidity;	// Humidity reading
       	  byte lobat :1;	// Supply voltage
 } Payload;

 Payload tinytx;
 
void setup() {
  // set up the LCD's number of rows and columns: 
  lcd.begin(screen_width, screen_height);
  // Print a message to the LCD.
  lcd.print("Hello!");
  rf12_initialize(myNodeID,freq,network);
  delay(2000);
}

void loop() {
  if (rf12_recvDone() && rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0) {
  nodeID = rf12_hdr & 0x1F;  // get node ID
  tinytx = *(Payload*) rf12_data;
  if (nodeID == 18) {lcd.setCursor(0,0);}
  if (nodeID == 19) {lcd.setCursor(0,1);}
  int temps = tinytx.temp;
  byte hum = tinytx.humidity;
  
  lcd.print(nodeID);
  lcd.print(" ");
  lcd.print(temps);
  lcd.print("C ");
  lcd.print(hum);
  lcd.print("%");
  }}

