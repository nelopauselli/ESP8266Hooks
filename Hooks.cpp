#ifndef Hooks_h
#define Hooks_h

#define HISTORY_MAX 20

#include "NameValueCollection.cpp"
#include "CharTools.h"

#define DEBUG_HOOKS
#ifdef DEBUG_HOOKS
#ifdef ARDUINO
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#define DEBUG_PRINTF(...)
#endif
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#define DEBUG_PRINTF(...)
#endif

#ifdef __cplusplus
#endif // __cplusplus
#ifdef ARDUINO
//typedef char *string;
#endif

struct Subscription
{
	char *target = NULL;
	char *format = NULL;
	Subscription *next;
};

struct Event
{
	const char *name = NULL;
	Subscription *subscriptions;
	const char *format = NULL;
	Event *next;
};

struct Message
{
	const char *target = NULL;
	char *body = NULL;
	bool success = false;
	int duration = 0;
	int attempts = 0;
	long at = 0;
	Message *next;
};

struct Action
{
	const char *name = NULL;
	const char *parameters = NULL;
	int (*callback)(NameValueCollection) = NULL;
	Action *next = NULL;
};

class Hooks
{
  public:
	Hooks(const char *mac, const char *deviceName)
	{
		DEBUG_PRINT("Creando instancia de Hooks. MAC:");
		DEBUG_PRINTLN(mac);
		_mac = new char[strlen(mac)];
		strcpy(_mac, mac);
		_deviceName = deviceName;
	}

	Message *get_messages()
	{
		return _messages;
	}

	char *get_definition()
	{
		char body[1024]; //TODO: ir calculando el size a medida que se registran eventos y acciones

		strcpy(body, "{");

		strcat(body, "\"name\": \"");
		strcat(body, _deviceName);
		strcat(body, "\", ");

		strcat(body, "\"mac\": \"");
		strcat(body, _mac);
		strcat(body, "\", ");

		strcat(body, "\"events\": [");
		Event *event = _events;
		while (event != NULL)
		{
			if (event != _events)
				strcat(body, ",");
			strcat(body, "{\"name\": \"");
			strcat(body, event->name);
			strcat(body, "\", \"template\": \"");
			strcat(body, event->format);
			strcat(body, "\", \"subscriptions\": [");

			Subscription *subscription = event->subscriptions;
			while (subscription != NULL)
			{
				if (subscription != event->subscriptions)
					strcat(body, ",");
				strcat(body, "{");
				strcat(body, "\"target\": \"");
				strcat(body, subscription->target);
				strcat(body, "\"");
				strcat(body, ",\"template\": \"");
				strcat(body, subscription->format);
				strcat(body, "\"");
				strcat(body, "}");

				subscription = subscription->next;
			}
			strcat(body, "]}");

			event = event->next;
		}
		strcat(body, "], ");

		strcat(body, "\"actions\": [");
		Action *action = _actions;
		while (action != NULL)
		{
			DEBUG_PRINTLN(action->name);

			if (action != _actions)
				strcat(body, ",");

			strcat(body, "{\"name\": \"");
			strcat(body, action->name);
			strcat(body, "\"");

			if (action->parameters != NULL)
			{
				strcat(body, ", \"parameters\": [");
				strcat(body, action->parameters);
				strcat(body, "]");
			}

			strcat(body, "}");

			action = action->next;
		}
		strcat(body, "]}");

		return body;
	}

	void registerEvent(const char *eventName, const char *format)
	{
		DEBUG_PRINT("Registrando evento ");
		DEBUG_PRINT(eventName);
		if (strlen(format) > 1)
		{
			DEBUG_PRINT(" con el template ");
			DEBUG_PRINTLN(format);
		}
		else
		{
			DEBUG_PRINTLN(" sin template");
		}

		Event *event = new Event();
		event->name = eventName;
		event->format = format;

		DEBUG_PRINT("Registracion completa ");
		DEBUG_PRINT(event->name);
		if (strlen(event->format) > 1)
		{
			DEBUG_PRINT(" con el template ");
			DEBUG_PRINTLN(event->format);
		}
		else
		{
			DEBUG_PRINTLN(" sin template");
		}

		event->next = _events;
		_events = event;
	}

