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

al::LiveActor* FlagActor::singleton = nullptr;

FlagActor::FlagActor(const char* name) : al::LiveActor(name) {
    kill();
}

void FlagActor::initProp(const char* archiveName) {
    mArchiveName = archiveName;
}

void FlagActor::init(al::ActorInitInfo const &initInfo) {
    al::initActorWithArchiveName(this, initInfo, /*"FireHydrant"*/mArchiveName, nullptr);

    auto* normalModel = this;
    // mCostumeInfo = PlayerFunction::initMarioModelCommon(normalModel, initInfo, bodyName, capName, 0, false, nullptr, false, false);

    // normalModel->mActorActionKeeper->mPadAndCamCtrl->mRumbleCount = 0; // set rumble count to zero so that no rumble actions will run

    // al::setClippingInfo(normalModel, 50000.0f, 0);
    // al::setClippingNearDistance(normalModel, 50000.0f);
    // al::hideSilhouetteModelIfShow(normalModel);
    // al::validateClipping(normalModel);
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


al::LiveActor *FlagActor::createFromFactory(al::ActorInitInfo const &rootInitInfo, al::PlacementInfo const &rootPlacementInfo, bool isDebug) {
    al::ActorInitInfo actorInitInfo = al::ActorInitInfo();
    actorInitInfo.initViewIdSelf(&rootPlacementInfo, rootInitInfo);

    al::createActor createActor = actorInitInfo.mActorFactory->getCreator("FlagActor");
    
    if(!createActor) {
        return nullptr;
    }

    FlagActor *newActor = (FlagActor*)createActor("FlagActor");

    Logger::log("Creating Flag Actor.\n");

    newActor->initProp("PropHuntSandWorldHomeLift001");
    newActor->init(actorInitInfo);
    // newActor->makeActorAlive();

    // if (Client::tryAddDebugPuppet(newActor)) {
    //     Logger::log("Debug Puppet Created!\n");
    // }

    return newActor;
}

void FlagActor::initAllActors(al::ActorInitInfo const &rootInfo, al::PlacementInfo const &placement) {
    FlagActor::singleton = FlagActor::createFromFactory(rootInfo, placement, false);
}