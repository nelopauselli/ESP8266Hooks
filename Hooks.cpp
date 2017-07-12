#ifndef Hooks_h
#define Hooks_h

#define HISTORY_MAX 20

#include "NameValueCollection.cpp"
#include "HookAction.cpp"

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

class Hooks
{
  public:
	Hooks(String mac, const char *deviceName)
	{
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
		Serial.println(_mac);
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

	void registerEvent(Event *event)
	{
		event->next = _events;
		_events = event;
	}

	void subscribeEvent(String eventName, Subscription *subscription)
	{
		Event *event = _events;
		while (event != NULL)
		{
			if (event->name == eventName)
			{
				subscription->next = event->subscriptions;
				event->subscriptions = subscription;

				break;
			}
			event = event->next;
		}
	}

	void unsubscribeEvent(String eventName, String target)
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

	void triggerEvent(String eventName, NameValueCollection values)
	{
		DEBUG_PRINT("desencadenado '");DEBUG_PRINT(eventName);DEBUG_PRINTLN("'");

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
						const char* key = values.getKey(i);
						const char* value = values[key];
						body.replace("{" + String(key) + "}", String(value));
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

	String get_history()
	{
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

			message = message->next;
		}
		body += "]";

		return body;
	}

	void registerAction(HookAction action)
	{
		_actions[_indexAction++] = action;
	}

	int triggerAction(String actionName, NameValueCollection parameters)
	{
		for (int i = 0; i < _indexAction; i++)
		{
			HookAction hookAction = _actions[i];
			if (actionName == hookAction.getActionName())
			{
				DEBUG_PRINTLN("Desencadenando accion con los parametros: ");
				for(int i=0;i<parameters.length();i++){
					DEBUG_PRINT("\t");
					
					const char* key = parameters.getKey(i);
					DEBUG_PRINT(key);DEBUG_PRINT(":");

					const char* value = parameters[key];
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
	Message *_messages = NULL;
	HookAction _actions[10];
	int _indexAction;
};

#endif