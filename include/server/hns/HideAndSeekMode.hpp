#pragma once

#include <math.h>
#include "al/camera/CameraTicket.h"
#include "server/gamemode/GameModeBase.hpp"
#include "server/gamemode/GameModeInfoBase.hpp"
#include "server/gamemode/GameModeConfigMenu.hpp"
#include "server/gamemode/GameModeTimer.hpp"
#include "server/hns/HideAndSeekConfigMenu.hpp"
#include "actors/PropActor.h"

struct HideAndSeekInfo : GameModeInfoBase {
    HideAndSeekInfo() { mMode = GameMode::HIDEANDSEEK; }
    bool mIsPlayerIt = false;
    bool mIsUseGravity = false;
    bool mIsUseGravityCam = false;
    GameTime mHidingTime;
    CaptureTypes::Type mPropType = CaptureTypes::Type::Unknown;
};

class HideAndSeekMode : public GameModeBase {
    public:
        static const char* getCurrentPropName();
        static void clearCurrentPropAndBecomeSeeker();

        HideAndSeekMode(const char* name);

        void init(GameModeInitInfo const& info) override;

        virtual void begin() override;
        virtual void update() override;
        virtual void end() override;

        bool isPlayerIt() const { return mInfo->mIsPlayerIt; };

        void setPlayerTagState(bool state) { mInfo->mIsPlayerIt = state; }

        void enableGravityMode() {mInfo->mIsUseGravity = true;}
        void disableGravityMode() { mInfo->mIsUseGravity = false; }
        bool isUseGravity() const { return mInfo->mIsUseGravity; }

        void setCameraTicket(al::CameraTicket *ticket) {mTicket = ticket;}

    private:
        float mInvulnTime = 0.0f;
        GameModeTimer* mModeTimer = nullptr;
        HideAndSeekIcon *mModeLayout = nullptr;
        HideAndSeekInfo* mInfo = nullptr;
        al::CameraTicket *mTicket = nullptr;

        PropActor* getPropActor();
        bool isPropActive() { return mInfo->mPropType != CaptureTypes::Type::Unknown; }
        void enablePropMode(PlayerActorBase* playerBase, bool isYukimaru);
        void disablePropMode(PlayerActorBase* playerBase, bool isYukimaru);
        void updatePropPosition(PlayerActorHakoniwa* player);

};