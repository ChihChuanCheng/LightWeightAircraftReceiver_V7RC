/*
 * Copyright (c) 2018, circuits4you.com
 * All rights reserved.
 * Create a TCP Server on ESP8266 NodeMCU. 
 * TCP Socket Server Send Receive Demo
*/

#include <ESP8266WiFi.h>
#include "Options.h"
#include "Motor.h"

//Server connect to WiFi Network
#define AP_SSID "AP_bird" //改成你的AP帳號
#define AP_PSW "lm654321" //改成你的密碼

IPAddress local_IP(192,168,4,1);   //IP地址
IPAddress gateway(192,168,4,9);     //Gateway IP地址
IPAddress subnet(255,255,255,0);    //網路遮罩

int port = 80;  //Port number
WiFiServer server(port);

//=======================================================================
//                    Power on setup
//=======================================================================
void setup() 
{
  Serial.begin(115200);
  Serial.println();
  delay(2000);

  WiFi.mode(WIFI_AP); 
  delay(2000);

  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "success" : "failure");

  Serial.print("setting AP......");
  bool result = WiFi.softAP(AP_SSID, AP_PSW);

  if(result){ 
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(AP_SSID);
  
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());  
    Serial.print(" on port ");
    Serial.println(port);

    server.begin();
    Serial.print("Socket server status: ");
    Serial.println(server.status()); 
    delay(100);
  }
  else
  {
    Serial.println("softAP setting failed...");
  }

  init_motor();
}
//=======================================================================
//                    Loop
//=======================================================================

String getValue(String data, char separator, int index);

void loop() 
{
  
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("Client Available");
    if(client.connected())
    {
      Serial.println("Client Connected");
    }
    
    while(client.connected()){      
      while(client.available()>0){
        String line = client.readStringUntil('\n');
        String rudder = getValue(line,',',0);
        String throttle = getValue(line,',',1);
 #ifdef __LOG__
        Serial.print("Read string; ");
        Serial.print(rudder + " , " + throttle);
        Serial.println();
        Serial.println(line);
 #endif /* __LOG__ */
        motor_control(atoi(rudder.c_str()), atoi(throttle.c_str()));
      }
      //Send Data to connected client
      while(Serial.available()>0)
      {
        client.write(Serial.read());
      }
    }
    client.stop();
    Serial.println("Client disconnected");    
    init_motor();
  }
  delay(5);
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
//=======================================================================
