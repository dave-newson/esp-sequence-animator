#include "SequenceStore.h"
#include "LittleFS.h"

SequenceStore::SequenceStore()
{

}

void SequenceStore::save(int id, const String data) {
    char filename[32];
    getFilename(filename, id);
    
    File testFile = LittleFS.open(filename, "w");
    testFile.print(data);
    testFile.close();
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

void SequenceStore::getFilename(char* buffer, int id)
{
    sprintf(buffer, "/sequence/%d", id);
}

int SequenceStore::freeBytes() {
    FSInfo info;
    LittleFS.info(info);
    return info.totalBytes - info.usedBytes;
}
