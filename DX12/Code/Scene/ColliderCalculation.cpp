#include "ColliderCalculation.h"
#include "SphereComponent.h"
#include "BoxComponent.h"
#include "CylinderComponent.h"
#include "CapsuleComponent.h"
// 面形成のポリゴン数
const int polygonSize = 3;

// 箱と線分の接触判定
bool Intersect(BoxComponent* box, VECTOR rayStartPos, VECTOR rayEndPos)
{
	for (int i = 0; i < BoxNumIndices; i += polygonSize)
	{
		VECTOR vertices[polygonSize] = {};
		for (int l = 0; l < polygonSize; ++l)
		{
			int index = i + l;
		}
		VECTOR crossPoint = {};
		//if (HitCheck_Triangle_Line(vertices, rayStartPos, rayEndPos, crossPoint))
		//{
		//	return true;
		//}
	}
	return false;
}

// 箱と箱の接触判定
bool Intersect(BoxComponent* a, BoxComponent* b)
{
	VECTOR distance = a->GetCenter() - b->GetCenter();

	
	// 分離軸の更新
	UpdateAxisOBB(a);
	UpdateAxisOBB(b);

	// 分離軸の比較
	if (!CompareLengthOBB(a, b, a->GetAxizeX(), distance))return false;
	if (!CompareLengthOBB(a, b, a->GetAxizeY(), distance))return false;
	if (!CompareLengthOBB(a, b, a->GetAxizeZ(), distance))return false;
	if (!CompareLengthOBB(a, b, b->GetAxizeX(), distance))return false;
	if (!CompareLengthOBB(a, b, b->GetAxizeY(), distance))return false;
	if (!CompareLengthOBB(a, b, b->GetAxizeZ(), distance))return false;

	// 分離軸同士の外積を比較
	VECTOR separating = {};

	separating = cross(a->GetAxizeX(), a->GetAxizeX());;
	if (!CompareLengthOBB(a, b, separating, distance))return false;
	separating = cross(a->GetAxizeX(), a->GetAxizeY());
	if (!CompareLengthOBB(a, b, separating, distance))return false;
	separating = cross(a->GetAxizeX(), a->GetAxizeZ());
	if (!CompareLengthOBB(a, b, separating, distance))return false;

	separating = cross(a->GetAxizeY(), a->GetAxizeX());;
	if (!CompareLengthOBB(a, b, separating, distance))return false;
	separating = cross(a->GetAxizeY(), a->GetAxizeY());
	if (!CompareLengthOBB(a, b, separating, distance))return false;
	separating = cross(a->GetAxizeY(), a->GetAxizeZ());
	if (!CompareLengthOBB(a, b, separating, distance))return false;

	separating = cross(a->GetAxizeZ(), a->GetAxizeX());;
	if (!CompareLengthOBB(a, b, separating, distance))return false;
	separating = cross(a->GetAxizeZ(), a->GetAxizeY());
	if (!CompareLengthOBB(a, b, separating, distance))return false;
	separating = cross(a->GetAxizeZ(), a->GetAxizeZ());
	if (!CompareLengthOBB(a, b, separating, distance))return false;

	return true;
}

