#include "Arduino.h"
#include "ESP8266Hooks.h"
#include <ESP8266HTTPClient.h>

#define DEBUG_HOOKS
#ifdef DEBUG_HOOKS
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#define DEBUG_PRINTF(...)
#endif

ESP8266Hooks::ESP8266Hooks()
{
	unsigned char mac[6];
	WiFi.macAddress(mac);

	_mac = "";
	for (int i = 0; i < 6; ++i)
	{
		String atom = String(mac[i], HEX);
		if (atom.length() < 2)
			_mac += String("0") + atom;
		else
			_mac += atom;

		if (i < 5)
			_mac += ':';
	}
	_mac.toUpperCase();
}

void ESP8266Hooks::init(String deviceName, bool reset)
{
	_storage.init();

	// Empty subscriptions at reset
	if (reset)
		_storage.saveSubscriptions("");

	_deviceName = deviceName;
	_server = ESP8266WebServer(80);

	_server.on("/ping", HTTP_GET, [&]() {
		_server.send(200, "text/plain", "pong");
	});

	_server.on("/hooks", HTTP_GET, [&]() {
		DEBUG_PRINTLN("enviando hooks");
		String body = this->definition();
		_server.send(200, "application/json", body);
	});

	_server.on("/hooks", HTTP_POST, [&]() {
		DEBUG_PRINT("Agregando suscripción ");

		String event = _server.arg("event");
		String target = _server.arg("target");

		DEBUG_PRINT(event);
		DEBUG_PRINT(" => ");
		DEBUG_PRINTLN(target);

		this->subscribeEvent(event, target);

		_storage.saveSubscriptions(getSubscriptionsAsRaw());

		_server.send(204);
	});

	_server.on("/hooks", HTTP_DELETE, [&]() {
		DEBUG_PRINT("Eliminando suscripción ");

		String event = _server.arg("event");
		String target = _server.arg("target");

		DEBUG_PRINT(event);
		DEBUG_PRINT(" => ");
		DEBUG_PRINTLN(target);

		this->unsubscribeEvent(event, target);

		_storage.saveSubscriptions(getSubscriptionsAsRaw());

		_server.send(204);
	});

	_server.on("/hooks", HTTP_OPTIONS, [&]() {
		_server.sendHeader("access-control-allow-credentials", "false");
		_server.sendHeader("access-control-allow-headers", "x-requested-with");
		_server.sendHeader("access-control-allow-methods", "GET,POST,DELETE,OPTIONS");

		_server.send(204);
	});

	_server.on("/actions", HTTP_POST, [&]() {
		DEBUG_PRINT("Recibiendo accion: ");
		String actionName = _server.arg("action");
		DEBUG_PRINTLN(actionName);

		DEBUG_PRINTLN("Creando parametros");
		delay(100);

		NameValueCollection parameters(_server.args());
		for (int i = 0; i < _server.args(); i++)
		{
			parameters.push(_server.argName(i), _server.arg(i));
		}

		DEBUG_PRINTLN("Buscando y lanzando accion");
		delay(100);

		for (int i = 0; i < _indexAction; i++)
		{
			HookAction hookAction = _actions[i];
			if (actionName == hookAction.getActionName())
			{
				DEBUG_PRINT("Desencadenando accion con los parametros: ");
				delay(100);
				DEBUG_PRINTLN(parameters.toString());
				delay(100);
				int statusCode = hookAction.invoke(parameters);
				_server.send(statusCode);
			}
		}
	});

	_server.on("/", HTTP_GET, [&]() {
		String content = "";
		content += "<html>";
		content += "<head>";
		content += "<title>IoT settings</title>";
		content += "</head>";
		content += "<body>";
		content += "<h1>Settings</h1>";
		content += "<form action='/settings' method='GET'>";
		content += "<div>";
		content += "<label>SSID:</label>";
		content += "<input type='text' name='ssid' />";
		content += "</div>";
		content += "<div>";
		content += "<label>PASSWORD:</label>";
		content += "<input type='password' name='pass' />";
		content += "</div>";
		content += "<div>";
		content += "<input type='submit' value='Save' />";
		content += "</div>";
		content += "</form>";
		content += "</body>";
		content += "</html>";
		_server.send(200, "text/html", content);
	});

	loadSubscriptionsFromConfig();

	_server.begin();
}

