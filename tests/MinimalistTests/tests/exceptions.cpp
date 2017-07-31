#include "../MinimalistTests.h"

/*
 * function to test
 */
int throwIntException(int n1, int n2)
{
    throw 1;
}

/*
 * test cases of function throwIntException
 */
void testException(TestContext &context)
{
    context.assertTrue(throwIntException(1, 1), "This case should by fail");
}

int main()
{
    {
        TestSuite suite = TestSuite("My tests group");
        suite.test("My sums test", testException);
    }
}
