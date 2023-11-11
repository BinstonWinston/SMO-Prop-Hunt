#pragma once

#include <math.h>
#include <sead/math/seadBoundBox.h>
#include <sead/math/seadVector.h>
#include <sead/math/seadQuat.h>

class OrientedBoundingBox {
public:
    OrientedBoundingBox(
        sead::BoundBox3f bbox,
        sead::Vector3f pos,
        sead::Quatf rot);

    bool isInside(sead::Vector3f const& p, f32 radius) const;
    std::array<sead::Vector3f, 8> getPoints() const;
    void scaleXZ(f32 s);
private:
    sead::BoundBox3f mBbox;
    sead::Vector3f mPos;
    sead::Quatf mRot;
};