// 箱と円柱の接触判定
bool Intersect(BoxComponent* a, CylinderComponent* b)
{
	//VECTOR distance = a->GetCenter() - b->GetCenter();


	//// 分離軸の更新
	//UpdateAxisOBB(a);
	//UpdateAxisOBB(b);

	//// 分離軸の比較
	//if (!CompareLengthOBB(a, b, a->GetAxizeX(), distance))return false;
	//if (!CompareLengthOBB(a, b, a->GetAxizeY(), distance))return false;
	//if (!CompareLengthOBB(a, b, a->GetAxizeZ(), distance))return false;
	//if (!CompareLengthOBB(a, b, b->GetAxizeX(), distance))return false;
	//if (!CompareLengthOBB(a, b, b->GetAxizeY(), distance))return false;
	//if (!CompareLengthOBB(a, b, b->GetAxizeZ(), distance))return false;

	//// 分離軸同士の外積を比較
	//VECTOR separating = {};

	//separating = cross(a->GetAxizeX(), a->GetAxizeX());;
	//if (!CompareLengthOBB(a, b, separating, distance))return false;
	//separating = cross(a->GetAxizeX(), a->GetAxizeY());
	//if (!CompareLengthOBB(a, b, separating, distance))return false;
	//separating = cross(a->GetAxizeX(), a->GetAxizeZ());
	//if (!CompareLengthOBB(a, b, separating, distance))return false;

	//separating = cross(a->GetAxizeY(), a->GetAxizeX());;
	//if (!CompareLengthOBB(a, b, separating, distance))return false;
	//separating = cross(a->GetAxizeY(), a->GetAxizeY());
	//if (!CompareLengthOBB(a, b, separating, distance))return false;
	//separating = cross(a->GetAxizeY(), a->GetAxizeZ());
	//if (!CompareLengthOBB(a, b, separating, distance))return false;

	//separating = cross(a->GetAxizeZ(), a->GetAxizeX());;
	//if (!CompareLengthOBB(a, b, separating, distance))return false;
	//separating = cross(a->GetAxizeZ(), a->GetAxizeY());
	//if (!CompareLengthOBB(a, b, separating, distance))return false;
	//separating = cross(a->GetAxizeZ(), a->GetAxizeZ());
	//if (!CompareLengthOBB(a, b, separating, distance))return false;

	//VECTOR crossPoint = {};
	//for (int i = 0; i < BoxNumIndices; i += polygonSize)
	//{
	//	VECTOR verticesA[polygonSize] = {};
	//	for (int j = 0; j < polygonSize; ++j)
	//	{
	//		int index = i + j;
	//		verticesA[j] = a->GetVertices(index);
	//	}
	//	for (int k = 0; k < CylinderNumIndices; k += polygonSize)
	//	{
	//		VECTOR verticesB[polygonSize] = {};
	//		for (int l = 0; l < polygonSize; ++l)
	//		{
	//			int index = k + l;
	//			verticesB[l] = b->GetVertices(index);
	//		}
	//		if (HitCheck_Triangle_Triangle(verticesA, verticesB, crossPoint))
	//		{
	//			return true;
	//		}
	//	}
	//}
	return false;
}

// カプセルと箱の接触判定
bool Intersect(CapsuleComponent* a, BoxComponent* b)
{
	return false;
}

// 線分(引数2)が箱(引数1)に接触した部分の法線と衝突地点を取得
bool Normal(BoxComponent* box, VECTOR rayStartPos, VECTOR rayEndPos, NORMALDATA& normalData)
{
	if (Intersect(box, rayStartPos, rayEndPos))
	{
		float minDistance = 100;
		for (int i = 0; i < BoxNumIndices; i += polygonSize)
		{
			VECTOR vertices[polygonSize] = {};
			for (int l = 0; l < polygonSize; ++l)
			{
				int index = i + l;
			}
			VECTOR crossPoint = {};
			//if (HitCheck_Triangle_Line(vertices, rayStartPos, rayEndPos, crossPoint))
			//{
			//	VECTOR vec = rayStartPos - rayEndPos;
			//	float distance = vec.mag();

			//	if (distance < minDistance)
			//	{
			//		minDistance = distance;
			//		int num = i / polygonSize;
			//		normalData.crossPoint = crossPoint;
			//	}

			//}
		}
		return true;		
	}
	return false;
}

// 箱(引数2)が箱(引数1)に接触した部分の法線と衝突地点を取得
bool NormalBxB(BoxComponent* a, BoxComponent* b, std::vector<NORMALDATA>& normalData)
{
	// 衝突しているか？
	if (Intersect(a, b))
	{
		// 交互にポリゴン同士の判定
		for (int i = 0; i < BoxNumIndices; i += polygonSize)
		{
			// ポリゴン作成
			VECTOR verticesA[polygonSize] = {};
			for (int j = 0; j < polygonSize; ++j)
			{
				int index = i + j;
			}
			for (int k = 0; k < BoxNumIndices; k += polygonSize)
			{
				// ポリゴン作成
				VECTOR verticesB[polygonSize] = {};
				for (int l = 0; l < polygonSize; ++l)
				{
					int index = k + l;
				}
				VECTOR crossPoint = {};
				// ポリゴンが衝突しているか？
				//if (HitCheck_Triangle_Triangle(verticesA, verticesB, crossPoint))
				//{
				//	int num = i / polygonSize;
				//	bool find = false;
				//	
				//}
			}
		}
		return true;
	}
	return false;
}


