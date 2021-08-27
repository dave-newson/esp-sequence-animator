#pragma once

#include "Sequencer/SequencePlayer.h"
#include "DFRobotDFPlayerMini.h"

class AudioHandler : public TrackHandler
{
    public:
        AudioHandler(DFRobotDFPlayerMini* driver);
        virtual void reset();
        virtual void tick(float time, JsonObject* kPrev, JsonObject* kNext);
    private:
        DFRobotDFPlayerMini* driver;
};
