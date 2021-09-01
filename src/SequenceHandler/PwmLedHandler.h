#pragma once

#include "Sequencer/SequencePlayer.h"
#include <FastLED.h>

class PwmLedHandler : public TrackHandler
{
    public:
        PwmLedHandler(int pin);
        virtual void reset();
        virtual void tick(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time);

    private:
        int pin;
        float lerp(float a, float b, float x);
};
