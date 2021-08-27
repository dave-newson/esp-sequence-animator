#include "FastLedHandler.h"

FastLedHandler::FastLedHandler(CFastLED* _driver)
{
    driver = _driver;
}

void FastLedHandler::reset()
{
    driver->clear();
    driver->show();
}

void FastLedHandler::tick(float time, JsonObject* kPrev, JsonObject* kNext)
{
    // TODO
    return;

    int index = (*kPrev)["index"];
    String color = (*kPrev)["color"];

    // Str to CRGB
    color = color.substring(1);
    leds[index] = strtol(color.c_str(), NULL, 16);

    // Update LEDs
    FastLED.show();
}
