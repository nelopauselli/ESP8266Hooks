/*
	Hook.h - 
	Created by Nelo Pauselli, 21 de Enero, 2017.
*/
#ifndef ESP8266Hooks_h
#define ESP8266Hooks_h

#include "Arduino.h"
#include "Hooks.cpp"
#include "HookAction.cpp"
#include <ESP8266WebServer.h>

struct Subscription
{
	String target;
	String format;
	Subscription *next;
};

struct Event
{
	String name;
	Subscription *subscriptions;
	String format;
	Event *next;
};

struct Message
{
	String target = "";
	String body = "";
	bool success = false;
	int duration = 0;
	int attempts = 0;
	long at = 0;
	Message *next;
};

class ESP8266Hooks
{
  public:
	ESP8266Hooks();
	String definition();
	void init(String deviceName);
	void registerEvent(String eventName, String format);
	void subscribeEvent(String eventName, String target, String format);
	void unsubscribeEvent(String eventName, String target);
	void triggerEvent(String eventName, NameValueCollection values);

	void registerAction(char *actionName, ValueCollection parameters, int (*callback)(NameValueCollection));

	void handleClient();

  private:
	ESP8266WebServer _server;
	Hooks* hooks = NULL;
	String history();
	void cleanMessagesAfter(Message *message);
	Event *_events = NULL;
	Message *_messages = NULL;
	HookAction _actions[10];
	int _indexAction;
	String _deviceName;
};

#endif
