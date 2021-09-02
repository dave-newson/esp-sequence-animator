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
    } else if ((*kPrev)["effect"] == "flame") {
        color = calcFlame(track, kPrev, kNext, time);        
    } else if ((*kPrev)["effect"] == "rainbow") {
        color = calcRainbow(track, kPrev, kNext, time);        
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

    // Flicker randomiser
    float intensity = (*kPrev)["intensity"];
    fract16 blend = random16() * intensity;

    CRGB black = 0;

    return color.lerp16(black, blend);
}

CRGB FastLedHandler::calcFlame(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time)
{
    String value = (*kPrev)["color"];
    CRGB color = strtol(value.substring(1).c_str(), NULL, 16);

    // Flash rate: Random chance rate at which full brightness is restored.
    fract16 flash = 65535.0f * (kPrev->containsKey("flash") ? (*kPrev)["flash"] : 0.1f);
    if (random16() < flash) {
        return color;
    }

    // Shrink rate: Random affector for shrinking the flame
    fract16 shrink = random16() * (kPrev->containsKey("shrink") ? (*kPrev)["shrink"] : 0.1f);
    shrink *= 0.5f;

    // Get the current color, to compound-subtract brightness from
    int index = (*track)["index"];

    CRGB black = color;
    black.nscale8_video(16);
    return leds[index].lerp16(black, shrink);
}

CRGB FastLedHandler::calcRainbow(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time)
{
    // Speed of hue iteration
    float speed = (kPrev->containsKey("speed") ? (*kPrev)["speed"] : 1.0f);

    // Size of color range between offsets (Hue delta)
    int size = (kPrev->containsKey("size") ? (*kPrev)["size"] : 1);

    // Offset of this LED
    int offset = (kPrev->containsKey("offset") ? (*kPrev)["offset"] : 1);

    int hue = 1;

    // Time
    hue *= (time * speed);

    // Offset
    hue += (size * offset);

    CHSV hsv;
    hsv.hue = hue;
    hsv.val = 255;
    hsv.sat = 240;

    return hsv;
}
