#include "SequencerApi.h"
#include "ArduinoJson.h"
#include "DFRobotDFPlayerMini.h"
#include <FastLED.h>
#include "Sequencer/SequenceStore.h"
#include "Sequencer/SequencePlayer.h"

StaticJsonDocument<512> inputJson;
StaticJsonDocument<512> sequence;

#define LOG //

void jsonResponse(AsyncWebServerRequest* request, int httpCode, JsonDocument& json) {
  String response;
  serializeJsonPretty(json, response);
  request->send(httpCode, "application/json", response);
}

void SequencerApi::setup(
    AsyncWebServer* server,
    SequenceStore* sequenceStore,
    SequencePlayer* sequencePlayer
) {

    // Save a sequence
    server->on("/api/sequence", HTTP_POST, [sequenceStore](AsyncWebServerRequest *request) {
        
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

    // Fetch a sequence
    server->on("/api/sequence", HTTP_GET, [sequenceStore](AsyncWebServerRequest *request) {
        
        // Content check
        if (!request->hasParam("id", false)) {
            request->send(400, "application/json", "{}");
            return;
        }

        int id = request->getParam("id", false)->value().toInt();
        String payload = sequenceStore->load(id);

        request->send(200, "application/json", payload);
    });

    server->on("/api/play", HTTP_POST, [sequenceStore, sequencePlayer](AsyncWebServerRequest *request) {
        // Content check
        if (!request->hasParam("id", false)) {
            request->send(400, "application/json", "{}");
            return;
        }

        int id = request->getParam("id", false)->value().toInt();
        String payload = sequenceStore->load(id);

        DeserializationError error = deserializeJson(sequence, payload);

        sequencePlayer->loop = true;
//        sequencePlayer->stop();
        sequencePlayer->load(&sequence);
        sequencePlayer->play();

        request->send(200, "application/json", "{}");
    });

}
