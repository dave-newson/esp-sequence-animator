#include "SequenceStore.h"
#include "LittleFS.h"

SequenceStore::SequenceStore(JsonDocument* _documentBuf)
{
    documentBuf = _documentBuf;
}

void SequenceStore::save(int id, const String data) {
    char filename[32];
    getFilename(filename, id);
    
    File testFile = LittleFS.open(filename, "w");
    testFile.print(data);
    testFile.close();
}

bool SequenceStore::exists(int id) {

    char filename[32];
    getFilename(filename, id);

    return LittleFS.exists(filename);
}

String SequenceStore::load(int id) {

    char filename[32];
    getFilename(filename, id);

    if (!LittleFS.exists(filename)) {
        return "{}";
    }

    File file = LittleFS.open(filename, "r");
    String data = file.readString();

    file.close();

    return data;
}

JsonLoadResponse SequenceStore::loadJson(int id) {

    JsonLoadResponse response;

    char filename[32];
    getFilename(filename, id);

    if (!LittleFS.exists(filename)) {
        response.error = JsonLoadStatus::DOES_NOT_EXIST;
        return response;
    }

    File file = LittleFS.open(filename, "r");
    DeserializationError error = deserializeJson((*documentBuf), file);
    file.close();

    if (error) {
        response.error = JsonLoadStatus::DECODE_ERROR;
        response.documentError = error;
        return response;
    }

    response.document = documentBuf;
    return response;
}

void SequenceStore::getFilename(char* buffer, int id)
{
    sprintf(buffer, "/sequence/%d", id);
}

unsigned int SequenceStore::freeBytes() {
    FSInfo info;
    LittleFS.info(info);
    return info.totalBytes - info.usedBytes;
}