// 円柱(引数2)が箱(引数1)に接触した部分の法線と衝突地点を取得
bool Normal(BoxComponent* a, CylinderComponent* b, std::vector<NORMALDATA>& normalData)
{
	// 衝突しているか？
	if (Intersect(a, b))
	{
		// 交互にポリゴン同士の判定
		for (int i = 0; i < BoxNumIndices; i += polygonSize)
		{
			// ポリゴン作成
			VECTOR verticesA[polygonSize] = {};
			for (int j = 0; j < polygonSize; ++j)
			{
				int index = i + j;
			}
			for (int k = 0; k < CylinderNumIndices; k += polygonSize)
			{
				// ポリゴン作成
				VECTOR verticesB[polygonSize] = {};
				for (int l = 0; l < polygonSize; ++l)
				{
					int index = k + l;
					verticesB[l] = b->GetVertices(index);
				}
				VECTOR crossPoint = {};
				// ポリゴンが衝突しているか？
				//if (HitCheck_Triangle_Triangle(verticesA, verticesB, crossPoint))
				//{
				//	int num = i / polygonSize;
				//	bool find = false;
				//	NORMALDATA nd;
				//	// あたったポリゴンの面法線代入（重複なし）
				//	for (auto nor : normalData)
				//	{
				//	}
				//	if (!find)
				//	{
				//		nd.crossPoint = crossPoint;
				//		normalData.push_back(nd);
				//	}
				//}
			}
		}
		return true;
	}
	return false;
}

// コライダー同士の当たり判定
bool HitCheck_Collider(ColliderComponent* a, ColliderComponent* b, VECTOR& progressVector)
{
	// 衝突しているか？
	std::vector<ColliderComponent::FaceInfo*> FaceA = a->GetFace();
	std::vector<ColliderComponent::FaceInfo*> FaceB = b->GetFace();
	std::vector<VECTOR> normalData;
	// 交互にポリゴン同士の判定
	for (int i = 0, size = (int)FaceA.size(); i < size; ++i)
	{
		if (dot(FaceA[i]->normal, progressVector) < 0)
		{
			continue;
		}
		for (int k = 0, size = (int)FaceB.size(); k < size; ++k)
		{
			// お互いの面の中心の距離が中心から辺までの距離を合わせた値より大きければ触れてない
			if (distance_of(FaceA[i]->faceCenter, FaceB[k]->faceCenter) > (FaceA[i]->distance + FaceB[k]->distance))
			{
				continue;
			}
			if (dot(-FaceB[k]->normal, progressVector) < 0)
			{
				continue;
			}
			if (dot(FaceA[i]->normal, FaceB[k]->normal) < 0)
			{
				bool hitFlg = false;
				if (VECTOR(FaceA[i]->normal + FaceB[k]->normal).mag() < FLT_EPSILON)
				{
					if (FaceA[i]->faceArea > FaceB[k]->faceArea)
					{
						if (HitCheck_Triangle_Line(FaceA[i], FaceB[k]->vertices[0], FaceB[k]->vertices[0] + FaceA[i]->normal))
						{
							hitFlg = true;
						}
					}
					else
					{
						if (HitCheck_Triangle_Line(FaceB[k], FaceA[i]->vertices[0], FaceA[i]->vertices[0] + FaceB[k]->normal))
						{
							hitFlg = true;
						}
					}
				}
				else
				{
					if (FaceA[i]->faceArea > FaceB[k]->faceArea)
					{
						// bがaにあたった時
						if (HitCheck_Triangle_Triangle(FaceA[i], FaceB[k]))
						{
							hitFlg = true;
						}
					}
					else
					{
						// aがbにあたった時
						if (HitCheck_Triangle_Triangle(FaceB[k], FaceA[i]))
						{
							hitFlg = true;
						}
					}
				}
				if (hitFlg)
				{
					bool find = false;
					// あたったポリゴンの面法線代入（重複なし）
					for (VECTOR nor : normalData)
					{
						if (nor == FaceB[k]->normal)
						{
							find = true;
						}
					}
					if (!find)
					{
						normalData.push_back(FaceB[k]->normal);
						break;
					}
				}
			 }
		}
	}

	if (normalData.size() == 0)
	{
		return false;
	}
	// より小さい数値を入れるため大き目の数値を入れる
	//VECTOR subVec = VECTOR(100, 100, 100);
	//for (auto nA : normalDataA)
	//{
	//	for (auto nB : normalDataB)
	//	{
	//		VECTOR nAB = nA + nB;
	//		if (subVec.mag() > nAB.mag())
	//		{
	//			VECTOR up = VECTOR(0, 1, 0);
	//			VECTOR right = VECTOR(1, 0, 0);
	//			MATRIX matA = LookAtMatrix(nA, up, right);
	//			matA.transpose();
	//			MATRIX matB = LookAtMatrix(-nB, up, right);
	//			MATRIX mat = matA * matB;
	//			float angle_x = (float)asin(mat._32);
	//			angle_x = ConvertToDegrees(angle_x);
	//			float angle_y = (float)atan2(mat._31, mat._33);
	//			angle_y = ConvertToDegrees(angle_y);
	//			float angle_z = (float)atan2(-mat._12, mat._22);
	//			angle_z = ConvertToDegrees(angle_z);
	//			//rotation = VECTOR(angle_x, angle_y, angle_z);
	//			//rotation = rotation.normalize();
	//			subVec = nAB;
	//		}
	//	}
	//}

	for (VECTOR n : normalData)
	{
		if (dot(progressVector, n) < 0)
		{
			progressVector = cross(progressVector, n);
			progressVector = cross(n, progressVector);
		}
	}

	return true;
}

