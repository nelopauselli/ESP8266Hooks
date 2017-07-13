/*
	HookAction.h - 
	Created by Nelo Pauselli, 16 de Febrero, 2017.
*/
#ifndef HookAction_h
#define HookAction_h

#ifdef ARDUINO
#include "Arduino.h"
#endif

#include "NameValueCollection.cpp"
#include "ValueCollection.cpp"

class HookAction
{
  public:
	HookAction()
	{
	}
	
	HookAction(const char *actionName, ValueCollection parameters, int (*callback)(NameValueCollection))
	{
		_actionName = actionName;
		_parameters = parameters;
		_callback = callback;
	}

	const char *getActionName()
	{
		return _actionName;
	}

	ValueCollection getParameters()
	{
		return _parameters;
	}

	int invoke(NameValueCollection parameters)
	{
		return _callback(parameters);
	}

  private:
	const char *_actionName;
	ValueCollection _parameters;
	int (*_callback)(NameValueCollection);
};

#endif
