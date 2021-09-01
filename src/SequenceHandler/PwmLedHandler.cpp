#include "PwmLedHandler.h"
#include "Arduino.h"
#include "ArduinoJson.h"

#define LED_RANGE 256

PwmLedHandler::PwmLedHandler(int _pin)
{
    pin = _pin;
    pinMode(pin, OUTPUT);
}

void PwmLedHandler::reset()
{
    pinMode(pin, OUTPUT);
    analogWrite(pin, 0);
}

void PwmLedHandler::tick(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time)
{
    // Timing
    float start = (*kPrev)["time"];
    float end = (*kNext)["time"];

    float pos = 0.0f;
    if (time > start && start < end) {
        float length = (end - start);
        float now = time - start;
        pos = now / length;
    }

    // Values
    float valueIn = (*kPrev)["value"];
    float valueOut = (*kNext)["value"];

    // Value
    float value = lerp(valueIn, valueOut, pos);

    int pinValue = (value * LED_RANGE);
    pinValue = min(LED_RANGE, max(0, pinValue));

    // Apply
    analogWrite(pin, pinValue);
}

float PwmLedHandler::lerp(float a, float b, float x)
{
    if (x > 1.0f) return b;
    if (x < 0.0f) return a;
    return a + x * (b - a);
}