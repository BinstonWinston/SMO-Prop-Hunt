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
        X(WorldEndType__CapWorld) \
        /* Waterfall */ \
        X(WaterfallWorldHomeRock000) \
        X(WorldEndType__WaterfallWorld) \
        /* Sand */ \
        X(SandWorldHomeMeltIce000) \
        X(WorldEndType__SandWorld) \
        /* Lake */ \
        X(LakeWorldHomeStonePillar002) \
        X(WorldEndType__LakeWorld) \
        /* Forest */ \
        X(ForestWorldHomeRock000) \
        X(WorldEndType__ForestWorld) \
        /* Cloud */ \
        X(CapAppearTargetStepC) \
        X(WorldEndType__CloudWorld) \
        /* Clash */ \
        X(ClashWorldHomeTree000) \
        X(WorldEndType__ClashWorld) \
        /* City */ \
        X(FireHydrant) \
        X(CityWorldHomeFence003) \
        X(BlowObjPylon) \
        X(CarBreakable) \
        X(WorldEndType__CityWorld) \
        /* Snow */ \
        X(SnowWorldHomeWindBlowParts000) \
        X(WorldEndType__SnowWorld) \
        /* Sea */ \
        X(SeaWorldHomeGatePillar000) \
        X(WorldEndType__SeaWorld) \
        /* Lava */ \
        X(LavaWorldWireStep000) \
        X(WorldEndType__LavaWorld) \
        /* Sky */ \
        X(SkyWorldHomeTorch000) \
        X(SkyWorldHomeLanternAStand) \
        X(WorldEndType__SkyWorld) \
        /* Moon */ \
        X(MoonRock) \
        X(MoonWorldTableB) \
        X(WorldEndType__MoonWorld) \
        /* Peach */ \
        X(PeachWorldHomeBridge) \
        X(GrowFlowerPot) \
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

    static constexpr size_t ToValue(Type type) { return static_cast<std::uint16_t>(type); }

    static constexpr Type ToType(std::uint16_t value) {return static_cast<Type>(value);}

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

        std::array<Type, 17> worldEndTypes = {
            Type::WorldEndType__CapWorld,
            Type::WorldEndType__WaterfallWorld,
            Type::WorldEndType__SandWorld,
            Type::WorldEndType__LakeWorld,
            Type::WorldEndType__ForestWorld,
            Type::WorldEndType__CloudWorld,
            Type::WorldEndType__ClashWorld,
            Type::WorldEndType__CityWorld,
            Type::WorldEndType__SnowWorld,
            Type::WorldEndType__SeaWorld,
            Type::WorldEndType__LavaWorld,
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
            return PropTypeRange{Type::SandWorldHomeMeltIce000, Type::WorldEndType__SandWorld}; // Just default to cap
        }
        return range;
    }

    extern s32 lastUsedCurrentWorldId;
    static PropTypeRange getTypesForCurrentWorld() {
        lastUsedCurrentWorldId = currentWorldId;
        return getTypesForWorld(currentWorldId);
    }
}