/*
	HookAction.h - 
	Created by Nelo Pauselli, 16 de Febrero, 2017.
*/
#ifndef HookAction_h
#define HookAction_h

#include "Arduino.h"
#include "NameValueCollection.cpp"
#include "ValueCollection.cpp"

class HookAction
{
  public:
	HookAction()
	{
	}
	
	HookAction(char *actionName, ValueCollection parameters, int (*callback)(NameValueCollection))
	{
		_actionName = actionName;
		_parameters = parameters;
		_callback = callback;
	}

	String getActionName()
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
	String _actionName;
	ValueCollection _parameters;
	int (*_callback)(NameValueCollection);
};

#endif
