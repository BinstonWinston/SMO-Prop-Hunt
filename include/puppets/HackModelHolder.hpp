#pragma once
#include "al/LiveActor/LiveActor.h"
#include "al/util.hpp"

#include "sead/prim/seadSafeString.hpp"

#include "helpers.hpp"
#include "actors/PuppetHackActor.h"

struct CaptureEntry {
    PuppetHackActor *actor;
    char className[PROP_ACTOR_NAME_MAX_LENGTH];
};

class HackModelHolder {
    public:
        HackModelHolder() = default;

        PuppetHackActor *getCapture(const char *hackName);
        PuppetHackActor *getCapture(int index);

        const char *getCaptureClass(int index);
        bool addCapture(PuppetHackActor *capture, const char *hackName);
        bool removeCapture(const char *hackName);

        int getEntryCount() { return mCaptureCount; };

        bool setCurrent(const char* hackName);

        PuppetHackActor *getCurrentActor();
        const char *getCurrentActorName();

        void resetList();
    private:
        int mCaptureCount;
        CaptureEntry *mCurCapture;
        CaptureEntry mOnlineCaptures[128];
};