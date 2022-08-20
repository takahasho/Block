#include "CapsuleComponent.h"
#include "Game.h"
#include "Actor.h"
#include "PointLineComponent.h"
#include "../GameObject/MeshCreate.h"
#include "../MATRIX.h"

CapsuleComponent::CapsuleComponent(Actor* owner, int order)
	:ColliderComponent(owner, order)
{
	MeshData meshData = CreateCapsule(10, 10,mHeight,mRadius);

	mBaseVertices.resize(meshData.indices.size());
	for (int i = 0; i < mBaseVertices.size(); i++)
	{
		mBaseVertices[i] = meshData.vertices[meshData.indices[i]];
		float distance = mBaseVertices[i].mag();
		if (distance > mMaxDistance)
		{
			mMaxDistance = distance;
		}
	}
	mVertices.resize(mBaseVertices.size());
	int num = (int)mBaseVertices.size() / 3;
	mFaces.resize(num);
	for (int i = 0; i < mFaces.size(); i++)
	{
		mFaces[i] = new FaceInfo;
	}
	// Collider�^�C�v�ݒ�
	mType = ColliderType::Capsule;

}
// ���_�@�����X�V
void CapsuleComponent::SetVerticesNormals()
{
	GameState state = mOwner->GetGame()->GetGameState();
	// ���ɕύX�������
	if (mSavePos != mOwner->GetPosition() || mSaveRadius != mRadius || mSaveHeight != mHeight || mSaveRota != mOwner->GetRotation() || mPositionGap!=mSavePosGap|| mSaveState != state)
	{
		if (mSaveRadius != mRadius || mSaveHeight != mHeight)
		{
			MeshData meshData = CreateCapsule(10, 10, mHeight, mRadius);

			mBaseVertices.resize(meshData.indices.size());
			for (int i = 0; i < mBaseVertices.size(); i++)
			{
				mBaseVertices[i] = meshData.vertices[meshData.indices[i]];
			}
			mVertices.resize(mBaseVertices.size());
			int num = (int)mBaseVertices.size() / 3;
			mFaces.resize(num);
		}
		// ���_��]
		XMMATRIX world_matrix;
		XMMATRIX translate = XMMatrixTranslation(mOwner->GetPosition().x, mOwner->GetPosition().y, mOwner->GetPosition().z);
		XMMATRIX rotate_x = XMMatrixRotationX(XMConvertToRadians(mOwner->GetRotation().x));
		XMMATRIX rotate_y = XMMatrixRotationY(XMConvertToRadians(mOwner->GetRotation().y));
		XMMATRIX rotate_z = XMMatrixRotationZ(XMConvertToRadians(mOwner->GetRotation().z));
		world_matrix = rotate_z * rotate_x * rotate_y * translate;

		MATRIX mat;
		mat = world_matrix;
		// ���_���
		for (int i = 0, size = (int)mVertices.size(); i < size; ++i)
		{
			// �T�C�Y�ύX
			// ���炵����������
			VECTOR Vertex = mBaseVertices[i] + GetPositionGap();
			mVertices[i] = mat * Vertex;
		}

		// �ʏ��
		for (int i = 0, size = (int)mFaces.size(); i < size; ++i)
		{
			for (int k = 0; k < polygonNum; k++)
			{
				int num = i * polygonNum + k;
				mFaces[i]->vertices[k] = mVertices[num];
			}
			mFaces[i]->UpdateCenter();
		}

		// ��]���Ă�����@���̍X�V
		if (mSaveRota != mOwner->GetRotation())
		{
			for (int i = 0, size = (int)mFaces.size(); i < size; ++i)
			{
				mFaces[i]->UpdateNormal();
			}
		}

		// �T�C�Y���ς�����猴�_���璸�_���̋������ʐς��ς��
		if (mSaveRadius != mRadius || mSaveHeight != mHeight)
		{
			for (int i = 0, size = (int)mVertices.size(); i < size; ++i)
			{
				float distance = distance_of(GetCenter(), mVertices[i]);
				if (distance > mMaxDistance)
				{
					mMaxDistance = distance;
				};

			}
			for (int i = 0, size = (int)mFaces.size(); i < size; ++i)
			{
				mFaces[i]->UpdateDistance();
				mFaces[i]->UpdateArea();
			}
		}

		if (state == GameState::ECreative)
		{
			// Line
			std::vector<LineData> data = {};
			for (int i = 0, size = (int)mFaces.size(); i < size; ++i)
			{
				for (int k = 0; k < polygonNum; ++k)
				{
					// �d����������폜
					bool flg = false;
					int nextNum = k + 1;
					if (nextNum >= 3)
					{
						nextNum = 0;
					}
					for (LineData d : data)
					{
						if (d.startPos == mFaces[i]->vertices[k] ||
							d.startPos == mFaces[i]->vertices[nextNum])
						{
							if (d.endPos == mFaces[i]->vertices[nextNum] ||
								d.endPos == mFaces[i]->vertices[k])
							{
								flg = true;
								break;
							}
						}
					}
					if (flg)
					{
						continue;
					}
					LineData linedata;
					linedata.startPos = mFaces[i]->vertices[k];
					linedata.endPos = mFaces[i]->vertices[nextNum];
					linedata.color = green;
					data.push_back(linedata);
				}
			}
			// PointLineComponent�ɓ����
			mLine->resizeLineData((int)data.size());
			for (int i = 0, size = (int)data.size(); i < size; ++i)
			{
				mLine->SetLineData(i, data[i]);
			}
			mLine->SetLine();
		}
		else if(mSaveState != state)
		{
			mLine->DeleteLine();
		}
		// ���ۑ�
		mSaveRadius = mRadius;
		mSaveHeight = mHeight;
		mSaveRota = mOwner->GetRotation();
		mSavePos = mOwner->GetPosition();
		mSavePosGap = GetPositionGap();
		mSaveState = state;

		// ���S���W�X�V
		GetCenter();
	}
}