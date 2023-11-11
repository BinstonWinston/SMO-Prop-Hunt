#include "server/hns/HideAndSeekMode.hpp"
#include <cmath>
#include "al/async/FunctorV0M.hpp"
#include "al/util.hpp"
#include "al/util/ControllerUtil.h"
#include "game/GameData/GameDataHolderAccessor.h"
#include "game/Layouts/CoinCounter.h"
#include "game/Layouts/MapMini.h"
#include "game/Player/PlayerActorBase.h"
#include "game/Player/PlayerActorHakoniwa.h"
#include "heap/seadHeapMgr.h"
#include "layouts/HideAndSeekIcon.h"
#include "logger.hpp"
#include "rs/util.hpp"
#include "server/gamemode/GameModeBase.hpp"
#include "server/Client.hpp"
#include "server/gamemode/GameModeTimer.hpp"
#include <heap/seadHeap.h>
#include "server/gamemode/GameModeManager.hpp"
#include "server/gamemode/GameModeFactory.hpp"

#include "basis/seadNew.h"
#include "server/hns/HideAndSeekConfigMenu.hpp"
#include "actors/PropActor.h"

#include "al/collision/CollisionDirector.h"
#include "al/collision/alCollisionUtil.h"
#include "al/collision/Triangle.h"
#include "al/model/ModelFunction.h"
#include "al/util/VectorUtil.h"

static constexpr f32 PROP_SWITCH_COOLDOWN_TIME_THRESHOLD_SECONDS = 5;

HideAndSeekMode::HideAndSeekMode(const char* name) : GameModeBase(name) {}

void HideAndSeekMode::init(const GameModeInitInfo& info) {
    mSceneObjHolder = info.mSceneObjHolder;
    mMode = info.mMode;
    mCurScene = (StageScene*)info.mScene;
    mPuppetHolder = info.mPuppetHolder;

    GameModeInfoBase* curGameInfo = GameModeManager::instance()->getInfo<HideAndSeekInfo>();

    if (curGameInfo) Logger::log("Gamemode info found: %s %s\n", GameModeFactory::getModeString(curGameInfo->mMode), GameModeFactory::getModeString(info.mMode));
    else Logger::log("No gamemode info found\n");
    if (curGameInfo && curGameInfo->mMode == mMode) {
        mInfo = (HideAndSeekInfo*)curGameInfo;
        mModeTimer = new GameModeTimer(mInfo->mHidingTime);
        Logger::log("Reinitialized timer with time %d:%.2d\n", mInfo->mHidingTime.mMinutes, mInfo->mHidingTime.mSeconds);
    } else {
        if (curGameInfo) delete curGameInfo;  // attempt to destory previous info before creating new one
        
        mInfo = GameModeManager::instance()->createModeInfo<HideAndSeekInfo>();
        
        mModeTimer = new GameModeTimer();
    }

    mModeLayout = new HideAndSeekIcon("HideAndSeekIcon", *info.mLayoutInitInfo);

    mModeLayout->showSeeking();

    mModeTimer->disableTimer();

}

void HideAndSeekMode::begin() {
    mModeLayout->appear();

    mIsFirstFrame = true;

    if (!mInfo->mIsPlayerIt) {
        mModeTimer->enableTimer();
        mModeLayout->showHiding();
    } else {
        mModeTimer->disableTimer();
        mModeLayout->showSeeking();
    }

    CoinCounter *coinCollect = mCurScene->mSceneLayout->mCoinCollectLyt;
    CoinCounter* coinCounter = mCurScene->mSceneLayout->mCoinCountLyt;
    MapMini* compass = mCurScene->mSceneLayout->mMapMiniLyt;
    al::SimpleLayoutAppearWaitEnd* playGuideLyt = mCurScene->mSceneLayout->mPlayGuideMenuLyt;

    mInvulnTime = 0;
    mPropSwitchCooldownTime = 0;

    if(coinCounter->mIsAlive)
        coinCounter->tryEnd();
    if(coinCollect->mIsAlive)
        coinCollect->tryEnd();
    if (compass->mIsAlive)
        compass->end();
    if (playGuideLyt->mIsAlive)
        playGuideLyt->end();

    GameModeBase::begin();

    Client::sendTagInfPacket();
}

void HideAndSeekMode::end() {

    mModeLayout->tryEnd();

    mModeTimer->disableTimer();

    CoinCounter *coinCollect = mCurScene->mSceneLayout->mCoinCollectLyt;
    CoinCounter* coinCounter = mCurScene->mSceneLayout->mCoinCountLyt;
    MapMini* compass = mCurScene->mSceneLayout->mMapMiniLyt;
    al::SimpleLayoutAppearWaitEnd* playGuideLyt = mCurScene->mSceneLayout->mPlayGuideMenuLyt;

    mInvulnTime = 0.0f;
    mPropSwitchCooldownTime = 0;

    if(!coinCounter->mIsAlive)
        coinCounter->tryStart();
    if(!coinCollect->mIsAlive)
        coinCollect->tryStart();
    if (!compass->mIsAlive)
        compass->appearSlideIn();
    if (!playGuideLyt->mIsAlive)
        playGuideLyt->appear();

    GameModeBase::end();

    Client::sendTagInfPacket();
}

