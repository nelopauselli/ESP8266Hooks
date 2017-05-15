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

	String body = String("{state: ") + (state ? "down" : "up") + String("}");
	hooks.triggerEvent("button_change", body);
}