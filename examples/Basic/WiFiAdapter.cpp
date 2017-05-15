/*
  HookAction.h - 
  Created by Nelo Pauselli, 16 de Febrero, 2017.
*/
#ifndef WiFiAdapter_Class
#define WiFiAdapter_Class

#include "Arduino.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define DEBUG_WIFI_ADAPTER
#ifdef DEBUG_WIFI_ADAPTER
 #define DEBUG_PRINT(...)  Serial.print (__VA_ARGS__)
 #define DEBUG_PRINTLN(...)  Serial.println (__VA_ARGS__)
#else
 #define DEBUG_PRINT(...)
 #define DEBUG_PRINTLN(...)
#endif

class WiFiAdapter
{
  public:
  WiFiAdapter(){ }

  bool connect(String ssid, String pwd) {
    if(ssid=="" || pwd=="") {
      DEBUG_PRINTLN("No hay red configurada");
      return false;
    }
  
    DEBUG_PRINT("Conectando a");
    DEBUG_PRINTLN(ssid);
    
    WiFi.begin(ssid.c_str(), pwd.c_str());
  
    // Wait for connection
    if (!testWifi()) {
      return false;
    }
  
    DEBUG_PRINTLN("");
    DEBUG_PRINT("Connected to ");
    DEBUG_PRINTLN(ssid);
    DEBUG_PRINT("IP address: ");
    DEBUG_PRINTLN(WiFi.localIP());
  
    return true;
  }

  bool testWifi(void) {
    int c = 0;
    DEBUG_PRINTLN("Waiting for Wifi to connect");
    while ( c < 20 ) {
      if (WiFi.status() == WL_CONNECTED) {
        return true;
      }
      delay(500);
      DEBUG_PRINT(".");
      c++;
    }
  
    DEBUG_PRINTLN("");
    DEBUG_PRINTLN("Connect timed out");
    DEBUG_PRINT("Status: ");
    DEBUG_PRINT(WiFi.status());
    return false;
  }

  void initAccessPoint(){
    DEBUG_PRINTLN("Iniciando como access point");
  
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
  
    WiFi.softAP(ap_ssid, ap_passphrase, 6);
    
    DEBUG_PRINT("Connect to wifi with ssid '");
    DEBUG_PRINT(ap_ssid);
    DEBUG_PRINT("' using password '");
    DEBUG_PRINT(ap_passphrase);
    DEBUG_PRINT("' and post setting about your wifi network to http://");
    DEBUG_PRINT(WiFi.softAPIP());
    DEBUG_PRINT("/settings?ssid=[ssid]&pass=[password]");
  }

  private:
    const char* ap_ssid = "IoT-Device";
    const char* ap_passphrase = "Passw0rd!";
};

#endif