std::optional<OrientedBoundingBox> HideAndSeekMode::getPropObb_static() {
    if (!GameModeManager::instance()->isMode(GameMode::HIDEANDSEEK)) {
        return {};
    }

    HideAndSeekMode* hsMode = GameModeManager::instance()->getMode<HideAndSeekMode>();

    if (!hsMode) {
        return {};
    }

    return hsMode->getPropObb();
}

std::optional<OrientedBoundingBox> HideAndSeekMode::getPropObb() {
    auto propActor = getPropActor();
    if (!propActor || !propActor->mModelKeeper || !propActor->mModelKeeper->mModelCtrl) {
        return {};
    }

    sead::BoundBox3<float> bbox;
    alModelFunction::calcBoundingBox(&bbox, propActor->mModelKeeper->mModelCtrl);
    return OrientedBoundingBox(bbox, al::getTrans(propActor), al::getQuat(propActor));
}

bool isCollideWithSeeker(al::LiveActor* actor, al::LiveActor* seeker) {
    // if (getPropActor()->getCollisionDirector()->checkStrikeSphere(al::getTrans(getPropActor()), MARIO_RADIUS*10000.f, true, curInfo->playerPos)) {

    sead::BoundBox3<float> bbox;
    alModelFunction::calcBoundingBox(&bbox, actor->mModelKeeper->mModelCtrl);

    auto seekerPos = al::getTrans(seeker);
    seekerPos -= al::getTrans(actor);
    auto rot = al::getQuat(actor);
    sead::Quatf rotI;
    rot.inverse(&rotI);
    al::rotateVectorQuat(&seekerPos, rotI);

    constexpr f32 SEEKER_RADIUS = 200.f;
    return (bbox.getMin().x < seekerPos.x + SEEKER_RADIUS &&
        bbox.getMin().y < seekerPos.y + SEEKER_RADIUS &&
        bbox.getMin().z < seekerPos.z + SEEKER_RADIUS &&
        seekerPos.x - SEEKER_RADIUS < bbox.getMax().x &&
        seekerPos.y - SEEKER_RADIUS < bbox.getMax().y &&
        seekerPos.z - SEEKER_RADIUS < bbox.getMax().z);

    // al::Triangle downTri; // Triangle information for below player
    // al::Triangle frontTri; // Triangle informaton for in front of player
    
    // // Raycast origin
    // sead::Vector3f rayOrg = al::getTrans(actor);
    // rayOrg.y += 30.f;

    // // Directional rays
    // sead::Vector3f downRay = { 0.f, -3000.f, 0.f };

    // sead::Vector3f frontRay;
    // al::calcFrontDir(&frontRay, actor);
    // frontRay *= 3000.f;

    // // Find triangles
    // if(alCollisionUtil::getFirstPolyOnArrow(actor, nullptr, &downTri, rayOrg, downRay, nullptr, nullptr) &&
    //     downTri.mCollisionParts) {
    //     return true;
    // }
    // if(alCollisionUtil::getFirstPolyOnArrow(actor, nullptr, &frontTri, rayOrg, frontRay, nullptr, nullptr)) {
    //     return true;
    // }
}

