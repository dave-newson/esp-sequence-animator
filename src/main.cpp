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
#define OTA_UPDATE_PORT 8266
#define OTA_UPDATE_PASS "password"

// Webserver
#include <ESPAsyncWebServer.h>
#include "ArduinoJson.h"
AsyncWebServer server(80);
const char* PARAM_INDEX = "index";
const char* PARAM_COLOR = "color";
StaticJsonDocument<512> inputJson;
StaticJsonDocument<512> outputJson;

// LEDs
#include <FastLED.h>
#define NUM_LEDS 50
#define DATA_PIN PIN_SPI_MISO
#define CLOCK_PIN PIN_SPI_SCK
CRGB leds[NUM_LEDS];

// Hardware: DFRobot MP3 Player & Audio Player
#include "DFRobotDFPlayerMini.h"
#include "SoftwareSerial.h"
#define PIN_AUDIO_TX 5
#define PIN_AUDIO_RX 4
SoftwareSerial audioSerial(PIN_AUDIO_RX, PIN_AUDIO_TX);
DFRobotDFPlayerMini audioPlayer;


void jsonResponse(AsyncWebServerRequest* request, int httpCode, JsonDocument& json) {
    String response;
    serializeJsonPretty(json, response);
    request->send(httpCode, "application/json", response);
}

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

    // MP3 Player
    Serial.println("Audio player...");
    audioSerial.begin(9600);
    audioPlayer.begin(audioSerial);

    Serial.println("Webserver...");
    Serial.println("Webserver starting ...");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(200, "text/html",
"<html>"
"\n <body>"
"\n   <h1>Addams Family Mansion</h1>"
"\n   <h2>API Endpoints</h2>"
"\n   <pre style=\"background: #eee;\">"
"\n"
"\n   GET  /api/device"
"\n     Show information about the device"
"\n"
"\n   GET  /api/state"
"\n     Get current device state"
"\n"
"\n   POST /api/state"
"\n     Set state for peripherals on the device"
"\n"
"\n   POST /api/sequence/{slot}"
"\n     Set a sequence on the device"
"\n"
"\n   GET  /api/sequence/{slot}"
"\n     Retrieve a sequence from the device"
"\n"
"\n   POST /api/sequence/{slot}/play"
"\n     Play the sequence"
"\n"
"\n   </pre>"
"\n   <h2>OTA Update</h2>"
"\n   <p>Over The Air Update is available on port 8622</p>"
"\n </body>"
"\n</html>"
      );
    });

    server.on("/api/state", HTTP_POST, [](AsyncWebServerRequest *request) {

      outputJson.clear();
      Serial.println("[WEB] /api/state");
      if (!request->hasParam("body", true)) {
        outputJson["error"] = "Body missing";
        jsonResponse(request, 400, outputJson);
        return;
      }

      // Derserialize
      DeserializationError error = deserializeJson(inputJson, request->getParam("body", true)->value());
      if (error) {
      Serial.println("[WEB] ERROR: Can't decode");
        outputJson["error"] = error.c_str();
        jsonResponse(request, 400, outputJson);
        return;
      }

      // Process lights
      JsonArray array = inputJson["keys"].as<JsonArray>();
      for(JsonVariant v : array) {

        // Light handler
        if (v["type"] == "light") {
          int index;
          String color;
          index = v["index"].as<int>();
          color = v["color"].as<String>();

          // Skip invalid indexes
          if (index > NUM_LEDS) {
            continue;
          }

          Serial.print("[WEB] Setting LED: ");
          Serial.print(index, DEC);
          Serial.print(": ");
          Serial.print(color);
          Serial.println("");

          // Str to CRGB
          color = color.substring(1);
          leds[index] = strtol(color.c_str(), NULL, 16);
        }

        if (v["type"] == "audio") {
          int index;
          int volume;
          index = v["index"].as<int>();
          volume = v["volume"].as<int>();
          
          audioPlayer.volume(volume); // From 0 to 30
          audioPlayer.play(index);
        }
      }

      // Update LEDs
      FastLED.show();

      request->send(200, "application/json", "{}");
    });

    server.onNotFound([](AsyncWebServerRequest *request) {
        outputJson["error"] = "Not found";
        jsonResponse(request, 404, outputJson);
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