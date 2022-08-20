#pragma once
#include "../VECTOR.h"
#include "../MATRIX.h"
#include "ColliderComponent.h"
#include <DirectXMath.h>
#include <vector>

// -------------- using namespace -------------
using namespace DirectX;

// -------------- �\���̐錾 -------------
typedef struct _NormalData_
{
	VECTOR normal;
	VECTOR crossPoint;
	_NormalData_()
	{
		normal = VECTOR(0, 0, 0);
		crossPoint = VECTOR(0, 0, 0);
	}
}NORMALDATA;

// -------------- �֐��錾 -------------
// ----- �����蔻�� -----

// ���Ɛ����̐ڐG����
bool Intersect(const class BoxComponent* box, VECTOR rayStartPos, VECTOR rayEndPos);

// ���Ɣ��̐ڐG����
bool Intersect(const class BoxComponent* a, const class BoxComponent* b);

// ���Ɖ~���̐ڐG����
bool Intersect(const class BoxComponent* a, const class CylinderComponent* b);

// �J�v�Z���Ɣ��̐ڐG����
bool Intersect(const class CapsuleComponent* a, const class BoxComponent* b);

// ����(����2)����(����1)�ɐڐG���������̖@���ƏՓ˒n�_���擾
bool Normal(const class BoxComponent* box, VECTOR rayStartPos, VECTOR rayEndPos, NORMALDATA& normalData);

// ��(����2)����(����1)�ɐڐG���������̖@���ƏՓ˒n�_���擾
bool NormalBxB(const class BoxComponent* a, const class BoxComponent* b, std::vector<NORMALDATA>& normalData);

// �~��(����2)����(����1)�ɐڐG���������̖@���ƏՓ˒n�_���擾
bool Normal(const class BoxComponent* a, const class CylinderComponent* b, std::vector<NORMALDATA>& normalData);

// �R���C�_�[���m�̓����蔻��
bool HitCheck_Collider(class ColliderComponent* a, class ColliderComponent* b, VECTOR& progressVector);

// �������̍X�V
void UpdateAxisOBB(ColliderComponent* a);

//�@������r
bool CompareLengthOBB(const ColliderComponent* a, const ColliderComponent* b, VECTOR separating, VECTOR distance);

// �O�p�`�ƒ����̓����蔻���擾
bool HitCheck_Triangle_Line(ColliderComponent::FaceInfo* poly1, VECTOR rayStartPos, VECTOR rayEndPos);

// �O�p�`�ƎO�p�`�̓����蔻��ƏՓ˒n�_���擾
const bool HitCheck_Triangle_Triangle(ColliderComponent::FaceInfo* poly1, const ColliderComponent::FaceInfo* poly2);

// ���ƎO�p�`�̓����蔻��ƏՓ˒n�_���擾
bool HitCheck_Sphere_Triangle(VECTOR center, float radius, ColliderComponent::FaceInfo* poly2);

// Z�������̕����Ɍ�����s��
MATRIX LookAtMatrix(VECTOR lookat, VECTOR up, VECTOR right);