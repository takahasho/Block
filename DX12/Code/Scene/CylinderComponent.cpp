#include "CylinderComponent.h"
#include "Actor.h"
#include <DirectXMath.h>
#include "PointLineComponent.h"
#include "../MATRIX.h"

CylinderComponent::CylinderComponent(Actor* owner, int order)
	:Component(owner, order)
	, mHalfW(0.5f)
	, mHalfH(0.5f)
	, mHalfD(0.5f)
	, mSaveHalfW(0.5f)
	, mSaveHalfH(0.5f)
	, mSaveHalfD(0.5f)
	, mPositionGap(VECTOR(0, 0, 0))
	, mRotationGap(VECTOR(0, 0, 0))
	, mAxizX(VECTOR(0, 0, 0))
	, mAxizY(VECTOR(0, 0, 0))
	, mAxizZ(VECTOR(0, 0, 0))
{
	mLine = new PointLineComponent(owner);
	VECTOR vertices[CylinderNumVertices] = {};
	LineData data[CylinderNumIndices - 1] = {};
	for (int i = 0; i < CylinderNumVertices; i++)
	{
		vertices[i].x = CylinderVertices[i * polygonNum + 0];
		vertices[i].y = CylinderVertices[i * polygonNum + 1];
		vertices[i].z = CylinderVertices[i * polygonNum + 2];
	}
	for (int i = 0; i < CylinderNumIndices; i++)
	{
		mBaseVertices[i] = vertices[CylinderIndices[i]];
		if (i >= 1)
		{
			data[i - 1].startPos = mVertices[i - 1];
			data[i - 1].endPos = mVertices[i];
			data[i - 1].color = green;

		}
	}

	for (int i = 0; i < CylinderNumFaces; i++)
	{
		VECTOR polygon[3];
		polygon[0] = mBaseVertices[i * polygonNum + 0];
		polygon[1] = mBaseVertices[i * polygonNum + 1];
		polygon[2] = mBaseVertices[i * polygonNum + 2];
		VECTOR p0 = polygon[1] - polygon[0];
		VECTOR p1 = polygon[2] - polygon[0];
		VECTOR n;
		n.x = p0.y * p1.z - p0.z * p1.y;
		n.y = p0.z * p1.x - p0.x * p1.z;
		n.z = p0.x * p1.y - p0.y * p1.x;
		mNormals[i] = n;
	}
}
// 頂点法線情報更新
void CylinderComponent::SetVerticesNormals()
{
	if (mSaveHalfW != mHalfW || mSaveHalfH != mHalfH || mSaveHalfD != mHalfD
		|| mSaveRota != GetRotation()|| mSavePos != GetCenter())
	{
		MATRIX rotationX;
		MATRIX rotationY;
		MATRIX rotationZ;
		rotationX.rotateX(ConvertToRadians(GetRotation().x));
		rotationY.rotateY(ConvertToRadians(GetRotation().y));
		rotationZ.rotateZ(ConvertToRadians(GetRotation().z));
		std::vector<LineData> data(CylinderNumIndices - 1);
		// 頂点情報
		for (int i = 0; i < CylinderNumIndices; i++)
		{
			// サイズ変更
			mVertices[i].x = mBaseVertices[i].x * (mHalfW + mHalfW);
			mVertices[i].y = mBaseVertices[i].y * (mHalfH + mHalfH);
			mVertices[i].z = mBaseVertices[i].z * (mHalfD + mHalfD);

			// 頂点回転
			mVertices[i] = rotationZ * rotationY * rotationX * mVertices[i];



			// 位置
			MATRIX basePosition;
			basePosition.translate(mPositionGap.x, mPositionGap.y, mPositionGap.z);
			basePosition = rotationZ * rotationY * rotationX * basePosition;
			VECTOR rotationPos = VECTOR(basePosition._14, basePosition._24, basePosition._34);
			VECTOR pos = mOwner->GetPosition() + rotationPos;
			mVertices[i] += pos;

			// 情報保存
			mSaveHalfW = mHalfW;
			mSaveHalfH = mHalfH;
			mSaveHalfD = mHalfD;
			mSaveRota = GetRotation();
			mSavePos = GetCenter();
		}
		// 法線
		for (int i = 0; i < CylinderNumFaces; i++)
		{
			// 法線回転
			mNormals[i] = rotationZ * rotationY * rotationX * mNormals[i];
		}
		// Line
		for (int i = 0; i < CylinderNumIndices - 1; i++)
		{
			data[i].startPos = mVertices[i];
			data[i].endPos = mVertices[i + 1];
			data[i].color = green;
		}
		//mLine->SetLine(data);
	}
}

VECTOR CylinderComponent::GetCenter()
{
	VECTOR position = mOwner->GetPosition() + mPositionGap;
	return position;
}

VECTOR CylinderComponent::GetRotation()
{
	VECTOR rotation = mOwner->GetRotation() + mRotationGap;
	return rotation;
}