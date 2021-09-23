#include "SequencerHandler.h"

SequencerHandler::SequencerHandler(SequencePlayer* _player, SequenceStore* _store)
{
    player = _player;
    store = _store;
}

void SequencerHandler::reset()
{
    kPlayed = nullptr;
}

void SequencerHandler::tick(JsonObject* track, JsonObject* kPrev, JsonObject* kNext, float time)
{
    // Guard: Dont play twice
    if (kPlayed == kPrev) return;

    // Guard: Wait for time
    if (time < (*kPrev)["time"]) return;

    // Prevent play twice by recording the played keyframe
    kPlayed = kPrev;

    // Switch sequence
    if (kPrev->containsKey("changeSequence")) {
        int seq = (*kPrev)["changeSequence"];
        JsonLoadResponse response = store->loadJson(seq);
        
        // Guard: Abort if sequence not loadable
        if (response.error != JsonLoadStatus::SUCCESS) {
            return;
        }

        player->stop();
        player->load(response.document);
        player->play();
    }
}
