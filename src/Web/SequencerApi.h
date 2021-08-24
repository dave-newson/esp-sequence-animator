#pragma once

#include "Sequencer/Sequencer.h"
#include "ESPAsyncWebServer.h"
#include "DFRobotDFPlayerMini.h"
#include <FastLED.h>

/**
 * Manage Sequences through web API
 */
class SequencerApi
{

public:
    static void setup(AsyncWebServer* server, Sequencer* sequencer, DFRobotDFPlayerMini* audioPlayer, CRGB leds[]);
};
