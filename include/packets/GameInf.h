#pragma once

#include "Packet.h"
#include "al/util.hpp"

struct PACKED GameInf : Packet {
    GameInf() : Packet() {this->mType = PacketType::GAMEINF; mPacketSize = sizeof(GameInf) - sizeof(Packet);};
    bool1 is2D_ = false;
    u8 scenarioNo = 255;
    char stageName[0x40] = {};

    bool is2D() {
        return is2D_ & 0x1;
    }

    bool isDecoyProp() {
        return is2D_ >> 4;
    }

    bool operator==(const GameInf &rhs) const {
        return (
            is2D_ == rhs.is2D_ &&
            scenarioNo == rhs.scenarioNo &&
            al::isEqualString(stageName, rhs.stageName)
        );
    }

    bool operator!=(const GameInf& rhs) const { return !operator==(rhs); }

};
