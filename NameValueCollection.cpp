/*
	NameValueCollection.h -
	Created by Nelo Pauselli, 21 de Enero, 2017.
*/
#ifndef NameValueCollection_h
#define NameValueCollection_h

#ifdef ARDUINO
#include "Arduino.h"
#else
#include <iostream>
#endif

struct KeyValue {
    const char* key;
    const char* value;
};

class NameValueCollection
{
  public:
	NameValueCollection(int capacity)
	{
		_capacity = capacity;
		_current = 0;
		_items = new KeyValue[_capacity];
	}

	bool push(const char* key, const char* value)
	{
		if (_current >= _capacity)
			return false;

		KeyValue item;
		item.key = key;
		item.value = value;
		_items[_current++] = item;

		return true;
	}

	const char* getKey(int index){
		return _items[index].key;
	}

	const char* operator[](const char* key)
	{
		for (int i = 0; i < _current; i++)
		{
			if (strcmp(_items[i].key, key)==0)
			{
				return _items[i].value;
			}
		}
		return NULL;
	}

	int length() {
		return _current;
	}
  private:
  	KeyValue *_items = NULL;
	int _capacity;
	int _current;
};

#endif
