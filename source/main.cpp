#include "main.hpp"
#include <cmath>
#include <math.h>
#include "al/execute/ExecuteOrder.h"
#include "al/execute/ExecuteTable.h"
#include "al/execute/ExecuteTableHolderDraw.h"
#include "al/util/GraphicsUtil.h"
#include "container/seadSafeArray.h"
#include "game/GameData/GameDataHolderAccessor.h"
#include "game/Player/PlayerActorBase.h"
#include "game/Player/PlayerActorHakoniwa.h"
#include "game/Player/PlayerHackKeeper.h"
#include "heap/seadHeap.h"
#include "math/seadVector.h"
#include "server/Client.hpp"
#include "puppets/PuppetInfo.h"
#include "actors/PuppetActor.h"
#include "al/LiveActor/LiveActor.h"
#include "al/util.hpp"
#include "al/util/AudioUtil.h"
#include "al/util/CameraUtil.h"
#include "al/util/ControllerUtil.h"
#include "al/util/LiveActorUtil.h"
#include "al/util/NerveUtil.h"
#include "debugMenu.hpp"
#include "game/GameData/GameDataFunction.h"
#include "game/HakoniwaSequence/HakoniwaSequence.h"
#include "game/Player/PlayerFunction.h"
#include "game/StageScene/StageScene.h"
#include "game/Player/PlayerOxygen.h"
#include "helpers.hpp"
#include "layouts/HideAndSeekIcon.h"
#include "logger.hpp"
#include "rs/util.hpp"
#include "server/gamemode/GameModeBase.hpp"
#include "server/hns/HideAndSeekMode.hpp"
#include "server/gamemode/GameModeManager.hpp"

static int pInfSendTimer = 0;
static int gameInfSendTimer = 0;

void updatePlayerInfo(GameDataHolderAccessor holder, PlayerActorBase* playerBase, bool isYukimaru) {
    
    if (pInfSendTimer >= 3) {

        Client::sendPlayerInfPacket(playerBase, isYukimaru);

        if (!isYukimaru) {
            Client::sendHackCapInfPacket(((PlayerActorHakoniwa*)playerBase)->mHackCap);

            Client::sendCaptureInfPacket((PlayerActorHakoniwa*)playerBase);
        }

        pInfSendTimer = 0;
    }

    if (gameInfSendTimer >= 60) {

        if (isYukimaru) {
            Client::sendGameInfPacket(holder);
        } else {
            Client::sendGameInfPacket((PlayerActorHakoniwa*)playerBase, holder);
        }
        
        gameInfSendTimer = 0;
    }

    pInfSendTimer++;
    gameInfSendTimer++;
}

// ------------- Hooks -------------

int debugPuppetIndex = 0;
int debugCaptureIndex = 0;
static int pageIndex = 0;

static const int maxPages = 3;

