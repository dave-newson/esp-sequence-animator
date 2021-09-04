#include "config.h"
#include <Arduino.h>

// Filesystem
#include "LittleFS.h"

// WIFI
#include <ESP8266WiFi.h>
String hostname(DEVICE_HOSTNAME);

// OTA
#include <ArduinoOTA.h>

// Webserver
#include <ESPAsyncWebServer.h>
#include "Web/Homepage.h"
#include "Web/SequencerApi.h"
AsyncWebServer server(80);

// LED: Windows
#include <FastLED.h>
#define DATA_PIN PIN_SPI_MISO
#define CLOCK_PIN PIN_SPI_SCK
CRGB leds[NUM_LEDS];

// Button
#include "OneButton.h"
OneButton button(PIN_BUTTON, false, true);

// Hardware: DFRobot MP3 Player & Audio Player
#include "DFRobotDFPlayerMini.h"
DFRobotDFPlayerMini audioPlayer;

// JSON Document buffer, used in sequencer
DynamicJsonDocument jsonDocument(JSON_DOC_SIZE);

// Sequencer
#include "Sequencer/SequencePlayer.h"
#include "Sequencer/SequenceStore.h"
SequenceStore sequenceStore(&jsonDocument);
TrackHandlerRegistry trackHandlers;
SequencePlayer sequencer(&trackHandlers);

// Handler
#include "SequenceHandler/PwmLedHandler.h"
PwmLedHandler pwmLedHandler(PIN_PORCH_LIGHT);
#include "SequenceHandler/FastLedHandler.h"
FastLedHandler fastLedHandler(leds, NUM_LEDS, &FastLED);
#include "SequenceHandler/AudioHandler.h"
AudioHandler audioHandler(&audioPlayer);

int currentSequence = 0;

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
  hostname += String(ESP.getChipId(), HEX);

  // Wifi
  LOG("Wifi Connection...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

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

  // Play first track on load
  if (sequenceStore.exists(1)) {
    JsonLoadResponse response = sequenceStore.loadJson(1);
    sequencer.load(response.document);
    sequencer.play();
  }

  // Button
  button.setDebounceTicks(BUTTON_DEBOUNCE);
  button.setClickTicks(BUTTON_PRESS);
  button.setPressTicks(BUTTON_HOLD);

  button.attachLongPressStop([]() {
    sequencer.stop();
  });

  button.attachClick([]() {
    currentSequence++;

    if (!sequenceStore.exists(currentSequence)) {
      currentSequence = 0;
    }
    sequencer.stop();

    JsonLoadResponse result = sequenceStore.loadJson(currentSequence);
    if (result.error == JsonLoadStatus::SUCCESS) {
      sequencer.load(result.document);
      sequencer.play();
    }
  });
}

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

    LOG("[TICK][FREE] ");
    LOG(system_get_free_heap_size());
  }

  button.tick();
}
