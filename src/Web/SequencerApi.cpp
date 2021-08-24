#include "SequencerApi.h"
#include "ArduinoJson.h"
#include "DFRobotDFPlayerMini.h"
#include <FastLED.h>

StaticJsonDocument<512> inputJson;
StaticJsonDocument<512> outputJson;

#define LOG //

void jsonResponse(AsyncWebServerRequest* request, int httpCode, JsonDocument& json) {
  String response;
  serializeJsonPretty(json, response);
  request->send(httpCode, "application/json", response);
}

void SequencerApi::setup(AsyncWebServer* server, Sequencer* sequencer, DFRobotDFPlayerMini* audioPlayer, CRGB leds[]) {
    server->on("/api/state", HTTP_POST, [&audioPlayer, &leds](AsyncWebServerRequest *request) {

      outputJson.clear();

      if (!request->hasParam("body", true)) {
        outputJson["error"] = "Body missing";
        jsonResponse(request, 400, outputJson);
        return;
      }

      // Derserialize
      DeserializationError error = deserializeJson(inputJson, request->getParam("body", true)->value());
      if (error) {
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

          // Str to CRGB
          color = color.substring(1);
          leds[index] = strtol(color.c_str(), NULL, 16);
        }

        if (v["type"] == "audio") {
          int index;
          int volume;
          index = v["index"].as<int>();
          volume = v["volume"].as<int>();
          
          audioPlayer->volume(volume); // From 0 to 30
          audioPlayer->play(index);
        }
      }

      // Update LEDs
      FastLED.show();

      request->send(200, "application/json", "{}");
    });

}
