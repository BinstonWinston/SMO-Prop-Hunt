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
#include "actors/PropActor.h"
#include "math/seadQuat.h"
#include "math/seadVector.h"
#include "server/gamemode/GameModeBase.hpp"
#include "random/seadRandom.h"
#include "time/seadTickTime.h"

PropActor* PropActor::props[CaptureTypes::MAX_PROPS_PER_KINGDOM] = {nullptr};

PropActor::PropActor(const char* name) : al::LiveActor(name) {
    kill();
}

void PropActor::initProp(const char* archiveName) {
    mArchiveName = archiveName;
}

void PropActor::init(al::ActorInitInfo const &initInfo) {

    al::initActorWithArchiveName(this, initInfo, mArchiveName, nullptr);

    al::initActorPoseTQSV(this);

    // al::hideSilhouetteModelIfShow(this);

    if (CaptureTypes::currentWorldId != 1) { // This crashes on Cascade for some reason
        if(al::isExistDitherAnimator(this)) {
            // Logger::log("Disabling Dither Animator.\n");
            al::invalidateDitherAnim(this);
        }
    }

    if (al::isExistCollisionParts(this)) {
        // Logger::log(("Disabling Collision.\n"));
        al::invalidateCollisionParts(this);
    }

    al::invalidateHitSensors(this);

    al::invalidateClipping(this);

    al::invalidateOcclusionQuery(this);

    al::offCollide(this);

    makeActorDead();
}

void PropActor::initAfterPlacement() { 
    al::LiveActor::initAfterPlacement(); 
}

void PropActor::movement() {
    al::LiveActor::movement();
}

void PropActor::control() {

    al::LiveActor* curModel = getCurrentModel();

    // Position & Rotation Handling
    sead::Vector3f* pPos = al::getTransPtr(this);
    sead::Quatf *pQuat = al::getQuatPtr(this);

    *pPos = m_info.pos + sead::Vector3f(0, 1, 0);
    *pQuat = m_info.rot;

    // Syncing
    syncPose();
}

void PropActor::makeActorAlive() {
    if (al::isDead(this)) {
        al::LiveActor::makeActorAlive();
    }
}

void PropActor::makeActorDead() {

    al::LiveActor *curModel = getCurrentModel();
    
    if (!al::isDead(curModel)) {
        curModel->makeActorDead();
    }

    if (!al::isDead(this)) {
        al::LiveActor::makeActorDead();
    }
}

al::LiveActor* PropActor::getCurrentModel() {
    return this;
}


void PropActor::syncPose() {
    al::LiveActor* curModel = getCurrentModel();
    curModel->mPoseKeeper->updatePoseQuat(al::getQuat(this)); // update pose using a quaternion instead of setting quaternion rotation
    al::setTrans(curModel, al::getTrans(this));
}


PropActor *PropActor::createFromFactory(al::ActorInitInfo const &rootInitInfo, al::PlacementInfo const &rootPlacementInfo, const char* propArchiveName) {
    al::ActorInitInfo actorInitInfo = al::ActorInitInfo();
    actorInitInfo.initViewIdSelf(&rootPlacementInfo, rootInitInfo);

    al::createActor createActor = actorInitInfo.mActorFactory->getCreator("PropActor");
    
    if(!createActor) {
        return nullptr;
    }

    PropActor *newActor = (PropActor*)createActor("PropActor");

    Logger::log("Creating Prop Actor: %s\n", propArchiveName);

    newActor->initProp(propArchiveName);
    newActor->init(actorInitInfo);

    return reinterpret_cast<PropActor*>(newActor);
}

void PropActor::initAllActors(al::ActorInitInfo const &rootInfo, al::PlacementInfo const &placement) {
    for (int32_t i = 0; i < sizeof(PropActor::props) / sizeof(PropActor::props[0]); i++) {
        PropActor::props[i] = nullptr;
    }

    const auto range = CaptureTypes::getTypesForCurrentWorld();
    for (int32_t i = 0; i < range.size(); i++) {
        const char* propArchiveName = CaptureTypes::FindStr(range.getPropType(i));
        PropActor::props[i] = PropActor::createFromFactory(rootInfo, placement, propArchiveName);
    }
}