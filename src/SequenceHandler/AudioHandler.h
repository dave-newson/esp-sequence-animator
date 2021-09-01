#pragma once

#include "Sequencer/SequencePlayer.h"
#include "DFRobotDFPlayerMini.h"

class AudioHandler : public TrackHandler
{
    public:
        AudioHandler(DFRobotDFPlayerMini* driver);
        virtual void reset();
        virtual void tick(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time);
    private:
        DFRobotDFPlayerMini* driver;
        JsonObject* kPlayed;
};