// 分離軸の更新
void UpdateAxisOBB(ColliderComponent* a)
{
	XMMATRIX rotationMat = XMMatrixIdentity();

	rotationMat = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(a->GetRotation().x),	// ピッチを指定
		XMConvertToRadians(a->GetRotation().y), // ヨーを指定
		XMConvertToRadians(a->GetRotation().z));// ロールを指定

	// 分離軸（XYZ軸の傾きを表す方向ベクトル）の取得
	a->SetAxizeX(VECTOR(rotationMat.r[0].m128_f32[0], rotationMat.r[0].m128_f32[1], rotationMat.r[0].m128_f32[2]));
	a->SetAxizeY(VECTOR(rotationMat.r[1].m128_f32[0], rotationMat.r[1].m128_f32[1], rotationMat.r[1].m128_f32[2]));
	a->SetAxizeZ(VECTOR(rotationMat.r[2].m128_f32[0], rotationMat.r[2].m128_f32[1], rotationMat.r[2].m128_f32[2]));
}

//　距離比較
bool CompareLengthOBB(const ColliderComponent* a, const ColliderComponent* b, VECTOR separating, VECTOR distance)
{
	// 分離軸上の自分(A)から相手(B)の距離
	float length = fabsf(dot(separating, distance));

	// 分離軸上でAから最も遠いAの頂点までの距離
	float DistanceA = fabsf(dot(a->GetAxizeX(), separating)) * (a->GetSize().x / 2)
		+ fabsf(dot(a->GetAxizeY(), separating)) * (a->GetSize().y / 2)
		+ fabsf(dot(a->GetAxizeZ(), separating)) * (a->GetSize().z / 2);

	// 分離軸上でBから最も遠いBの頂点までの距離
	float DistanceB = fabsf(dot(b->GetAxizeX(), separating)) * (b->GetSize().x / 2)
		+ fabsf(dot(b->GetAxizeY(), separating)) * (b->GetSize().y / 2)
		+ fabsf(dot(b->GetAxizeZ(), separating)) * (b->GetSize().z / 2);

	if (length > DistanceA + DistanceB)
		return false;

	return true;
}

