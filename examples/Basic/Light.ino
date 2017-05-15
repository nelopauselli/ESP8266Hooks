int lightValue = 0;
int lightTimes = 0;

void readLight()
{
  int value = analogRead(A0);
  lightValue += value;
  lightTimes++;

  Serial.println(lightValue);
}

void sendLight()
{
  int value = lightValue / lightTimes;
  String body = "{light: " + String(value, DEC) + "}";
  hooks.triggerEvent("Light_each_30_seconds", body);
  lightValue = 0;
  lightTimes = 0;
}
