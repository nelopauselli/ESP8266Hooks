#include "ESP8266Hooks.h"
#define LED LED_BUILTIN

ESP8266Hooks hooks;

bool blinking = true;

void setup() {
	Serial.begin(115200);
	Serial.println("");

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

	Serial.print("Init IoT Blink with LED in pin ");
	Serial.println(LED_BUILTIN);

	hooks.init("IoT Blink");

	hooks.registerEvent("led_on", "");
	hooks.registerEvent("led_off", "");

	ValueCollection parametersLedOn(0);
	hooks.registerAction("blink_on", parametersLedOn, listenerBlinkOn);
	hooks.registerAction("blink_off", parametersLedOn, listenerBlinkOff);

	Serial.println("Ready");
	digitalWrite(LED_BUILTIN, LOW);
}

bool state;

void loop() {
	hooks.handleClient();

	if(!blinking) return;

	int seconds = millis() / 1000;
	if(seconds % 2==0 && !state){
		ledOn();
	} else if (seconds % 2==1 && state) {
		ledOff();
	}

	delay(50);
}

int listenerBlinkOn(NameValueCollection parameters){
	blinking = true;
	return 204;
}

int listenerBlinkOff(NameValueCollection parameters){
	blinking = false;
	ledOff();
	return 204;
}

void ledOn() {
	hooks.triggerEvent("led_on", NULL);
	state = true;
	digitalWrite(LED, LOW);
	Serial.println("on");
}

void ledOff() {
	hooks.triggerEvent("led_off", NULL);
	state = false;
	digitalWrite(LED, HIGH);
	Serial.println("off");
}