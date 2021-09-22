#include "SequencePlayer.h"

SequencePlayer::SequencePlayer(TrackHandlerRegistry* registry)
{
    handlers = registry;
};

void SequencePlayer::load(JsonDocument* document)
{
    Serial.println("[SEQ] Load");
    sequence = document;
};

void SequencePlayer::tick()
{
    // Guard: only tick when playing
    if (status != Status::PLAYING) return;

    if (!lastMillis) {
        lastMillis = millis();
    }

    // Set time based on delta
    unsigned long now = millis();
    unsigned long iDelta = now - lastMillis;
    float fDelta = (iDelta * 0.001);

    // Scrub forward to position
    scrub(position + fDelta);

    // Update last time
    lastMillis = now;

    // Stop when past end of sequence
    if ((*sequence).containsKey("length") && position > (*sequence)["length"]) {

        // Reset all
        handlers->reset();

        // Loop or stop
        if ((*sequence)["loop"].as<bool>()) {
            position = 0.0f;
        } else {
            stop();
        }
    }
};

void SequencePlayer::scrub(float time)
{
    if (!sequence) return;

    position = time;

    JsonArray tracks = (*sequence)["tracks"].as<JsonArray>();

    for (JsonObject track : tracks) {

        // Locate handler
        String handlerId = track["type"];

        TrackHandler* handler = handlers->getHandler(handlerId.c_str());
        if (!handler) continue;

        JsonArray frames = track["frames"].as<JsonArray>();

        // Guard: Skip tracks with no frames
        if (!frames.size()) continue;

        // Seed frames from zero
        JsonObject framePrev = frames[0].as<JsonObject>();
        JsonObject frameNext = framePrev;

        // Iterate frames to find current position
        for (auto frame : frames) {
            if (frame["time"].as<float>() < position) {
                framePrev = frame;
                frameNext = frame;
            }

            if (framePrev == frameNext) {
                frameNext = frame;
            }
        }

        // Tick handler
        handler->tick(&track, &framePrev, &frameNext, position);
    }
}

void SequencePlayer::play()
{
    position = 0.0f;
    lastMillis = millis();
    status = Status::PLAYING;
}

bool SequencePlayer::isPlaying() {
    return status == Status::PLAYING;
};

void SequencePlayer::stop()
{
    status = Status::STOPPED;
    position = 0.0f;
    handlers->reset();
};

void SequencePlayer::pause()
{
    status = Status::PAUSED;
}

void TrackHandlerRegistry::addHandler(int idx, const char* name, TrackHandler* handler)
{
    // Guard: Prevent unbounded memory writes
    if (idx < 0 || idx > HANDLER_COUNT) {
        return;
    }

    handlers[idx] = handler;
    hashMap[idx] = name;
}

void TrackHandlerRegistry::reset()
{
    for (int i=0; i<HANDLER_COUNT; ++i) {
        if (handlers[i] == nullptr) continue;
        handlers[i]->reset();
    }
}

TrackHandler* TrackHandlerRegistry::getHandler(const char* name)
{
    for (int i=0; i<HANDLER_COUNT; ++i) {
        if (hashMap[i] != nullptr && strcmp(hashMap[i], name) == 0) {
            return handlers[i];
        }
    }

    return nullptr;
}
