#pragma once

#include "Arduino.h"
#include "FS.h"

/**
 * Stores raw sequences as JSON to disk
 */
class SequenceStore
{

public:
    SequenceStore();
    void save(int id, const String data);
    String load(int id);
    int freeBytes();
    
private:
    void getFilename(char* filename, int id);

};