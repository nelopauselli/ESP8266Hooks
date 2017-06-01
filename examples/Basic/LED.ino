#define LED D1

char *currentState;

void initLEDs()
{
	pinMode(LED, OUTPUT);
	digitalWrite(LED, HIGH);
	currentState = "off";
}

int listenerLed(NameValueCollection parameters) //?state=[on|off]
{
	String state = parameters["state"];
	if (state == "on")
	{
		writeLed("on");
		return 204;
	}
	else if (state == "off")
	{
		writeLed("off");
		return 204;
	}

	String body = "";
	body += "bad state: ";
	body += state;
	return 400;
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
