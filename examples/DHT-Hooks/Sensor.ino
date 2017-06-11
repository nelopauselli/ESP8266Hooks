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
			NameValueCollection parameters(1);
			parameters.push("humidity", String(hum, DEC));
			hooks.triggerEvent("humedad_mayor_65", parameters);
		}
		if (hum < 65 && lastHum > 65)
		{
			NameValueCollection parameters(1);
			parameters.push("humidity", String(hum, DEC));
			hooks.triggerEvent("humedad_menor_65", parameters);
		}
		lastHum = hum;
	}
}

void sendDht()
{
	float avgTemperature = dhtTemperature / dhtTimes;
	float avgHumidity = dhtHumidity / dhtTimes;

	NameValueCollection parameters(2);
	parameters.push("temperature", String(avgTemperature, DEC));
	parameters.push("humidity", String(avgHumidity, DEC));
	hooks.triggerEvent("dht_each_30_seconds", parameters);

	dhtTemperature = 0;
	dhtHumidity = 0;
	dhtTimes = 0;
}