String ESP8266Hooks::definition()
{
	String body = "{";

	body += "\"name\": \"";
	body += _deviceName;
	body += "\", ";

	body += "\"mac\": \"";
	body += _mac;
	body += "\", ";

	body += "\"events\": [";
	for (int i = 0; i < this->_indexEvent; i++)
	{
		String eventName = this->_events[i];

		if (i > 0)
			body += ",";
		body += "\"";
		body += eventName;
		body += "\"";
	}
	body += "], ";

	body += "\"subscriptions\": [";
	for (int i = 0; i < this->_indexSubscription; i++)
	{
		String subscripcion = this->_subscriptions[i];

		if (i > 0)
			body += ",";
		body += "\"";
		body += subscripcion;
		body += "\"";
	}
	body += "],";

	body += "\"actions\": [";
	for (int i = 0; i < this->_indexAction; i++)
	{
		String action = this->_actions[i].getActionName();

		if (i > 0)
			body += ",";
		body += "\"";
		body += action;
		body += "\"";
	}
	body += "]";

	body += "}";

	return body;
}

void ESP8266Hooks::registerEvent(String event)
{
	_events[_indexEvent++] = event;
}

void ESP8266Hooks::subscribeEvent(String subscription)
{
	_subscriptions[_indexSubscription++] = subscription;
}

void ESP8266Hooks::subscribeEvent(String event, String target)
{
	String subscription = String(event + ":" + target);
	subscribeEvent(subscription);
}

void ESP8266Hooks::unsubscribeEvent(String event, String target)
{
	String subscription = String(event + ":" + target);
	for (int i = 0; i < _indexSubscription; i++)
	{
		if (_subscriptions[i] == subscription)
		{
			for (int j = i + 1; j < _indexSubscription; j++)
				_subscriptions[j - 1] = _subscriptions[j];

			_indexSubscription--;
			return;
		}
	}
}

void ESP8266Hooks::triggerEvent(String event, String body)
{
	// TODO: agendar los eventos y desencadenarlos cuando no esté el server activo.

	DEBUG_PRINT("desencadenado '");
	DEBUG_PRINT(event);
	DEBUG_PRINTLN("'");

	for (int i = 0; i < _indexSubscription; i++)
	{
		HTTPClient http;

		String subscripcion = _subscriptions[i];

		if (subscripcion.startsWith(event))
		{
			String host = subscripcion.substring(subscripcion.indexOf(":") + 1);

			DEBUG_PRINT("Enviando hook a ");
			DEBUG_PRINTLN(host);

			DEBUG_PRINTLN("Http begin...");

			http.begin(host); //HTTP

			DEBUG_PRINTLN("Http POST...");

			http.addHeader("Content-Type", "application/x-www-form-urlencoded");

			String content = "mac=" + _mac + "&event=" + event + "&" + body;

			int requestStart = millis();
			int httpCode = http.POST(content);
			int requestEnd = millis();
			//http.writeToStream(&Serial);

			if (httpCode > 0)
			{
				// HTTP header has been send and Server response header has been handled
				DEBUG_PRINTF("[HTTP] POST in %dms. code: %d\n", requestEnd - requestStart, httpCode);

				// file found at server
				if (httpCode == HTTP_CODE_OK)
				{
					String payload = http.getString();
					DEBUG_PRINTLN(payload);
				}
			}
			else
			{
				DEBUG_PRINTF("[HTTP] POST... failed in %dms, error: %s\n", requestEnd - requestStart, http.errorToString(httpCode).c_str());
			}
		}
		http.end();
	}
}

void ESP8266Hooks::registerAction(char *actionName, int (*callback)(NameValueCollection))
{
	HookAction action(actionName, callback);
	_actions[_indexAction++] = action;
}

String ESP8266Hooks::getSubscriptionsAsRaw()
{
	String raw = "";
	for (int i = 0; i < _indexSubscription; i++)
	{
		raw += _subscriptions[i];
		raw += ";";
	}
	return raw;
}

void ESP8266Hooks::loadSubscriptionsFromConfig()
{
	String raw = _storage.loadSubscriptions();

	int from = 0;
	int at = raw.indexOf(';', from);
	while (at != -1)
	{
		String subscripcion = raw.substring(from, at);
		DEBUG_PRINTLN(subscripcion);

		subscribeEvent(subscripcion);

		from = at + 1;
		at = raw.indexOf(';', from);
	}
}

void ESP8266Hooks::handleClient()
{
	_server.handleClient();
}
