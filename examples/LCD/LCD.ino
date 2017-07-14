/*
 * Conexiones:
 *  D1: SCL
 *  D2: SDA
 */
#include "ESP8266Hooks.h"

#include <Wire.h>
#include <LiquidCrystal_I2C.h> // from: https://github.com/agnunez/ESP8266-I2C-LCD1602

#define I2C_ADDR 0x3F
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);

ESP8266Hooks hooks;

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);

	Serial.begin(115200);

	// Simple WiFi connection
	const char *ssid = "<WIFI SSID>";		  //your wifi SSID
	const char *password = "<WIFI PASSOWRD>"; //your wifi password

	WiFi.mode(WIFI_STA);
	IPAddress ip(192, 168, 1, 20);		// set a valid ip for your network
	IPAddress gateway(192, 168, 1, 1);  // set gateway to match your network
	IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your
	WiFi.config(ip, gateway, subnet);
	WiFi.begin(ssid, password);

	Serial.println("");
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(250);
		digitalWrite(LED_BUILTIN, HIGH);
		delay(250);
		digitalWrite(LED_BUILTIN, LOW);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("connected to " + String(ssid) + ". IP: " + WiFi.localIP());

	initLCD();
	lcd.setCursor(0, 0);
	lcd.print("Hello, hooks");
	lcd.setCursor(0, 1);
	lcd.print(WiFi.localIP());

	hooks.init("LCD");

	hooks.registerEvent("text_change", "mac={mac}&line1={line1}&line2={line2}");

	string p[] = {"line1", "line2"};
	hooks.registerAction("show_text", p, showText);

	NameValueCollection parameters(1);
	parameters.push("start", "1");
	hooks.triggerEvent("start", parameters);

	digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
	hooks.handleClient();
	delay(10);
}

void initLCD()
{
	lcd.begin(D2, D1);
	lcd.backlight();
}

int showText(NameValueCollection parameters)
{
	String line1 = parameters["line1"];
	String line2 = parameters["line2"];

	lcd.clear();

	lcd.setCursor(0, 0);
	lcd.print(line1);

	lcd.setCursor(0, 1);
	lcd.print(line2);

	NameValueCollection triggerParameters(2);
	triggerParameters.push("line1", line1);
	triggerParameters.push("line2", line2);
	hooks.triggerEvent("text_change", triggerParameters);

	return 204;
}