/*
 * Copyright (c) 2018, circuits4you.com
 * All rights reserved.
 * Create a TCP Server on ESP8266 NodeMCU. 
 * TCP Socket Server Send Receive Demo
*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "Arduino.h"
#include "Motor.h"
#include "Options.h"

//Server connect to WiFi Network
#define AP_SSID "AP_bird" //改成你的AP帳號
#define AP_PSW "lm123456" //改成你的密碼

const char* ssid = AP_SSID;
const char* password = AP_PSW;

WiFiUDP Udp; // Creation of wifi Udp instance

char packetBuffer[255];
int datafromV7RC[8]={1500,1500,1500,1500,1500,1500,1500,1500};
uint8_t rudder = 0;
uint8_t throttle = 0;

unsigned int localPort = 6188;

//=======================================================================
//                    Power on setup
//=======================================================================
void setup() 
{
  Serial.begin(115200);
  Serial.setDebugOutput(false);

  init_motor();

  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("Connect to the ESP8266 AP and go to: http://");
  Serial.println(IP);

  ////UDP Here ... 
  Udp.begin(localPort);
}
//=======================================================================
//                    Loop
//=======================================================================

void loop() 
{
  if (0 == WiFi.softAPgetStationNum())
  {
    /* When there is no client connected, turn off throttle and set rudder to 90 degree */
    throttle = 0;
    rudder = 90;
#ifdef __LOG__
    Serial.print("Waiting for clients...., current number of clients is ");
    Serial.println(WiFi.softAPgetStationNum());
#endif /* __LOG__ */
    motor_control(rudder, throttle);
  }
  else
  {
    int packetSize = Udp.parsePacket();
    if (packetSize) {
      String rxData;
      String data;
      int len = Udp.read(packetBuffer, 255);
      if (len > 0) packetBuffer[len-1] = 0;

#ifdef __LOG__
      Serial.println(packetBuffer);
      Serial.println(len);
#endif /* __LOG__ */

      /* reply received data to transmitter for debugging */
      Udp.beginPacket(Udp.remoteIP(),Udp.remotePort());
      Udp.printf("received: ");
      Udp.printf(packetBuffer);
      Udp.printf("\r\n");
      Udp.endPacket();

      /* convert received data to string */
      if (len > 0) {
        for (int i = 0; i < len; i++){
          rxData+=packetBuffer[i];
        }
      }

      /* parse received string to V7RC commands */
      if(packetBuffer[1]=='R'){
        for(int i=0;i<4;i++){
          data = rxData.substring(i*4+3, i*4+7); 
          datafromV7RC[i] = data.toInt();
        }
      }

#ifdef __LOG__
      Serial.print(packetBuffer[2]);  /// should be V / T / 8 (2 ch, 4 ch , 8 ch )
      Serial.print(",");
      for(int i=0;i<8;i++){
        Serial.print(datafromV7RC[i]);
        Serial.print(",");
      }
      Serial.println(",");
      Serial.println();
#endif /* __LOG__ */

      /* mapping V7RC commands to receiver throttle/rudder commands */
      rudder = map(datafromV7RC[0], 1000, 2000, 0, 180);
      throttle = map(datafromV7RC[1], 1000, 2000, 0, 255);
      motor_control(rudder, throttle);
    }
  }
}
//=======================================================================
