/*
 * Conexiones:
 *  D8: push button (GND: R 10k ohm)
 *  D0: led yellow (R 47 ohm)
 *  A0: photoresistor (GND: 200 ohm, A0: 220 ohm, 3v3: direct)
 */
#include "ESP8266Hooks.h"

ESP8266Hooks hooks;

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);

	Serial.begin(115200);

	initLEDs();
	initButton();

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

	hooks.init("IoT Luz & Botón");
	// Use true as second parameter to empty subscriptors list
	//hooks.init("IoT Luz & Botón", true);

	hooks.registerEvent("start");
	hooks.registerEvent("button_change");

	hooks.registerEvent("light_each_30_seconds");
	hooks.registerAction("light_configure", configureLight);

	hooks.registerEvent("led_change");
	hooks.registerEvent("led_on");
	hooks.registerEvent("led_off");
	hooks.registerAction("led_on", listenerLedOn);
	hooks.registerAction("led_off", listenerLedOff);

	hooks.triggerEvent("start", "start=1");

	digitalWrite(LED_BUILTIN, LOW);
}

long lastReadLight = 0; // timer to read light
long lastSendLight = 0; // timer to send light

void loop()
{
	hooks.handleClient();

	bool buttonChange = readButtonChange();
	if (buttonChange)
		sendButtonChange();

	if (lastReadLight + 2 * 1000 < millis())
	{
		readLight();
		lastReadLight = millis();
	}

	if (lastSendLight + 30 * 1000 < millis())
	{
		sendLight();
		lastSendLight = millis();
	}

	delay(10);
}
