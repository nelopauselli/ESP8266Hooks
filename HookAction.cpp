/*
  HookAction.h - 
  Created by Nelo Pauselli, 16 de Febrero, 2017.
*/
#ifndef HookAction_h
#define HookAction_h

#include "Arduino.h"
#include "NameValueCollection.cpp"

class HookAction
{
  public:
    HookAction() {}
    
    HookAction(char* actionName, int (*callback)(NameValueCollection)){
      _actionName = actionName;
      _callback = callback;
    }
    
    String getActionName(){
      return _actionName;
    }
    
    int invoke(NameValueCollection parameters){
      return _callback(parameters);
    }
    
  private:
    String _actionName;
    int (*_callback)(NameValueCollection);
};

#endif
