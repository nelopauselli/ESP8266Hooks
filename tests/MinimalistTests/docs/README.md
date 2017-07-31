# Minimalist Tests
Minimalist framework to testing C++ **and** Arduino

## Arduino
See [Arduino example](/examples/Testing_setup_and_loop/Testing_setup_and_loop.ino) 

### 1. Include 
```cpp
#include "MinimalistTests.h"
```
### 2. Create a TestSuite
```cpp
TestSuite suite = TestSuite("My tests group");
```
this code is creating a TestSuite named **My tests group**

### 3. Run test cases
```cpp
suite.test("My sums test", testSums);
suite.test("My millis test", testMillis);
```
this code is running two test case named **My sums test** and **My millis test**, calling to **testSums** and to **testMillis** respectively

### 4. Function with the test code
```cpp
void testSums(TestContext &context){
	context.assertTrue(1+1==2, "1+1 should be equal 2");
	context.assertTrue(2+2==4, "2+2 should be equal 4");
	context.assertTrue(0+2==2, "0+2 should be equal 2");
}

void testMillis(TestContext &context){
	context.assertTrue(millis() > 0, "millis() should be greater than 0");
}
```
Here is two function with test code, they receive a parameter of type TestContext by **reference**, it contains methods as assertTrue to do assertions
