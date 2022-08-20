#pragma once
#include "Component.h"
#include "../VECTOR.h"
#include "../Global.h"
class CameraComponent:
    public Component
{
public:
    CameraComponent(class Actor*, int drawOrder = 100);  // コンストラクタ

    void ProcessInput() override;

    int GetDrawOrder() const { return mDrawOrder; }

    void SetRotateSpeed(float speed) { mRotateSpeed = speed; }
    void UseSubjectiveCamera(Actor* actor);         // 主観カメラを使う
    void UseObjectiveCamera(const VECTOR targetPositin, const VECTOR eyeAngle, const float zoom);        // 客観カメラを使う
    void Use2DCamera();         // 2Dカメラを使う
    VECTOR GetEyeAngle();
    VECTOR GetTargetPosition();
    // 実際にCamerクラスに送るZoom情報
    float GetZoom();
    void SetZoom(FLOAT value);
    // 3Dモードのzoom情報保存
    FLOAT GetZoom3D() const {
        return  mZoom3D;
    }
    void SetZoom3D(FLOAT value) {
        mZoom3D = value;
    }
    // 2Dモードのzoom情報保存
    FLOAT GetZoom2D() const {
        return  mZoom2D;
    }
    void SetZoom2D(FLOAT value) {
        mZoom2D = value;
    }
private:
    VECTOR mCamRotate;
    int mDrawOrder;
    bool mSubCam;
    POINT mSubCenter;
    float mRotateSpeed;
    bool SubjectiveInit;
    bool ObjectiveInit;
    FLOAT mZoom3D;
    FLOAT mZoom2D;
};

