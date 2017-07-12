/*
	Hook.h - 
	Created by Nelo Pauselli, 21 de Enero, 2017.
*/
#ifndef ESP8266Hooks_h
#define ESP8266Hooks_h

#ifdef ARDUINO
#include "Arduino.h"
#endif

#include "Hooks.cpp"
#include "HookAction.cpp"
#include <ESP8266WebServer.h>

class ESP8266Hooks
{
public:
	ESP8266Hooks();
	void init(const char *deviceName, int port=80);
	void registerEvent(const char *eventName, String format);
	void triggerEvent(const char *eventName, NameValueCollection values);

	void registerAction(char *actionName, ValueCollection parameters, int (*callback)(NameValueCollection));

	void handleClient();

private:
	ESP8266WebServer *_server = NULL;
	Hooks *hooks = NULL;
};

#endif
