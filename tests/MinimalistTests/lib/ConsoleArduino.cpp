#include "Console.h"
#include "Arduino.h"

#ifndef CONSOLE_ARDUINO_CLASS
#define CONSOLE_ARDUINO_CLASS

class ConsoleArduino : public Console
{
  public:
    void running(int indent, const char *name)
    {
        printHeader(indent);

        Serial.print("Running ");
        Serial.println(name);
    }

    void fail(int indent, const char *message)
    {
        printHeader(indent);

        Serial.print("ERROR: ");
        Serial.println(message);
    }

    void complete(int indent, const char *name, int asserts, int fails, int success)
    {
        printHeader(indent);
        if (asserts == 0)
            Serial.print("WARN: ");

        Serial.print(name);
        Serial.print(" complete. ");

        Serial.print(asserts);
        Serial.print(" asserts. ");
        Serial.print(fails);
        Serial.print(" fails. ");
        Serial.print(success);
        Serial.println(" success.");
    }

  private:
    void printHeader(int indent)
    {
        Serial.print("[");
        Serial.print(millis());
        Serial.print("] ");

        for (int i = 0; i < indent; i++)
            Serial.print("\t");
    }
};

ConsoleArduino Console;

#endif // CONSOLE_ARDUINO_CLASS