	void subscribeEvent(const char *eventName, const char *target, const char *format)
	{
		Event *event = _events;
		while (event != NULL)
		{
			if (strcmp(event->name, eventName) == 0)
			{
				DEBUG_PRINT("Suscribiendo ");
				DEBUG_PRINT(target);
				DEBUG_PRINT(" a ");
				DEBUG_PRINT(event->name);
				if (strlen(format) > 1)
				{
					DEBUG_PRINT(" con el template ");
					DEBUG_PRINTLN(format);
				}
				else
				{
					DEBUG_PRINTLN(" sin template");
				}

				Subscription *subscription = new Subscription();
				subscription->target = new char[strlen(target)];
				strcpy(subscription->target, target);

				if (strlen(format) > 1)
				{
					subscription->format = new char[strlen(format)];
					strcpy(subscription->format, format);
				}
				else
				{
					subscription->format = NULL;
				}

				subscription->next = event->subscriptions;
				event->subscriptions = subscription;

				DEBUG_PRINTLN("Subscripcion completada");
				DEBUG_PRINT(subscription->target);
				if (subscription->format != NULL)
				{
					DEBUG_PRINT(" con el template ");
					DEBUG_PRINTLN(subscription->format);
				}
				else
				{
					DEBUG_PRINTLN(" sin template");
				}
				break;
			}
			event = event->next;
		}
	}

