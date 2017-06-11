int lastState;
#define BUTTON D8

void initButton()
{
	pinMode(BUTTON, INPUT);
}

bool readButtonChange()
{
	int state = digitalRead(BUTTON);

	bool change = state != lastState;

	lastState = state;
	
	if(change && state)
		writeLed("on");

	return change;
}

void sendButtonChange()
{
	int state = digitalRead(BUTTON);

	NameValueCollection parameters(1);
	parameters.push("state", state ? "down" : "up");
	hooks.triggerEvent("button_change", parameters);
}