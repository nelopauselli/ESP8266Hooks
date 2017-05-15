/*
 * Conexiones:
 *  D8: push button (GND: R 10k ohm)
 *  D0: led yellow (R 47 ohm)
 *  A0: photoresistor (GND: 200 ohm, A0: 220 ohm, 3v3: direct)
 */
#include <SimpleTimer.h>
#include "ESP8266Hooks.h"

ESP8266Hooks hooks;
SimpleTimer timer;

void setup()
{
	Serial.begin(115200);

	// Simple WiFi connection
	const char *ssid = "<WIFI SSID>"; //your wifi SSID
	const char *password = "<WIFI PASSOWRD>"; //your wifi password

	WiFi.mode(WIFI_STA);
	IPAddress ip(192, 168, 1, 20); // set a valid ip for your network
	IPAddress gateway(192, 168, 1, 1); // set gateway to match your network
	IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your
	WiFi.config(ip, gateway, subnet);
	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}

	initLEDs();
	initButton();

	hooks.init("IoT Luz & Botón");
	// Use true as second parameter to empty subscriptors list
	//hooks.init("IoT Luz & Botón", true);

	hooks.registerEvent("start");
	hooks.registerEvent("ping");
	hooks.registerEvent("button_change");

	hooks.registerEvent("light_each_30_seconds");
	hooks.registerAction("light_configure", configureLight);

	hooks.registerEvent("led_change");
	hooks.registerEvent("led_on");
	hooks.registerEvent("led_off");
	hooks.registerAction("led_1", listenerLed);

	hooks.triggerEvent("start", "start=1");

	configureTimers();

	digitalWrite(LED_BUILTIN, LOW);
}

void configureTimers()
{
	int count = timer.getNumTimers();
	for (int i = 0; i < count; i++)
		timer.deleteTimer(i);

	timer.setInterval(10 * 1000, pingping);
	timer.setInterval(2 * 1000, readLight);
	timer.setInterval(30 * 1000, sendLight);
}

void pingping()
{
	hooks.triggerEvent("ping", "ping=1");
}

void loop()
{
	hooks.handleClient();

	bool buttonChange = readButtonChange();
	if (buttonChange)
		sendButtonChange();

	timer.run();

	delay(10);
}
