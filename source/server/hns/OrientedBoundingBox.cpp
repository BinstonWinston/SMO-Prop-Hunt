#include "server/hns/OrientedBoundingBox.hpp"
#include "al/util/VectorUtil.h"

OrientedBoundingBox::OrientedBoundingBox(
        sead::BoundBox3f bbox,
        sead::Vector3f pos,
        sead::Quatf rot)
    : mBbox(bbox)
    , mPos(pos)
    , mRot(rot) 
{}

bool OrientedBoundingBox::isInside(sead::Vector3f const& p, f32 radius) const {
    auto localPos = p;
    localPos -= mPos;
    sead::Quatf rot = mRot;
    sead::Quatf rotI = mRot;
    rot.inverse(&rotI);
    al::rotateVectorQuat(&localPos, rotI);
    sead::BoundBox3f bboxExpandedBySearchRadius{
        mBbox.getMin() - sead::Vector3f(1.f,1.f,1.f)*radius,
        mBbox.getMax() + sead::Vector3f(1.f,1.f,1.f)*radius,
    };
    return bboxExpandedBySearchRadius.isInside(localPos);
}

std::array<sead::Vector3f, 8> OrientedBoundingBox::getPoints() const {
    std::array<sead::Vector3f, 8> points{
        sead::Vector3f{mBbox.getMin().x, mBbox.getMin().y, mBbox.getMin().z},
        sead::Vector3f{mBbox.getMin().x, mBbox.getMin().y, mBbox.getMax().z},
        sead::Vector3f{mBbox.getMin().x, mBbox.getMax().y, mBbox.getMin().z},
        sead::Vector3f{mBbox.getMin().x, mBbox.getMax().y, mBbox.getMax().z},
        sead::Vector3f{mBbox.getMax().x, mBbox.getMin().y, mBbox.getMin().z},
        sead::Vector3f{mBbox.getMax().x, mBbox.getMin().y, mBbox.getMax().z},
        sead::Vector3f{mBbox.getMax().x, mBbox.getMax().y, mBbox.getMin().z},
        sead::Vector3f{mBbox.getMax().x, mBbox.getMax().y, mBbox.getMax().z},
    };
    for (s32 i = 0; i < points.size(); i++) {
        al::rotateVectorQuat(&points[i], mRot);
        points[i] += mPos;
    }
    return points;
}

void OrientedBoundingBox::scale(f32 s) {
    mBbox.scaleX(s);
    mBbox.scaleY(s);
    mBbox.scaleZ(s);
}