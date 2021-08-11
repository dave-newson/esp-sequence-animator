#include <Arduino.h>

// WIFI
#include <ESP8266WiFi.h>
#define DEVICE_PREFIX "test"
String hostname("");

// WIFI Auth
const char* ssid = "wifi_ssid";
const char* password = "wifi_password";

// OTA
#include <ArduinoOTA.h>
#define OTA_UPDATE_PORT 8622
#define OTA_UPDATE_PASS "password"

// Webserver
#include <ESPAsyncWebServer.h>
AsyncWebServer server(80);
const char* PARAM_INDEX = "index";
const char* PARAM_COLOR = "color";


// LEDs
#include <FastLED.h>
#define NUM_LEDS 50
#define DATA_PIN PIN_SPI_MISO
#define CLOCK_PIN PIN_SPI_SCK
CRGB leds[NUM_LEDS];


void setup() {
    Serial.begin(9600);

    Serial.println("");
    delay(1000);
    Serial.println("Booting...");

    // Network
    hostname += (String(DEVICE_PREFIX) + String(ESP.getChipId(), HEX));

    // Wifi
    Serial.println("Wifi Connection...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        return;
    }

  // OTA
  Serial.println("OTA service...");
  ArduinoOTA.setPort(OTA_UPDATE_PORT);
  ArduinoOTA.setHostname(hostname.c_str());
  ArduinoOTA.setPassword(OTA_UPDATE_PASS);
  ArduinoOTA.begin();

    // LEDs
    Serial.println("LED controller...");
    FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    FastLED.clear();

    Serial.println("Webserver...");
    Serial.println("Webserver starting ...");
        Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "application/json", "{}");
    });

    server.on("/test", HTTP_GET, [](AsyncWebServerRequest *request){
        String index;
        String color;
        index = request->hasParam(PARAM_INDEX, false) ? request->getParam(PARAM_INDEX, false)->value() : "0";
        color = request->hasParam(PARAM_COLOR, false) ? request->getParam(PARAM_COLOR, false)->value() : "#000000";
        
        // Str to CRGB
        leds[index.toInt()] = strtol(color.c_str(), NULL, 16);
        FastLED.show();

        request->send(200, "application/json", "{}");
    });

    server.onNotFound([](AsyncWebServerRequest *request){
        request->send(404, "application/json", "{\"error\": \"Not Found\"}");
    });
    
    server.begin();

    Serial.println("Boot complete.");
}


void loop() {

    ArduinoOTA.handle();

/*
    // Move a single white led 
    for(int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1) {
        // Turn our current led on to white, then show the leds
        leds[whiteLed] = 0x101010; //CRGB::White;

        // Show the leds (only one of which is set to white, from above)
        FastLED.show();

        // Wait a little bit
        delay(50);

        // Turn our current led back to black for the next loop around
        leds[whiteLed] = CRGB::Black;
    }
*/
}