void HideAndSeekMode::update() {

    PlayerActorBase* playerBase = rs::getPlayerActor(mCurScene);

    bool isYukimaru = !playerBase->getPlayerInfo(); // if PlayerInfo is a nullptr, that means we're dealing with the bound bowl racer

    if (mIsFirstFrame) {

        if (mInfo->mIsUseGravityCam && mTicket) {
            al::startCamera(mCurScene, mTicket, -1);
        }

        mIsFirstFrame = false;
    }

    if (!mInfo->mIsPlayerIt) {
        if (mInvulnTime >= 5) {  

            if (playerBase) {
                for (size_t i = 0; i < mPuppetHolder->getSize(); i++)
                {
                    PuppetInfo *curInfo = Client::getPuppetInfo(i);

                    if (!curInfo) {
                        Logger::log("Checking %d, hit bounds %d-%d\n", i, mPuppetHolder->getSize(), Client::getMaxPlayerCount());
                        break;
                    }

                    if(curInfo->isConnected && curInfo->isInSameStage && curInfo->isIt) { 

                        float pupDist = al::calcDistance(playerBase, curInfo->playerPos); // TODO: remove distance calculations and use hit sensors to determine this

                        if (!isYukimaru) {
                            constexpr f32 MARIO_RADIUS = 200.f;
                            if (isCollideWithSeeker(getPropActor(), Client::getPuppet(i))) {
                                killLocalPlayer((PlayerActorHakoniwa*)playerBase);
                            } else if(pupDist < MARIO_RADIUS && ((PlayerActorHakoniwa*)playerBase)->mDimKeeper->is2DModel == curInfo->is2D) {
                                // killLocalPlayer((PlayerActorHakoniwa*)playerBase);
                            } else if (PlayerFunction::isPlayerDeadStatus(playerBase)) {

                                mInfo->mIsPlayerIt = true;
                                mModeTimer->disableTimer();
                                mModeLayout->showSeeking();

                                Client::sendTagInfPacket();
                                
                            }
                        }
                    }
                }
            }
            
        }else {
            mInvulnTime += Time::deltaTime;
        }

        mModeTimer->updateTimer();
    }

    if (!isYukimaru && mLocalPlayerKillQueued) {
        mLocalPlayerKillQueued = false;
        killLocalPlayer(reinterpret_cast<PlayerActorHakoniwa*>(playerBase));
    }

    if (mInfo->mIsUseGravity && !isYukimaru) {
        sead::Vector3f gravity;
        if (rs::calcOnGroundNormalOrGravityDir(&gravity, playerBase, playerBase->getPlayerCollision())) {
            gravity = -gravity;
            al::normalize(&gravity);
            al::setGravity(playerBase, gravity);
            al::setGravity(((PlayerActorHakoniwa*)playerBase)->mHackCap, gravity);
        }
        
        if (al::isPadHoldL(-1)) {
            if (al::isPadTriggerRight(-1)) {
                if (al::isActiveCamera(mTicket)) {
                    al::endCamera(mCurScene, mTicket, -1, false);
                    mInfo->mIsUseGravityCam = false;
                } else {
                    al::startCamera(mCurScene, mTicket, -1);
                    mInfo->mIsUseGravityCam = true;
                }
            }
        } else if (al::isPadTriggerZL(-1)) {
            if (al::isPadTriggerLeft(-1)) {
                killMainPlayer(((PlayerActorHakoniwa*)playerBase));
            }
        }
    }

    if (al::isPadTriggerUp(-1) && !al::isPadHoldZL(-1))
    {
        mInfo->mIsPlayerIt = !mInfo->mIsPlayerIt;

        mModeTimer->toggleTimer();

        if(!mInfo->mIsPlayerIt) {
            mInvulnTime = 0;
            mModeLayout->showHiding();
            enablePropMode(playerBase, isYukimaru);
        } else {
            mModeLayout->showSeeking();
            disablePropMode(playerBase, isYukimaru);
        }

        Client::sendTagInfPacket();
    }

    mPropSwitchCooldownTime += Time::deltaTime;

    if (al::isPadHoldR(-1) && al::isPadTriggerPressLeftStick(-1) && !mInfo->mIsPlayerIt && mInfo->mPropType != CaptureTypes::Type::Unknown && !getPropCooldown().has_value()) {
        disablePropMode(playerBase, isYukimaru); // Hide current prop
        auto propId = static_cast<s32>(mInfo->mPropType) - static_cast<s32>(CaptureTypes::getTypesForCurrentWorld().start);
        propId = std::max(0, propId);
        propId++; // Go to next prop
        propId %= CaptureTypes::getTypesForCurrentWorld().size();
        propId += static_cast<s32>(CaptureTypes::getTypesForCurrentWorld().start);
        mInfo->mPropType = static_cast<CaptureTypes::Type>(propId);
        enablePropMode(playerBase, isYukimaru);
        // Standard capture sync assumes you can only switch
        // capture types by first going uncaptured (which is 100% true for captures)
        // For props, we can switch without going uncaptured first
        // so we need to clear the flag that checks if the packet has been sent
        // so a new updated packet will be sent
        Client::resetIsSentCaptureInf();
        mPropSwitchCooldownTime = 0;
    }

    // Sync prop state with hiding/seeking state
    if (mInfo->mIsPlayerIt && isPropActive()) {
        disablePropMode(playerBase, isYukimaru);
    }
    else if (!mInfo->mIsPlayerIt && !isPropActive()) {
        enablePropMode(playerBase, isYukimaru);
    }

    if (isPropActive() && !isYukimaru) {
        updatePropPosition(reinterpret_cast<PlayerActorHakoniwa*>(playerBase));
    }

    mInfo->mHidingTime = mModeTimer->getTime();
}

