#pragma once

#include <cstddef>
#include <cstdint>
#include "crc32.h"
#include "basis/seadTypes.h"
#include "game/GameData/GameDataFunction.h"

namespace CaptureTypes {

    static constexpr u32 MAX_PROPS_PER_KINGDOM = 20;

    extern s32 currentWorldId;

    #define PROP_OBJECT_LIST \
        /* Cap */ \
        X(SignBoardDanger) \
        X(CapWorldHomeCapHanger004) \
        X(FrailBox) \
        X(CapWorldTowerFloaterParts000) \
        X(CoinRing) \
        X(CapWorldCandlestand000) \
        X(WorldEndType__CapWorld) \
        /* Waterfall */ \
        X(WaterfallWorldHomeRock000) \
        X(WaterfallWorldHomeBreakFence000) \
        X(WaterfallWorldBreakParts006) \
        X(WaterfallWorldBreakParts003) \
        X(WaterfallWorldHomeBreakBone000) \
        X(WaterfallWorldHomeBone000) \
        X(WorldEndType__WaterfallWorld) \
        /* Sand */ \
        X(SandWorldHomeMeltIce000) \
        X(SandWorldHomeStonePillar001) \
        X(SandWorldHomeLift001) \
        X(SphinxQuiz) \
        X(CollectAnimal) \
        X(SandWorldCandlestand000) \
        X(WorldEndType__SandWorld) \
        /* Forest */ \
        X(ForestWorldHomeRock000) \
        X(ForestWorldHomeRiseParts001) \
        X(ForestWorldHomeFallParts000) \
        X(WorldEndType__ForestWorld) \
        /* Lake */ \
        X(LakeWorldHomeStonePillar002) \
        X(LakeWorldHomeFloatingIsland000) \
        X(LakeWorldHomeTown002) \
        X(LakeWorldHomeStonePillar000) \
        X(LakeWorldHomeStonePillar008) \
        X(WorldEndType__LakeWorld) \
        /* Cloud */ \
        X(CapAppearTargetStepC) \
        X(WorldEndType__CloudWorld) \
        /* Clash */ \
        X(ClashWorldHomeTree000) \
        X(CageBreakable) \
        X(ClashWorldBreakParts000) \
        X(ClashWorldHomeTree003) \
        X(WorldEndType__ClashWorld) \
        /* City */ \
        X(FireHydrant) \
        X(CityWorldHomeFence003) \
        X(Car) \
        X(CityWorldTableB) \
        X(CityWorldTableC) \
        X(CityWorldChairB) \
        X(StreetlightA) \
        X(ParkingMeter) \
        X(CityWorldHomeStep033) \
        X(CityWorldHomeFrame009) \
        X(CityWorldHomeTree000) \
        X(CityWorldHomeSignal000) \
        X(WorldEndType__CityWorld) \
        /* Sea */ \
        X(SeaWorldHomeGatePillar000) \
        X(SeaWorldHomeWaveSurfParts000) \
        X(SeaWorldHomeGlassWaveSurfParts) \
        X(SeaWorldHomePillar002) \
        X(VolleyballNet) \
        X(WorldEndType__SeaWorld) \
        /* Snow */ \
        X(SnowWorldHomeWindBlowParts000) \
        X(SnowWorldHomeWindBlowParts001) \
        X(SnowMan) \
        X(SnowWorldHomeIce000) \
        X(TreasureBox) \
        X(WorldEndType__SnowWorld) \
        /* Lava */ \
        X(LavaWorldWireStep000) \
        X(LavaWorldHomeRock000) \
        X(LavaWorldHomeGroundParts002) \
        X(WorldEndType__LavaWorld) \
        /* BossRaidWorld (Ruined) */ \
        X(WorldEndType__BossRaidWorld) \
        /* Sky */ \
        X(SkyWorldHomeTorch000) \
        X(SkyWorldHomeLanternAStand) \
        X(SkyWorldHomeStatue001) \
        X(SkyWorldHomeTimer000FixParts000) \
        X(SkyWorldHomeStep000) \
        X(SkyWorldHomeStep002) \
        X(WorldEndType__SkyWorld) \
        /* Moon */ \
        X(MoonRock) \
        X(MoonWorldTableB) \
        X(MoonWorldCaptureParadeFence000) \
        X(MoonWorldBreakParts) \
        X(WorldEndType__MoonWorld) \
        /* Peach */ \
        X(PeachWorldHomeBridge) \
        X(PeachWorldHomeTree000) \
        X(PeachWorldHomeTree001) \
        X(PeachWorldHomeTower000) \
        X(PeachWorldHomeGarden) \
        X(WorldEndType__PeachWorld) \
        /* Special1 */ \
        X(WorldEndType__Special1World) \
        /* Special2 */ \
        X(WorldEndType__Special2World) \
        X(End) \

