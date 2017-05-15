#ifndef STORAGE_H
#define STORAGE_H

#include "Arduino.h"
#include <EEPROM.h>

class Storage
{
  public:
	void init();
	void saveSubscriptions(String raw);
	String loadSubscriptions();
};

#endif
