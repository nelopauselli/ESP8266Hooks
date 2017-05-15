#ifndef Blinker_h
#define Blinker_h

#include "Arduino.h"

class Blinker {
  public:
    Blinker() {
      pinMode(LED_BUILTIN, OUTPUT);
      digitalWrite(LED_BUILTIN, HIGH);
  
      blink(1);
    }
  
    void blink(byte times) {
      for (int i = 0; i < times; i++) {
        digitalWrite(LED_BUILTIN, LOW);
        delay(500 / times);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500 / times);
      }
    }

    void turnOn(){
      digitalWrite(LED_BUILTIN, LOW);
    }
};

#endif
