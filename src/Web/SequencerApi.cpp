#include "SequencerApi.h"
#include "ArduinoJson.h"
#include "DFRobotDFPlayerMini.h"
#include <FastLED.h>
#include "Sequencer/SequenceStore.h"

StaticJsonDocument<512> inputJson;
StaticJsonDocument<512> outputJson;

#define LOG //

void jsonResponse(AsyncWebServerRequest* request, int httpCode, JsonDocument& json) {
  String response;
  serializeJsonPretty(json, response);
  request->send(httpCode, "application/json", response);
}

void SequencerApi::setup(
    AsyncWebServer* server,
    SequenceStore* sequenceStore,
    DFRobotDFPlayerMini* audioPlayer,
    CRGB leds[]
) {

    server->on("/api/sequence", HTTP_POST, [&sequenceStore](AsyncWebServerRequest *request) {
        
        // Content check
        if (!request->hasParam("id", false) || !request->hasParam("body", true)) {
            request->send(400, "application/json", "{}");
            return;
        }

        int id = request->getParam("id", false)->value().toInt();
        String payload = request->getParam("body", true)->value();

        // Size check
        if (sequenceStore->freeBytes() < payload.length()) {
            request->send(413, "application/json", "{}");
            return;
        }

        sequenceStore->save(id, payload);

        String output = String("{\"id\":");
        output += id + "}";
        request->send(200, "application/json", output);
    });

    server->on("/api/sequence", HTTP_GET, [&sequenceStore](AsyncWebServerRequest *request) {
        
        // Content check
        if (!request->hasParam("id", false)) {
            request->send(400, "application/json", "{}");
            return;
        }

        int id = request->getParam("id", false)->value().toInt();
        String payload = sequenceStore->load(id);

        request->send(200, "application/json", payload);
    });

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
