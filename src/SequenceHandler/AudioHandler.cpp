#include "AudioHandler.h"
#include "ArduinoJson.h"

AudioHandler::AudioHandler(DFRobotDFPlayerMini* _driver)
{
    driver = _driver;
}

void AudioHandler::reset()
{
    // TODO
}

void AudioHandler::tick(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time)
{
    // TODO
    return;

    int index = (*kPrev)["index"];
    int volume = (*kPrev)["volume"];

    driver->volume(volume); // From 0 to 30
    driver->play(index);
}