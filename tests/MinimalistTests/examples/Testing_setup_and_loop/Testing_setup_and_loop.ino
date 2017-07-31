#include "MinimalistTests.h"

void setup() {
  Serial.begin(9600);

  TestSuite suite = TestSuite("My tests group");
  suite.test("My sums test", testSums);
  suite.test("My millis test", testMillis);
}

void loop() {
  TestSuite suite = TestSuite("Other tests group");
  suite.test("My millis test", testMillis);

  delay(1000);
}

void testSums(TestContext &context){
	context.assertTrue(1+1==2, "1+1 should be equal 2");
  context.assertTrue(2+2==4, "2+2 should be equal 4");
  context.assertTrue(0+2==2, "0+2 should be equal 2");
}

void testMillis(TestContext &context){
	context.assertTrue(millis() > 0, "millis() should be greater than 0");
}