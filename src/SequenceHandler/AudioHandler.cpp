#include "AudioHandler.h"
#include "ArduinoJson.h"

AudioHandler::AudioHandler(DFRobotDFPlayerMini* _driver)
{
    driver = _driver;
}

void AudioHandler::reset()
{
    driver->stop();
    kPlayed = nullptr;
}

void AudioHandler::tick(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time)
{
    // Guard: Dont play twice
    if (kPlayed == kPrev) return;

    // Guard: Wait for time
    if (time < (*kPrev)["time"]) return;

    // Play
    int index = (*kPrev)["index"];
    int volume = (*kPrev)["volume"];

    // Prevent play twice by recording the played keyframe
    kPlayed = kPrev;

    driver->volume(volume); // 0 to 30
    driver->play(index);
}