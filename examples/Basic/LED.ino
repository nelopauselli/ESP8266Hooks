#define LED D3

char *currentState;

void initLEDs()
{
	pinMode(LED, OUTPUT);
	digitalWrite(LED, HIGH);
	currentState = "off";
}

int listenerLedOn(NameValueCollection parameters)
{
	writeLed("on");
	return 204;
}

int listenerLedOff(NameValueCollection parameters)
{
	writeLed("off");
	return 204;
}

void writeLed(char *state)
{
	if (currentState == state)
		return;

	NameValueCollection parameters(1);
	parameters.push("state", state);
	hooks.triggerEvent("led_change", parameters);

	if (state == "on")
	{
		digitalWrite(LED, LOW);
		hooks.triggerEvent("led_on", parameters);
	}
	else
	{
		digitalWrite(LED, HIGH);
		hooks.triggerEvent("led_off", parameters);
	}
	currentState = state;
}
