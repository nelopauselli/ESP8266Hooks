/*
	ValueCollection.h - 
	Created by Nelo Pauselli, 13 de Junio, 2017.
*/
#ifndef ValueCollection_h
#define ValueCollection_h

#ifdef ARDUINO
#include "Arduino.h"
#endif

#include "assert.h"
#define MAX_SIZE 10

class ValueCollection
{
  public:
	ValueCollection()
	{
		_capacity = MAX_SIZE;
		_current = 0;
	}

	ValueCollection(int capacity)
	{
		assert(capacity <= MAX_SIZE);

		_capacity = capacity;
		_current = 0;
	}

	bool push(String value)
	{
		if (_current >= _capacity)
			return false;
		_values[_current++] = value;

		return true;
	}

	String operator[](const int index)
	{
		if (index <= _current)
			return _values[index];
		return "";
	}

	String toJSON()
	{
		String sb = "[";
		for (int i = 0; i < _current; i++)
		{
			if (i > 0)
				sb += ", ";
			sb += "\"" + _values[i] + "\"";
		}
		sb += "]";

		return sb;
	}

	int length()
	{
		return _current;
	}

  private:
	String _values[MAX_SIZE];
	String _keys[MAX_SIZE];
	int _capacity;
	int _current;
};

#endif
