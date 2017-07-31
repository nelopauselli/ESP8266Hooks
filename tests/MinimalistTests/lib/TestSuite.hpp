#include "TestContext.hpp"

#ifndef TEST_SUITE_CLASS
#define TEST_SUITE_CLASS

class TestSuite
{
public:
    TestSuite(const char* name)
    {
        _name = name;
        _context = new TestContext(name);
    }
    ~TestSuite()
    {
        delete _context;
    }
    void test(const char *name, void (*callback) (TestContext&) )
    {
        TestContext *context = new TestContext(name);
#ifndef ARDUINO
        try
        {
#else
    // Arduino doesn't like try catch. why? 
#endif
            callback(*(context));
#ifndef ARDUINO
        }
        catch (...)
        {
            context->fail("exception!");
        }
#endif
        _context->add(context);

        delete context;
    }
private:
    TestContext *_context = NULL;
    const char *_name;
    void report(const char *name, int fails, int success, int indent=0);
};

#endif // SUITE_CLASS
