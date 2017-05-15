#ifndef Button_h
#define Button_h

#include "Arduino.h"

//#define DEBUG_BUTTON
#ifdef DEBUG_BUTTON
 #define DEBUG_PRINT(...)  Serial.print (__VA_ARGS__)
 #define DEBUG_PRINTLN(...)  Serial.println (__VA_ARGS__)
#else
 #define DEBUG_PRINT(...)
 #define DEBUG_PRINTLN(...)
#endif

class Button
{
  public:
    Button(byte pin) {
      _pin = pin;
      
      pinMode(_pin, INPUT);
      _wasPushed = false;
    }

    void run() {
      if(_wasPushed) return;
      
      int state = digitalRead(_pin);
    
      if (state && !lastState) {
        _wasPushed = true;
        DEBUG_PRINTLN("Button pressed");
      }
      lastState = state;
    }
    bool wasPushed(){
      if(_wasPushed){
        _wasPushed = false;
        return true;
      }
      return false;
    }
    
  private:
    byte _pin;
    int lastState;
    bool _wasPushed;
};

#endif