void drawMainHook(HakoniwaSequence *curSequence, sead::Viewport *viewport, sead::DrawContext *drawContext) {

    // sead::FrameBuffer *frameBuffer;
    // __asm ("MOV %[result], X21" : [result] "=r" (frameBuffer));

    // if(Application::sInstance->mFramework) {
    //     Application::sInstance->mFramework->mGpuPerf->drawResult((agl::DrawContext *)drawContext, frameBuffer);
    // }

    Time::calcTime();  // this needs to be ran every frame, so running it here works

    if(!debugMode) {
        al::executeDraw(curSequence->mLytKit, "２Ｄバック（メイン画面）");
        return;
    }

    // int dispWidth = al::getLayoutDisplayWidth();
    int dispHeight = al::getLayoutDisplayHeight();

    gTextWriter->mViewport = viewport;

    gTextWriter->mColor = sead::Color4f(1.f, 1.f, 1.f, 0.8f);

    drawBackground((agl::DrawContext *)drawContext);

    gTextWriter->beginDraw();
    gTextWriter->setCursorFromTopLeft(sead::Vector2f(10.f, 10.f));

    gTextWriter->printf("FPS: %d\n", static_cast<int>(round(Application::sInstance->mFramework->calcFps())));

    gTextWriter->setCursorFromTopLeft(sead::Vector2f(10.f, (dispHeight / 3) + 30.f));
    gTextWriter->setScaleFromFontHeight(20.f);

    sead::Heap* clientHeap = Client::getClientHeap();
    sead::Heap *gmHeap = GameModeManager::instance()->getHeap();

    if (clientHeap) {
        gTextWriter->printf("Client Heap Free Size: %f/%f\n", clientHeap->getFreeSize() * 0.001f, clientHeap->getSize() * 0.001f);
        gTextWriter->printf("Gamemode Heap Free Size: %f/%f\n", gmHeap->getFreeSize() * 0.001f, gmHeap->getSize()* 0.001f);
    }

    al::Scene *curScene = curSequence->curScene;

    gTextWriter->printf("Client Socket Connection Status: %s\n", Client::instance()->mSocket->getStateChar());
    gTextWriter->printf("Udp socket status: %s\n", Client::instance()->mSocket->getUdpStateChar());
    //gTextWriter->printf("nn::socket::GetLastErrno: 0x%x\n", Client::instance()->mSocket->socket_errno);
    gTextWriter->printf("Connected Players: %d/%d\n", Client::getConnectCount() + 1, Client::getMaxPlayerCount());
    
    gTextWriter->printf("Send Queue Count: %d/%d\n", Client::instance()->mSocket->getSendCount(), Client::instance()->mSocket->getSendMaxCount());
    gTextWriter->printf("Recv Queue Count: %d/%d\n", Client::instance()->mSocket->getRecvCount(), Client::instance()->mSocket->getRecvMaxCount());

    if(curScene && isInGame) {
        PlayerActorBase* playerBase = rs::getPlayerActor(curScene);

        gTextWriter->printf("\nPlayer Anim: %s\n", ((PlayerActorHakoniwa*)playerBase)->mPlayerAnimator->mAnimFrameCtrl->getActionName());

        sead::LookAtCamera *cam = al::getLookAtCamera(curScene, 0);
        sead::Projection* projection = al::getProjectionSead(curScene, 0);

        auto const propObbOpt = HideAndSeekMode::getPropObb_static();

        PuppetActor* curPuppet = Client::getPuppet(debugPuppetIndex);

        PuppetActor *debugPuppet = Client::getDebugPuppet();

        if (debugPuppet) {
            curPuppet = debugPuppet;
        }

        sead::PrimitiveRenderer *renderer = sead::PrimitiveRenderer::instance();
        renderer->setDrawContext(drawContext);
        renderer->setCamera(*cam);
        renderer->setProjection(*projection);

        renderer->begin();

        //sead::Matrix34f mat = sead::Matrix34f::ident;
        //mat.setBase(3, sead::Vector3f::zero); // Sets the position of the matrix.
                             // For cubes, you need to put this at the location.
                             // For spheres, you can leave this at 0 0 0 since you set it in its draw function.
        renderer->setModelMatrix(sead::Matrix34f::ident);

        if (curPuppet) {
            renderer->drawSphere4x8(curPuppet->getInfo()->playerPos, 20, sead::Color4f(1.f, 0.f, 0.f, 0.25f));
            renderer->drawSphere4x8(al::getTrans(curPuppet), 20, sead::Color4f(0.f, 0.f, 1.f, 0.25f));
        }

        if (propObbOpt.has_value()) {
            sead::Color4f const c{.9f, .2f, .2f, 0.5f};
            auto const propObb = propObbOpt.value();
            auto const obbPoints = propObb.getPoints();

            auto const drawLine = [&renderer](sead::Vector3f const& p1, sead::Vector3f const& p2, sead::Color4f const& c) {
                auto dir = (p2 - p1);
                auto const length = dir.normalize();
                f32 const radius = 20.f;
                for (f32 t = 0.f; t < length; t += radius) {
                    renderer->drawSphere4x8(p1 + dir*t, radius, c);
                }
            };

            // Min-X YZ-plane face
            drawLine(
                obbPoints[0],
                obbPoints[1],
                c);
            drawLine(
                obbPoints[1],
                obbPoints[3],
                c);
            drawLine(
                obbPoints[3],
                obbPoints[2],
                c);
            drawLine(
                obbPoints[2],
                obbPoints[0],
                c);

            // Max-X YZ-plane face
            drawLine(
                obbPoints[4],
                obbPoints[5],
                c);
            drawLine(
                obbPoints[5],
                obbPoints[7],
                c);
            drawLine(
                obbPoints[7],
                obbPoints[6],
                c);
            drawLine(
                obbPoints[6],
                obbPoints[4],
                c);

            // Edges connecting two faces above
            drawLine(
                obbPoints[0],
                obbPoints[4],
                c);
            drawLine(
                obbPoints[1],
                obbPoints[5],
                c);
            drawLine(
                obbPoints[2],
                obbPoints[6],
                c);
            drawLine(
                obbPoints[3],
                obbPoints[7],
                c);
        }

        renderer->end();

        isInGame = false;
    }

    gTextWriter->endDraw();

    al::executeDraw(curSequence->mLytKit, "２Ｄバック（メイン画面）");

}

void sendShinePacket(GameDataHolderAccessor thisPtr, Shine* curShine) {

    if (!curShine->isGot()) {

        GameDataFile::HintInfo* curHintInfo =
            &thisPtr.mData->mGameDataFile->mShineHintList[curShine->mShineIdx];

        Client::sendShineCollectPacket(curHintInfo->mUniqueID);
    }

    GameDataFunction::setGotShine(thisPtr, curShine->curShineInfo);
}

