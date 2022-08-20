#pragma once
#include "Component.h"
#include "../VECTOR.h"
#include "../GameObject/GeometryData.h"
class CylinderComponent:
    public Component
{
public:
    CylinderComponent(class Actor* owner, int order = 100);
    VECTOR GetCenter();
    void SetHalfW(float w) { mHalfW = w; }
    void SetHalfH(float h) { mHalfH = h; }
    void SetHalfD(float d) { mHalfD = d; }
    void SetAxizeX(const VECTOR& axiz) { mAxizX = axiz; }
    void SetAxizeY(const VECTOR& axiz) { mAxizY = axiz; }
    void SetAxizeZ(const VECTOR& axiz) { mAxizZ = axiz; }
    void SetPositionGap(const VECTOR& posgap) { mPositionGap = posgap; }
    void SetRotationGap(const VECTOR& rotegap) { mRotationGap = rotegap; }
    // ���_�@�����X�V
    void SetVerticesNormals(); 
    float GetHalfW() const { return mHalfW; }
    float GetHalfH() const { return mHalfH; }
    float GetHalfD() const { return mHalfD; }
    VECTOR GetRotation();
    VECTOR GetAxizeX() const { return mAxizX; }
    VECTOR GetAxizeY() const { return mAxizY; }
    VECTOR GetAxizeZ() const { return mAxizZ; }
    VECTOR GetVertices(int num) const { return mVertices[num]; }
    VECTOR GetNormals(int num) const { return mNormals[num]; }
private:
    class PointLineComponent* mLine;
    float mHalfW;
    float mHalfH;
    float mHalfD;
    // �A�N�^�[�̈ʒu���炸�炷
    VECTOR mPositionGap;
    VECTOR mRotationGap;
    // ������
    VECTOR mAxizX;
    VECTOR mAxizY;
    VECTOR mAxizZ;
    // Vertices
    // �ʌ`���̃|���S����
    const int polygonNum = 3;		

    VECTOR mBaseVertices[CylinderNumIndices] = {};
    VECTOR mVertices[CylinderNumIndices] = {};

    VECTOR mNormals[CylinderNumFaces] = {};

    VECTOR mSavePos;
    VECTOR mSaveRota;
    float mSaveHalfW;
    float mSaveHalfH;
    float mSaveHalfD;
};