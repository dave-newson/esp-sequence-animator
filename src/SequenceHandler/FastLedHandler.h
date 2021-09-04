#pragma once

#include "Sequencer/SequencePlayer.h"
#include "FastLED.h"

class FastLedHandler : public TrackHandler
{
    public:
        FastLedHandler(CRGB* leds, int ledCount, CFastLED* driver);
        virtual void reset();
        virtual void tick(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time);

    private:
        CFastLED* driver;
        CRGB* leds;
        int ledCount;

        CRGB calcStep(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time);
        CRGB calcLinear(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time);
        CRGB calcFlicker(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time);
        CRGB calcFlame(JsonObject* track, JsonObject* kPrev, int index, float time);
        CRGB calcRainbow(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time);
};
