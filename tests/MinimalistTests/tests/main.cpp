#include "../MinimalistTests.h"

/*
 * function to test
 */
int sum(int n1, int n2)
{
    return n1+n2;
}

/*
 * test cases of function sum
 */
void testSums(TestContext &context)
{
    context.assertTrue(sum(1, 1)==2, "1+1 should be equal 2");
    context.assertTrue(sum(2, 2)==4, "2+2 should be equal 4");
    context.assertTrue(sum(0, 2)==2, "0+2 should be equal 2");
}

/*
 * Running tests
 */
int main()
{
    TestSuite suite = TestSuite("My tests group");
    suite.test("My sums test", testSums);
}
