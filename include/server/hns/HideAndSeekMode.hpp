#pragma once

#include <math.h>
#include "al/camera/CameraTicket.h"
#include "server/gamemode/GameModeBase.hpp"
#include "server/gamemode/GameModeInfoBase.hpp"
#include "server/gamemode/GameModeConfigMenu.hpp"
#include "server/gamemode/GameModeTimer.hpp"
#include "server/hns/HideAndSeekConfigMenu.hpp"
#include "actors/PropActor.h"
#include "server/hns/OrientedBoundingBox.hpp"

struct HideAndSeekInfo : GameModeInfoBase {
    HideAndSeekInfo() { mMode = GameMode::HIDEANDSEEK; }
    bool mIsPlayerIt = false;
    bool mIsUseGravity = false;
    bool mIsUseGravityCam = false;
    GameTime mHidingTime;
    CaptureTypes::Type mPropType = CaptureTypes::Type::Unknown;
};

struct DecoyPropInfo {
    sead::Vector3f pos;
    sead::Quatf rot;
    sead::FixedSafeString<0x40> stageName;
    u8 scenario;
    CaptureTypes::Type propType;
};

class HideAndSeekMode : public GameModeBase {
    public:
        static const char* getCurrentPropName();
        static CaptureTypes::Type getCurrentPropType();
        static void clearCurrentPropAndBecomeSeeker();
        static void queueUpKillLocalPlayer();

        void setDecoyPropInfo();
        static std::optional<DecoyPropInfo> getDecoyPropInfo_static();
        std::optional<DecoyPropInfo> getDecoyPropInfo();

        static std::optional<OrientedBoundingBox> getPropObb_static();
        std::optional<OrientedBoundingBox> getPropObb();
        bool isCollideWithSeeker(sead::Vector3f const& seekerPos);

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

        // returns empty optional if no cooldown is active, otherwise returns the
        // cooldown percentage remaining
        std::optional<f32> getPropCooldown();

        void killLocalPlayer(PlayerActorHakoniwa* player);

    private:
        bool mLocalPlayerKillQueued = false;
        float mInvulnTime = 0.0f;
        f32 mPropSwitchCooldownTime = 0.0f;
        GameModeTimer* mModeTimer = nullptr;
        HideAndSeekIcon *mModeLayout = nullptr;
        HideAndSeekInfo* mInfo = nullptr;
        al::CameraTicket *mTicket = nullptr;
        std::optional<DecoyPropInfo> mDecoyPropInfo{};

        PropActor* getPropActor();
        bool isPropActive() { return mInfo->mPropType != CaptureTypes::Type::Unknown; }
        void enablePropMode(PlayerActorBase* playerBase, bool isYukimaru);
        void disablePropMode(PlayerActorBase* playerBase, bool isYukimaru);
        void updatePropPosition(PlayerActorHakoniwa* player);

};