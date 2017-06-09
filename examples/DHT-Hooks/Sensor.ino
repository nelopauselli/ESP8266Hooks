#include "DHT.h"

#define PIN 2
#define DHTTYPE DHT22

DHT dht(PIN, DHTTYPE);
int dhtTimes = 0;

float dhtTemperature = 0;
float dhtHumidity = 0;
float lastHum = 0;

void initDht()
{
	dht.begin();
}

void readDht()
{
	// Reading temperature or humidity takes about 250 milliseconds!
	// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
	float hum = dht.readHumidity();
	// Read temperature as Celsius (the default)
	float temp = dht.readTemperature();
	if (!isnan(hum) && !isnan(temp))
	{
		dhtTemperature += temp;
		dhtHumidity += hum;
		dhtTimes++;

		if (hum > 65 && lastHum < 65)
		{
			hooks.triggerEvent("humedad_mayor_65", "humidity=" + String(hum, DEC));
		}
		if (hum < 65 && lastHum > 65)
		{
			hooks.triggerEvent("humedad_menor_65", "humidity=" + String(hum, DEC));
		}
		lastHum = hum;
	}
}

void sendDht()
{
	float avgTemperature = dhtTemperature / dhtTimes;
	float avgHumidity = dhtHumidity / dhtTimes;
	String body = "temperature=" + String(avgTemperature, DEC) + "&humidity=" + String(avgHumidity, DEC);
	hooks.triggerEvent("dht_each_30_seconds", body);

	dhtTemperature = 0;
	dhtHumidity = 0;
	dhtTimes = 0;
}