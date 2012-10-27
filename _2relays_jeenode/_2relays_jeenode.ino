// Small demo for the Relay Plug, receives wireless packets and sets relays
// 2010-07-05 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

#include <JeeLib.h>

Port relay1 (1);
Port relay2 (2);

void setup () {
    rf12_initialize(17, RF12_868MHZ, 212);
    
    relay1.digiWrite(0);
    relay1.mode(OUTPUT);
    relay1.digiWrite2(0);
    relay1.mode2(OUTPUT);
    relay2.digiWrite(0);
    relay2.mode(OUTPUT);
    relay2.digiWrite2(0);
    relay2.mode2(OUTPUT);
}

void loop () {
    if (rf12_recvDone() && rf12_crc == 0 && rf12_len == 3) {
      if (rf12_data[0] == 1) {
        relay1.digiWrite(rf12_data[1]);
        relay1.digiWrite2(rf12_data[2]);
      }
      if (rf12_data[0] == 2) {
        relay2.digiWrite(rf12_data[1]);
        relay2.digiWrite(rf12_data[2]);
      }
    }
}
