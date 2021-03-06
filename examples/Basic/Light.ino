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
}

void sendLight()
{
	int value = lightValue / lightTimes;
	NameValueCollection parameters(1);
	parameters.push("light", String(value, DEC));
	hooks.triggerEvent("light_each_30_seconds", parameters);
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