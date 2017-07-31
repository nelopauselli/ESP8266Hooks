#include <iostream>
#include <windows.h>
#include "Console.h"

#ifndef CONSOLE_WINDOWS_CLASS
#define CONSOLE_WINDOWS_CLASS

using namespace std;

class ConsoleWindows : public Console
{
  public:
    void running(int indent, const char *name)
    {
        short color = getConsoleColor();
        setConsoleColor(2);

        printHeader(indent);
        cout << "Running " << name << endl;

        setConsoleColor(color);
    }
    void fail(int indent, const char *message)
    {
        short color = getConsoleColor();
        setConsoleColor(12);

        printHeader(indent);
        cout << message << endl;

        setConsoleColor(color);
    }
    void complete(int indent, const char *name, int asserts, int fails, int success)
    {
        short oldColor = getConsoleColor();
        short currentColor = (fails > 0) ? 12 : (success > 0) ? 2 : 6;
        setConsoleColor(12);
        setConsoleColor(currentColor);

        printHeader(indent);

        cout << name << " complete. " << asserts << " asserts. " << fails << " fails. " << success << " success." << endl;

        setConsoleColor(oldColor);
    }

  private:
    void printHeader(int indent)
    {

        for (int i = 0; i < indent; i++)
            cout << '\t';
    }
    short getConsoleColor()
    {
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info))
            return 0;
        short ret = info.wAttributes;
        return ret;
    }
    void setConsoleColor(short color)
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);
    }
};

ConsoleWindows Console;

#endif // CONSOLE_WINDOWS_CLASS
