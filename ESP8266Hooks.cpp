#ifdef ARDUINO
#include "Arduino.h"
#endif

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
}

void ESP8266Hooks::init(const char *deviceName, int port)
{
	DEBUG_PRINT("Init Hooks on port ");
	DEBUG_PRINTLN(port);

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

	hooks = new Hooks(mac2.c_str(), deviceName);

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

		String eventName = _server->arg("event");
		String target = _server->arg("target");
		String format = _server->arg("template");

		DEBUG_PRINT(eventName);
		DEBUG_PRINT(" => ");
		DEBUG_PRINT(target);
		DEBUG_PRINT(" [");
		DEBUG_PRINT(format);
		DEBUG_PRINTLN("] ");

		hooks->subscribeEvent(eventName.c_str(), target.c_str(), format.c_str());

		_server->send(204);
	});

	_server->on("/hooks", HTTP_DELETE, [&]() {
		DEBUG_PRINT("Eliminando suscripción ");

		const char *eventName = _server->arg("event").c_str();
		const char *target = _server->arg("target").c_str();

		DEBUG_PRINT(eventName);
		DEBUG_PRINT(" => ");
		DEBUG_PRINTLN(target);

		hooks->unsubscribeEvent(eventName, target);

		_server->send(204);
	});

	_server->on("/hooks", HTTP_OPTIONS, [&]() {
		_server->sendHeader("access-control-allow-credentials", "false");
		_server->sendHeader("access-control-allow-headers", "x-requested-with");
		_server->sendHeader("access-control-allow-methods", "GET,POST,DELETE,OPTIONS");

		_server->send(204);
	});

	_server->on("/actions", HTTP_POST, [&]() {
		String v = _server->arg("action");
		DEBUG_PRINT("Recibiendo accion: ");
		DEBUG_PRINTLN(v);
		const char *actionName = v.c_str();
		DEBUG_PRINTLN(actionName);

		DEBUG_PRINTLN("Creando parametros");

		NameValueCollection parameters(_server->args());
		for (int i = 0; i < _server->args(); i++)
		{
			const char *key = _server->argName(i).c_str();
			const char *value = _server->arg(i).c_str();
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

void ESP8266Hooks::registerEvent(const char *eventName, const char *format)
{
	hooks->registerEvent(eventName, format);
}

void ESP8266Hooks::triggerEvent(const char *eventName, NameValueCollection values)
{
	hooks->triggerEvent(eventName, values);
}

void ESP8266Hooks::registerAction(const char *actionName, const char *parameters, int (*callback)(NameValueCollection))
{
	hooks->registerAction(actionName, parameters, callback);
}

void ESP8266Hooks::handleClient()
{
	DEBUG_PRINTLN("Atendiendo http");
	_server->handleClient();

	DEBUG_PRINTLN("Buscando mensajes");
	Message *message = hooks->get_messages();
	while (message != NULL)
	{
		DEBUG_PRINTLN("Analizando mensaje");
		long now = millis(); //TODO: ver que hacemos cuando se reinicia millis()
		if (!message->success && message->attempts < 5 && message->at < now - 5000)
		{
			DEBUG_PRINT("Enviando '");
			DEBUG_PRINT(message->body);
			DEBUG_PRINT("' hook a ");
			DEBUG_PRINTLN(message->target);

			HTTPClient http;
			http.setTimeout(500);

			bool begined = http.begin(message->target);
			message->at = now;

			if (begined)
			{
				http.addHeader("Content-Type", "application/x-www-form-urlencoded");

				int requestStart = millis();
				String body2 = String(message->body);
				DEBUG_PRINT("Enviando '");
				DEBUG_PRINT(body2);
				DEBUG_PRINTLN("'");

				int httpCode = http.POST(body2);
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
						DEBUG_PRINTF("[HTTP] POST failed in %dms, error: %d - %s\n", message->duration, httpCode, http.errorToString(httpCode).c_str());
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
	DEBUG_PRINTLN("Terminando busqueda de mensajes");

#ifdef DEBUG_HOOKS
	message = hooks->get_messages();
	while (message != NULL)
	{
		DEBUG_PRINT("Message to " + String(message->target) + " ");
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