#pragma once

#include "Sequencer/SequencePlayer.h"
#include <FastLED.h>

#define LED_COUNT 50

class FastLedHandler : public TrackHandler
{
    public:
        FastLedHandler(CFastLED* driver);
        virtual void reset();
        virtual void tick(float time, JsonObject* keyframeBefore, JsonObject* keyframeAfter);

    private:
        CFastLED* driver;
        CRGB leds[LED_COUNT];
};
