#include "BoxComponent.h"
#include "Game.h"
#include "Actor.h"
#include "PointLineComponent.h"
#include "../GameObject/GeometryData.h"
#include "../MATRIX.h"

BoxComponent::BoxComponent(Actor* owner, int order)
	:ColliderComponent(owner, order)
{
	VECTOR vertices[BoxNumVertices] = {};
	for (int i = 0; i < BoxNumVertices; i++)
	{
		vertices[i].x = BoxVertices[i * polygonNum + 0];
		vertices[i].y = BoxVertices[i * polygonNum + 1];
		vertices[i].z = BoxVertices[i * polygonNum + 2];
	}
	mBaseVertices.resize(BoxNumIndices);
	for (int i = 0; i < BoxNumIndices; i++)
	{
		mBaseVertices[i] = vertices[BoxIndices[i]];
		float distance = mBaseVertices[i].mag();
		if (distance > mMaxDistance)
		{
			mMaxDistance = distance;
		}
	}
	mVertices.resize(BoxNumIndices);
	mFaces.resize(BoxNumFaces);
	for (int i = 0; i < BoxNumFaces; i++)
	{
		mFaces[i] = new FaceInfo;
	}
	// Collider�^�C�v�ݒ�
	mType = ColliderType::Box;

	// ���S���W�X�V
	GetCenter();
}
// ���_�@�����X�V
void BoxComponent::SetVerticesNormals()
{
	GameState state = mOwner->GetGame()->GetGameState();
	// ���ɕύX�������
	if (mSavePos != mOwner->GetPosition() || mSaveSize != mSize || mSaveRota != mOwner->GetRotation() || mSavePosGap != GetPositionGap() || mSaveState != state)
	{
		// ���_��]
		XMMATRIX world_matrix;
		XMMATRIX translate = XMMatrixTranslation(mOwner->GetPosition().x, mOwner->GetPosition().y, mOwner->GetPosition().z);
		XMMATRIX rotate_x = XMMatrixRotationX(XMConvertToRadians(mOwner->GetRotation().x));
		XMMATRIX rotate_y = XMMatrixRotationY(XMConvertToRadians(mOwner->GetRotation().y));
		XMMATRIX rotate_z = XMMatrixRotationZ(XMConvertToRadians(mOwner->GetRotation().z));
		XMMATRIX scale_mat = XMMatrixScaling(mSize.x, mSize.y, mSize.z);
		world_matrix = scale_mat * rotate_z * rotate_x * rotate_y * translate;

		MATRIX mat;
		mat = world_matrix;
		// ���_���
		for (int i = 0; i < BoxNumIndices; ++i)
		{
			// �T�C�Y�ύX
			// ���炵����������
			VECTOR Vertex = mBaseVertices[i] + GetPositionGap();
			mVertices[i] = mat * Vertex;
		}

		// �ʏ��
		for (int i = 0; i < BoxNumFaces; ++i)
		{
			for (int k = 0; k < polygonNum; ++k)
			{
				int num = i * polygonNum + k;
				mFaces[i]->vertices[k] = mVertices[num];
			}
			mFaces[i]->UpdateCenter();
		}

		// ��]���Ă�����@���̍X�V
		if (mSaveRota != mOwner->GetRotation())
		{
			for (int i = 0; i < BoxNumFaces; ++i)
			{
				mFaces[i]->UpdateNormal();
			}
		}

		// �T�C�Y���ς�����猴�_���璸�_���̋������ʐς��ς��
		if (mSaveSize != mSize)
		{
			for (int i = 0; i < BoxNumIndices; ++i)
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
			std::vector<LineData> data;
			for (int i = 0; i < BoxNumFaces; ++i)
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
		else if (mSaveState != state)
		{
			mLine->DeleteLine();
		}
		// ���ۑ�
		mSaveSize = mSize;
		mSaveRota = mOwner->GetRotation();
		mSavePos = mOwner->GetPosition();
		mSavePosGap = GetPositionGap();
		mSaveState = state;

		// ���S���W�X�V
		GetCenter();
	}
}