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

	initDht();
	
	// Simple WiFi connection
	const char *ssid = "<WIFI SSID>"; //your wifi SSID
	const char *password = "<WIFI PASSOWRD>"; //your wifi password

	WiFi.mode(WIFI_STA);
	IPAddress ip(192, 168, 1, 21);		// set a valid ip for your network
	IPAddress gateway(192, 168, 1, 1);  // set gateway to match your network
	IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your
	WiFi.config(ip, gateway, subnet);
	WiFi.begin(ssid, password);
	
	Serial.println("");
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(250);
		digitalWrite(LED_BUILTIN, HIGH);
		delay(250);
		digitalWrite(LED_BUILTIN, LOW);
		Serial.print(".");
	}
	Serial.println("");
	Serial.print("connected to " + String(ssid) + ". IP: ");
	Serial.println(WiFi.localIP());

	hooks.init("IoT DHT");
	hooks.registerEvent("dht_each_30_seconds", "temperature={temperature}&humidity={humidity}");
	hooks.registerEvent("humedad_mayor_65", "humidity={humidity}");
	hooks.registerEvent("humedad_menor_65", "humidity={humidity}");

	digitalWrite(LED_BUILTIN, LOW);
}

long lastReadDht = 0; // timer to read light
long lastSendDht = 0; // timer to send light

void loop()
{
	hooks.handleClient();

	if (lastReadDht + 2 * 1000 < millis())
	{
		readDht();
		lastReadDht = millis();
	}

	if (lastSendDht + 30 * 1000 < millis())
	{
		sendDht();
		lastSendDht = millis();
	}

	delay(10);
}
