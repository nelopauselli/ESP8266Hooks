#ifndef Hooks_h
#define Hooks_h

#define HISTORY_MAX 20

#include "NameValueCollection.cpp"
#include "HookAction.cpp"

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
typedef char *string;
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
	const string *parameters = NULL;
	int (*callback)(NameValueCollection) = NULL;
	Action *next = NULL;
};

class Hooks
{
  public:
	Hooks(String mac, const char *deviceName)
	{
		DEBUG_PRINT("Creando instancia de Hooks. MAC:");
		DEBUG_PRINTLN(mac);
		_mac = mac;
		_deviceName = deviceName;
	}

	Message *get_messages()
	{
		return _messages;
	}

	String get_definition()
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
			body += "{\"name\": \"" + String(event->name) + "\", ";
			body += "\"template\": \"" + String(event->format) + "\", ";
			body += "\"subscriptions\": [";

			Subscription *subscription = event->subscriptions;
			while (subscription != NULL)
			{
				if (subscription != event->subscriptions)
					body += ",";
				body += "{";
				body += "\"target\": \"";
				body += String(subscription->target);
				body += "\"";
				body += ",\"template\": \"";
				body += String(subscription->format);
				body += "\"";
				body += "}";

				subscription = subscription->next;
			}
			body += "]}";

			event = event->next;
		}
		body += "], ";

		body += "\"actions\": [";
		for (int i = 0; i < _indexAction; i++)
		{
			String action = _actions[i].getActionName();
			ValueCollection parameters = this->_actions[i].getParameters();

			if (i > 0)
				body += ",";
			body += "{\"name\": \"" + action + "\", \"parameters\": " + parameters.toJSON() + "}";
		}
		body += "]";

		body += "}";

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

	void triggerEvent(const char *eventName, NameValueCollection values)
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
					String body = "";
					if (subscription->format != NULL)
					{
						DEBUG_PRINT("0. subscription.format: ");
						DEBUG_PRINTLN(subscription->format);
						body = String(subscription->format);
					}
					else if (event->format != NULL && strlen(event->format) > 1)
					{
						DEBUG_PRINT("0. event.format: ");
						DEBUG_PRINTLN(event->format);
						body = String(event->format);
					}
					else
					{
						body = "mac={mac}&event={event}";
						DEBUG_PRINT("0. default body: ");
						DEBUG_PRINTLN(body);
					}
					DEBUG_PRINT("1. ");
					DEBUG_PRINTLN(body);

					body.replace("{mac}", _mac);
					DEBUG_PRINT("2. ");
					DEBUG_PRINTLN(body);
					body.replace("{event}", String(event->name));
					DEBUG_PRINT("3. ");
					DEBUG_PRINTLN(body);
					for (int i = 0; i < values.length(); i++)
					{
						const char *key = values.getKey(i);
						const char *value = values[key];
						body.replace("{" + String(key) + "}", String(value));
						DEBUG_PRINT("4.");
						DEBUG_PRINT(i);
						DEBUG_PRINTLN(body);
					}

					DEBUG_PRINT("Encolando mensaje '");
					DEBUG_PRINT(body);
					DEBUG_PRINT("' para ");
					DEBUG_PRINT(strlen(subscription->target));
					DEBUG_PRINT(" '");
					DEBUG_PRINT(subscription->target);
					DEBUG_PRINTLN("'");

					Message *message = new Message();
					message->target = subscription->target;
					message->body = new char[body.length() + 1];
					strcpy(message->body, body.c_str());
					message->next = _messages;
					_messages = message;

					subscription = subscription->next;
				}
			}
			event = event->next;
		}
	}

	String get_history()
	{
		String body = "[";

		Message *message = _messages;
		while (message != NULL)
		{
			if (message != _messages)
				body += ",";

			body += "{";
			body += "\"target\":\"" + String(message->target) + "\", ";
			body += "\"body\":\"" + String(message->body) + "\", ";
			body += "\"success\":\"" + String(message->success ? "yes" : "no") + "\", ";
			body += "\"duration\":" + String(message->duration, DEC) + ", ";
			body += "\"attempts\":" + String(message->attempts, DEC) + ", ";
			body += "\"at\":" + String(message->at, DEC) + "";
			body += "}";

			message = message->next;
		}
		body += "]";

		return body;
	}

	void registerAction(char *actionName, string *parameters, int (*callback)(NameValueCollection))
	{
		Action *action = new Action();
		action->name = actionName;
		action->parameters = parameters;
		action->callback = callback;
		action->next = _actions2;
		_actions2 = action;
	}

	int triggerAction(String actionName, NameValueCollection parameters)
	{
		for (int i = 0; i < _indexAction; i++)
		{
			HookAction hookAction = _actions[i];
			if (actionName == hookAction.getActionName())
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
				return hookAction.invoke(parameters);
			}
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

		DEBUG_PRINTLN("Quitando mensaje at: " + String(message->at));
		this->cleanMessagesAfter(message->next);

		message->next = NULL;
	}

  private:
	String _mac;
	const char *_deviceName;
	Event *_events = NULL;
	Action *_actions2 = NULL;
	Message *_messages = NULL;
	HookAction _actions[10];
	int _indexAction;
};

#endif
