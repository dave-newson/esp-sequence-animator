#include <Arduino.h>

// Pins
#define LED_BUILTIN 2 // For ESP12-F

// Filesystem
#include "FS.h"
#include "LittleFS.h"

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
#include "Web/Homepage.h"
#include "Web/SequencerApi.h"
#include "ArduinoJson.h"
AsyncWebServer server(80);

// LEDs
#include <FastLED.h>
#define NUM_LEDS 50
#define LED_BRIGHTNESS 10
#define DATA_PIN PIN_SPI_MISO
#define CLOCK_PIN PIN_SPI_SCK
CRGB leds[NUM_LEDS];

// Hardware: DFRobot MP3 Player & Audio Player
#include "DFRobotDFPlayerMini.h"
DFRobotDFPlayerMini audioPlayer;

// Sequencer
#include "Sequencer/SequencePlayer.h"
#include "Sequencer/SequenceStore.h"
SequenceStore sequenceStore;
TrackHandlerRegistry trackHandlers;
SequencePlayer sequencer(&trackHandlers);

// Handler
#include "SequenceHandler/PwmLedHandler.h"
PwmLedHandler pwmLedHandler(PIN_SPI_MOSI);
#include "SequenceHandler/FastLedHandler.h"
FastLedHandler fastLedHandler(leds, &FastLED);
#include "SequenceHandler/AudioHandler.h"
AudioHandler audioHandler(&audioPlayer);

#define LOG Serial.println

void setup() {

  Serial.begin(9600);

  // Debug LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, false);

  delay(1000);
  LOG("Booting...");

  // Handler
  trackHandlers.addHandler(0, "led1", &pwmLedHandler);
  trackHandlers.addHandler(1, "fastled", &fastLedHandler);
  trackHandlers.addHandler(2, "audio", &audioHandler);

  LOG("Filesystem...");
  if (!LittleFS.begin()){
    LOG("ERROR: Failed to init filesystem.");
  }

  if (!LittleFS.exists(".filesystem")) {
    LOG("Formatting filesystem...");
    LittleFS.format();
    File testFile = LittleFS.open(F("/.filesystem"), "w");
    testFile.print(" ");
    testFile.close();
  }

  FSInfo info;
  LittleFS.info(info);
  LOG("Total space:");
  LOG(info.totalBytes);
  LOG("Used space:");
  LOG(info.usedBytes);

  // Network
  hostname += (String(DEVICE_PREFIX) + String(ESP.getChipId(), HEX));

  // Wifi
  LOG("Wifi Connection...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      LOG("WiFi Failed!\n");
      return;
  }

  LOG("IP Address: ");
  LOG(WiFi.localIP());

  // OTA
  LOG("OTA service...");
  ArduinoOTA.setPort(OTA_UPDATE_PORT);
  ArduinoOTA.setHostname(hostname.c_str());
  ArduinoOTA.setPassword(OTA_UPDATE_PASS);
  ArduinoOTA.begin();

  // LEDs
  LOG("LED controller...");
  FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(LED_BRIGHTNESS);
  FastLED.clear();

  // MP3 Player
  LOG("Audio player...");
  audioPlayer.begin(Serial);

  // Webserver
  LOG("Webserver starting ...");
  WebHompage::setup(&server);
  SequencerApi::setup(&server, &sequenceStore, &sequencer);

  server.onNotFound([](AsyncWebServerRequest *request) {
      request->send(404, "application/json", "{\"error\": \"Not found\"}");
  });

  server.begin();

  LOG("Boot complete.");
  digitalWrite(LED_BUILTIN, true);
}

#define FRAMES_PER_SECOND 120

int tickLoop = 0;
int tickSeq = 0;

void loop() {
  tickLoop++;

  ArduinoOTA.handle();

  // Play sequence
  EVERY_N_MILLISECONDS( 1000 / FRAMES_PER_SECOND ) {
    tickSeq++;
    sequencer.tick();
    FastLED.show();
  }

  // Blink LED
  EVERY_N_SECONDS( 1 ) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN) );
    LOG("[TICK][LOOP] ");
    LOG(tickLoop);
    tickLoop = 0;

    LOG("[TICK][SEQN] ");
    LOG(tickSeq);
    tickSeq = 0;
  }
}