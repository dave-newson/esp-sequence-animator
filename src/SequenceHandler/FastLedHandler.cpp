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
    fract8 blend = (pos * 255.f);

    // Colors
    String valueIn = (*kPrev)["color"];
    CRGB colorIn = strtol(valueIn.substring(1).c_str(), NULL, 16); 

    String valueOut = (*kNext)["color"];
    CRGB colorOut = strtol(valueOut.substring(1).c_str(), NULL, 16); 

    // Color interpolate
    CRGB fin = colorIn.lerp8(colorOut, blend);
    leds[index] = fin;
}
