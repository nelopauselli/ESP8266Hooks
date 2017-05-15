/*
 * Conexiones:
 *  D8: push button (GND: R 10k ohm)
 *  D0: led yellow (R 47 ohm)
 *  A0: photoresistor (GND: 200 ohm, A0: 220 ohm, 3v3: direct)
 */
#include <SimpleTimer.h>
#include "ESP8266Hooks.h"
#include "Blinker.cpp"
#include "WifiAdapter.cpp"

//#define DEBUG
#ifdef DEBUG
#define DEBUG_PRINT(...) Serial.println(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#endif

bool started = false;

Storage storage;
ESP8266Hooks hooks;
Blinker blinker;
SimpleTimer timer;

void setup()
{
	Serial.begin(115200);
	DEBUG_PRINTLN("Inicializando webhooks");

	storage.init();

	// Empty subscriptions at reset
	storage.saveSubscriptions("");

	String ssid = "";
	String pwd = "";
	storage.readWifiSetting(ssid, pwd);

	DEBUG_PRINTLN("Conectando con Wifi");
	WiFiAdapter wifi;
	if (wifi.connect(ssid, pwd))
	{
		initLEDs();
		initButton();

		hooks.init(storage, "IoT Luz & Botón");

		hooks.registerEvent("start");
		hooks.registerEvent("ping");
		hooks.registerEvent("button_change");

		hooks.registerEvent("light_each_30_seconds");
		hooks.registerAction("light_configure", configureLight);

		hooks.registerEvent("led_change");
		hooks.registerEvent("led_on");
		hooks.registerEvent("led_off");
		hooks.registerAction("led_1", listenerLed);

		blinker.blink(1);

		started = true;
		hooks.triggerEvent("start", "start=1");
		DEBUG_PRINTLN("Dispositivo inicializado:");
		DEBUG_PRINTLN(hooks.definition());

		configureTimers();

		blinker.turnOn();
	}
	else
	{
		wifi.initAccessPoint();
		hooks.init(storage, "IoT Luz & Botón");
	}
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

	if (started)
	{
		bool buttonChange = readButtonChange();
		if(buttonChange)
			sendButtonChange();

		timer.run();
	}
	else
	{
		blinker.blink(1);
	}

	delay(10);
}