void stageInitHook(al::ActorInitInfo *info, StageScene *curScene, al::PlacementInfo const *placement, al::LayoutInitInfo const *lytInfo, al::ActorFactory const *factory, al::SceneMsgCtrl *sceneMsgCtrl, al::GameDataHolderBase *dataHolder) {
    {
        auto const newWorldId = GameDataFunction::getCurrentWorldId(curScene->mHolder);
        if (CaptureTypes::currentWorldId != newWorldId) {
            // When changing kingdoms, reset prop and mode to prevent crash that occurs when a stale prop is active from previous kingdom
            HideAndSeekMode::clearCurrentPropAndBecomeSeeker();
        }
        CaptureTypes::currentWorldId = newWorldId;
    }
    
    
    al::initActorInitInfo(info, curScene, placement, lytInfo, factory, sceneMsgCtrl,
                          dataHolder);

    Client::clearArrays();

    Client::setSceneInfo(*info, curScene);

    if (GameModeManager::instance()->getGameMode() != NONE) {
        GameModeInitInfo initModeInfo(info, curScene);
        initModeInfo.initServerInfo(GameModeManager::instance()->getGameMode(), Client::getPuppetHolder());

        GameModeManager::instance()->initScene(initModeInfo);
    }

    Client::sendGameInfPacket(info->mActorSceneInfo.mSceneObjHolder);

}

PlayerCostumeInfo *setPlayerModel(al::LiveActor *player, const al::ActorInitInfo &initInfo, const char *bodyModel, const char *capModel, al::AudioKeeper *keeper, bool isCloset) {
    Client::sendCostumeInfPacket(bodyModel, capModel);
    return PlayerFunction::initMarioModelActor(player, initInfo, bodyModel, capModel, keeper, isCloset);
}

al::SequenceInitInfo* initInfo;

ulong constructHook() {  // hook for constructing anything we need to globally be accesible

    __asm("STR X21, [X19,#0x208]"); // stores WorldResourceLoader into HakoniwaSequence

    __asm("MOV %[result], X20"
          : [result] "=r"(
              initInfo));  // Save our scenes init info to a gloabl ptr so we can access it later

    Client::createInstance(al::getCurrentHeap());
    GameModeManager::createInstance(al::getCurrentHeap()); // Create the GameModeManager on the current al heap

    return 0x20;
}

bool threadInit(HakoniwaSequence *mainSeq) {  // hook for initializing client class

    al::LayoutInitInfo lytInfo = al::LayoutInitInfo();

    al::initLayoutInitInfo(&lytInfo, mainSeq->mLytKit, 0, mainSeq->mAudioDirector, initInfo->mSystemInfo->mLayoutSys, initInfo->mSystemInfo->mMessageSys, initInfo->mSystemInfo->mGamePadSys);

    Client::instance()->init(lytInfo, mainSeq->mGameDataHolder);

    return GameDataFunction::isPlayDemoOpening(mainSeq->mGameDataHolder);
}

bool isHoldPositionInAir = false;
void holdPositionInAir(PlayerActorBase* player) {
    // From here https://github.com/CraftyBoss/SMO-Exlaunch-Base/blob/master/src/program/main.cpp#L37C1-L37C1
    sead::Vector3f *playerPos = al::getTransPtr(player);
    // Its better to do this here because loading zones reset this.
    al::setVelocityZero(player);
    // Mario slightly goes down even when velocity is 0. This is a hacky fix for that.
    playerPos->y += 1.5203f;
}

// For oxygen/prop cooldown timer
static StageScene* cachedStageSceneRef = nullptr;

