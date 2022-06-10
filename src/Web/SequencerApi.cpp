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
    server->on("/api/device", HTTP_GET, [sequencePlayer, sequenceStore](AsyncWebServerRequest *request) {
        String out = "{";

        // Device
        out += "\"id\":\"";
        out += ESP.getChipId();
        out += "\", \"heapFree\":\"";
        out += ESP.getFreeHeap();

        // Disk
        FSInfo64 info;
        LittleFS.info64(info);
        out += "\", \"diskSize\":\"";
        out += info.totalBytes;
        out += "\", \"diskFree\":\"";
        out += (info.totalBytes - info.usedBytes);

        // Playing
        out += "\", \"playing\":";
        out += sequencePlayer->isPlaying() ? "true" : "false";

        out += "\"}";

        request->send(200, "application/json", out);
    });

    server->on("/api/sequence-list", HTTP_GET, [](AsyncWebServerRequest *request) {
        // TODO: Drive this from disk-based json
        const char* out = R"JSON(
        {"sequences": [
            {"id": 1, "name": "Random room lights"},
            {"id": 2, "name": "Addams Family Theme"},
            {"id": 3, "name": "Flames"},
            {"id": 4, "name": "White"},
            {"id": 5, "name": "Rainbow"},
            {"id": 6, "name": "Electrocution"},
            {"id": 7, "name": "Golf"},
            {"id": 8, "name": "Tulley is here"},
            {"id": 9, "name": "Ghost"}
        ]}
        )JSON";

        request->send(200, "application/json", out);      
    });

    // Save sequence list
    server->on("/api/sequence-list", HTTP_POST, [](AsyncWebServerRequest* request) {
        // TODO
            request->send(404, "application/json", "{}");
    });

    // Save a sequence
    server->on("/api/sequence", HTTP_POST, [](AsyncWebServerRequest* request) {
        
        // Content check
        if (!request->hasParam("id", false) || !request->hasParam("body", true)) {
            request->send(400, "application/json", "{}");
            return;
        }

        // ContentType requiremnt, for onRequestBody function to be called.
        if (!request->hasHeader("ContentType") || request->header("ContentType").startsWith("application/json")) {
            request->send(400, "application/json", "{\"error\":\"ContentType must be application/json\"}");
        }

    }, NULL, [sequenceStore](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
 
        int id = request->getParam("id", false)->value().toInt();
        File f = sequenceStore->getFile(id);

        // Truncate for overwrite on first open
        if (index == 0) {
            f.truncate(0);
            f.seek(0);
        }

        for (size_t i = 0; i < len; i++) {
            f.write(data[i]);
        }

        if (index + len == total) {
            f.close();
            String output = "{";
            output += "\"id\":";
            output += id;
            output += "\"length\":"; 
            output += f.size();
            output += "}";
            request->send(200, "application/json", output);
        }
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

        sequencePlayer->stop();

        int id = request->getParam("id", false)->value().toInt();
        JsonLoadResponse result = sequenceStore->loadJson(id);

        // Handle errors
        if (result.error != JsonLoadStatus::SUCCESS) {
            String response = "{\"error\":";
            response.concat(result.documentError.f_str());
            response.concat("}");
            request->send(500, "application/json", response);
            return;
        }

        sequencePlayer->load(result.document);
        sequencePlayer->play();

        request->send(200, "application/json", "{}");
    });
}
