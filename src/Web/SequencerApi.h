#pragma once

#include "Sequencer/SequenceStore.h"
#include "Sequencer/SequencePlayer.h"
#include "ESPAsyncWebServer.h"
#include "DFRobotDFPlayerMini.h"
#include <FastLED.h>

/**
 * Manage Sequences through web API
 */
class SequencerApi
{

public:
    static void setup(
        AsyncWebServer*,
        SequenceStore*,
        SequencePlayer*
    );
};
