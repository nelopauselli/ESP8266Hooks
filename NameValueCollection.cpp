/*
	NameValueCollection.h -
	Created by Nelo Pauselli, 21 de Enero, 2017.
*/
#ifndef NameValueCollection_h
#define NameValueCollection_h

#ifdef ARDUINO
#include "Arduino.h"
#endif

#ifdef __cplusplus
#include <iostream>
#endif // __cplusplus

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
		if (_current >= _capacity)
			return NULL;

		return _items[index].key;
	}

	const char* operator[](const char* key)
	{
		for (int i = 0; i < _current; i++)
		{
			if (_items[i].key == key)
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
	int _capacity=0;
	int _current=0;
};

#endif
