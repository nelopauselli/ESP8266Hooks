#include "ESP8266Hooks.h"
#define LED D1

ESP8266Hooks hooks;

void setup() {
	Serial.begin(115200);

	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);

	pinMode(LED, OUTPUT);
	digitalWrite(LED, HIGH);

	// Simple WiFi connection
	const char *ssid = "<WIFI SSID>"; //your wifi SSID
	const char *password = "<WIFI PASSOWRD>"; //your wifi password

	WiFi.mode(WIFI_STA);
	IPAddress ip(192, 168, 1, 20);		// set a valid ip for your network
	IPAddress gateway(192, 168, 1, 1);  // set gateway to match your network
	IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your
	WiFi.config(ip, gateway, subnet);
	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}

	hooks.init("IoT Blink " + String(ip[3]));

	hooks.registerEvent("blink_on");
	hooks.registerEvent("blink_off");

	hooks.registerAction("led_on", listenerLedOn);
	hooks.registerAction("led_off", listenerLedOff);

	Serial.println("Ready");
	digitalWrite(LED_BUILTIN, LOW);
}

bool state;

void loop() {
	hooks.handleClient();

	int seconds = millis() / 1000;
	if(seconds % 2==0 && !state){
		hooks.triggerEvent("blink_on", NULL);
		state = true;
		Serial.println("on");
	} else if (seconds % 2==1 && state){
		hooks.triggerEvent("blink_off", NULL);
		state = false;
		Serial.println("off");
	}

	delay(50);
}

int listenerLedOn(NameValueCollection parameters){
	digitalWrite(LED, LOW);
	return 204;
}

int listenerLedOff(NameValueCollection parameters){
	digitalWrite(LED, HIGH);
	return 204;
}