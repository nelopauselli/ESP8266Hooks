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

	String body;
	body = "state=" + String(state);
	hooks.triggerEvent("led_change", body);

	if (state == "on")
	{
		digitalWrite(LED, LOW);
		hooks.triggerEvent("led_on", body);
	}
	else
	{
		digitalWrite(LED, HIGH);
		hooks.triggerEvent("led_off", body);
	}
	currentState = state;
}
