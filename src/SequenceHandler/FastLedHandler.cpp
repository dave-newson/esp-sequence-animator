#include "FastLedHandler.h"

FastLedHandler::FastLedHandler(CRGB* _leds, int _ledCount, CFastLED* _driver)
{
    driver = _driver;
    leds = _leds;
    ledCount = _ledCount;
    lastChange = 0.0f;
}

void FastLedHandler::reset()
{
    driver->clear();
    driver->show();
    lastChange = 0.0f;
}

void FastLedHandler::tick(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time)
{
    // LED by singular index
    if (!(*track).containsKey("indexes")) {
        return;
    }

    if ((*kPrev)["effect"] == "come-and-go") {
        calcComeAndGo(track, kPrev, time);
        return;
    }

    for (int index : (*track)["indexes"].as<JsonArray>()) {
        CRGB color;

        if ((*kPrev)["effect"] == "step") {
            color = calcStep(track, kPrev, kNext, time);
        } else if ((*kPrev)["effect"] == "flicker") {
            color = calcFlicker(track, kPrev, kNext, time);        
        } else if ((*kPrev)["effect"] == "flame") {
            color = calcFlame(track, kPrev, index, time);        
        } else if ((*kPrev)["effect"] == "rainbow") {
            color = calcRainbow(track, kPrev, kNext, time);        
        } else {
            color = calcLinear(track, kPrev, kNext, time);
        }

        // Update index in colors array
        leds[index] = color;
    }
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

CRGB FastLedHandler::calcFlame(JsonObject* track, JsonObject* kPrev, int index, float time)
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

    // Shrink to black over current index
    CRGB black = color;
    black.nscale8_video(16);
    return leds[index].lerp16(black, shrink);
}

CRGB FastLedHandler::calcRainbow(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time)
{
    // Speed of hue iteration
    float speed = (kPrev->containsKey("speed") ? (*kPrev)["speed"] : 1.0f);

    // Size of color range between offsets (Hue delta)
    uint8 size = (kPrev->containsKey("size") ? (*kPrev)["size"] : 1);

    // Offset of this LED
    uint8 offset = (kPrev->containsKey("offset") ? (*kPrev)["offset"] : 1);

    uint8 hue = 1;

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

void FastLedHandler::calcComeAndGo(JsonObject* track, JsonObject* kPrev, float time)
{
    if (!(*track).containsKey("indexes")) return;

    // Guard: Wait for minimum delay
    float delay = (float) ((*kPrev).containsKey("delayMin") ? (*kPrev)["delayMin"].as<float>() : 1.0f);
    if (time < (lastChange + delay)) return;

    // Pick a random LED to change state
    JsonArray indexes = (*track)["indexes"].as<JsonArray>();
    int index = indexes[random(0, indexes.size())];

    // Check if minLeds are active
    int minActive = ((*kPrev).containsKey("minActive") ? (*kPrev)["minActive"].as<int>() : 1);
    int active = 0;
    for (int index : indexes) {
        active += (leds[index] != CRGB(0));
    }
    bool canDeactivate = (active > minActive);

    // Color
    String colorStr = (*kPrev).containsKey("color") ? (*kPrev)["color"].as<String>() : "#ffffff";
    CRGB color = strtol(colorStr.substring(1).c_str(), NULL, 16);
    CRGB current = leds[index];

    // When deactivate is not allowed, always turn this LED on.
    if (!canDeactivate) {
        leds[index] = color;
    }

    // Guard: Chance that the light will be left on.
    uint16_t chance = 65535.0f * ((*kPrev).containsKey("delayChance") ? (*kPrev)["delayChance"].as<float>() : 1.0f);
    if (chance > random16()) {
        lastChange = time;
        return;
    }

    // Switch to BLACK if current color is NOT BLACK
    if (current != CRGB(0)) {
        color = CRGB::Black;
    }

    leds[index] = color;
    lastChange = time;
}
