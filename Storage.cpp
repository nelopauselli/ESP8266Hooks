#include "Arduino.h"
#include "Storage.h"

#define STORAGE_SIZE 1024

//#define DEBUG_STORAGE
#ifdef DEBUG_STORAGE
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#endif

void Storage::init()
{
	EEPROM.begin(STORAGE_SIZE);
	delay(10);
	DEBUG_PRINTLN();
	DEBUG_PRINTLN("EEPROM inited");
}

void Storage::saveSubscriptions(String raw)
{
	DEBUG_PRINTLN("Saving subscriptions");
	DEBUG_PRINTLN(raw);

	for (int i = 0; i < raw.length(); ++i)
		EEPROM.write(0 + i, raw[i]);

	for (int i = raw.length(); i < STORAGE_SIZE; ++i)
		EEPROM.write(i, 0);

	EEPROM.commit();
}

String Storage::loadSubscriptions()
{
	DEBUG_PRINTLN("Reading subscripcions from EEPROM");
	String raw = "";
	for (int i = 0; i < STORAGE_SIZE; ++i)
	{
		byte c = EEPROM.read(i);
		raw += char(c);
	}
	DEBUG_PRINTLN();
	DEBUG_PRINTLN(raw);

	return raw;
}
