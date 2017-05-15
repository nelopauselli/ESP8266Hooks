#ifndef STORAGE_H
#define STORAGE_H

#include "Arduino.h"
#include <EEPROM.h>

class Storage
{
  public:
	void init();
	void readWifiSetting(String &ssid, String &password);
	void saveWifiSetting(String qsid, String qpass);
	void saveSubscriptions(String raw);
	String loadSubscriptions();
};

#endif
