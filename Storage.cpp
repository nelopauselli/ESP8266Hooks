#include "Arduino.h"
#include "Storage.h"

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
	EEPROM.begin(1024);
	delay(10);
	DEBUG_PRINTLN();
	DEBUG_PRINTLN();
	DEBUG_PRINTLN("EEPROM inited");
}

void Storage::readWifiSetting(String &ssid, String &password)
{
	DEBUG_PRINTLN("Reading EEPROM ssid");
	ssid = "";
	for (int i = 0; i < 32; ++i)
	{
		ssid += char(EEPROM.read(i));
	}
	DEBUG_PRINT("SSID: ");
	DEBUG_PRINTLN(ssid);

	DEBUG_PRINTLN("Reading EEPROM pass");
	password = "";
	for (int i = 32; i < 96; ++i)
	{
		password += char(EEPROM.read(i));
	}
	DEBUG_PRINT("PASS: ");
	DEBUG_PRINTLN(password);
}

void Storage::saveWifiSetting(String qsid, String qpass)
{
	DEBUG_PRINTLN("clearing eeprom");
	for (int i = 0; i < 96; ++i)
	{
		EEPROM.write(i, 0);
	}
	DEBUG_PRINTLN(qsid);
	DEBUG_PRINTLN("");
	DEBUG_PRINTLN(qpass);
	DEBUG_PRINTLN("");

	DEBUG_PRINTLN("writing eeprom ssid:");
	for (int i = 0; i < qsid.length(); ++i)
	{
		EEPROM.write(i, qsid[i]);
	}
	DEBUG_PRINTLN("writing eeprom pass:");
	for (int i = 0; i < qpass.length(); ++i)
	{
		EEPROM.write(32 + i, qpass[i]);
	}
	EEPROM.commit();
}

void Storage::saveSubscriptions(String raw)
{
	DEBUG_PRINTLN("clearing listeners");

	for (int i = 97; i < 512; ++i)
	{
		EEPROM.write(i, 0);
	}
	DEBUG_PRINTLN("Saving listeners");
	DEBUG_PRINTLN(raw);

	for (int i = 0; i < raw.length(); ++i)
		EEPROM.write(97 + i, raw[i]);

	EEPROM.commit();
}

String Storage::loadSubscriptions()
{
	DEBUG_PRINTLN("Reading Listeners from EEPROM");
	String raw = "";
	for (int i = 97; i < 512; ++i)
	{
		byte c = EEPROM.read(i);
		raw += char(c);
	}
	DEBUG_PRINTLN();
	DEBUG_PRINTLN(raw);

	return raw;
}
