#define PHOTORESISTOR A0

int lightValue = 0;
int lightTimes = 0;
int lightLimit = 600;

void readLight()
{
	int value = analogRead(PHOTORESISTOR);
	Serial.println(value);

	lightValue += value;
	lightTimes++;

	if (value < lightLimit)
		writeLed("on");
	else
		writeLed("off");
}

void sendLight()
{
	int value = lightValue / lightTimes;
	String body = "{light: " + String(value, DEC) + "}";
	hooks.triggerEvent("light_each_30_seconds", body);
	lightValue = 0;
	lightTimes = 0;
}

int configureLight(NameValueCollection parameters)
{
	String limit = parameters["limit"];
	if (limit)
	{
		lightLimit = limit.toInt();
	}
	return 204;
}