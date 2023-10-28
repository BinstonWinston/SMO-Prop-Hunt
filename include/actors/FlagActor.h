#pragma once

#include <optional>

#include "al/LiveActor/LiveActor.h"
#include "al/async/FunctorV0M.hpp"
#include "al/async/FunctorBase.h"
#include "al/util.hpp"
#include "al/string/StringTmp.h"
#include "al/layout/BalloonMessage.h"

#include "game/Player/PlayerFunction.h"
#include "game/Player/PlayerJointControlPartsDynamics.h"
#include "game/Player/PlayerConst.h"
#include "game/Player/PlayerModelHolder.h"

#include "actors/PuppetCapActor.h"
#include "actors/PuppetHackActor.h"
#include "layouts/NameTag.h"
#include "sead/math/seadVector.h"
#include "server/DeltaTime.hpp"

#include "logger.hpp"
#include "puppets/PuppetInfo.h"
#include "puppets/HackModelHolder.hpp"
#include "helpers.hpp"
#include "algorithms/CaptureTypes.h"

struct FlagInfo {
    // Flag transform
    sead::Vector3f pos = sead::Vector3f(0.f,0.f,0.f);
    sead::Quatf rot = sead::Quatf(0.f,0.f,0.f,0.f);
};

class FlagActor : public al::LiveActor {
    public:
        static al::LiveActor* singleton;
        static al::LiveActor *createFromFactory(al::ActorInitInfo const &rootInitInfo, al::PlacementInfo const &rootPlacementInfo, const char* propArchiveName);
        static void initAllActors(al::ActorInitInfo const &rootInfo, al::PlacementInfo const &placement);

        FlagActor(const char* name);
        virtual void init(al::ActorInitInfo const &) override;
        virtual void initAfterPlacement(void) override;
        virtual void control(void) override;
        virtual void movement(void) override;
        virtual void makeActorAlive(void) override;
        virtual void makeActorDead(void) override;

        void initProp(const char* archiveName);

        void setXform(sead::Vector3f const& pos, sead::Quatf const& rot) {
            m_info.pos = pos;
            m_info.rot = rot;
        }

        al::LiveActor* getCurrentModel();

        float m_closingSpeed = 0;
    private:
        void changeModel(const char* newModel);
        void syncPose();
        
        FlagInfo m_info{};
        const char* mArchiveName = nullptr;
};
