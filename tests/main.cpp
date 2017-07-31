#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <assert.h>
#include "../Hooks.cpp"
#include "../CharTools.cpp"

using namespace std;

void crear_instancia_de_hooks(){
    Hooks *hooks = new Hooks("10:5A", "Dispositivo de Prueba");
    assert(hooks!=NULL);

    char *definition = hooks->get_definition();
    cout << definition << endl;
    assert(strcmp(definition, "{\"name\":\"Dispositivo de Prueba\",\"mac\":\"10:5A\",\"events\":[],\"actions\":[]}")==0);

    delete hooks;
    cout << "tests passed!" << endl;
}

void definir_evento_sin_formato(){
    Hooks *hooks = new Hooks("10:5A", "Dispositivo de Prueba");
    hooks->registerEvent("mi_evento", "");

    char *definition = hooks->get_definition();
    cout << definition << endl;
    assert(strcmp(definition, "{\"name\":\"Dispositivo de Prueba\",\"mac\":\"10:5A\",\"events\":[{\"name\":\"mi_evento\",\"template\":\"\",\"subscriptions\":[]}],\"actions\":[]}")==0);

    delete hooks;
    cout << "tests passed!" << endl;
}

void definir_evento_con_formato(){
    Hooks *hooks = new Hooks("10:5A", "Dispositivo de Prueba");
    hooks->registerEvent("mi_evento", "mac={mac}&tiempo={millis}");

    char *definition = hooks->get_definition();
    cout << definition << endl;
    assert(strcmp(definition, "{\"name\":\"Dispositivo de Prueba\",\"mac\":\"10:5A\",\"events\":[{\"name\":\"mi_evento\",\"template\":\"mac={mac}&tiempo={millis}\",\"subscriptions\":[]}],\"actions\":[]}")==0);

    delete hooks;
    cout << "tests passed!" << endl;
}

void suscribirse_a_evento(){
    Hooks *hooks = new Hooks("10:5A", "Dispositivo de Prueba");
    hooks->registerEvent("mi_evento", "mac={mac}&tiempo={millis}");

    hooks->subscribeEvent("mi_evento", "http://127.0.0.1:1234/", "mac={mac}&otro={millis}");

    char *definition = hooks->get_definition();
    cout << definition << endl;
    assert(strcmp(definition, "{\"name\":\"Dispositivo de Prueba\",\"mac\":\"10:5A\",\"events\":[{\"name\":\"mi_evento\",\"template\":\"mac={mac}&tiempo={millis}\",\"subscriptions\":[{\"target\":\"http://127.0.0.1:1234/\",\"template\":\"mac={mac}&otro={millis}\"}]}],\"actions\":[]}")==0);

    delete hooks;
    cout << "tests passed!" << endl;
}

void suscribirse_a_evento_y_lanzarlo(){
    Hooks *hooks = new Hooks("10:5A", "Dispositivo de Prueba");
    hooks->registerEvent("mi_evento", "mac={mac}&tiempo={millis}");
    hooks->subscribeEvent("mi_evento", "http://127.0.0.1:1234/", "mac={mac}&otro={millis}");

    cout << "Creando parametros del evento" << endl;

    NameValueCollection parameters(1);
    parameters.push("millis", "1598");

    cout << "Lanzando evento" << endl;

    hooks->triggerEvent("mi_evento", parameters);

    cout << "Buscando mensajes" << endl;

    Message *message = hooks->get_messages();
    assert(message!=NULL);
    assert(strcmp(message->target, "http://127.0.0.1:1234/")==0);
    assert(strcmp(message->body, "mac=10:5A&otro=1598")==0);

    delete hooks;
    cout << "tests passed!" << endl;
}

int main(){
    crear_instancia_de_hooks();
    definir_evento_sin_formato();
    definir_evento_con_formato();
    suscribirse_a_evento();
    suscribirse_a_evento_y_lanzarlo();
}