bool hakoniwaSequenceHook(HakoniwaSequence* sequence) {
    StageScene* stageScene = (StageScene*)sequence->curScene;

    cachedStageSceneRef = stageScene;

    static bool isCameraActive = false;

    bool isFirstStep = al::isFirstStep(sequence);

    al::PlayerHolder *pHolder = al::getScenePlayerHolder(stageScene);
    PlayerActorBase* playerBase = al::tryGetPlayerActor(pHolder, 0);
    
    bool isYukimaru = !playerBase->getPlayerInfo();

    isInGame = !stageScene->isPause();

    GameModeManager::instance()->setPaused(stageScene->isPause());
    Client::setStageInfo(stageScene->mHolder);

    Client::update();

    updatePlayerInfo(stageScene->mHolder, playerBase, isYukimaru);

    static bool isDisableMusic = false;

    if (!isFirstStep && playerBase && !isYukimaru && isInGame && isHoldPositionInAir) {
        holdPositionInAir(playerBase);
    }

    if (al::isPadHoldL(-1) && al::isPadTriggerPressLeftStick(-1)) {
        isHoldPositionInAir = !isHoldPositionInAir;
    }

    if (al::isPadHoldZR(-1)) {
        if (al::isPadTriggerUp(-1)) debugMode = !debugMode;
        if (al::isPadTriggerLeft(-1)) pageIndex--;
        if (al::isPadTriggerRight(-1)) pageIndex++;
        if(pageIndex < 0) {
            pageIndex = maxPages - 1;
        }
        if(pageIndex >= maxPages) pageIndex = 0;

    } else if (al::isPadHoldZL(-1)) {

        if (debugMode) {
            if (al::isPadTriggerLeft(-1)) debugPuppetIndex--;
            if (al::isPadTriggerRight(-1)) debugPuppetIndex++;

            if(debugPuppetIndex < 0) {
                debugPuppetIndex = Client::getMaxPlayerCount() - 2;
            }
            if (debugPuppetIndex >= Client::getMaxPlayerCount() - 1)
                debugPuppetIndex = 0;
        }

    } else if (al::isPadHoldL(-1)) {
        if (al::isPadTriggerLeft(-1)) GameModeManager::instance()->toggleActive();
        if (al::isPadTriggerRight(-1)) {
            if (debugMode) {
                
                PuppetInfo* debugPuppet = Client::getDebugPuppetInfo();
                
                if (debugPuppet) {

                    debugPuppet->playerPos = al::getTrans(playerBase);
                    al::calcQuat(&debugPuppet->playerRot, playerBase);

                    const auto propType = HideAndSeekMode::getCurrentPropType();
                    debugPuppet->isCaptured = propType != CaptureTypes::Type::Unknown;
                    debugPuppet->curHack = propType;                 
                }
            }
        }
        if (al::isPadTriggerUp(-1)) {
            if (debugMode) {
                PuppetActor* debugPuppet = Client::getDebugPuppet();
                if (debugPuppet) {
                    PuppetInfo *info = debugPuppet->getInfo();
                    // info->isIt = !info->isIt;

                    debugPuppet->emitJoinEffect();
                    
                }
            } else {
                isDisableMusic = !isDisableMusic;
            }
        }
    }

    if (isDisableMusic) {
        if (al::isPlayingBgm(stageScene)) {
            al::stopAllBgm(stageScene, 0);
        }
    }

    return isFirstStep;

}

void seadPrintHook(const char *fmt, ...)
{
    va_list args;
	va_start(args, fmt);

    Logger::log(fmt, args);

    va_end(args);
}

std::optional<f32> shouldOverrideOxygenForce() {
    if (!GameModeManager::instance()->isMode(GameMode::HIDEANDSEEK)) {
        return {};
    }

    HideAndSeekMode* hsMode = GameModeManager::instance()->getMode<HideAndSeekMode>();
    if (!hsMode) {
        return {};
    }
    return hsMode->getPropCooldown();
}

static bool isPInWater = false;

// Oxygen force stamina wheel/progress bar adapted from Amethyst's code here
// https://github.com/Amethyst-szs/time-travel-standalone/blob/84cc30773007c7a94768a30c11b98f942fcee22d/source/main.cpp#L215

HOOK_ATTR
bool reduceOxygenForce()
{
    if(!cachedStageSceneRef) return false;

    isPInWater = rs::isPlayerInWater(rs::getPlayerActor(cachedStageSceneRef));

    //Perform usual check for if player is in water, but if not, force it on if cooldown is active
    if(isPInWater) return true;
    else return shouldOverrideOxygenForce().has_value();
}

HOOK_ATTR
void oxygenReduce(PlayerOxygen* thisPtr)
{
    float oxygenRingCalc = 0;
    
    //If the player is in water, perform usual calculation
    if(isPInWater){
        thisPtr->mOxygenFrames++;
        if (thisPtr->mOxygenFrames >= thisPtr->mOxygenTarget) { thisPtr->mDamageFrames++; }

        oxygenRingCalc = 1.f - (static_cast<float>(thisPtr->mOxygenFrames) / static_cast<float>(thisPtr->mOxygenTarget));
        if (oxygenRingCalc <= 0.f) oxygenRingCalc = 0.f;
    }

    //If the cooldown is running, replace the value in the ring
    auto const overriddenOxygenForce = shouldOverrideOxygenForce();
    if(overriddenOxygenForce.has_value()){
        oxygenRingCalc = overriddenOxygenForce.value();
        if(thisPtr->mOxygenFrames == 0) thisPtr->mOxygenFrames = thisPtr->mPercentageDelay;
        if(thisPtr->mOxygenFrames >= thisPtr->mOxygenTarget) thisPtr->mOxygenFrames = thisPtr->mOxygenTarget-1;
    }

    thisPtr->mPercentage = oxygenRingCalc;
    return;
}