#include "AudioHandler.h"
#include "ArduinoJson.h"

AudioHandler::AudioHandler(DFRobotDFPlayerMini* _driver)
{
    driver = _driver;
}

void AudioHandler::reset()
{
    driver->stop();
}

void AudioHandler::tick(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time)
{
    // Guard: Dont play twice
    if (kPlayed == kPrev) return;

    // Guard: Wait for time
    if (time < (*kPrev)["time"]) return;

    // Play
    int index = (*kNext)["index"];
    int volume = (*kNext)["volume"];

    driver->volume(volume); // From 0 to 30
    driver->play(index);
}