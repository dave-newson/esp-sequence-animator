#include "SequencerApi.h"
#include "Sequencer/SequenceStore.h"
#include "Sequencer/SequencePlayer.h"
#include "LittleFS.h"

#define LOG //

void SequencerApi::setup(
    AsyncWebServer* server,
    SequenceStore* sequenceStore,
    SequencePlayer* sequencePlayer
) {

    // Device status
    server->on("/api/device", HTTP_GET, [sequenceStore](AsyncWebServerRequest *request) {
        String out = "{";
        out += "\"id\":\"";
        out += ESP.getChipId();
        out += "\", \"heapFree\":\"";
        out += ESP.getFreeHeap();

        FSInfo64 info;
        LittleFS.info64(info);
        out += "\", \"diskSize\":\"";
        out += info.totalBytes;
        out += "\", \"diskFree\":\"";
        out += (info.totalBytes - info.usedBytes);
        out += "\"}";

        request->send(200, "application/json", out);
    });

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
        JsonLoadResponse result = sequenceStore->loadJson(id);

        // Handle errors
        if (result.error != JsonLoadStatus::SUCCESS) {
            request->send(500, "application/json", "{\"error\":\"Cannot parse json\"}");
            return;
        }

        sequencePlayer->stop();
        sequencePlayer->load(result.document);
        sequencePlayer->play();

        request->send(200, "application/json", "{}");
    });
}
