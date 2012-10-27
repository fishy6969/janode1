#include <LiquidCrystal.h>

#include <NanodeUNIO.h>
#include <NanodeUIP.h>
#include <NanodeMQTT.h>

NanodeMQTT mqtt(&uip);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void message_callback(const char* topic, uint8_t* payload, int payload_length)
{
  Serial.print(topic);
  Serial.print(" => ");
  Serial.write(payload, payload_length);
  Serial.println();
  lcd.clear();
  lcd.write(payload, payload_length);
}

void setup() {
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");
  byte macaddr[6];
  NanodeUNIO unio(NANODE_MAC_DEVICE);

  Serial.begin(9600);
  Serial.println("MQTT Subscribe test");
  
  unio.read(macaddr, NANODE_MAC_ADDRESS, 6);
  uip.init(macaddr);

  // FIXME: use DHCP instead
  uip.set_ip_addr(192, 168, 1, 51);
  uip.set_netmask(255, 255, 255, 0);

  uip.wait_for_link();
  Serial.println("Link is up");
  lcd.print("cooking on gas!");

  // FIXME: resolve using DNS instead
  mqtt.set_server_addr(192, 168, 1, 104);
  mqtt.set_callback(message_callback);
  mqtt.connect();
  Serial.println("Connected to MQTT server");
  lcd.print("the gas is connected");

  mqtt.subscribe("/#");
  Serial.println("Subscribed.");

  Serial.println("setup() done");
}

void loop() {
  uip.poll();
}
