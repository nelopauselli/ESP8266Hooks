#include "HookAction.cpp"

char* currentState;

void initLEDs() {
  pinMode(D0, OUTPUT);
  digitalWrite(D0, LOW);
  currentState = "off";

  hooks.registerEvent("led");

  HookAction hookAction("led_1", [](NameValueCollection parameters){ //?state=[on|off]
    
    String state = parameters["state"];
    if (state == "on") {
      writeLed("on");
      return 204;
    } else if (state == "off") {
      writeLed("off");
      return 204;
    } else {
      String body = "";
      body += "bad state: ";
      body += state;
      return 400;
    }
  });
  
  hooks.registerAction(hookAction);
}

void writeLed(char* state){
  if(currentState==state) return;

  String body;
  body = "state=" + String(state);
  hooks.triggerEvent("led", body);

  if(state=="on") digitalWrite(D0, HIGH);
  else digitalWrite(D0, LOW);
  currentState = state;
}
