#pragma once
#include "Component.h"
#include "Game.h"
#include "../VECTOR.h"
#include <vector>

// ��������ǂ̃R���C�_�[���g���Ă��邩
enum class ColliderType
{
    Box, Cylinder, Capsule, ModelMesh
};

class ColliderComponent :
    public Component
{
public:
    struct FaceInfo
    {
        std::vector<VECTOR> vertices;       // ���_���
        VECTOR normal;      // �@�����
        VECTOR faceCenter;   // �ʂ̒��S���W
        float distance;     // �ʂ̒��S���璸�_�܂ł̋���
        float faceArea;     // �ʖʐ�

        // �R���X�g���N�^
        FaceInfo()
        {
            // �|���S���̒��_��:3
            vertices.resize(3);
            normal = {};
            faceArea = 0;
            faceCenter = 0;
            distance = 0;
        }

        // �@���X�V
        void UpdateNormal()
        {
            VECTOR p0 = vertices[1] - vertices[0];
            VECTOR p1 = vertices[2] - vertices[0];
            VECTOR n;
            n.x = p0.y * p1.z - p0.z * p1.y;
            n.y = p0.z * p1.x - p0.x * p1.z;
            n.z = p0.x * p1.y - p0.y * p1.x;
            n = normalize(n);
            normal = n;
        }

        // ���S���W&�����X�V
        void UpdateCenter()
        {
            faceCenter.x = (vertices[0].x + vertices[1].x + vertices[2].x) / 3;
            faceCenter.y = (vertices[0].y + vertices[1].y + vertices[2].y) / 3;
            faceCenter.z = (vertices[0].z + vertices[1].z + vertices[2].z) / 3;
        }

        // �����X�V
        void UpdateDistance()
        {
            float d0 = distance_of(vertices[0], faceCenter);
            float d1 = distance_of(vertices[1], faceCenter);
            float d2 = distance_of(vertices[2], faceCenter);
            distance = d0;
            if (distance < d1)
            {
                distance = d1;
            }
            if (distance < d2)
            {
                distance = d2;
            }
        }

        // �ʖʐύX�V
        void UpdateArea()
        {
            VECTOR p0 = vertices[1] - vertices[0];
            VECTOR p1 = vertices[2] - vertices[0];
            VECTOR p2 = vertices[2] - vertices[1];
            float side0 = p0.mag();
            float side1 = p1.mag();
            float side2 = p2.mag();
            float s = (side0 + side1 + side2) / 2;
            faceArea = sqrtf(s * (s - side0) * (s - side1) * (s - side2));
        }
    };
public:
    ColliderComponent(class Actor* owner, int order = 100);
    ~ColliderComponent();
    // ���̃R���|�[�l���g�̍X�V����
    void Update() override;
    void SetSize(VECTOR size) { mSize = size; }
    void SetHeight(float height) { mHeight = height; }
    void SetRadius(float radius) { mRadius = radius; }
    void SetAxizeX(const VECTOR& axiz) { mAxizX = axiz; }
    void SetAxizeY(const VECTOR& axiz) { mAxizY = axiz; }
    void SetAxizeZ(const VECTOR& axiz) { mAxizZ = axiz; }
    void SetPositionGap(const VECTOR& posgap) { mPositionGap = posgap; }
    VECTOR GetSize() const { return mSize; }
    float GetHeight() const { return mHeight; }
    float GetRadius() const { return mRadius; }
    VECTOR GetPositionGap() { return mPositionGap; }
    VECTOR GetRotation();
    VECTOR GetAxizeX() const { return mAxizX; }
    VECTOR GetAxizeY() const { return mAxizY; }
    VECTOR GetAxizeZ() const { return mAxizZ; }
    std::vector<FaceInfo*> GetFace() const { return mFaces; }
    ColliderType GetColliderType() const { return mType; }
    VECTOR GetCenter();
    float GetMaxDistance() { return mMaxDistance; }
protected:
    class PointLineComponent* mLine;
    VECTOR mSize;
    float mHeight;
    float mRadius;
    VECTOR mPositionGap;
    // ������
    VECTOR mAxizX;
    VECTOR mAxizY;
    VECTOR mAxizZ;
    // ���_�����ԗ���Ă��钸�_�̋���
    float mMaxDistance;
    // Vertices
    const int polygonNum = 3;		// �ʌ`���̃|���S����

    std::vector<VECTOR> mBaseVertices;  // ���_�̊�ʒu
    std::vector<VECTOR> mVertices;

    std::vector<FaceInfo*> mFaces;    // �ʌv�Z����Ƃ��Ɏg��
    VECTOR mSavePos;
    VECTOR mSavePosGap;
    VECTOR mSaveRota;
    VECTOR mSaveSize;
    float mSaveHeight;
    float mSaveRadius;
    GameState mSaveState;

    ColliderType mType;
public:
    // ���_���@�����̍X�V
    virtual void SetVerticesNormals();
};