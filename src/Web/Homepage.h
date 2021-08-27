#pragma once
#include "Sequencer/Sequencer.h"
#include "ESPAsyncWebServer.h"

/**
 * Primary routine for testing the attached physical hardware.
 * Intended for development testing purposes only, unless you like flashing lights.
 */
class WebHompage
{

public:
    static void setup(AsyncWebServer*);
};
