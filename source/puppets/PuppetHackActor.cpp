#include "actors/PuppetHackActor.h"
#include "al/util.hpp"
#include "logger.hpp"
#include "actors/PuppetActor.h"
#include "algorithms/CaptureTypes.h"
#include "server/Client.hpp"
#include "al/factory/ActorFactory.h"

PuppetHackActor::PuppetHackActor(const char *name) : al::LiveActor(name) {}

void PuppetHackActor::init(al::ActorInitInfo const &initInfo) {

    // Logger::log("Creating Hack Puppet: %s\n", mHackType.cstr());

    al::initActorWithArchiveName(this, initInfo, mHackType, nullptr);

    al::initActorPoseTQSV(this);

    if (CaptureTypes::currentWorldId == 3) { // Wooded
        // Needed for Poison Pirahna plant silhouette, causes crashes in some other kingdoms so not always calling this func
        al::hideSilhouetteModelIfShow(this);
    }

    if (CaptureTypes::currentWorldId != 1) { // This crashes on Cascade for some reason
        if(al::isExistDitherAnimator(this)) {
            // Logger::log("Disabling Dither Animator.\n");
            al::invalidateDitherAnim(this);
        }
    }

    // if (al::isExistCollisionParts(this)) {
    //     // Logger::log(("Disabling Collision.\n"));
    //     al::invalidateCollisionParts(this);
    // }

    al::invalidateHitSensors(this);

    al::invalidateClipping(this);

    al::invalidateOcclusionQuery(this);

    al::forceLodLevel(this, 0);

    // al::offCollide(this);

    makeActorDead();
}

void PuppetHackActor::initAfterPlacement() {
    al::LiveActor::initAfterPlacement();
}

void PuppetHackActor::initOnline(PuppetInfo *pupInfo, const char *hackType) {
    mInfo = pupInfo;
    mHackType = hackType;
}

void PuppetHackActor::movement() {
    al::LiveActor::movement();
}

void PuppetHackActor::control() {

}

al::ModelKeeper* overrideModelKeeper = nullptr;

void hookInitActorModelKeeper(al::LiveActor* actor, al::ActorInitInfo const& initInfo, al::ActorResource* actorResource, int param_4) {
    if (!overrideModelKeeper || !actor) {
        al::initActorModelKeeper(actor, initInfo, actorResource, param_4);
        return;
    }

    actor->initModelKeeper(overrideModelKeeper);
};

PuppetHackActor *createPuppetHackActorFromFactory(al::ActorInitInfo const &rootInitInfo, const al::PlacementInfo *rootPlacementInfo, PuppetInfo *curInfo, const char *hackType) {
    al::ActorInitInfo actorInitInfo = al::ActorInitInfo();
    actorInitInfo.initViewIdSelf(rootPlacementInfo, rootInitInfo);

    int serverMaxPlayers = Client::getMaxPlayerCount(); // TODO: Find a way around needing to do this, such as creating a single hack actor per puppet that can dynamically switch models

    if (serverMaxPlayers > 8) { // disable capture sync if dealing with more than 8 players
        return nullptr;
    }

    al::createActor createActor = actorInitInfo.mActorFactory->getCreator("PuppetHackActor");
    
    if(createActor) {
        PuppetHackActor *newActor = (PuppetHackActor*)createActor("PuppetHackActor");

        newActor->initOnline(curInfo, hackType); // set puppet info first before calling init so we can get costume info from the info

        newActor->init(actorInitInfo);

        return newActor;
    }else {
        return nullptr;
    }
}


void initAllActorsForPropType(al::ActorInitInfo const &initInfo, al::PlacementInfo const& placement, const char* propArchiveName) { 
    int serverMaxPlayers = Client::getMaxPlayerCount();

    for (size_t i = 0; i < serverMaxPlayers - 1; i++)
    {
        PuppetActor* curPuppet = Client::getPuppet(i);
        if (curPuppet) {

            const char* hackName = tryConvertName(propArchiveName);

            PuppetHackActor* dupliActor = createPuppetHackActorFromFactory(
                initInfo, &placement, curPuppet->getInfo(), hackName);
            if (dupliActor) {
                curPuppet->addCapture(dupliActor, hackName);
                if (!overrideModelKeeper) {
                    overrideModelKeeper = dupliActor->mModelKeeper;
                }
            }

            PuppetHackActor* decoyActor = createPuppetHackActorFromFactory(
                initInfo, &placement, curPuppet->getInfo(), hackName);
            if (decoyActor) {
                curPuppet->addDecoyProp(decoyActor);
                if (!overrideModelKeeper) {
                    overrideModelKeeper = dupliActor->mModelKeeper;
                }
            }
        }
    }

    PuppetActor* debugPuppet = Client::getDebugPuppet();

    if (debugPuppet) {
        const char* hackName = tryConvertName(propArchiveName);
        PuppetHackActor *dupliActor = createPuppetHackActorFromFactory(initInfo, &placement, debugPuppet->getInfo(), hackName);
        if (dupliActor) {
            debugPuppet->addCapture(dupliActor, hackName);
        }
    }

    overrideModelKeeper = nullptr;
}

void PuppetHackActor::initAllActors(al::ActorInitInfo const &initInfo, al::PlacementInfo const& placement) {
    const auto range = CaptureTypes::getTypesForCurrentWorld();
    for (int32_t i = 0; i < range.size(); i++) {
        const char* propArchiveName = CaptureTypes::FindStr(range.getPropType(i));
        initAllActorsForPropType(initInfo, placement, propArchiveName);
    }
}