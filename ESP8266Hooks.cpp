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

void ESP8266Hooks::init(String deviceName)
{
	_deviceName = deviceName;
	_server = ESP8266WebServer(80);

	_server.on("/ping", HTTP_GET, [&]() {
		DEBUG_PRINTLN("ping/pong");
		_server.send(200, "text/plain", "pong");
	});

	_server.on("/hooks", HTTP_GET, [&]() {
		DEBUG_PRINTLN("enviando hooks");
		String body = this->definition();
		_server.send(200, "application/json", body);
	});

	_server.on("/history", HTTP_GET, [&]() {
		DEBUG_PRINTLN("sending history");
		String body = this->history();
		_server.send(200, "application/json", body);
	});

	_server.on("/hooks", HTTP_POST, [&]() {
		DEBUG_PRINT("Agregando suscripción ");

		String event = _server.arg("event");
		String target = _server.arg("target");
		String format = _server.arg("template");

		DEBUG_PRINT(event);
		DEBUG_PRINT(" => ");
		DEBUG_PRINT(target);
		DEBUG_PRINT(" [");
		DEBUG_PRINT(format);
		DEBUG_PRINT("] ");

		this->subscribeEvent(event, target, format);

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

		NameValueCollection parameters(_server.args());
		for (int i = 0; i < _server.args(); i++)
		{
			parameters.push(_server.argName(i), _server.arg(i));
		}

		DEBUG_PRINTLN("Buscando y lanzando accion");

		for (int i = 0; i < _indexAction; i++)
		{
			HookAction hookAction = _actions[i];
			if (actionName == hookAction.getActionName())
			{
				DEBUG_PRINT("Desencadenando accion con los parametros: ");
				DEBUG_PRINTLN(parameters.toString());
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

	_server.onNotFound([&]() {
		String path = _server.uri();
		DEBUG_PRINTLN("RESPONSE NOT FOUND FOR: " + path);
		_server.send(404, "text/plain", "");
	});

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
	Event *event = _events;
	while (event != NULL)
	{
		if (event != _events)
			body += ",";
		body += "{\"name\": \"" + event->name + "\", ";
		body += "\"template\": \"" + event->format + "\", ";
		body += "\"subscriptions\": [";

		Subscription *subscription = event->subscriptions;
		while (subscription != NULL)
		{
			if (subscription != event->subscriptions)
				body += ",";
			body += "{";
			body += "\"target\": \"";
			body += subscription->target;
			body += "\"";
			body += ",\"template\": \"";
			body += subscription->format;
			body += "\"";
			body += "}";

			subscription = subscription->next;
		}
		body += "]}";

		event = event->next;
	}
	body += "], ";

	body += "\"actions\": [";
	for (int i = 0; i < this->_indexAction; i++)
	{
		String action = this->_actions[i].getActionName();
		ValueCollection parameters = this->_actions[i].getParameters();

		if (i > 0)
			body += ",";
		body += "{\"name\": \"" + action + "\", \"parameters\": " + parameters.toJSON() + "}";
	}
	body += "]";

	body += "}";

	return body;
}

String ESP8266Hooks::history()
{
	int count = 0;

	String body = "[";

	Message *message = _messages;
	while (message != NULL)
	{
		if (message != _messages)
			body += ",";

		body += "{";
		body += "\"target\":\"" + message->target + "\", ";
		body += "\"body\":\"" + message->body + "\", ";
		body += "\"success\":\"" + String(message->success ? "yes" : "no") + "\", ";
		body += "\"duration\":" + String(message->duration, DEC) + ", ";
		body += "\"attempts\":" + String(message->attempts, DEC) + ", ";
		body += "\"at\":" + String(message->at, DEC) + "";
		body += "}";

		if (count++ > 10)
			break;

		message = message->next;
	}
	body += "]";

	return body;
}

void ESP8266Hooks::registerEvent(String eventName, String format)
{
	Event *event = new Event();
	event->name = eventName;
	event->format = format;

	event->next = _events;
	_events = event;
}

void ESP8266Hooks::subscribeEvent(String eventName, String target, String format)
{
	Event *event = _events;
	while (event != NULL)
	{
		if (event->name == eventName)
		{
			Subscription *subscription = new Subscription();
			subscription->target = target;
			subscription->format = format;
			subscription->next = event->subscriptions;

			event->subscriptions = subscription;

			break;
		}
		event = event->next;
	}
}

void ESP8266Hooks::unsubscribeEvent(String eventName, String target)
{
	Event *event = _events;
	while (event != NULL)
	{
		if (event->name == eventName)
		{
			Subscription *subscription = event->subscriptions;
			while (subscription != NULL)
			{
				if (subscription->target == target)
				{
					//TODO: quitar Subscription de la pila
					break;
				}
				subscription = subscription->next;
			}
			break;
		}
		event = event->next;
	}
}

void ESP8266Hooks::triggerEvent(String eventName, NameValueCollection values)
{
	// TODO: agendar los eventos y desencadenarlos cuando no esté el server activo.

	DEBUG_PRINT("desencadenado '");
	DEBUG_PRINT(eventName);
	DEBUG_PRINTLN("'");

	Event *event = _events;

	while (event != NULL)
	{
		if (event->name == eventName)
		{
			Subscription *subscription = event->subscriptions;

			while (subscription != NULL)
			{
				String target = subscription->target;
				String format = subscription->format;
				if (format == NULL || format == "")
					format = event->format;

				String body = "";
				body = String(format);
				body.replace("{mac}", _mac);
				body.replace("{event}", event->name);
				for (int i = 0; i < values.length(); i++)
				{
					String key = values.getKey(i);
					String value = values[key];
					body.replace("{" + key + "}", value);
				}

				DEBUG_PRINTLN("Encolando mensaje a enviar");
				Message *message = new Message();
				message->target = target;
				message->body = body;
				message->next = _messages;
				_messages = message;

				subscription = subscription->next;
			}
		}
		event = event->next;
	}
}

void ESP8266Hooks::registerAction(char *actionName, ValueCollection parameters, int (*callback)(NameValueCollection))
{
	HookAction action(actionName, parameters, callback);
	_actions[_indexAction++] = action;
}

void ESP8266Hooks::handleClient()
{
	_server.handleClient();

	DEBUG_PRINTLN("Iniciando envío de mensajes");
	Message *message = _messages;
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
	message = _messages;
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
}
