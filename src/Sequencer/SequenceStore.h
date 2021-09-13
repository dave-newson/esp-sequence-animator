#pragma once

#include "Arduino.h"
#include "FS.h"
#include "ArduinoJson.h"

enum JsonLoadStatus {
    SUCCESS,
    DOES_NOT_EXIST,
    DECODE_ERROR,
};

struct JsonLoadResponse
{
    JsonLoadStatus error = JsonLoadStatus::SUCCESS;
    DeserializationError documentError;
    JsonDocument* document;
};

/**
 * Store & load raw sequence as JSON to disk.
 * Load from disk as JsonDocument, using provided buffer
 */
class SequenceStore
{

public:
    SequenceStore(JsonDocument* sequenceBuf);
    void save(int id, const String data);
    File getFile(int id);
    bool exists(int id);
    String load(int id);
    JsonLoadResponse loadJson(int id);
    unsigned int freeBytes();
    
private:
    void getFilename(char* filename, int id);
    JsonDocument* documentBuf;
};
