#ifndef CONSOLE_CLASS
#define CONSOLE_CLASS

class Console {
public:
    void running(int indent, const char *name);
    void fail(int indent, const char *message);
    void complete(int indent, const char *name, int asserts, int fails, int success);
};

#endif // CONSOLE_CLASS
