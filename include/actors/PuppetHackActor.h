#pragma once

#include "al/LiveActor/LiveActor.h"
#include "al/util.hpp"

#include "logger.hpp"
#include "puppets/PuppetInfo.h"
#include "helpers.hpp"

// TODO: Make this actor only created once per puppet, and use SubActorKeeper to create PartsModel actors for each capture

class PuppetHackActor : public al::LiveActor {
    public:
        static void initAllActors(al::ActorInitInfo const &initInfo, al::PlacementInfo const& placement);

        PuppetHackActor(const char *name);
        virtual void init(al::ActorInitInfo const &) override;
        virtual void initAfterPlacement() override;
        virtual void control(void) override;
        virtual void movement(void) override;
        void initOnline(PuppetInfo *info, const char *hackType);
        
    private:
        PuppetInfo *mInfo;
        sead::FixedSafeString<PROP_ACTOR_NAME_MAX_LENGTH> mHackType;
};