	void unsubscribeEvent(const char *eventName, const char *target)
	{
		Event *event = _events;
		while (event != NULL)
		{
			if (strcmp(event->name, eventName) == 0)
			{
				Subscription *subscription = event->subscriptions;
				while (subscription != NULL)
				{
					if (strcmp(subscription->target, target) == 0)
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

	void triggerEvent(const char *eventName, NameValueCollection parameters)
	{
		DEBUG_PRINT("desencadenado '");
		DEBUG_PRINT(eventName);
		DEBUG_PRINTLN("'");

		Event *event = _events;

		while (event != NULL)
		{
			if (strcmp(event->name, eventName) == 0)
			{
				Subscription *subscription = event->subscriptions;

				while (subscription != NULL)
				{
					const char *format;
					if (subscription->format != NULL)
					{
						DEBUG_PRINT("0. subscription.format: ");
						DEBUG_PRINTLN(subscription->format);
						format = subscription->format;
					}
					else if (event->format != NULL && strlen(event->format) > 1)
					{
						DEBUG_PRINT("0. event.format: ");
						DEBUG_PRINTLN(event->format);
						format = event->format;
					}
					else
					{
						format = "mac={mac}&event={event}";
						DEBUG_PRINT("0. default body: ");
					}
					DEBUG_PRINT("1. ");
					DEBUG_PRINTLN(format);

					int length = parameters.length() + 2;
					const char *keys[length];
					const char *values[length];

					keys[0] = "{mac}";
					values[0] = _mac;
					keys[1] = "{event}";
					values[1] = event->name;

					for (int i = 0; i < parameters.length(); i++)
					{
						const char *key = parameters.getKey(i);
						const char *value = parameters[key];
						keys[i+2] = key;
						values[i+2] = value;

						DEBUG_PRINT('\t');
						DEBUG_PRINT(key);
						DEBUG_PRINT(':');
						DEBUG_PRINTLN(value);
					}

					char buffer[1024];
					CharTools.replace(format, buffer, keys, values, length);
					DEBUG_PRINT("3. ");
					DEBUG_PRINTLN(buffer);

					DEBUG_PRINT("Encolando mensaje '");
					DEBUG_PRINT(buffer);
					DEBUG_PRINT("' para ");
					DEBUG_PRINT(strlen(subscription->target));
					DEBUG_PRINT(" '");
					DEBUG_PRINT(subscription->target);
					DEBUG_PRINTLN("'");

					Message *message = new Message();
					message->target = subscription->target;
					message->body = buffer;
					message->next = _messages;
					_messages = message;

					subscription = subscription->next;
				}
			}
			event = event->next;
		}
	}

	char *get_history()
	{
		char body[2048];

		strcpy(body, "[");

		Message *message = _messages;
		while (message != NULL)
		{
			DEBUG_PRINTLN("Procesando message");
			DEBUG_PRINT("\tat: ");
			DEBUG_PRINTLN(message->at);
			DEBUG_PRINT("\ttarget: ");
			DEBUG_PRINTLN(message->target);
			DEBUG_PRINT("\tbody: ");
			DEBUG_PRINTLN(message->body);
			DEBUG_PRINT("\tsuccess: ");
			DEBUG_PRINTLN(message->success);
			DEBUG_PRINT("\tduration: ");
			DEBUG_PRINTLN(message->duration);

			char buffer[256];
			strcpy(buffer, "{");
			strcat(buffer, "\"target\":\"");
			strcat(buffer, message->target);
			strcat(buffer, "\", ");
			strcat(buffer, "\"body\":\"");
			strcat(buffer, message->body);
			strcat(buffer, "\", ");
			strcat(buffer, "\"success\":\"");
			strcat(buffer, message->success ? "yes" : "no");
			strcat(buffer, "\", ");
			strcat(buffer, "\"duration\":");

			char tmp[33];
			itoa(message->duration, tmp, 10);
			strcat(buffer, tmp);
			strcat(buffer, ", ");
			strcat(buffer, "\"attempts\":");
			itoa(message->attempts, tmp, 10);
			strcat(buffer, tmp);
			strcat(buffer, ", ");
			strcat(buffer, "\"at\":");
			itoa(message->at, tmp, 10);
			strcat(buffer, tmp);
			strcat(buffer, "}");

			DEBUG_PRINT("strlen(body): ");
			DEBUG_PRINTLN(strlen(body));
			DEBUG_PRINT("strlen(buffer): ");
			DEBUG_PRINTLN(strlen(buffer));
			DEBUG_PRINT("sizeof(body): ");
			DEBUG_PRINTLN(sizeof(body));
			DEBUG_PRINT("sizeof(body[0]): ");
			DEBUG_PRINTLN(sizeof(body[0]));

			if (strlen(body) + strlen(buffer) < sizeof(body) / sizeof(body[0]))
			{
				if (message != _messages)
					strcat(body, ",");
				strcat(body, buffer);
			}

			else
			{
				DEBUG_PRINTLN("break");

				break;
			}

			message = message->next;
		}
		strcat(body, "]");

		DEBUG_PRINT("History [");
		DEBUG_PRINT(strlen(body));
		DEBUG_PRINT("]: ");
		DEBUG_PRINTLN(body);
		return body;
	}

	void registerAction(const char *actionName, const char *parameters, int (*callback)(NameValueCollection))
	{
		DEBUG_PRINT("Registrando acción: ");
		DEBUG_PRINTLN(actionName);
		Action *action = new Action();
		action->name = actionName;

		action->parameters = parameters;
		action->callback = callback;
		action->next = _actions;
		_actions = action;

		DEBUG_PRINT("Acción ");
		DEBUG_PRINT(action->name);
		DEBUG_PRINTLN(" registrada");
	}

	int triggerAction(const char *actionName, NameValueCollection parameters)
	{
		Action *action = _actions;
		while (action != NULL)
		{
			if (strcmp(action->name, actionName) == 0)
			{
				DEBUG_PRINTLN("Desencadenando accion con los parametros: ");
				for (int i = 0; i < parameters.length(); i++)
				{
					DEBUG_PRINT("\t");

					const char *key = parameters.getKey(i);
					DEBUG_PRINT(key);
					DEBUG_PRINT(":");

					const char *value = parameters[key];
					DEBUG_PRINTLN(value);
				}
				return action->callback(parameters);
			}

			action = action->next;
		}
		return 404;
	}

	void cleanHistory()
	{
		int count = 0;
		Message *message = _messages;
		while (message != NULL)
		{
			if (count++ > HISTORY_MAX)
				break;

			message = message->next;
		}

		if (message != NULL)
		{
			this->cleanMessagesAfter(message->next);
			message->next = NULL;
		}
	}

	void cleanMessagesAfter(Message *message)
	{
		if (message == NULL)
			return;

		DEBUG_PRINT("Quitando mensaje at: ");
		DEBUG_PRINTLN(message->at);
		this->cleanMessagesAfter(message->next);

		message->next = NULL;
	}

  private:
	char *_mac;
	const char *_deviceName;
	Event *_events = NULL;
	Action *_actions = NULL;
	Message *_messages = NULL;
};

#endif
