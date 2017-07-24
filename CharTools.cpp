#include "CharTools.h"
#include "Arduino.h"

CharToolsClass CharTools;

void CharToolsClass::replace(const char *source, char buffer[], const char *key, const char *value)
{
	int sizeOfSource = strlen(source);
	//cout << "size of source: " << sizeOfSource << endl;

	int sizeOfKey = strlen(key);
	//cout << "size of key: " << sizeOfKey << endl;

	int sizeOfValue = strlen(value);
	//cout << "size of value: " << sizeOfValue << endl;

	const char *ptrFrom = &key[0];

	strcpy(buffer, "");

	int bufferPos = 0;
	for (int pos = 0; pos < sizeOfSource; pos++)
	{
		const char *ptrSource = &source[pos];
		if (memcmp(ptrSource, ptrFrom, sizeOfKey) == 0)
		{
			//cout << "found '" << key << "' at " << pos << endl;

			memcpy(&buffer[bufferPos], &value[0], sizeOfValue);
			bufferPos += sizeOfValue;
			pos += sizeOfKey - 1;
		}
		else
		{
			memcpy(&buffer[bufferPos], &source[pos], 1);
			bufferPos++;
		}
	}
	buffer[bufferPos] = '\0';
}

void CharToolsClass::replace(const char *source, char buffer[], const char *keys[], const char *values[], int length)
{
	int sizeOfSource = strlen(source);
	//cout << "size of source: " << sizeOfSource << endl;

	int sizeOfKeys = length; //sizeof(keys);
	//cout << "size of keys: " << sizeOfKeys << endl;

	int bufferPos = 0;
	strcpy(buffer, "");
	for (int pos = 0; pos < sizeOfSource; pos++)
	{
		bool found = false;
		for (int i = 0; i < sizeOfKeys; i++)
		{
			const char *key = keys[i];
			const char *value = values[i];

			int sizeOfKey = strlen(key);
			//cout << "size of key: " << sizeOfKey << endl;

			int sizeOfValue = strlen(value);
			//cout << "size of value: " << sizeOfValue << endl;

			const char *ptrFrom = &key[0];

			const char *ptrSource = &source[pos];
			if (memcmp(ptrSource, ptrFrom, sizeOfKey) == 0)
			{
				//cout << "found '" << key << "' at " << pos << endl;

				memcpy(&buffer[bufferPos], &value[0], sizeOfValue);
				bufferPos += sizeOfValue;
				pos += sizeOfKey - 1;

				found = true;
				break;
			}
		}

		if (!found)
		{
			memcpy(&buffer[bufferPos], &source[pos], 1);
			bufferPos++;
		}
	}
	buffer[bufferPos] = '\0';
}
