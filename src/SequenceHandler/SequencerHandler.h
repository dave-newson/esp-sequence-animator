#pragma once

#include "Sequencer/SequencePlayer.h"
#include "Sequencer/SequenceStore.h"

class SequencerHandler : public TrackHandler
{
    public:
        SequencerHandler(SequencePlayer* _player, SequenceStore* _store);
        virtual void reset();
        virtual void tick(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time);
    private:
        SequencePlayer* player;
        SequenceStore* store;
        JsonObject* kPlayed;
};
