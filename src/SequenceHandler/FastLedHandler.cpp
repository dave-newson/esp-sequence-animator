#include "FastLedHandler.h"

#define LED_RANGE 512

FastLedHandler::FastLedHandler(CRGB* _leds, CFastLED* _driver)
{
    driver = _driver;
    leds = _leds;
}

void FastLedHandler::reset()
{
    driver->clear();
    driver->show();
}

void FastLedHandler::tick(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time)
{
    // Track props
    int index = (*track)["index"];

    CRGB color;

    if ((*kPrev)["effect"] == "step") {
        color = calcStep(track, kPrev, kNext, time);
    } else if ((*kPrev)["effect"] == "flicker") {
        color = calcFlicker(track, kPrev, kNext, time);        
    } else {
        color = calcLinear(track, kPrev, kNext, time);
    }

    // Update index in colors array
    leds[index] = color;

}

CRGB FastLedHandler::calcStep(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time)
{
    String valueIn = (*kPrev)["color"];
    return strtol(valueIn.substring(1).c_str(), NULL, 16);
}

CRGB FastLedHandler::calcLinear(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time)
{
    // Timing
    float start = (*kPrev)["time"];
    float end = (*kNext)["time"];

    // Calculate position between frames
    float pos = 0.0f;
    if (time > start && start < end) {
        float length = (end - start);
        float now = time - start;
        pos = now / length;
    }

    // Float to unsigned-fract16
    fract16 blend = (pos * 65535.f);

    // Colors
    String valueIn = (*kPrev)["color"];
    CRGB colorIn = strtol(valueIn.substring(1).c_str(), NULL, 16); 

    String valueOut = (*kNext)["color"];
    CRGB colorOut = strtol(valueOut.substring(1).c_str(), NULL, 16); 

    // Color interpolate
    return colorIn.lerp16(colorOut, blend);
}

CRGB FastLedHandler::calcFlicker(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time)
{
    String valueIn = (*kPrev)["color"];
    CRGB color = strtol(valueIn.substring(1).c_str(), NULL, 16);

    // Black
    CRGB flicker = 0;

    // Flicker randomiser
    fract16 blend = random16();

    return color.lerp16(flicker, blend);
}