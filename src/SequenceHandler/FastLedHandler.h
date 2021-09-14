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
        int ledCount = 0;
        float lastChange = 0.0f;

        void calcStep       (JsonObject* track, JsonObject* kPrev, JsonObject* kNext, JsonVariant node, float time);
        void calcLinear     (JsonObject* track, JsonObject* kPrev, JsonObject* kNext, JsonVariant node, float time);
        void calcFlicker    (JsonObject* track, JsonObject* kPrev, JsonObject* kNext, JsonVariant node, float time);
        void calcFlame      (JsonObject* track, JsonObject* kPrev, JsonObject* kNext, JsonVariant node, float time);
        void calcRainbow    (JsonObject* track, JsonObject* kPrev, JsonObject* kNext, JsonVariant node, float time);
        void calcComeAndGo  (JsonObject* track, JsonObject* kPrev, float time);

        CRGB getColor(JsonVariant node);
        void setColor(const JsonVariant node, const CRGB color);
};
