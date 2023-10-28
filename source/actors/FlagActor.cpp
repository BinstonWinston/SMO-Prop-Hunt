#include "main.hpp"
#include "server/Client.hpp"
#include "al/LiveActor/LiveActor.h"
#include "al/layout/BalloonMessage.h"
#include "al/layout/LayoutInitInfo.h"
#include "al/string/StringTmp.h"
#include "al/util.hpp"
#include "al/util/LiveActorUtil.h"
#include "algorithms/CaptureTypes.h"
#include "logger.hpp"
#include "actors/FlagActor.h"
#include "math/seadQuat.h"
#include "math/seadVector.h"
#include "server/gamemode/GameModeBase.hpp"
#include "random/seadRandom.h"
#include "time/seadTickTime.h"
#include "server/hns/HideAndSeekMode.hpp"
#include "algorithms/CaptureTypes.h"

FlagActor* FlagActor::singleton = nullptr;

FlagActor::FlagActor(const char* name) : al::LiveActor(name) {
    kill();
}

void FlagActor::initProp(const char* archiveName) {
    mArchiveName = archiveName;
}

void FlagActor::init(al::ActorInitInfo const &initInfo) {

    auto* normalModel = this;
    al::initActorWithArchiveName(this, initInfo, /*"FireHydrant"*/mArchiveName, nullptr);

    al::initActorPoseTQSV(this);

    al::hideSilhouetteModelIfShow(this);

    if(al::isExistDitherAnimator(this)) {
        // Logger::log("Disabling Dither Animator.\n");
        al::invalidateDitherAnim(this);
    }

    if (al::isExistCollisionParts(this)) {
        // Logger::log(("Disabling Collision.\n"));
        al::invalidateCollisionParts(this);
    }

    al::invalidateHitSensors(this);

    al::invalidateClipping(this);

    al::offCollide(this);

    makeActorDead();
}

void FlagActor::initAfterPlacement() { 
    al::LiveActor::initAfterPlacement(); 
}

void FlagActor::movement() {
    al::LiveActor::movement();
}

void FlagActor::control() {

    al::LiveActor* curModel = getCurrentModel();

    // Position & Rotation Handling
    sead::Vector3f* pPos = al::getTransPtr(this);
    sead::Quatf *pQuat = al::getQuatPtr(this);

    *pPos = m_info.pos;
    *pQuat = m_info.rot;

    // Syncing
    syncPose();
}

void FlagActor::makeActorAlive() {
    if (al::isDead(this)) {
        al::LiveActor::makeActorAlive();
    }
}

void FlagActor::makeActorDead() {

    al::LiveActor *curModel = getCurrentModel();
    
    if (!al::isDead(curModel)) {
        curModel->makeActorDead();
    }

    if (!al::isDead(this)) {
        al::LiveActor::makeActorDead();
    }
}

al::LiveActor* FlagActor::getCurrentModel() {
    return this;
}


void FlagActor::syncPose() {
    al::LiveActor* curModel = getCurrentModel();
    curModel->mPoseKeeper->updatePoseQuat(al::getQuat(this)); // update pose using a quaternion instead of setting quaternion rotation
    al::setTrans(curModel, al::getTrans(this));
}


FlagActor *FlagActor::createFromFactory(al::ActorInitInfo const &rootInitInfo, al::PlacementInfo const &rootPlacementInfo, const char* propArchiveName) {
    al::ActorInitInfo actorInitInfo = al::ActorInitInfo();
    actorInitInfo.initViewIdSelf(&rootPlacementInfo, rootInitInfo);

    al::createActor createActor = actorInitInfo.mActorFactory->getCreator("FlagActor");
    
    if(!createActor) {
        return nullptr;
    }

    FlagActor *newActor = (FlagActor*)createActor("FlagActor");

    Logger::log("Creating Prop Actor: %s\n", propArchiveName);

    newActor->initProp(propArchiveName);
    newActor->init(actorInitInfo);

    return reinterpret_cast<FlagActor*>(newActor);
}

void FlagActor::initAllActors(al::ActorInitInfo const &rootInfo, al::PlacementInfo const &placement) {
    sead::Random random(static_cast<u32>(sead::TickTime().toTicks()));

    const auto propType = static_cast<CaptureTypes::Type>(random.getU32(static_cast<u32>(CaptureTypes::Type::End)));

    FlagActor::singleton = FlagActor::createFromFactory(rootInfo, placement, CaptureTypes::FindStr(propType));
}

const char* FlagActor::getCurrentPropName() {
    if (!GameModeManager::instance()->isMode(GameMode::HIDEANDSEEK)) {
        return nullptr;
    }

    if (!FlagActor::singleton) {
        return nullptr;
    }

    HideAndSeekMode* hsMode = GameModeManager::instance()->getMode<HideAndSeekMode>();
    if (hsMode->isPlayerIt()) {
        // Seekers have no props
        return nullptr;
    }

    return FlagActor::singleton->mArchiveName;
}