#pragma once

#include "Packet.h"

struct PACKED CaptureInf : Packet {
    CaptureInf() : Packet() {
        this->mType = PacketType::CAPTUREINF;
        mPacketSize = sizeof(CaptureInf) - sizeof(Packet);
    };

    bool isDecoyProp() {
        return hackName[1] != false;
    }

    char hackName[0x20] = {};

};