// 三角形と直線の当たり判を取得
bool HitCheck_Triangle_Line(ColliderComponent::FaceInfo* poly1, VECTOR rayStartPos, VECTOR rayEndPos)
{
	VECTOR ray = rayEndPos - rayStartPos;
	// 面に含まれる点から直線の二点に向かうVector
	VECTOR FaceRay1 = rayStartPos - poly1->vertices[0];
	float dot1 = dot(&ray, &poly1->normal);
	float dot2 = dot(&FaceRay1, &poly1->normal);
	if (dot1 != 0 || dot2== 0)
	{
		VECTOR FaceRay2 = rayEndPos - poly1->vertices[0];
		float dot3 = dot(&FaceRay2, &poly1->normal);
		if (dot2 * dot3 <= 0)
		{
			float a = poly1->normal.x;
			float b = poly1->normal.y;
			float c = poly1->normal.z;
			// ３頂点のどれかの座標を代入
			float d = -(a * poly1->vertices[0].x + b * poly1->vertices[0].y + c * poly1->vertices[0].z);


			float t = -(a * rayStartPos.x + b * rayStartPos.y + c * rayStartPos.z
				+ d) / (a * ray.x + b * ray.y + c * ray.z);

			VECTOR crossPoint = rayStartPos + ray * t;
			// 交点がレイ上に存在するか確認
			bool onLine = false;
			if (dot(crossPoint - rayStartPos, crossPoint - rayEndPos) < 0.0f)
			{
				onLine = true;
			}

			// 交点がポリゴンに内包してるか確認
			if (onLine)
			{
				// ポリゴンの外周ベクトル
				VECTOR outLine[polygonSize] = {};
				for (int i = 0; i < polygonSize; ++i)
				{
					outLine[i] = poly1->vertices[(i + 1) % polygonSize] - poly1->vertices[i];
				}
				// ポリゴンの各頂点から交点迄のベクトル
				VECTOR inLine[polygonSize] = {};
				for (int i = 0; i < polygonSize; ++i)
				{
					inLine[i] = crossPoint - poly1->vertices[i];
				}
				VECTOR baseCross = cross(outLine[0], inLine[0]);
				for (int i = 1; i < polygonSize; ++i)
				{
					VECTOR Cross = cross(outLine[i], inLine[i]);
					if (dot(baseCross, Cross) < 0.0f)
						return false;
				}
				return true;
			}
		}
	}
	return false;
}

// 三角形と三角形の当たり判定と衝突地点を取得
const bool HitCheck_Triangle_Triangle(ColliderComponent::FaceInfo* poly1, const ColliderComponent::FaceInfo* poly2)
{
	// 片方の三角形がもう片方の三角形を貫いているか？
	if (HitCheck_Triangle_Line(poly1, poly2->vertices[0], poly2->vertices[1]))
	{
		return true;
	}

	if (HitCheck_Triangle_Line(poly1, poly2->vertices[1], poly2->vertices[2]))
	{
		return true;
	}

	if (HitCheck_Triangle_Line(poly1, poly2->vertices[2], poly2->vertices[0]))
	{
		return true;
	}

	return false;
}

// 球と三角形の当たり判定と衝突地点を取得
bool HitCheck_Sphere_Triangle(VECTOR center, float radius, ColliderComponent::FaceInfo* poly2)
{
	VECTOR point = {};
	bool flg = false;
	VECTOR rayStartPos = center;
	VECTOR rayEndPos = -poly2->normal * radius;
	if (rayEndPos.mag() == radius)
	{
		int a = 0;
	}
	rayEndPos += center;
	//球の中心から伸びる長さが半径の直線がもう片方の三角形を貫いているか？
	if (HitCheck_Triangle_Line(poly2, rayStartPos, rayEndPos))
	{
		flg = true;
	}

	if (flg)
	{
		return true;
	}
	else
	{
		return false;
	}
}
// Z軸を特定の方向に向ける行列
// lookat 向かせたい方向ベクトル
// up	  上ベクトル
// right  右ベクトル
MATRIX  LookAtMatrix(VECTOR lookat, VECTOR up, VECTOR right)
{
	// 向かせたい方向
	VECTOR vz = lookat;

	// （向かせたい方向を向かせたときの）仮のy軸ベクトル
	VECTOR vy = up;

	// (向かせたい方向を向かせたときの)y軸
	VECTOR vx = cross(vy, vz);
	vx = vx.normalize();
	vy = cross(vz, vx);
	vy = vy.normalize();
	// LookAtとupが同じ方向を向いていたらrightを基準にして作り直す
	if (std::abs(dot(vy, vz)) == 1.0f)
	{
		// 仮のx方向を定義
		vx = normalize(right);

		// 向かせたい方向を向かせたときのY軸計算
		vy = normalize(cross(vz, vx));

		// 真のx軸計算
		vx = normalize(cross(vy, vz));
	}

	MATRIX ret;
	ret.identity();
	ret.line0(vx);
	ret.line1(vy);
	ret.line2(vz);
	return ret;
}