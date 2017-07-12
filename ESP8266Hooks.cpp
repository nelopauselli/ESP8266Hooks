#ifdef ARDUINO
#include "Arduino.h"
#endif

#include "ESP8266Hooks.h"
#include <ESP8266HTTPClient.h>

//#define DEBUG_HOOKS
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
}

void ESP8266Hooks::init(const char *deviceName, int port)
{
	DEBUG_PRINT("Init Hooks on port ");DEBUG_PRINTLN(port);
	
	unsigned char mac[6];
	WiFi.macAddress(mac);

	String mac2 = "";
	for (int i = 0; i < 6; ++i)
	{
		String atom = String(mac[i], HEX);
		if (atom.length() < 2)
			mac2 += String("0") + atom;
		else
			mac2 += atom;

		if (i < 5)
			mac2 += ':';
	}
	mac2.toUpperCase();

	hooks = new Hooks(mac2, deviceName);

	_server = new ESP8266WebServer(port);

	_server->on("/ping", HTTP_GET, [&]() {
		DEBUG_PRINTLN("ping/pong");
		_server->send(200, "text/plain", "pong");
	});

	_server->on("/hooks", HTTP_GET, [&]() {
		DEBUG_PRINTLN("enviando hooks");
		String body = hooks->get_definition();
		_server->send(200, "application/json", body);
	});

	_server->on("/history", HTTP_GET, [&]() {
		DEBUG_PRINTLN("sending history");
		String body = hooks->get_history();
		_server->send(200, "application/json", body);
	});

	_server->on("/hooks", HTTP_POST, [&]() {
		DEBUG_PRINT("Agregando suscripción ");

		String event = _server->arg("event");
		String target = _server->arg("target");
		String format = _server->arg("template");

		DEBUG_PRINT(event);
		DEBUG_PRINT(" => ");
		DEBUG_PRINT(target);
		DEBUG_PRINT(" [");
		DEBUG_PRINT(format);
		DEBUG_PRINT("] ");

		Subscription *subscription = new Subscription();
		subscription->target = target;
		subscription->format = format;

		hooks->subscribeEvent(event, subscription);

		_server->send(204);
	});

	_server->on("/hooks", HTTP_DELETE, [&]() {
		DEBUG_PRINT("Eliminando suscripción ");

		String event = _server->arg("event");
		String target = _server->arg("target");

		DEBUG_PRINT(event);
		DEBUG_PRINT(" => ");
		DEBUG_PRINTLN(target);

		hooks->unsubscribeEvent(event, target);

		_server->send(204);
	});

	_server->on("/hooks", HTTP_OPTIONS, [&]() {
		_server->sendHeader("access-control-allow-credentials", "false");
		_server->sendHeader("access-control-allow-headers", "x-requested-with");
		_server->sendHeader("access-control-allow-methods", "GET,POST,DELETE,OPTIONS");

		_server->send(204);
	});

	_server->on("/actions", HTTP_POST, [&]() {
		DEBUG_PRINT("Recibiendo accion: ");
		String actionName = _server->arg("action");
		DEBUG_PRINTLN(actionName);

		DEBUG_PRINTLN("Creando parametros");

		NameValueCollection parameters(_server->args());
		for (int i = 0; i < _server->args(); i++)
		{
			const char* key = _server->argName(i).c_str();
			const char* value = _server->arg(i).c_str();
			parameters.push(key, value);
		}

		DEBUG_PRINTLN("Buscando y lanzando accion");

		int statusCode = hooks->triggerAction(actionName, parameters);

		_server->send(statusCode);
	});

	_server->onNotFound([&]() {
		String path = _server->uri();
		DEBUG_PRINTLN("RESPONSE NOT FOUND FOR: " + path);
		_server->send(404, "text/plain", "");
	});

	_server->begin();
}

void ESP8266Hooks::registerEvent(String eventName, String format)
{
	Event *event = new Event();
	event->name = eventName;
	event->format = format;

	hooks->registerEvent(event);
}

void ESP8266Hooks::triggerEvent(String eventName, NameValueCollection values)
{
	hooks->triggerEvent(eventName, values);
}

void ESP8266Hooks::registerAction(char *actionName, ValueCollection parameters, int (*callback)(NameValueCollection))
{
	HookAction action(actionName, parameters, callback);
	hooks->registerAction(action);
}

void ESP8266Hooks::handleClient()
{
	_server->handleClient();

	DEBUG_PRINTLN("Iniciando envío de mensajes");
	Message *message = hooks->get_messages();
	while (message != NULL)
	{
		long now = millis(); //TODO: ver que hacemos cuando se reinicia millis()
		if (!message->success && message->attempts < 5 && message->at < now - 5000)
		{
			DEBUG_PRINTLN("Enviando '" + message->body + "' hook a " + message->target);

			HTTPClient http;
			http.setTimeout(500);

			bool begined = http.begin(message->target);
			message->at = now;

			if (begined)
			{
				http.addHeader("Content-Type", "application/x-www-form-urlencoded");

				int requestStart = millis();
				int httpCode = http.POST(message->body);
				int requestEnd = millis();

				message->duration = requestEnd - requestStart;
				message->attempts++;

				if (httpCode > 0)
				{
					if (httpCode < 400)
					{
						DEBUG_PRINTF("[HTTP] POST in %dms. code: %d\n", message->duration, httpCode);
						message->success = true;
					}
					else
					{
						DEBUG_PRINTF("[HTTP] POST failed in %dms, error: %s\n", message->duration, http.errorToString(httpCode).c_str());
						message->success = false;
					}
				}
				else
				{
					DEBUG_PRINTF("[HTTP] POST failed in %dms\n", message->duration);
					message->success = false;
				}
			}
			else
			{
				DEBUG_PRINTLN("[HTTP] BEGIN failed");
				message->success = false;
			}
			http.end();
		}

		message = message->next;
	}

#ifdef DEBUG_HOOKS
	message = hooks->get_messages();
	while (message != NULL)
	{
		DEBUG_PRINT("Message to " + message->target + " ");
		if (message->success)
			DEBUG_PRINT(" was success ");
		else
			DEBUG_PRINT(" fail " + String(message->attempts) + " times");

		DEBUG_PRINTLN("");
		message = message->next;
	}
#endif

	hooks->cleanHistory();
}