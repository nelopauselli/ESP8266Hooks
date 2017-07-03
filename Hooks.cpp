#ifndef Hooks_h
#define Hooks_h

#define HISTORY_MAX 20

#include "NameValueCollection.cpp"

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
	Hooks(const char *mac)
	{
		_mac = mac;
	}

	const char *get_mac()
	{
		return _mac;
	}

	Message *get_messages()
	{
		return _messages;
	}

	Event * get_events(){
		return _events;
	}

	void registerEvent(Event *event)
	{
		event->next = _events;
		_events = event;
	}

	void subscribeEvent(String eventName, Subscription* subscription)
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
	const char *_mac;
	Event *_events = NULL;
	Message *_messages = NULL;
};

#endif