void HideAndSeekMode::killLocalPlayer(PlayerActorHakoniwa* player) {
    if(!PlayerFunction::isPlayerDeadStatus(player)) {
                                    
    GameDataFunction::killPlayer(GameDataHolderAccessor(this));
    player->startDemoPuppetable();
    al::setVelocityZero(player);
    rs::faceToCamera(player);
    (player)->mPlayerAnimator->endSubAnim();
    (player)->mPlayerAnimator->startAnimDead();

    mInfo->mIsPlayerIt = true;
    mModeTimer->disableTimer();
    mModeLayout->showSeeking();
    
    Client::sendTagInfPacket();
    }
}

const char* HideAndSeekMode::getCurrentPropName() {
    if (!GameModeManager::instance()->isMode(GameMode::HIDEANDSEEK)) {
        return nullptr;
    }

    HideAndSeekMode* hsMode = GameModeManager::instance()->getMode<HideAndSeekMode>();
    if (!hsMode) {
        return nullptr;
    }

    auto propActor = hsMode->getPropActor();
    if (!propActor) {
        return nullptr;
    }
    return propActor->getPropArchiveName();
}

CaptureTypes::Type HideAndSeekMode::getCurrentPropType() {
    if (!GameModeManager::instance()->isMode(GameMode::HIDEANDSEEK)) {
        return CaptureTypes::Type::Unknown;
    }

    HideAndSeekMode* hsMode = GameModeManager::instance()->getMode<HideAndSeekMode>();
    if (!hsMode || !hsMode->mInfo) {
        return CaptureTypes::Type::Unknown;
    }
    return hsMode->mInfo->mPropType;
}

PropActor* HideAndSeekMode::getPropActor() {
    if (mInfo->mPropType == CaptureTypes::Type::Unknown) {
        return nullptr;
    }
    return PropActor::props[static_cast<u32>(mInfo->mPropType) - static_cast<u32>(CaptureTypes::getTypesForCurrentWorld().start)];
}

void HideAndSeekMode::enablePropMode(PlayerActorBase* playerBase, bool isYukimaru) {
    sead::Random random(static_cast<u32>(sead::TickTime().toTicks()));

    mInfo->mPropType = static_cast<CaptureTypes::Type>(random.getU32(CaptureTypes::getTypesForCurrentWorld().size()) + CaptureTypes::ToValue(CaptureTypes::getTypesForCurrentWorld().start));
    if (!isYukimaru) {
        reinterpret_cast<PlayerActorHakoniwa*>(playerBase)->mModelChanger->hideModel();
    }
    auto propActor = getPropActor();
    if (propActor) { // Show prop
        propActor->makeActorAlive();
        al::showModel(propActor);
    }
}

void HideAndSeekMode::disablePropMode(PlayerActorBase* playerBase, bool isYukimaru) {
    auto propActor = getPropActor();
    mInfo->mPropType = CaptureTypes::Type::Unknown;
    if (!isYukimaru) {
        reinterpret_cast<PlayerActorHakoniwa*>(playerBase)->mModelChanger->showModel();
    }
    if (propActor) { // Hide prop
        al::hideModel(propActor);
    }
}

void HideAndSeekMode::updatePropPosition(PlayerActorHakoniwa* player) {
    auto propActor = getPropActor();
    if (propActor == nullptr || player == nullptr) {
        return;
    }

    auto const& p = al::getTrans(player);
    auto const& r = al::getQuat(player);
    if (al::isDead(propActor)) {
        propActor->makeActorAlive();
    }
    propActor->setXform(p, r);

    // Set this every frame in case player goes in a capture or dies that would cause the player to re-appear
    player->mModelChanger->hideModel();
}

void HideAndSeekMode::clearCurrentPropAndBecomeSeeker() {
    if (!GameModeManager::instance()->isMode(GameMode::HIDEANDSEEK)) {
        return;
    }

    HideAndSeekMode* hsMode = GameModeManager::instance()->getMode<HideAndSeekMode>();
    if (!hsMode || !hsMode->mInfo) {
        return;
    }

    hsMode->mInfo->mIsPlayerIt = true;
    hsMode->mInfo->mPropType = CaptureTypes::Type::Unknown;
}

std::optional<f32> HideAndSeekMode::getPropCooldown() {
    if (mInfo->mPropType == CaptureTypes::Type::Unknown) {
        return {};
    }
    if (mPropSwitchCooldownTime >= PROP_SWITCH_COOLDOWN_TIME_THRESHOLD_SECONDS) {
        return {};
    }
    return mPropSwitchCooldownTime / PROP_SWITCH_COOLDOWN_TIME_THRESHOLD_SECONDS;
}

void HideAndSeekMode::queueUpKillLocalPlayer() {
    if (!GameModeManager::instance()->isMode(GameMode::HIDEANDSEEK)) {
        return;
    }

    HideAndSeekMode* hsMode = GameModeManager::instance()->getMode<HideAndSeekMode>();
    if (!hsMode) {
        return;
    }
    hsMode->mLocalPlayerKillQueued = true;
}