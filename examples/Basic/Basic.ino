/*
 * Conexiones:
 *  D8: push button (GND: R 10k ohm)
 *  D0: led yellow (R 47 ohm)
 *  A0: photoresistor (GND: 200 ohm, A0: 220 ohm, 3v3: direct)
 */
#include <SimpleTimer.h>
#include "ESP8266Hooks.h"
#include "Button.cpp"
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
Button button(D8);
Blinker blinker;
SimpleTimer timer;

struct Configuration
{
	int temperatureInterval;
	int pingInterval;
};
Configuration cfg = {40, 10};

void setup()
{
	Serial.begin(115200);
	DEBUG_PRINTLN("Inicializando webhooks");

	storage.init();

	//storage.saveSubscriptions("temperature:http://192.168.1.10:3000/hub;humidity:http://192.168.1.10:3000/hub;button:http://192.168.1.10:3000/hub;");
	//storage.saveSubscriptions("temperature:http://192.168.1.10:3000/hub;humidity:http://192.168.1.10:3000/hub;");
	storage.saveSubscriptions("");

	/*
   * TODO: ir a esto
  
  hooks.on("ready", [](){
    hooks.registerEvent("start"); //este evento debería estar por defecto en Hooks
    hooks.registerEvent("ping"); //este evento tamibén debería estar por defecto en Hooks

    initButtons();
    initLEDs();
    initDht();
  });
  
  hooks.begin(storage);
  */

	String ssid = "";
	String pwd = "";
	storage.readWifiSetting(ssid, pwd);

	DEBUG_PRINTLN("Conectando con Wifi");
	WiFiAdapter wifi;
	if (wifi.connect(ssid, pwd))
	{
		hooks.init(storage, "IoT Luz & Botón");

		hooks.registerEvent("start");
		hooks.registerEvent("ping");
		hooks.registerEvent("button");
		hooks.registerEvent("Light_each_30_seconds");

		initLEDs();
		//initConfiguration();

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

	timer.setInterval(cfg.pingInterval * 1000, pingping);

	//timer.setInterval(2*1000, readDht);
	//timer.setInterval(cfg.temperatureInterval*1000, sendDht);
	timer.setInterval(2 * 1000, readLight);
	timer.setInterval(30 * 1000, sendLight);

	timer.setInterval(500, [&]() {
		if (button.wasPushed())
		{
			hooks.triggerEvent("button", "button_8=pressed");
		}
	});
}

void pingping()
{
	hooks.triggerEvent("ping", "ping=1");
}

void loop()
{
	// put your main code here, to run repeatedly:
	hooks.handleClient();

	if (started)
	{
		button.run();
		timer.run();
	}
	else
	{
		blinker.blink(1);
	}

	delay(10);
}
