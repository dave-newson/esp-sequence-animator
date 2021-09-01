#pragma once

#include "Sequencer/SequencePlayer.h"
#include <FastLED.h>

#define LED_COUNT 50

class FastLedHandler : public TrackHandler
{
    public:
        FastLedHandler(CRGB* leds, CFastLED* driver);
        virtual void reset();
        virtual void tick(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time);

    private:
        CFastLED* driver;
        CRGB* leds;
};