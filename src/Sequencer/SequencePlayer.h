#pragma once

#include "ArduinoJSON.h"

#define HANDLER_COUNT 3
#define SEQUENCE_JSON_SIZE 512

class TrackHandler
{
    public:
        virtual void reset() = 0;
        virtual void tick(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time) = 0;
};

class TrackHandlerRegistry
{
    public:
        void addHandler(int idx, const char* name, TrackHandler* handler);
        TrackHandler* getHandler(const char* name);
        void reset();

    private:
        TrackHandler* handlers[HANDLER_COUNT] = {};
        const char* hashMap[HANDLER_COUNT] = {};
};

/**
 * Plays a given Sequence.
 */
class SequencePlayer
{
    enum Status {
        PLAYING,
        STOPPED,
        PAUSED,
    };

public:
    SequencePlayer(TrackHandlerRegistry*);

    void load(StaticJsonDocument<SEQUENCE_JSON_SIZE>*); 
    void play();
    void pause();
    void stop();
    void tick();
    void scrub(float time);
    bool isPlaying();
    bool loop = false;

private:
    Status status = Status::STOPPED;
    unsigned long lastMillis = 0;
    float position = 1.0f;
    StaticJsonDocument<SEQUENCE_JSON_SIZE>* sequence = nullptr;
    TrackHandlerRegistry* handlers;
};
