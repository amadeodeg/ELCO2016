#include <SPI.h>
#include "RF24.h"
#include "printf.h"

RF24 radio(9,10);
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

bool TX=1,RX=0,role=0;

void setTransmitter(){
  radio.begin();                           // Setup and configure rf radio
  radio.setChannel(1);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.setAutoAck(1);                     // Ensure autoACK is enabled
  radio.setRetries(15,15);                  // Optionally, increase the delay be$
  radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance
    role = TX;
    radio.openWritingPipe(pipes[0]);
  }
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  setTransmitter();
  

}
void transmit(){
  unsigned long time = millis();
  Serial.print("Now sending ...");
  Serial.println(time);
  bool ok = radio.write( &time, sizeof(unsigned long) );
  if (ok)
      Serial.println("ok...");
    else
      Serial.println("failed");
  }
  
  
void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  transmit();

}
