/*
	Hook.h - 
	Created by Nelo Pauselli, 21 de Enero, 2017.
*/
#ifndef ESP8266Hooks_h
#define ESP8266Hooks_h

#include "Arduino.h"
#include "HookAction.cpp"
#include <ESP8266WebServer.h>

struct Subscription {
	String target;
	String pattern;
	Subscription* next;
};

struct Event {
	String name;
	Subscription* subscriptions;
	Event* next;
};

class ESP8266Hooks
{
  public:
	ESP8266Hooks();
	String definition();
	void init(String deviceName);
	void registerEvent(String eventName);
	void subscribeEvent(String eventName, String target, String pattern);
	void unsubscribeEvent(String eventName, String target);
	void triggerEvent(String eventName, NameValueCollection values);

	void registerAction(char *actionName, int (*callback)(NameValueCollection));

	void handleClient();

  private:
	ESP8266WebServer _server;
	Event* _events = NULL;
	HookAction _actions[10];
	int _indexAction;
	String _mac;
	String _deviceName;
};

#endif
