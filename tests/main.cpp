#include <stdlib.h>
#include <cstring>
#include "../Hooks.cpp"
#include "../CharTools.cpp"
#include "MinimalistTests/MinimalistTests.h"

void crear_instancia_de_hooks(TestContext &context){
    Hooks *hooks = new Hooks("10:5A", "Dispositivo de Prueba");
    context.assertTrue(hooks!=NULL, "No se pudo instanciar hooks");

    char *definition = hooks->get_definition();
    context.assertAreEqual("{\"name\":\"Dispositivo de Prueba\",\"mac\":\"10:5A\",\"events\":[],\"actions\":[]}", definition);

    delete hooks;
}

void definir_evento_sin_formato(TestContext &context){
    Hooks *hooks = new Hooks("10:5A", "Dispositivo de Prueba");
    hooks->registerEvent("mi_evento", "");

    char *definition = hooks->get_definition();
    context.assertAreEqual("{\"name\":\"Dispositivo de Prueba\",\"mac\":\"10:5A\",\"events\":[{\"name\":\"mi_evento\",\"template\":\"\",\"subscriptions\":[]}],\"actions\":[]}", definition);

    delete hooks;
}

void definir_evento_con_formato(TestContext &context){
    Hooks *hooks = new Hooks("10:5A", "Dispositivo de Prueba");
    hooks->registerEvent("mi_evento", "mac={mac}&tiempo={millis}");

    char *definition = hooks->get_definition();
    context.assertAreEqual("{\"name\":\"Dispositivo de Prueba\",\"mac\":\"10:5A\",\"events\":[{\"name\":\"mi_evento\",\"template\":\"mac={mac}&tiempo={millis}\",\"subscriptions\":[]}],\"actions\":[]}", definition);

    delete hooks;
}

void suscribirse_a_evento(TestContext &context){
    Hooks *hooks = new Hooks("10:5A", "Dispositivo de Prueba");
    hooks->registerEvent("mi_evento", "mac={mac}&tiempo={millis}");

    hooks->subscribeEvent("mi_evento", "http://127.0.0.1:1234/", "mac={mac}&otro={millis}");

    char *definition = hooks->get_definition();
    context.assertAreEqual("{\"name\":\"Dispositivo de Prueba\",\"mac\":\"10:5A\",\"events\":[{\"name\":\"mi_evento\",\"template\":\"mac={mac}&tiempo={millis}\",\"subscriptions\":[{\"target\":\"http://127.0.0.1:1234/\",\"template\":\"mac={mac}&otro={millis}\"}]}],\"actions\":[]}", definition);

    delete hooks;
}

void suscribirse_a_evento_y_lanzarlo(TestContext &context){
    Hooks *hooks = new Hooks("10:5A", "Dispositivo de Prueba");
    hooks->registerEvent("mi_evento", "mac={mac}&tiempo={millis}");
    hooks->subscribeEvent("mi_evento", "http://127.0.0.1:1234/", "mac={mac}&otro={millis}");

    NameValueCollection parameters(1);
    parameters.push("millis", "1598");
    hooks->triggerEvent("mi_evento", parameters);

    Message *message = hooks->get_messages();
    context.assertTrue(message!=NULL, "Se esperaba encontrar un mensaje");
    context.assertAreEqual("http://127.0.0.1:1234/", (char *)message->target);
    context.assertAreEqual("mac=10:5A&otro=1598", (char *)message->body);

    delete hooks;
}

int main(){
    TestSuite suite("Tests eventos");

    suite.test("Crear una nueva instancia de hooks", crear_instancia_de_hooks);
    suite.test("Definir un evento sin formato", definir_evento_sin_formato);
    suite.test("Definir un evento con formato", definir_evento_con_formato);
    suite.test("Suscribirse a un evento", suscribirse_a_evento);
    suite.test("Lanzar evento con suscriptor", suscribirse_a_evento_y_lanzarlo);
}
