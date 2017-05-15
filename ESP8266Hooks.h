/*
	Hook.h - 
	Created by Nelo Pauselli, 21 de Enero, 2017.
*/
#ifndef ESP8266Hooks_h
#define ESP8266Hooks_h

#include "Arduino.h"
#include "HookAction.cpp"
#include "Storage.h"
#include <ESP8266WebServer.h>

class ESP8266Hooks
{
  public:
	ESP8266Hooks();
	String definition();
	void init(Storage &storage, String deviceName);
	void registerEvent(String event);
	void listenEvent(String event, String target);
	void registerSubscription(String listener);
	void triggerEvent(String event, String body);

	void registerAction(char *actionName, int (*callback)(NameValueCollection));

	void loadSubscriptionsFromConfig();
	String getSubscriptionsAsRaw();

	void handleClient();

  private:
	ESP8266WebServer _server;
	Storage _storage;
	String _events[20];
	String _subscriptions[40];
	HookAction _actions[10];
	int _indexEvent;
	int _indexListener;
	int _indexAction;
	String _mac;
	String _deviceName;
};

#endif
