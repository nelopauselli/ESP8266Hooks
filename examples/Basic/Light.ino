#define PHOTORESISTOR A0

int lightValue = 0;
int lightTimes = 0;

void readLight()
{
  int value = analogRead(PHOTORESISTOR);
  Serial.println(value);

  lightValue += value;
  lightTimes++;

  if(value < 600)
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
