#pragma once
#include "../VECTOR.h"
#include "../MATRIX.h"
#include "ColliderComponent.h"
#include <DirectXMath.h>
#include <vector>

// -------------- using namespace -------------
using namespace DirectX;

// -------------- 構造体宣言 -------------
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

// -------------- 関数宣言 -------------
// ----- 当たり判定 -----

// 箱と線分の接触判定
bool Intersect(const class BoxComponent* box, VECTOR rayStartPos, VECTOR rayEndPos);

// 箱と箱の接触判定
bool Intersect(const class BoxComponent* a, const class BoxComponent* b);

// 箱と円柱の接触判定
bool Intersect(const class BoxComponent* a, const class CylinderComponent* b);

// カプセルと箱の接触判定
bool Intersect(const class CapsuleComponent* a, const class BoxComponent* b);

// 線分(引数2)が箱(引数1)に接触した部分の法線と衝突地点を取得
bool Normal(const class BoxComponent* box, VECTOR rayStartPos, VECTOR rayEndPos, NORMALDATA& normalData);

// 箱(引数2)が箱(引数1)に接触した部分の法線と衝突地点を取得
bool NormalBxB(const class BoxComponent* a, const class BoxComponent* b, std::vector<NORMALDATA>& normalData);

// 円柱(引数2)が箱(引数1)に接触した部分の法線と衝突地点を取得
bool Normal(const class BoxComponent* a, const class CylinderComponent* b, std::vector<NORMALDATA>& normalData);

// コライダー同士の当たり判定
bool HitCheck_Collider(class ColliderComponent* a, class ColliderComponent* b, VECTOR& progressVector);

// 分離軸の更新
void UpdateAxisOBB(ColliderComponent* a);

//　距離比較
bool CompareLengthOBB(const ColliderComponent* a, const ColliderComponent* b, VECTOR separating, VECTOR distance);

// 三角形と直線の当たり判を取得
bool HitCheck_Triangle_Line(ColliderComponent::FaceInfo* poly1, VECTOR rayStartPos, VECTOR rayEndPos);

// 三角形と三角形の当たり判定と衝突地点を取得
const bool HitCheck_Triangle_Triangle(ColliderComponent::FaceInfo* poly1, const ColliderComponent::FaceInfo* poly2);

// 球と三角形の当たり判定と衝突地点を取得
bool HitCheck_Sphere_Triangle(VECTOR center, float radius, ColliderComponent::FaceInfo* poly2);

// Z軸を特定の方向に向ける行列
MATRIX LookAtMatrix(VECTOR lookat, VECTOR up, VECTOR right);