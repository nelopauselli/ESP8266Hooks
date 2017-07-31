#include "MinimalistTests.h"

void setup()
{
  Serial.begin(9600);
  TestSuite suite = TestSuite("Tests de encriptacion"); // creo un grupo de pruebas
  suite.test("Encriptar ABC", encryptABCTest); // ejecuto la primera prueba
  suite.test("Encriptar XXXX", encryptXXXXTest); // ejecuto la segunda prueba
}

void loop() {}

/*
 * Esta es la función que quiero testear
 */
void encrypt(char *source, char *buffer)
{
  int length = strlen(source);
  for (int i = 0; i < length; i++)
  {
    char c = source[i];
    if (c != '\0')
      buffer[i] = (c + 2); // very very bad idea to encrypt.
    else
      break;
  }
  buffer[length] = '\0';
}

/*
 * Estos son los tests
 */
void encryptABCTest(TestContext &context)
{
  char buffer[16]; // creo un buffer para el resultado
  encrypt("ABC", buffer); // encripto la cadena ABC en el buffer
  context.assertTrue(3 == strlen(buffer), "Tamaño incorrecto!"); // verifico que el tamaño de la cadena encriptada sea el esperado
  context.assertAreEqual("CDE", buffer, "Resultado incorrecto!"); // verifico que el contenido de la cadena encriptada sea el esperado
}

void encryptXXXXTest(TestContext &context)
{
  char buffer[16]; // creo un buffer para el resultado
  encrypt("XXXX", buffer); // encripto la cadena XXXX en el buffer
  context.assertAreEqual("ZZZX", buffer); // esta afirmación va a fallar.
}