    enum class Type: s16
    {
        Unknown = -1,
        #define X(name) name,
        PROP_OBJECT_LIST
        #undef X
    };

    static_assert(static_cast<uint64_t>(Type::End) <= std::numeric_limits<unsigned char>::max(),
                    "Prop list exceeds maximum allowed in current implementation");
                    // Because I pack prop type into the first char of the capture hackName packet string

    static constexpr size_t ToValue(Type type) { return static_cast<std::uint16_t>(type); }

    static constexpr Type ToType(std::uint16_t value) {
        if (value > static_cast<uint64_t>(Type::End)) {
            return Type::Unknown;
        }
        return static_cast<Type>(value);
    }

    static constexpr std::array<const char*, ToValue(Type::End)+1> s_Strs {
        #define X(name) #name,
        PROP_OBJECT_LIST
        #undef X
    };

    static constexpr const char *FindStr(Type type) {
        const s16 type_ = (s16)type;
        if (0 <= type_ && type_ < s_Strs.size())
            return s_Strs[type_];
        else
            return "";
    }

    // these ifdefs are really dumb but it makes clangd happy so /shrug
#ifndef ANALYZER
    static constexpr crc32::HashArray s_Hashes(s_Strs);
#endif

    static constexpr Type FindType(std::string_view const& str) {

#ifndef ANALYZER
        return ToType(s_Hashes.FindIndex(str));
#else
        return Type::Unknown;
#endif
    }


    struct PropTypeRange {
        Type start;
        Type end;

        s32 size() const {
            return static_cast<s32>(end) - static_cast<s32>(start);
        }

        Type getPropType(s32 i) const {
            if (i < 0 || i >= size()) {
                return Type::Unknown;
            }
            return static_cast<Type>(static_cast<s32>(start) + i);
        }
    };

    static constexpr Type getTypeEndForWorld(int worldIndex) {
        if (worldIndex < 0) {
            return Type::Unknown;
        }

        std::array<Type, 18> worldEndTypes = {
            Type::WorldEndType__CapWorld,
            Type::WorldEndType__WaterfallWorld,
            Type::WorldEndType__SandWorld,
            Type::WorldEndType__ForestWorld,
            Type::WorldEndType__LakeWorld,
            Type::WorldEndType__CloudWorld,
            Type::WorldEndType__ClashWorld,
            Type::WorldEndType__CityWorld,
            Type::WorldEndType__SeaWorld,
            Type::WorldEndType__SnowWorld,
            Type::WorldEndType__LavaWorld,
            Type::WorldEndType__BossRaidWorld,
            Type::WorldEndType__SkyWorld,
            Type::WorldEndType__MoonWorld,
            Type::WorldEndType__PeachWorld,
            Type::WorldEndType__Special1World,
            Type::WorldEndType__Special2World
        };

        if (worldIndex >= worldEndTypes.size()) {
            return Type::Unknown;
        }
        return worldEndTypes[worldIndex];
    }

    static PropTypeRange getTypesForWorld(int worldIndex) {
        const PropTypeRange range{static_cast<Type>(static_cast<s32>(getTypeEndForWorld(worldIndex-1))+1), getTypeEndForWorld(worldIndex)};
        if (range.start == range.end) { // No object list for specified kingdom
            return PropTypeRange{Type::SignBoardDanger, Type::WorldEndType__CapWorld}; // Just default to cap
        }
        return range;
    }

    extern s32 lastUsedCurrentWorldId;
    static PropTypeRange getTypesForCurrentWorld() {
        lastUsedCurrentWorldId = currentWorldId;
        return getTypesForWorld(currentWorldId);
    }
}