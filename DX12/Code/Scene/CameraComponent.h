#pragma once
#include "Component.h"
#include "../VECTOR.h"
#include "../Global.h"
class CameraComponent:
    public Component
{
public:
    CameraComponent(class Actor*, int drawOrder = 100);  // �R���X�g���N�^

    void ProcessInput() override;

    int GetDrawOrder() const { return mDrawOrder; }

    void SetRotateSpeed(float speed) { mRotateSpeed = speed; }
    void UseSubjectiveCamera(Actor* actor);         // ��σJ�������g��
    void UseObjectiveCamera(const VECTOR targetPositin, const VECTOR eyeAngle, const float zoom);        // �q�σJ�������g��
    void Use2DCamera();         // 2D�J�������g��
    VECTOR GetEyeAngle();
    VECTOR GetTargetPosition();
    // ���ۂ�Camer�N���X�ɑ���Zoom���
    float GetZoom();
    void SetZoom(FLOAT value);
    // 3D���[�h��zoom���ۑ�
    FLOAT GetZoom3D() const {
        return  mZoom3D;
    }
    void SetZoom3D(FLOAT value) {
        mZoom3D = value;
    }
    // 2D���[�h��zoom���ۑ�
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

