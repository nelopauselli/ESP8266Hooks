#include <DHT.h>
#define DHTTYPE DHT11

//#define DEBUG_DHT
#ifdef DEBUG_DHT
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#endif

DHT dht(D2, DHTTYPE);
int times = 0;

float temperature;
float humidity;

const int limitHumidityOn = 60;
const int limitHumidityOff = 55;

void initDht()
{
  DEBUG_PRINTLN("Inicializando sensor de humedad y temperatura");
  dht.begin();

  hooks.registerEvent("temperature");
  hooks.registerEvent("humidity");
}

void readDht()
{
  DEBUG_PRINT("Leyendo humedad y temperatura: ");

  float h = dht.readHumidity();
  DEBUG_PRINT(h);
  float t = dht.readTemperature();
  DEBUG_PRINT(",");
  DEBUG_PRINTLN(t);

  if (!isnan(h) && !isnan(t))
  {
    temperature += t;
    humidity += h;
    times++;
  }
}
void sendDht()
{
  if (times == 0)
    return;

  DEBUG_PRINT("Enviando humedad y temperatura: ");

  humidity = humidity / times;
  temperature = temperature / times;

  float hic = dht.computeHeatIndex(temperature, humidity, false);

  if (humidity > limitHumidityOn)
    writeLed("on");
  else if (humidity < limitHumidityOff)
    writeLed("off");

  String body;
  body = "temperature=" + String(temperature) + "&heat=" + String(hic) + "&unit=Celsius";
  hooks.triggerEvent("temperature", body);

  body = "humidity=" + String(humidity) + "&unit=Percent";
  hooks.triggerEvent("humidity", body);

  humidity = 0;
  temperature = 0;
  times = 0;
}
