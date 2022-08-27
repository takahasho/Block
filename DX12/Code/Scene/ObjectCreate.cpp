#include "ObjectCreate.h"
#include "../Camera/Camera.h"
#include "../GameObject/Field.h"
#include "../MATRIX.h"
// .txt�ɏo�͂��邽��
#include<fstream>
// �g�p����Actor
#include "Game.h"
#include "Empty.h"
// �g�p����Component
#include "ModelComponent.h"
#include "BoxComponent.h"
#include "CapsuleComponent.h"
#include "PlayerInputComponent.h"
#include "RigidBodyComponent.h"
#include "ImageComponent.h"

ObjectCreate::ObjectCreate(Game* game)
	:mGame(game), mClientRect(), mMousePoint(), mButtonNum(0), mSelectActorNum(-1), mSelectButton(nullptr), mSelectComponentNum(0),
	mClientHeight(0), mClientWidth(0), mMousePosX(0), mMousePosY(0), mPutOnWindow(false), mPreviousSelectButton(nullptr)
{
	mActor = game->GetActors();

	// Model�������Ă���t�H���_��
	mModelFolderPath = _T("Resouse\\Model");

	// Image�������Ă���t�H���_��
	mImageFolderPath = _T("Resouse\\Texture\\Image");

	// �g�����b�V���̖��O������
	mMeshName.push_back(_T("Box"));
	mMeshName.push_back(_T("Cylinder"));
	mMeshName.push_back(_T("Sphere"));
}

ObjectCreate::~ObjectCreate()
{
	for (auto sc : mSelectComponent)
	{
		delete sc;
	}
}

void ObjectCreate::LoadFile()
{
	// �t�@�C���ǂݍ���
	GetModelFileNames(mModelFolderPath, mModelName);
	GetImageFileNames(mImageFolderPath, mImageName);
	// ----- HierarchyWindow�쐬 -----
	mHierarchyWindow.SetMetaData();
	// ----- InspectorWindow�쐬 -----
	mInspectorWindow.SetMetaData();
	// ----- AddObjWindow�쐬 -----
	mAddObjWindow.SetMetaData(mSimpleModelName, mSimpleImageName);
	// ----- AddComponentWindow�쐬 -----
	mAddCW.SetMetaData();
	// ----- SelectWindow�쐬 -----
	mSelectWindow.SetMetaData();

	// ----- PlayButton&TitleButton&SaveButton -----
	CreativeUI Button(100, 50);
	Button.fileName = "Resouse\\Texture\\UI\\PlayButton.png";
	Button.position = VECTOR2((float)g_window_width - 600, 50);
	mPlayButton = Button;
	Button.fileName = "Resouse\\Texture\\UI\\TitleButton.png";
	Button.position = VECTOR2((float)g_window_width - 500, 50);
	mTitleButton = Button;
	Button.fileName = "Resouse\\Texture\\UI\\SaveButton.png";
	Button.position = VECTOR2((float)g_window_width - 400, 50);
	mSaveButton = Button;

	LoadText(_T("Resouse\\�Z�[�u.txt"));
}
void ObjectCreate::Init()
{
	// �ۑ������ʒu��Actor�ɖ߂�
	for (int i = 0; i < mSavePosition.size(); i++)
	{
		mActor[i]->SetPosition(mSavePosition[i]);
	}
	// �ۑ�������]��Actor�ɖ߂�
	for (int i = 0; i < mSaveRotation.size(); i++)
	{
		mActor[i]->SetRotation(mSaveRotation[i]);
	}

	// �ψڏ�񏉊���
	mTitleTrans = false;
	mPlayTrans = false;

	SetDrawOrder();
}

// �`�揇�Ԃ����߂�
void ObjectCreate::SetDrawOrder()
{
	// �ォ�珇�ɕ`�悳���

	// ----- SelectWindow -----
	mSelectWindow.Window.drawOrder = -1;

	// ----- AddComponentWindow -----
	mAddCW.AddButton.drawOrder = -1;
	mAddCW.Window.drawOrder = -1;

	// ----- AddObjWindow -----
	// UI
	mAddObjWindow.AddObjectButton.drawOrder = -2;
	// ImageWidow
	mAddObjWindow.ImageWindow.drawOrder = -1;
	// ModelWidow
	mAddObjWindow.ModelWindow.drawOrder = -1;
	// MeshWidow
	mAddObjWindow.MeshWindow.drawOrder = -1;

	// ------- InsWindow-------
	// UI
	mInspectorWindow.SelectTextBox.drawOrder = 2;
	mInspectorWindow.TopFrame.NameBox.Box.drawOrder = 1;
	mInspectorWindow.Window.drawOrder = -1;
	mInspectorWindow.PullButton.drawOrder = -1;
	mInspectorWindow.Border.drawOrder = -1;
	// ���̉摜�̕`�揇�����߂Ă�������
	mInspectorWindow.Transform.Bottom.drawOrder = -1;
	// ���̉摜�̕`�揇�����߂Ă�������
	mInspectorWindow.ColorFrame.Frame.drawOrder = -1;

	// ----- HierarchyWindow-----
	// UI
	mHierarchyWindow.Window.drawOrder = -1;
	mHierarchyWindow.Border.drawOrder = -1;
}

void ObjectCreate::ProcessInput()
{


	// �g�傳�ꂽ�Ƃ��ɂ��Ή��ł���悤�ɃN���C�A���g�̈���󂯎��
	GetClientRect(g_hwnd, &mClientRect);

	// �E�B���h�E�̈�̃}�E�X���W
	GetCursorPos(&mMousePoint);

	// �E�B���h�E�̈悩��N���C�A���g���W��
	ScreenToClient(g_hwnd, &mMousePoint);

	// �����ϐ��X�V
	mClientWidth = (float)mClientRect.right;
	// �c���ϐ��X�V
	mClientHeight = (float)mClientRect.bottom;
	// �}�E�X���W�ϐ��X�V
	mMousePosX = (float)mMousePoint.x;
	mMousePosY = (float)mMousePoint.y;

	//�@�E�B���h�E�g�厞�Ɉ����L�΂��ꂽ���������}�E�X���W�ɂ�����
	float w = g_window_width / mClientWidth;
	float h = g_window_height / mClientHeight;
	mMousePosX *= w;
	mMousePosY *= h;

	// ���N���b�N�𗣂��Ă�����{�[�_�[������
	if (!MOUSE(0))
	{
		mHierarchyWindow.mGetLeftBorder = false;
		mInspectorWindow.mGetRightBorder = false;
		mHierarchyWindow.mGetLeftBorderSlider = false;
	}
	ObjectDrag();	// �I�u�W�F�N�g������
	// ---------- �f�[�^���� ----------
	HierarchyWindowInput();
	AddComponentInput();
	InspectorWindowInput();
	AddObjWindowInput();
	SelectWindowInput();
	// ----- PlayButton&TitleButton&SaveButton -----
	// �{�^���������ꂽ�Ƃ��w�肳�ꂽ�V�[���֕ψ�
	if (RectOnMouse(mTitleButton))
	{
		if (MOUSE_ENTER(0))
		{
			mTitleTrans = true;
		}
	}
	if (RectOnMouse(mPlayButton))
	{
		if (MOUSE_ENTER(0))
		{
			mPlayTrans = true;
		}
	}
	if (RectOnMouse(mSaveButton))
	{
		if (MOUSE_ENTER(0))
		{
			SaveText();
		}
	}

	// UI�ƃJ�[�\�����d�Ȃ��Ă���Ƃ��J�����������ƍ���̂ŏd�Ȃ��Ă��邩�m�F����
	if (mHierarchyWindow.mPutOnHie || mInspectorWindow.mPutOnIns || mAddObjWindow.mPutOnAddOW || mAddCW.mPutOnAddCW || mSelectWindow.mPutOnSW)
	{
		mPutOnWindow = true;
	}
	else
	{
		mPutOnWindow = false;
	}

	mActorName.clear();
	// mActorName�X�V
	for (auto& button : mHierarchyWindow.Button2D)
	{
		mActorName[button.Text.Text] = button.addNum;
	}
	for (auto& button : mHierarchyWindow.Button3D)
	{
		mActorName[button.Text.Text] = button.addNum;
	}
	mGame->SetActorName(mActorName);
}

void ObjectCreate::UpdateGame()
{
	// ---------- �X�V ----------
	HierarchyWindowUpdate();
	InspectorWindowUpdate();
	AddObjWindowUpdate();
	AddComponentUpdate();
	SelectWindowUpdate();
	// ----- PlayButton&TitleButton -----
	SetUI(mPlayButton);
	SetUI(mTitleButton);
	SetUI(mSaveButton);
}

// �I�u�W�F�N�g������
void ObjectCreate::ObjectDrag()
{
	if (!mPutOnWindow)
	{
		if (mHierarchyWindow.m2D)
		{
			if (MOUSE_ENTER(0))
			{
				XMVECTOR mousePositionNear = XMVectorSet(static_cast<FLOAT>
					(mMousePosX), static_cast<FLOAT>(mMousePosY),
					0.0f, 0.0f);
				XMVECTOR mousePositionFar = XMVectorSet(static_cast<FLOAT>
					(mMousePosX), static_cast<FLOAT>(mMousePosY),
					1.0f, 0.0f);

				const UINT polygonSize = 3;

				// �r���[�|�[�g�̐ݒ�
				D3D12_VIEWPORT viewport = {};
				viewport.Width = (float)g_window_width;	// �o�͐�̕�(�s�N�Z����)
				viewport.Height = (float)g_window_height;	// �o�͐�̍���(�s�N�Z����)
				viewport.TopLeftX = 0;			// �o�͐�̍�����WX
				viewport.TopLeftY = 0;			// �o�͐�̍�����WY
				viewport.MaxDepth = 1.0f;		// �[�x�ő�l
				viewport.MinDepth = 0.0f;		// �[�x�ŏ��l

				XMVECTOR vNear = {}, vFar = {};
				// ���[���h�s��
				XMMATRIX worldMat = XMMatrixIdentity();

				// �X�N���[����Ԃ���I�u�W�F�N�g��ԂɃx�N�g�������W�ϊ�
				vNear = XMVector3Unproject(
					mousePositionNear,	// �ϊ��O�x�N�g��
					viewport.TopLeftX,	// �r���[�|�[�gX���W
					viewport.TopLeftY,	// �r���[�|�[�gY���W
					viewport.Width,		// �r���[�|�[�g�̕�
					viewport.Height,	// �r���[�|�[�g�̍���
					viewport.MinDepth,	// �r���[�|�[�g��OZ�l
					viewport.MaxDepth,	// �r���[�|�[�g���sZ�l
					Camera::GetInstance()->GetProjection(),	// �ˉe�s��
					Camera::GetInstance()->GetView(),		// �r���[�s��
					worldMat);			// ���[���h�s��

				vFar = XMVector3Unproject(
					mousePositionFar,	// �ϊ��O�x�N�g��
					viewport.TopLeftX,	// �r���[�|�[�gX���W
					viewport.TopLeftY,	// �r���[�|�[�gY���W
					viewport.Width,		// �r���[�|�[�g�̕�
					viewport.Height,	// �r���[�|�[�g�̍���
					viewport.MinDepth,	// �r���[�|�[�g��OZ�l
					viewport.MaxDepth,	// �r���[�|�[�g���sZ�l
					Camera::GetInstance()->GetProjection(),	// �ˉe�s��
					Camera::GetInstance()->GetView(),		// �r���[�s��
					worldMat);			// ���[���h�s��

				VECTOR rayStart = {}, rayEnd = {};
				rayStart = VECTOR(vNear.m128_f32[0], vNear.m128_f32[1], vNear.m128_f32[2]);
				rayEnd = VECTOR(vFar.m128_f32[0], vFar.m128_f32[1], vFar.m128_f32[2]);

				// ���C���[�̑傫���Œ͂�Image�����߂�
				int LayerNum = -10;
				// �ꎞ�I�ɔԍ��ۑ�
				int addNum = -1;
				for (HierarchyButton button : mHierarchyWindow.Button2D)
				{
					mAddNum = button.addNum;
					if (mActor[mAddNum]->GetComponent(_T("ImageComponent")))
					{
						if (LayerNum <= mActor[mAddNum]->GetImageComponent()[0]->GetDrawOrder())
						{
							std::vector<VECTOR> vertices(3);
							mWidthHalf = mActor[mAddNum]->GetImageComponent()[0]->GetImageData()->width / 2;
							mHeightHalf = mActor[mAddNum]->GetImageComponent()[0]->GetImageData()->height / 2;

							VECTOR position = mActor[mAddNum]->GetPosition();
							VECTOR rotation = mActor[mAddNum]->GetRotation();
							VECTOR scale = mActor[mAddNum]->GetScale();

							XMMATRIX world_matrix;
							// Canvas�ɕ\���ʒu�����킹�Ă���̂ňʒu����
							position.x += (mWidthHalf);
							position.y *= -1;
							position.y += (float)g_window_height - (mHeightHalf);
							XMMATRIX translate = XMMatrixTranslation(position.x, position.y, position.z);
							XMMATRIX rotate_x = XMMatrixRotationX(XMConvertToRadians(rotation.x));
							XMMATRIX rotate_y = XMMatrixRotationY(XMConvertToRadians(rotation.y));
							XMMATRIX rotate_z = XMMatrixRotationZ(XMConvertToRadians(rotation.z));
							XMMATRIX scale_mat = XMMatrixScaling(scale.x, scale.y, scale.z);
							world_matrix = scale_mat * rotate_z * rotate_x * rotate_y * translate;

							MATRIX mat;
							mat = world_matrix;
							vertices[0] = VECTOR(-mWidthHalf, mHeightHalf, 0);		// ����
							vertices[1] = VECTOR(mWidthHalf, mHeightHalf, 0);		// �E��
							vertices[2] = VECTOR(-mWidthHalf, -mHeightHalf, 0);		// ����
							vertices[0] = mat * vertices[0];
							vertices[1] = mat * vertices[1];
							vertices[2] = mat * vertices[2];

							// �g��Ȃ��̂Ń_�~�[
							VECTOR cp;
							// �|���S���ꖇ��
							if (HitCheckPolygonLine(vertices, rayStart, rayEnd, cp))
							{
								LayerNum = mActor[mAddNum]->GetImageComponent()[0]->GetDrawOrder();
								addNum = mAddNum;
							}
							else
							{
								vertices[0] = VECTOR(mWidthHalf, -mHeightHalf, 0);		// �E��
								vertices[0] = mat * vertices[0];

								// �|���S���񖇖�
								if (HitCheckPolygonLine(vertices, rayStart, rayEnd, cp))
								{
									LayerNum = mActor[mAddNum]->GetImageComponent()[0]->GetDrawOrder();
									addNum = mAddNum;
								}
							}
						}
					}
				}
				if (addNum != -1)
				{
					mHit = true;
					mAddNum = addNum;
				}
			}

			// ��������h���b�O���Ă��Ȃ�
			if (MOUSE_EXIT(0))
			{
				mHit = false;
			}

			// �h���b�O��
			if (mHit)
			{
				XMVECTOR mousePositionNear = XMVectorSet(static_cast<FLOAT>
					(mMousePosX), static_cast<FLOAT>(mMousePosY),
					0.0f, 0.0f);
				XMVECTOR mousePositionFar = XMVectorSet(static_cast<FLOAT>
					(mMousePosX), static_cast<FLOAT>(mMousePosY),
					1.0f, 0.0f);

				const UINT polygonSize = 3;

				// �r���[�|�[�g�̐ݒ�
				D3D12_VIEWPORT viewport = {};
				viewport.Width = (float)g_window_width;	// �o�͐�̕�(�s�N�Z����)
				viewport.Height = (float)g_window_height;	// �o�͐�̍���(�s�N�Z����)
				viewport.TopLeftX = 0;			// �o�͐�̍�����WX
				viewport.TopLeftY = 0;			// �o�͐�̍�����WY
				viewport.MaxDepth = 1.0f;		// �[�x�ő�l
				viewport.MinDepth = 0.0f;		// �[�x�ŏ��l

				XMVECTOR vNear = {}, vFar = {};
				// ���[���h�s��
				XMMATRIX worldMat = XMMatrixIdentity();

				// �X�N���[����Ԃ���I�u�W�F�N�g��ԂɃx�N�g�������W�ϊ�
				vNear = XMVector3Unproject(
					mousePositionNear,	// �ϊ��O�x�N�g��
					viewport.TopLeftX,	// �r���[�|�[�gX���W
					viewport.TopLeftY,	// �r���[�|�[�gY���W
					viewport.Width,		// �r���[�|�[�g�̕�
					viewport.Height,	// �r���[�|�[�g�̍���
					viewport.MinDepth,	// �r���[�|�[�g��OZ�l
					viewport.MaxDepth,	// �r���[�|�[�g���sZ�l
					Camera::GetInstance()->GetProjection(),	// �ˉe�s��
					Camera::GetInstance()->GetView(),		// �r���[�s��
					worldMat);			// ���[���h�s��

				vFar = XMVector3Unproject(
					mousePositionFar,	// �ϊ��O�x�N�g��
					viewport.TopLeftX,	// �r���[�|�[�gX���W
					viewport.TopLeftY,	// �r���[�|�[�gY���W
					viewport.Width,		// �r���[�|�[�g�̕�
					viewport.Height,	// �r���[�|�[�g�̍���
					viewport.MinDepth,	// �r���[�|�[�g��OZ�l
					viewport.MaxDepth,	// �r���[�|�[�g���sZ�l
					Camera::GetInstance()->GetProjection(),	// �ˉe�s��
					Camera::GetInstance()->GetView(),		// �r���[�s��
					worldMat);			// ���[���h�s��

				VECTOR rayStart = {}, rayEnd = {};
				rayStart = VECTOR(vNear.m128_f32[0], vNear.m128_f32[1], vNear.m128_f32[2]);
				rayEnd = VECTOR(vFar.m128_f32[0], vFar.m128_f32[1], vFar.m128_f32[2]);

				XMMATRIX translate = XMMatrixTranslation((float)g_window_width / 2, (float)g_window_height / 2, 0);
				MATRIX mat;
				mat = translate;
				std::vector<VECTOR> vertices(3);
				vertices[0] = VECTOR(-1.0f, 1.0f, 0);		// ����
				vertices[1] = VECTOR(1.0f, 1.0f, 0);		// �E��
				vertices[2] = VECTOR(-1.0f, -1.0f, 0);		// ����
				vertices[0] = mat * vertices[0];
				vertices[1] = mat * vertices[1];
				vertices[2] = mat * vertices[2];
				for (int i = 0, size = (int)vertices.size(); i < size; ++i)
				{
					vertices[i].x *= 3000.0f;
					vertices[i].y *= 3000.0f;
					vertices[i].z = 0;
				}

				VECTOR crosPoint;
				// �|���S���ꖇ��
				if (!HitCheckPolygonLine(vertices, rayStart, rayEnd, crosPoint))
				{
					vertices[0] = VECTOR(1.0f, -1.0f, 0);		// �E��
					vertices[0] = mat * vertices[0];
					vertices[0].x *= 3000.0f;
					vertices[0].y *= 3000.0f;
					// �|���S���񖇖�
					HitCheckPolygonLine(vertices, rayStart, rayEnd, crosPoint);
				}
				mWidthHalf = mActor[mAddNum]->GetImageComponent()[0]->GetImageData()->width / 2;
				mHeightHalf = mActor[mAddNum]->GetImageComponent()[0]->GetImageData()->height / 2;

				crosPoint.x -= (mWidthHalf);
				crosPoint.y *= -1;
				crosPoint.y += g_window_height - (mHeightHalf);
				mActor[mAddNum]->SetPosition(crosPoint);
			}
		}
	}
}

// ======================================================= �f�[�^���� ===================================================
void ObjectCreate::HierarchyWindowInput()
{
	// ----- HierarchyWindow -----
	// ----------------------------------- �\���ʒu -------------------------------------
	// �{�[�_�[������ł�����mHierarchyWindow�ړ�
	if (mHierarchyWindow.mGetLeftBorder)
	{
		mHierarchyWindow.Border.position.x = mMousePosX;
		// 3�`303�̈ړ�������t����
		if (mHierarchyWindow.Border.position.x >= 303)
		{
			mHierarchyWindow.Border.position.x = 303;
		}
		if (mHierarchyWindow.Border.position.x < 3)
		{
			mHierarchyWindow.Border.position.x = 3;
		}

		// �X���C�_�[��x���̓{�[�_�[�Ɠ���
		mHierarchyWindow.BorderSlider.position.x = mHierarchyWindow.Border.position.x;

		// �{�[�_�[�̕�(�R)+�{�[�_�[�Ƃ̏����ʒu�̍�(150)�������B
		mHierarchyWindow.Window.position.x = mMousePosX - 153;
		// -150�`150�̈ړ�������t����
		if (mHierarchyWindow.Window.position.x >= mHierarchyWindow.Window.right)
		{
			mHierarchyWindow.Window.position.x = mHierarchyWindow.Window.right;
		}
		if (mHierarchyWindow.Window.position.x < -mHierarchyWindow.Window.right)
		{
			mHierarchyWindow.Window.position.x = -mHierarchyWindow.Window.right;
		}


		for (auto& button : mHierarchyWindow.Button3D)
		{
			// �{�[�_�[�̕�(�R)+�{�[�_�[�Ƃ̏����ʒu�̍�(200)�������B
			button.RectBlue.position.x = (mMousePosX / 2) - 3;
			button.RectBlue.scale.x = mMousePosX / 300;
			if (button.RectBlue.scale.x >= 1)
			{
				button.RectBlue.scale.x = 1;
			}
			// -200�`200�̈ړ�������t����
			if (button.RectBlue.position.x >= mHierarchyWindow.Window.right)
			{
				button.RectBlue.position.x = mHierarchyWindow.Window.right;
			}
			if (button.RectBlue.position.x < -mHierarchyWindow.Window.right)
			{
				button.RectBlue.position.x = -mHierarchyWindow.Window.right;
			}
		}

		for (auto& button : mHierarchyWindow.Button2D)
		{
			// �{�[�_�[�̕�(�R)+�{�[�_�[�Ƃ̏����ʒu�̍�(200)�������B
			button.RectBlue.position.x = (mMousePosX / 2) - 3;
			button.RectBlue.scale.x = mMousePosX / 300;
			if (button.RectBlue.scale.x >= 1)
			{
				button.RectBlue.scale.x = 1;
			}
			// -200�`200�̈ړ�������t����
			if (button.RectBlue.position.x >= mHierarchyWindow.Window.right)
			{
				button.RectBlue.position.x = mHierarchyWindow.Window.right;
			}
			if (button.RectBlue.position.x < -mHierarchyWindow.Window.right)
			{
				button.RectBlue.position.x = -mHierarchyWindow.Window.right;
			}
		}

		// 3D2D�^�u
		mHierarchyWindow.Tab3D2D[0].position.x = mHierarchyWindow.Border.position.x + 18;
		mHierarchyWindow.Tab3D2D[1].position.x = mHierarchyWindow.Border.position.x + 18;
	}

	for (auto& button : mHierarchyWindow.Button3D)
	{
		// �Ɉʒu�����킹��
		button.RectRed.position = button.RectBlue.position;
		button.RectGreen.position = button.RectBlue.position;
		button.RectRed.scale = button.RectBlue.scale;
		button.RectGreen.scale = button.RectBlue.scale;
	}

	for (auto& button : mHierarchyWindow.Button2D)
	{
		// �Ɉʒu�����킹��
		button.RectRed.position = button.RectBlue.position;
		button.RectGreen.position = button.RectBlue.position;
		button.RectRed.scale = button.RectBlue.scale;
		button.RectGreen.scale = button.RectBlue.scale;
	}

	// �J�[�\�����q�G�����L�[�E�B���h�E�̏�ɂ����
	if (mHierarchyWindow.mPutOnHie)
	{
		LPDIMOUSESTATE2 pMouseState = DInput::GetInstance()->GetMouseState();

		//	�X�N���[��
		if (-1000 < pMouseState->lZ && pMouseState->lZ < 1000)
		{
			if (pMouseState->lZ > 0.0f)	// ��
			{
				mHierarchyWindow.BorderSlider.position.y -= 50;
			}
			if (pMouseState->lZ < 0.0f)	//�@��
			{
				mHierarchyWindow.BorderSlider.position.y += 50;
			}
		}
	}
	// �{�[�_�[�X���C�_�[������ł�����{�^���X���C�h
	if (mHierarchyWindow.mGetLeftBorderSlider)
	{
		mHierarchyWindow.BorderSlider.position.y = mMousePosY;
	}

	// ��ʊO�ɍs���Ȃ��悤�Ɉړ�������t����
	if (mHierarchyWindow.BorderSlider.position.y < mHierarchyWindow.BorderSlider.bottom)
	{
		mHierarchyWindow.BorderSlider.position.y = mHierarchyWindow.BorderSlider.bottom;
	}
	if (mHierarchyWindow.BorderSlider.position.y >= g_window_height + mHierarchyWindow.BorderSlider.top)
	{
		mHierarchyWindow.BorderSlider.position.y = g_window_height + mHierarchyWindow.BorderSlider.top;
	}

	// 100�̓{�^���̍���
	float AllButtonHeight = 100 * (float)mButtonNum;
	if (g_window_height <= AllButtonHeight)
	{
		// ��ʊO�ɂ͂ݏo�Ă��鍂��
		float outHeight = AllButtonHeight - g_window_height;
		// �ړ���������
		float SliderDistance = mHierarchyWindow.BorderSlider.position.y - mHierarchyWindow.BorderSlider.bottom;
		// �X���C�_�[���ő喘�ړ��ł��鋗��
		float MaxSliderDistance = g_window_height - (mHierarchyWindow.BorderSlider.bottom * 2);
		// �ړ�����
		float Direction = (SliderDistance / MaxSliderDistance) * outHeight;
		// ��ʒu�������
		for (auto& button : mHierarchyWindow.Button3D)
		{
			button.RectBlue.position.y = button.basePos.y - Direction;
		}
		for (auto& button : mHierarchyWindow.Button2D)
		{
			button.RectBlue.position.y = button.basePos.y - Direction;
		}
	}

	// �{�^�������ʒu����
	for (auto& button : mHierarchyWindow.Button3D)
	{
		// Text�̕`��͈͂����߂�
		button.Text.maxWidth = mHierarchyWindow.Window.position.x + mHierarchyWindow.Window.right;
		button.Text.position.x = 0;
		button.Text.position.y = button.RectBlue.position.y - button.RectBlue.bottom;
	}

	for (auto& button : mHierarchyWindow.Button2D)
	{
		// Text�̕`��͈͂����߂�
		button.Text.maxWidth = mHierarchyWindow.Window.position.x + mHierarchyWindow.Window.right;
		button.Text.position.x = 0;
		button.Text.position.y = button.RectBlue.position.y - button.RectBlue.bottom;
	}

	// ----------------------------------- ���͏��� -------------------------------------
	// �}�E�X�J�[�\�������̃E�B���h�E�ɏd�Ȃ���������
	if (mMousePosX >= 0 && mMousePosX <= (mHierarchyWindow.Window.position.x + mHierarchyWindow.Window.right)
		&& mMousePosY >= 0 && mMousePosY <= mClientHeight)
	{
		mHierarchyWindow.mPutOnHie = true;
	}
	else
	{
		mHierarchyWindow.mPutOnHie = false;
	}

	// �}�E�X�J�[�\�����{�[�_�[�ɏd�Ȃ��Ă�����
	if (mMousePosX >= (mHierarchyWindow.Border.position.x + mHierarchyWindow.Border.left) && mMousePosX <= (mHierarchyWindow.Border.position.x + mHierarchyWindow.Border.right)
		&& mMousePosY >= 0 && mMousePosY <= mClientHeight)
	{
		// ���N���b�N�Ń{�[�_�[������
		if (MOUSE_ENTER(0))
		{
			mHierarchyWindow.mGetLeftBorder = true;
		}
	}

	// �}�E�X�J�[�\�����{�[�_�[�X���C�_�[�ɏd�Ȃ��Ă�����
	if (RectOnMouse(mHierarchyWindow.BorderSlider))
	{
		// ���N���b�N�Ń{�[�_�[�X���C�_�[������
		if (MOUSE_ENTER(0))
		{
			mHierarchyWindow.mGetLeftBorderSlider = true;
		}
	}

	// �{�^���ƃJ�[�\�����d�Ȃ��Ă���Ƃ�
	if (mHierarchyWindow.m2D)
	{
		for (int i = 0 ,size = (int)mHierarchyWindow.Button2D.size(); i < size; i++)
		{
			if (RectOnMouse(mHierarchyWindow.Button2D[i].RectBlue))
			{
				// ���N���b�N���������Ƃ�
				if (MOUSE_ENTER(0))
				{
					mSelectButton = &mHierarchyWindow.Button2D[i];
					mSelectButtonNum = i;
					mSelectActorNum = mSelectButton->addNum;
				}
			}
		}
	}
	else
	{
		for (int i = 0, size = (int)mHierarchyWindow.Button3D.size(); i < size; i++)
		{
			if (RectOnMouse(mHierarchyWindow.Button3D[i].RectBlue))
			{
				// ���N���b�N���������Ƃ�
				if (MOUSE_ENTER(0))
				{
					mSelectButton = &mHierarchyWindow.Button3D[i];
					mSelectButtonNum = i;
					mSelectActorNum = mSelectButton->addNum;
				}
			}
		}
	}

	// 3D2D�^�u����������A���[�h�ؑ�
	if (mHierarchyWindow.m2D)
	{
		if (mMousePosX >= (mHierarchyWindow.Tab3D2D[1].position.x+ mHierarchyWindow.Tab3D2D[1].left) && mMousePosX <= (mHierarchyWindow.Tab3D2D[1].position.x + mHierarchyWindow.Tab3D2D[1].right)
			&& mMousePosY >= (mHierarchyWindow.Tab3D2D[1].position.y + mHierarchyWindow.Tab3D2D[1].top)&& mMousePosY <= mHierarchyWindow.Tab3D2D[1].position.y)
		{
			// ���N���b�N���������Ƃ�
			if (MOUSE_ENTER(0))
			{
				mSelectButton = nullptr;
				RiseInspector();
				mHierarchyWindow.m2D = false;
				RiseUI(mHierarchyWindow.Tab3D2D[0]);
				for (int i = 0, size = (int)mHierarchyWindow.Button2D.size(); i < size; ++i)
				{
					RiseText(mHierarchyWindow.Button2D[i].Text);
					// ���\������Ă���{�^�����\��
					if (mHierarchyWindow.Button2D[i].ColorNum == 0)
					{
						RiseUI(mHierarchyWindow.Button2D[i].RectBlue);
					}
					else if (mHierarchyWindow.Button2D[i].ColorNum == 1)
					{
						RiseUI(mHierarchyWindow.Button2D[i].RectRed);

					}
					else if (mHierarchyWindow.Button2D[i].ColorNum == 2)
					{
						RiseUI(mHierarchyWindow.Button2D[i].RectGreen);
					}
				}
				// ���\�����Ă���Window���\��
				// ImageWindow					
				for (int i = 0, size = (int)mAddObjWindow.ImageNameText.size(); i < size; ++i)
				{
					RiseUI(mAddObjWindow.ImageButton[i]);
					RiseText(mAddObjWindow.ImageNameText[i]);
				}
				RiseUI(mAddObjWindow.ImageWindow);

				// SelectWindow�폜
				mOpenSelectWindow = false;
				for (int i = 0, size = (int)mSelectWindow.Button.size(); i < size; ++i)
				{
					RiseUI(mSelectWindow.Button[i]);
					RiseText(mSelectWindow.NameText[i]);
				}
			}
		}
	}
	else
	{
		if (mMousePosX >= (mHierarchyWindow.Tab3D2D[0].position.x + mHierarchyWindow.Tab3D2D[0].left) && mMousePosX <= (mHierarchyWindow.Tab3D2D[0].position.x + mHierarchyWindow.Tab3D2D[0].right)
			&& mMousePosY >= mHierarchyWindow.Tab3D2D[0].position.y && mMousePosY <= mHierarchyWindow.Tab3D2D[0].position.y + mHierarchyWindow.Tab3D2D[0].bottom)
		{
			// ���N���b�N���������Ƃ�
			if (MOUSE_ENTER(0))
			{
				mSelectButton = nullptr;
				RiseInspector();
				mHierarchyWindow.m2D = true;
				RiseUI(mHierarchyWindow.Tab3D2D[1]);
				for (int i = 0, size = (int)mHierarchyWindow.Button3D.size(); i < size; ++i)
				{
					RiseText(mHierarchyWindow.Button3D[i].Text);
					// ���\������Ă���{�^�����\��
					if (mHierarchyWindow.Button3D[i].ColorNum == 0)
					{
						RiseUI(mHierarchyWindow.Button3D[i].RectBlue);
					}
					else if (mHierarchyWindow.Button3D[i].ColorNum == 1)
					{
						RiseUI(mHierarchyWindow.Button3D[i].RectRed);

					}
					else if (mHierarchyWindow.Button3D[i].ColorNum == 2)
					{
						RiseUI(mHierarchyWindow.Button3D[i].RectGreen);
					}
				}
				
				// ���\�����Ă���Window���\��
				RiseText(mAddObjWindow.MeshText);
				RiseText(mAddObjWindow.ModelText);
				// AddMeshWindow
				if (!mAddObjWindow.MeshOrModelWindow)
				{
					RiseUI(mAddObjWindow.BOXButtonOff);
					RiseUI(mAddObjWindow.CylinderButtonOff);
					RiseUI(mAddObjWindow.SphereButtonOff);
					RiseUI(mAddObjWindow.MeshWindow);
				}
				// AddModelWindow
				else
				{
					// ModelText
					for (int i = 0, size = (int)mAddObjWindow.ModelNameText.size(); i < size; ++i)
					{
						RiseUI(mAddObjWindow.ModelButton[i]);
						RiseText(mAddObjWindow.ModelNameText[i]);
					}
					RiseUI(mAddObjWindow.ModelWindow);
				}

				// SelectWindow�폜
				mOpenSelectWindow = false;
				for (int i = 0, size = (int)mSelectWindow.Button.size(); i < size; ++i)
				{
					RiseUI(mSelectWindow.Button[i]);
					RiseText(mSelectWindow.NameText[i]);
				}
			}
		}
	}

}
void ObjectCreate::InspectorWindowInput()
{
	// ----- InspectorWindow -----
	// ----------------------------------- ���͏��� -------------------------------------

	// �{�[�_�[������ł�����mHierarchyWindow�ړ�
	if (mInspectorWindow.mGetRightBorder)
	{
		mInspectorWindow.Border.position.x = mMousePosX;
		// (g_window_width-3)�`(g_window_width-303)�̈ړ�������t����
		if (mInspectorWindow.Border.position.x < (float)(g_window_width - 303))
		{
			mInspectorWindow.Border.position.x = (float)(g_window_width - 303);
		}
		if (mInspectorWindow.Border.position.x >= (float)(g_window_width - 3))
		{
			mInspectorWindow.Border.position.x = (float)(g_window_width - 3);
		}
		// �{�[�_�[�̕�(�R)+�{�[�_�[�Ƃ̏����ʒu�̍�(150)�𑫂��B
		mInspectorWindow.Window.position.x = mMousePosX + 153;
		// (g_window_width - 150)�`(g_window_width + 150)�̈ړ�������t����
		if (mInspectorWindow.Window.position.x < (float)(g_window_width - mInspectorWindow.Window.right))
		{
			mInspectorWindow.Window.position.x = (float)(g_window_width - mInspectorWindow.Window.right);
		}
		if (mInspectorWindow.Window.position.x >= (float)(g_window_width + mInspectorWindow.Window.right))
		{
			mInspectorWindow.Window.position.x = (float)(g_window_width + mInspectorWindow.Window.right);
		}
	}
	if (mInspectorWindow.mPull)
	{
		// �ő喘������Ă��Ȃ�������ő喘���������łȂ�������window�����
		if (mInspectorWindow.Border.position.x != (float)(g_window_width - 303))
		{
			mInspectorWindow.Border.position.x = (float)(g_window_width - 303);
			mInspectorWindow.Window.position.x = (float)(g_window_width - mInspectorWindow.Window.right);
		}
		else
		{
			mInspectorWindow.Border.position.x = (float)(g_window_width - 3);
			mInspectorWindow.Window.position.x = (float)(g_window_width + mInspectorWindow.Window.right);
		}
		mInspectorWindow.mPull = false;
	}
	// Border��x������PullButton�̔��������E�ɂ��炷�B
	mInspectorWindow.PullButton.position.x = mInspectorWindow.Border.position.x + mInspectorWindow.Border.left + mInspectorWindow.PullButton.left;

	// �}�E�X�J�[�\�����E�̃E�B���h�E�ɏd�Ȃ���������
	if (mMousePosX >= mInspectorWindow.Window.position.x + mInspectorWindow.Window.left && mMousePosX <= mClientWidth
		&& mMousePosY >= 0 && mMousePosY <= mClientHeight)
	{
		mInspectorWindow.mPutOnIns = true;
	}
	else
	{
		mInspectorWindow.mPutOnIns = false;
	}

	// �}�E�X�J�[�\�����{�[�_�[�ɏd�Ȃ��Ă�����
	if (mMousePosX >= (mInspectorWindow.Border.position.x + mInspectorWindow.Border.left) && mMousePosX <= (mInspectorWindow.Border.position.x + mInspectorWindow.Border.right)
		&& mMousePosY >= 0 && mMousePosY <= mClientHeight)
	{
		// ���N���b�N�Ń{�[�_�[������
		if (MOUSE_ENTER(0))
		{
			mInspectorWindow.mGetRightBorder = true;
		}
	}
	// pullButton����������
	if (RectOnMouse(mInspectorWindow.PullButton))
	{
		// ���N���b�N�Ń{�[�_�[������
		if (MOUSE_ENTER(0))
		{
			mInspectorWindow.mPull = true;
		}
	}

	// �ǂꂩ�{�^�����I�΂�Ă�����
	if (mSelectButton)
	{
		// ================ TopFrame�̏��� ==========================
		// ----------------------------------- �\���ʒu -------------------------------------
		// UI
		mInspectorWindow.TopFrame.Frame.position.x = mInspectorWindow.Window.position.x;
		mInspectorWindow.TopFrame.Frame.position.y = 25;
		mInspectorWindow.TopFrame.NameBox.Box.position = mInspectorWindow.TopFrame.Frame.position + VECTOR2(50, 0);
		mInspectorWindow.TopFrame.XButton.position = mInspectorWindow.TopFrame.Frame.position + VECTOR2(-125, 0);
		mInspectorWindow.TopFrame.ColorBlue.position = mInspectorWindow.TopFrame.Frame.position + VECTOR2(-75, 0);
		mInspectorWindow.TopFrame.ColorRed.position = mInspectorWindow.TopFrame.Frame.position + VECTOR2(-75, 0);
		mInspectorWindow.TopFrame.ColorGreen.position = mInspectorWindow.TopFrame.Frame.position + VECTOR2(-75, 0);
		// ----------------------------------- ���͏��� -------------------------------------
		// �t���[���F�ύX
		if (RectOnMouse(mInspectorWindow.TopFrame.ColorBlue))
		{
			if (MOUSE_ENTER(0))
			{
				// ��U�폜
				if (mSelectButton->ColorNum == 0)
				{
					RiseUI(mInspectorWindow.TopFrame.ColorBlue);
				}
				else if (mSelectButton->ColorNum == 1)
				{
					RiseUI(mInspectorWindow.TopFrame.ColorRed);
				}
				else if (mSelectButton->ColorNum == 2)
				{
					RiseUI(mInspectorWindow.TopFrame.ColorGreen);
				}
				mSelectButton->ColorNum++;
				if (mSelectButton->ColorNum >= mColorNumMax)
				{
					mSelectButton->ColorNum = 0;
				}
			}
		}
		TextBoxSelect(mInspectorWindow.TopFrame.NameBox);
		if (mInspectorWindow.TopFrame.NameBox.Select)
		{
			swprintf(mSelectButton->Text.Text, MAX_PATH, Input(mSelectButton->Text.Text).c_str());
		}
		//Text
		// �ʒu���������炷
		CreativeUI NameBox;
		NameBox = mInspectorWindow.TopFrame.NameBox.Box;
		NameBox.position += VECTOR2(0, 10);

		mInspectorWindow.TopFrame.NameBox.Text.position = TextPos(mInspectorWindow.TopFrame.NameBox.Text, NameBox);
		swprintf(mInspectorWindow.TopFrame.NameBox.Text.Text, MAX_PATH, mSelectButton->Text.Text);


		// Actor�폜�{�^��
		if (RectOnMouse(mInspectorWindow.TopFrame.XButton))
		{
			// �~�{�^������������
			if (MOUSE_ENTER(0))
			{
				DeleteObj();

				// �����ꂽ�{�^���̎��̃{�^����I������
				if (mHierarchyWindow.m2D)
				{
					if ((int)mHierarchyWindow.Button2D.size() != mSelectButtonNum)
					{
						mSelectButton = &mHierarchyWindow.Button2D[mSelectButtonNum];
					}
					//�@�{�^�����Ȃ��̂�nullptr
					else
					{
						mSelectButton = nullptr;
					}
				}
				else
				{
					if ((int)mHierarchyWindow.Button3D.size() != mSelectButtonNum)
					{
						mSelectButton = &mHierarchyWindow.Button3D[mSelectButtonNum];
					}
					//�@�{�^�����Ȃ��̂�nullptr
					else
					{
						mSelectButton = nullptr;
					}
				}
				// �����l�ɖ߂�
				mPreviousSelectButton = nullptr;
			}
		}
	}

	// �~�{�^����������Ă����珈�����������Ȃ��̂ň�U�����Ă܂�����
	if (mSelectButton)
	{
		// ================ TransformComponent�̏��� ==========================
		// ----------------------------------- �\���ʒu -------------------------------------
		// ----- Top -----

		mInspectorWindow.Transform.Top.position = InspectorFramePos(mInspectorWindow.Transform.Top, mInspectorWindow.TopFrame.Frame);
		// ----- Bottom -----
		// TransformTop�t���[���ʒu����Ƃ��Ĉʒu���o��
		// �����͎����̍���/2����TopFrame�̍���+TransformTop�t���[���̍���/2
		mInspectorWindow.Transform.Bottom.position = InspectorFramePos(mInspectorWindow.Transform.Bottom, mInspectorWindow.Transform.Top);
		// ----- RightArrow -----
		// TransformTop�t���[���ʒu����Ƃ��Ĉʒu���o��
		// 135��TransformTop�t���[���̒[�Ɉړ�������������
		mInspectorWindow.Transform.RightArrow.position = mInspectorWindow.Transform.Top.position + VECTOR2(-135, 0);
		// ----- DownArrow -----
		// RightArrow�Ɠ���
		mInspectorWindow.Transform.DownArrow.position = mInspectorWindow.Transform.RightArrow.position;
		// ----- TextBox -----
		// TransformBottom�t���[���ʒu����Ƃ��Ĉʒu���o��
		// 3��xyz
		for (int i = 0; i < 3; i++)
		{
			if (i == 0)
			{
				mInspectorWindow.Transform.Position[i].Box.position = mInspectorWindow.Transform.Bottom.position + VECTOR2(-42, -50);
				mInspectorWindow.Transform.Rotation[i].Box.position = mInspectorWindow.Transform.Bottom.position + VECTOR2(-42, -10);
				mInspectorWindow.Transform.Scale[i].Box.position = mInspectorWindow.Transform.Bottom.position + VECTOR2(-42, 30);
			}
			if (i == 1)
			{
				mInspectorWindow.Transform.Position[i].Box.position = mInspectorWindow.Transform.Bottom.position + VECTOR2(35, -50);
				mInspectorWindow.Transform.Rotation[i].Box.position = mInspectorWindow.Transform.Bottom.position + VECTOR2(35, -10);
				mInspectorWindow.Transform.Scale[i].Box.position = mInspectorWindow.Transform.Bottom.position + VECTOR2(35, 30);
			}
			if (i == 2)
			{
				mInspectorWindow.Transform.Position[i].Box.position = mInspectorWindow.Transform.Bottom.position + VECTOR2(115, -50);
				mInspectorWindow.Transform.Rotation[i].Box.position = mInspectorWindow.Transform.Bottom.position + VECTOR2(115, -10);
				mInspectorWindow.Transform.Scale[i].Box.position = mInspectorWindow.Transform.Bottom.position + VECTOR2(115, 30);
			}

			// �����Ă���̂�TextPos������񂹂��邽��
			mInspectorWindow.Transform.Position[i].Text.Text;
			// �������Ă킩��Â炢������Ă��邱�Ƃ͏�ňʒu�����߂�Box��Box��left*scale.x��top*scale.y�𑫂��Ă���B
			mInspectorWindow.Transform.Position[i].Text.position = TextPos(mInspectorWindow.Transform.Position[i].Text, mInspectorWindow.Transform.Position[i].Box);
			mInspectorWindow.Transform.Rotation[i].Text.position = TextPos(mInspectorWindow.Transform.Rotation[i].Text, mInspectorWindow.Transform.Rotation[i].Box);
			mInspectorWindow.Transform.Scale[i].Text.position = TextPos(mInspectorWindow.Transform.Scale[i].Text, mInspectorWindow.Transform.Scale[i].Box);
		}

		// ----------------------------------- ���͏��� -------------------------------------

		if (mInspectorWindow.Transform.Open)
		{
			if (RectOnMouse(mInspectorWindow.Transform.DownArrow))
			{
				if (MOUSE_ENTER(0))
				{
					// �J���Ă�Ƃ��������N���b�N�����Open=false
					mInspectorWindow.Transform.Open = false;
				}
			}
		}
		else
		{
			if (RectOnMouse(mInspectorWindow.Transform.RightArrow))
			{
				if (MOUSE_ENTER(0))
				{
					// �J���Ă�Ƃ��E�����N���b�N�����Open=true
					mInspectorWindow.Transform.Open = true;
				}
			}
		}
		// ���ݑI������Ă���Actor
		Actor* selectActor = mActor[mSelectActorNum];
		// ----- TextBox�ɐ��l���� -----
		if (mHierarchyWindow.m2D)
		{
			// Position
			TextBoxSelect(mInspectorWindow.Transform.Position[0]);
			TextBoxSelect(mInspectorWindow.Transform.Position[1]);

			// Rotation
			TextBoxSelect(mInspectorWindow.Transform.Rotation[2]);

			// Scale
			TextBoxSelect(mInspectorWindow.Transform.Scale[0]);
			TextBoxSelect(mInspectorWindow.Transform.Scale[1]);


			// ���͑O�Ɍ��݈ʒu����ꍞ��

			float getPos[2] = { selectActor->GetPosition().x,selectActor->GetPosition().y };
			float getRote = selectActor->GetRotation().z;
			float getScale[2] = { selectActor->GetScale().x,selectActor->GetScale().y };

			TextBoxInput(mInspectorWindow.Transform.Position[0], getPos[0]);
			TextBoxInput(mInspectorWindow.Transform.Position[1], getPos[1]);

			TextBoxInput(mInspectorWindow.Transform.Rotation[2], getRote);

			TextBoxInput(mInspectorWindow.Transform.Scale[0], getScale[0]);
			TextBoxInput(mInspectorWindow.Transform.Scale[1], getScale[1]);

			selectActor->SetPosition(VECTOR(getPos[0], getPos[1], 0));
			selectActor->SetRotation(VECTOR(0, 0, getRote));
			selectActor->SetScale(VECTOR(getScale[0], getScale[1], 0));
		}
		else
		{
			for (int i = 0; i < 3; i++)
			{
				// Position
				TextBoxSelect(mInspectorWindow.Transform.Position[i]);

				// Rotation
				TextBoxSelect(mInspectorWindow.Transform.Rotation[i]);

				// Scale
				TextBoxSelect(mInspectorWindow.Transform.Scale[i]);
			}

			// ���͑O�Ɍ��݈ʒu����ꍞ��
			// ���ݑI������Ă���Actor
			float getPos[3] = { selectActor->GetPosition().x,selectActor->GetPosition().y,selectActor->GetPosition().z };
			float getRote[3] = { selectActor->GetRotation().x,selectActor->GetRotation().y,selectActor->GetRotation().z };
			float getScale[3] = { selectActor->GetScale().x,selectActor->GetScale().y,selectActor->GetScale().z };

			// ���͂������l��߂�
			for (int i = 0; i < 3; i++)
			{
				// Position
				TextBoxInput(mInspectorWindow.Transform.Position[i], getPos[i]);
				// Rotation
				TextBoxInput(mInspectorWindow.Transform.Rotation[i], getRote[i]);
				// Scale
				TextBoxInput(mInspectorWindow.Transform.Scale[i], getScale[i]);
			}

			selectActor->SetPosition(VECTOR(getPos[0], getPos[1], getPos[2]));
			selectActor->SetRotation(VECTOR(getRote[0], getRote[1], getRote[2]));
			selectActor->SetScale(VECTOR(getScale[0], getScale[1], getScale[2]));
		}

		// ================ Color�̏��� ==========================
		// ----------------------------------- �\���ʒu -------------------------------------
		// �I�𒆂̃{�^����Mesh�Ȃ�ColorFrame�\��
		bool flg = false;		// Mesh���ǂ���
		for (auto mesh : mMeshName)
		{
			if (mSelectButton->ObjType._Equal(mesh))
			{
				flg = true;
				break;
			}
		}

		if (flg)
		{
			mInspectorWindow.mColorFrameFlag = true;
			if (mInspectorWindow.Transform.Open)
			{
				mInspectorWindow.ColorFrame.Frame.position = InspectorFramePos(mInspectorWindow.ColorFrame.Frame, mInspectorWindow.Transform.Bottom);
			}
			else
			{
				mInspectorWindow.ColorFrame.Frame.position = InspectorFramePos(mInspectorWindow.ColorFrame.Frame, mInspectorWindow.Transform.Top);
			}
			mInspectorWindow.ColorFrame.rgb[0].Box.position = mInspectorWindow.ColorFrame.Frame.position + VECTOR2(-42, 0);
			mInspectorWindow.ColorFrame.rgb[1].Box.position = mInspectorWindow.ColorFrame.Frame.position + VECTOR2(35, 0);
			mInspectorWindow.ColorFrame.rgb[2].Box.position = mInspectorWindow.ColorFrame.Frame.position + VECTOR2(115, 0);
			// 3��rgb
			for (int i = 0; i < 3; i++)
			{
				mInspectorWindow.ColorFrame.rgb[i].Text.position = TextPos(mInspectorWindow.ColorFrame.rgb[i].Text, mInspectorWindow.ColorFrame.rgb[i].Box);
			}

			// ----------------------------------- ���͏��� -------------------------------------
			// 3��rgb
			for (int i = 0; i < 3; i++)
			{
				TextBoxSelect(mInspectorWindow.ColorFrame.rgb[i]);
			}

			// ���͑O�Ɍ��݈ʒu����ꍞ��
			float getColor[3] = { selectActor->GetColor().x,selectActor->GetColor().y,selectActor->GetColor().z };
			float num = 0;
			for (int i = 0; i < 3; i++)
			{
				getColor[i] *= 255;
				// ���͒��͍X�V���Ȃ�
				if (!mInspectorWindow.ColorFrame.rgb[i].Select)
				{
					swprintf(mInspectorWindow.ColorFrame.rgb[i].Text.Text, MAX_PATH, _T("%d"), (int)getColor[i]);
				}
			}
			// ����
			for (int i = 0; i < 3; i++)
			{
				if (mInspectorWindow.ColorFrame.rgb[i].Select)
				{
					std::wstring Text = Input(mInspectorWindow.ColorFrame.rgb[i].Text.Text, true, 3).c_str();
					int l = 0;
					int size = (int)Text.size();
					for (; l < size; ++l)
					{
						if (Text[l] != 48)
						{
							break;
						}
					}
					if (l != 0 && size != l)
					{
						for (int k = 0; k < size - 1; ++k)
						{
							// �����ԍ��ȏ�̃{�^������������炷
							if (k >= l - 1)
							{
								int NextButton = k + 1;
								int num = k - (l - 1);
								Text[num] = Text[NextButton];
							}
						}
						int num = size - l;
						Text.resize(num);
					}
					if (size > 0)
					{
						swprintf(mInspectorWindow.ColorFrame.rgb[i].Text.Text, MAX_PATH, Text.c_str());
					}
					else
					{
						swprintf(mInspectorWindow.ColorFrame.rgb[i].Text.Text, MAX_PATH, _T("0"));
					}
					break;// ������Ȃ̂�break
				}
			}
			// ���͂������l��߂�
			for (int i = 0; i < 3; i++)
			{
				swscanf_s(mInspectorWindow.ColorFrame.rgb[i].Text.Text, _T("%f"), &getColor[i]);
				// rgb��0�`255
				if (getColor[i] > 255)
				{
					getColor[i] = 255;
				}
				else if (getColor[i] < 0)
				{
					getColor[i] = 0;
				}
				getColor[i] /= 255;
			}
			selectActor->SetColor(VECTOR(getColor[0], getColor[1], getColor[2]));
		}
		else
		{
			mInspectorWindow.mColorFrameFlag = false;
		}

		if (mTab)
		{
			if (mInspectorWindow.TopFrame.NameBox.Select)
			{
				TabClickInput(mInspectorWindow.TopFrame.NameBox, mInspectorWindow.Transform.Position[0]);
			}
			else
			{
				if (mHierarchyWindow.m2D)
				{
					if (mInspectorWindow.Transform.Position[0].Select)
					{
						TabClickInput(mInspectorWindow.Transform.Position[0], mInspectorWindow.Transform.Position[1]);
					}
					else if (mInspectorWindow.Transform.Position[1].Select)
					{
						TabClickInput(mInspectorWindow.Transform.Position[1], mInspectorWindow.Transform.Rotation[2]);
					}
					else if (mInspectorWindow.Transform.Rotation[2].Select)
					{
						TabClickInput(mInspectorWindow.Transform.Rotation[2], mInspectorWindow.Transform.Scale[0]);
					}
					else if (mInspectorWindow.Transform.Scale[0].Select)
					{
						TabClickInput(mInspectorWindow.Transform.Scale[0], mInspectorWindow.Transform.Scale[1]);
					}
				}
				else
				{
					for (int i = 0; i < 3; i++)
					{
						// Position
						if (mInspectorWindow.Transform.Position[i].Select)
						{
							if (i == 2)
							{
								TabClickInput(mInspectorWindow.Transform.Position[i], mInspectorWindow.Transform.Rotation[0]);
								break;
							}
							else
							{
								int num = i + 1;
								TabClickInput(mInspectorWindow.Transform.Position[i], mInspectorWindow.Transform.Position[num]);
								break;
							}
						}

						// Rotation
						if (mInspectorWindow.Transform.Rotation[i].Select)
						{
							if (i == 2)
							{
								TabClickInput(mInspectorWindow.Transform.Rotation[i], mInspectorWindow.Transform.Scale[0]);
								break;
							}
							else
							{
								int num = i + 1;
								TabClickInput(mInspectorWindow.Transform.Rotation[i], mInspectorWindow.Transform.Rotation[num]);
								break;
							}
						}

						// Scale
						if (mInspectorWindow.Transform.Scale[i].Select)
						{
							if (i != 2)
							{
								int num = i + 1;
								TabClickInput(mInspectorWindow.Transform.Scale[i], mInspectorWindow.Transform.Scale[num]);
								break;
							}
						}
					}
				}
			}
			mTab = false;
		}

		// ================ ComponentWindow�̏��� ======================
		if (mPreviousSelectButton != mSelectButton && mSelectButton)
		{
			// �O��񏉊���
			for (int i = 0; i < mSelectComponent.size(); i++)
			{
				RiseUI(mSelectComponent[i]->Top);
				RiseText(mSelectComponent[i]->Title);
				RiseUI(mSelectComponent[i]->Bottom);
				RiseUI(mSelectComponent[i]->DownArrow);
				RiseUI(mSelectComponent[i]->RightArrow);
				RiseUI(mSelectComponent[i]->CheckBoxOn);
				RiseUI(mSelectComponent[i]->CheckBoxOff);
				RiseUI(mSelectComponent[i]->XButton);
				mSelectComponent[i]->RiseDraw();
				delete mSelectComponent[i];
			}
			mSelectComponent.clear();
			mSelectComponentNum = 0;
			for (auto uc : mUseComponent)
			{
				// �I������Ă���R���|�[�l���g�� - 1
				int PreviousNum = (int)mSelectComponent.size() - 1;
				// �������ꂽ���ǂ���
				bool flg = false;
				if (uc._Equal(_T("PlayerInput")))
				{
					if (selectActor->GetComponent(uc))
					{
						mSelectComponent.push_back(new InputUI);
						mSelectComponent.back()->Type = uc;
						mSelectComponentNum++;
						flg = true;
					}
				}
				else if (uc._Equal(_T("MeshComponent")))
				{
					if (selectActor->GetComponent(uc))
					{
						mSelectComponent.push_back(new MeshUI);
						mSelectComponent.back()->Type = uc;
						mSelectComponentNum++;
						flg = true;
					}
				}
				else if (uc._Equal(_T("ModelComponent")))
				{
					if (selectActor->GetComponent(uc))
					{
						mSelectComponent.push_back(new ModelUI);
						mSelectComponent.back()->Type = uc;
						mSelectComponentNum++;
						flg = true;
					}
				}
				else if (uc._Equal(_T("BoxCollider")))
				{
					if (selectActor->GetComponent(uc))
					{
						mSelectComponent.push_back(new BoxColliderUI);
						mSelectComponent.back()->Type = uc;
						mSelectComponentNum++;
						flg = true;
					}
				}
				else if (uc._Equal(_T("CylinderCollider")))
				{

				}
				else if (uc._Equal(_T("CapsuleCollider")))
				{
					if (selectActor->GetComponent(uc))
					{
						mSelectComponent.push_back(new CapsuleColliderUI);
						mSelectComponent.back()->Type = uc;
						mSelectComponentNum++;
						flg = true;
					}
				}
				else if (uc._Equal(_T("RigidBody")))
				{
					if (selectActor->GetComponent(uc))
					{
						mSelectComponent.push_back(new RigidBodyUI);
						mSelectComponent.back()->Type = uc;
						mSelectComponentNum++;
						flg = true;
					}
				}
				else if (uc._Equal(_T("ImageComponent")))
				{
					int i = 1;
					for (auto comp : selectActor->GetImageComponent())
					{
						mSelectComponent.push_back(new ImageUI);
						mSelectComponent.back()->Type = uc;
						mSelectComponent.back()->ComponentNum = i;
						mSelectComponentNum++;
						flg = true;
						++i;
						if (mSelectComponent.size() != 0 && flg)
						{
							mSelectComponent.back()->SetMetaData();
							if (PreviousNum == -1)
							{
								if (mInspectorWindow.mColorFrameFlag)
								{
									mSelectComponent.back()->basePos = InspectorFramePos(mSelectComponent.back()->Top, mInspectorWindow.ColorFrame.Frame);
								}
								else
								{
									mSelectComponent.back()->basePos = InspectorFramePos(mSelectComponent.back()->Top, mInspectorWindow.Transform.Bottom);
								}
							}
							else
							{
								if (mSelectComponent[PreviousNum]->Bottom.fileName)
								{
									// Bottom������Ƃ�
									mSelectComponent.back()->basePos = InspectorFramePos(mSelectComponent.back()->Top, mSelectComponent[PreviousNum]->Bottom);
								}
								else
								{
									// Bottom���Ȃ��Ƃ�
									mSelectComponent.back()->basePos = InspectorFramePos(mSelectComponent.back()->Top, mSelectComponent[PreviousNum]->Top);
								}
							}
						}
						flg = false;
					}
				}
				else
				{
					if (selectActor->GetComponent(uc))
					{
						mSelectComponent.push_back(new ScriptUI);
						mSelectComponent.back()->Type = uc;
						swprintf(mSelectComponent.back()->Title.Text, MAX_PATH, uc.c_str());
						mSelectComponentNum++;
						flg = true;
					}
				}
				if (mSelectComponent.size() != 0 && flg)
				{
					mSelectComponent.back()->SetMetaData();
					if (PreviousNum == -1)
					{
						if (mInspectorWindow.mColorFrameFlag)
						{
							mSelectComponent.back()->basePos = InspectorFramePos(mSelectComponent.back()->Top, mInspectorWindow.ColorFrame.Frame);
						}
						else
						{
							mSelectComponent.back()->basePos = InspectorFramePos(mSelectComponent.back()->Top, mInspectorWindow.Transform.Bottom);
						}
					}
					else
					{
						if (mSelectComponent[PreviousNum]->Bottom.fileName)
						{
							// Bottom������Ƃ�
							mSelectComponent.back()->basePos = InspectorFramePos(mSelectComponent.back()->Top, mSelectComponent[PreviousNum]->Bottom);
						}
						else
						{
							// Bottom���Ȃ��Ƃ�
							mSelectComponent.back()->basePos = InspectorFramePos(mSelectComponent.back()->Top, mSelectComponent[PreviousNum]->Top);
						}
					}
				}
			}

			// SelectWindow������
			mOpenSelectWindow = false;
			for (int i = 0, size = (int)mSelectWindow.Button.size(); i < size; ++i)
			{
				RiseUI(mSelectWindow.Button[i]);
				RiseText(mSelectWindow.NameText[i]);
			}

			// �O�t���[���̍X�V
			mPreviousSelectButton = mSelectButton;
		}
		// ----------------------------------- �\���ʒu -------------------------------------
		// �O�t���[���ɑI�����Ă����{�^���ƈႤ�Ȃ�I�𒆂̃{�^�����g����R���|�[�l���g������

		VECTOR2 PrevPos;
		if (mInspectorWindow.mColorFrameFlag)
		{
			PrevPos = mInspectorWindow.ColorFrame.Frame.position + VECTOR2(0, mInspectorWindow.ColorFrame.Frame.bottom);
		}
		else
		{
			if (mInspectorWindow.Transform.Open)
			{
				PrevPos = mInspectorWindow.Transform.Bottom.position + VECTOR2(0, mInspectorWindow.Transform.Bottom.bottom);
			}
			else
			{
				PrevPos = mInspectorWindow.Transform.Top.position + VECTOR2(0, mInspectorWindow.Transform.Top.bottom);
			}
		}
		for (int i = 0; i < mSelectComponent.size(); i++)
		{
			// ----- Top -----
			mSelectComponent[i]->Top.position = PrevPos + VECTOR2(0, mSelectComponent[i]->Top.bottom);
			// Bottom������Ƃ�
			if (mSelectComponent[i]->Bottom.fileName)
			{
				// ----- Bottom -----
				mSelectComponent[i]->Bottom.position = InspectorFramePos(mSelectComponent[i]->Bottom, mSelectComponent[i]->Top);

				// ----- RightArrow -----
				// 135��Top�t���[���̒[�Ɉړ�������������
				mSelectComponent[i]->RightArrow.position = mSelectComponent[i]->Top.position + VECTOR2(-135, 0);
				// ----- DownArrow -----
				// RightArrow�Ɠ���
				mSelectComponent[i]->DownArrow.position = mSelectComponent[i]->RightArrow.position;
			}
			if (mSelectComponent[i]->Open)
			{
				PrevPos = mSelectComponent[i]->Bottom.position + VECTOR2(0, mSelectComponent[i]->Bottom.bottom * mSelectComponent[i]->Bottom.scale.y);
			}
			else
			{
				PrevPos = mSelectComponent[i]->Top.position + VECTOR2(0, mSelectComponent[i]->Top.bottom * mSelectComponent[i]->Top.scale.y);
			}
			// ----- XButton -----
			mSelectComponent[i]->XButton.position = mSelectComponent[i]->Top.position + VECTOR2(-110, 0);
			// ----- CheckBox(On) -----
			// 115��Top�t���[���̒[�Ɉړ�������������
			mSelectComponent[i]->CheckBoxOn.position = mSelectComponent[i]->Top.position + VECTOR2(-85, 0);
			// ----- CheckBox(Off) -----
			// RightArrow�Ɠ���
			mSelectComponent[i]->CheckBoxOff.position = mSelectComponent[i]->CheckBoxOn.position;
			// ----- Title -----
			mSelectComponent[i]->Title.position = mSelectComponent[i]->Top.position + VECTOR2(-85, -10);

			mSelectComponent[i]->SetPos();
		}

		// ----------------------------------- ���͏��� -------------------------------------
		for (int i = 0; i < mSelectComponent.size(); i++)
		{
			if (mSelectComponent[i]->Open)
			{
				if (RectOnMouse(mSelectComponent[i]->DownArrow))
				{
					if (MOUSE_ENTER(0))
					{
						// �J���Ă�Ƃ��������N���b�N�����Open=false
						mSelectComponent[i]->Open = false;
					}
				}
			}
			else
			{
				if (RectOnMouse(mSelectComponent[i]->RightArrow))
				{
					if (MOUSE_ENTER(0))
					{
						// �J���Ă�Ƃ��E�����N���b�N�����Open=true
						mSelectComponent[i]->Open = true;
					}
				}
			}

			if (mSelectComponent[i]->Check)
			{
				if (RectOnMouse(mSelectComponent[i]->CheckBoxOn))
				{
					if (MOUSE_ENTER(0))
					{
						// �J���Ă�Ƃ��������N���b�N�����Open=false
						mSelectComponent[i]->Check = false;
					}
				}
			}
			else
			{
				if (RectOnMouse(mSelectComponent[i]->CheckBoxOff))
				{
					if (MOUSE_ENTER(0))
					{
						// �J���Ă�Ƃ��E�����N���b�N�����Open=true
						mSelectComponent[i]->Check = true;
					}
				}
			}

			InputComponentData(mSelectComponent[i]);

			if (RectOnMouse(mSelectComponent[i]->XButton))
			{
				// �~�{�^������������
				if (MOUSE_ENTER(0))
				{
					DeleteComponent(i);
				}
			}
		}
	}
}

void ObjectCreate::AddObjWindowInput()
{
	// ----- AddObjWindow -----
	// ----------------------------------- �\���ʒu -------------------------------------
	// +����Ă���͍̂��W����������邽��
	mAddObjWindow.AddObjectButton.position = VECTOR2(mHierarchyWindow.Border.position.x + 17, g_window_height - mAddObjWindow.AddObjectButton.bottom);
	// MwshWindow
	mAddObjWindow.MeshText.position = mAddObjWindow.MeshWindow.position + VECTOR2(-75, -125);
	mAddObjWindow.BOXButton.position = mAddObjWindow.MeshWindow.position + VECTOR2(0, -80);
	mAddObjWindow.BOXButtonOff.position = mAddObjWindow.BOXButton.position;
	mAddObjWindow.CylinderButton.position = mAddObjWindow.MeshWindow.position + VECTOR2(0, -40);
	mAddObjWindow.CylinderButtonOff.position = mAddObjWindow.CylinderButton.position;
	mAddObjWindow.SphereButton.position = mAddObjWindow.MeshWindow.position + VECTOR2(0, 0);
	mAddObjWindow.SphereButtonOff.position = mAddObjWindow.SphereButton.position;
	// ModelWindow
	mAddObjWindow.ModelText.position = mAddObjWindow.ModelWindow.position + VECTOR2(0, -123);
	for (int i = 0, size = (int)mAddObjWindow.ModelButton.size(); i < size; ++i)
	{
		int num = i % 5;
		mAddObjWindow.ModelButton[i].position = mAddObjWindow.MeshWindow.position + VECTOR2(0, (float)num * 40 - 80);

		mAddObjWindow.ModelNameText[i].position = mAddObjWindow.ModelButton[i].position + VECTOR2(-75, 0);
	}
	// ImagWindow
	for (int i = 0, size = (int)mAddObjWindow.ImageButton.size(); i < size; ++i)
	{
		int num = i % 5;
		mAddObjWindow.ImageButton[i].position = mAddObjWindow.ImageWindow.position + VECTOR2(0, (float)num * 40 - 80);

		mAddObjWindow.ImageNameText[i].position = mAddObjWindow.ImageButton[i].position + VECTOR2(-75, 0);
	}

	mAddObjWindow.LeftArrow.position = mAddObjWindow.MeshWindow.position + VECTOR2(-50, 110);
	mAddObjWindow.RightArrow.position = mAddObjWindow.MeshWindow.position + VECTOR2( 50, 110);
	mAddObjWindow.PageNumText.position = mAddObjWindow.MeshWindow.position + VECTOR2(-40, 100);
	// ----------------------------------- ���͏��� -------------------------------------
	// AddObjButton�ɏd�Ȃ��Ă���Ƃ�
	if (RectOnMouse(mAddObjWindow.AddObjectButton))
	{
		// �E�N���b�N����������
		if (MOUSE_ENTER(0))
		{
			// �~�j�E�B���h�E���o��
			mAddObjWindow.OpenWindow = true;
		}
	}
	// �}�E�X�J�[�\�����E�̃E�B���h�E�ɏd�Ȃ���������
	if (RectOnMouse(mAddObjWindow.MeshWindow) || RectOnMouse(mAddObjWindow.ModelWindow) || RectOnMouse(mAddObjWindow.ImageWindow))
	{
		mAddObjWindow.mPutOnAddOW = true;
	}
	else
	{
		mAddObjWindow.mPutOnAddOW = false;
	}
	if (mAddObjWindow.OpenWindow)
	{
		mAddObjWindow.MeshWindow.position = VECTOR2(mHierarchyWindow.Border.position.x + 77, g_window_height - mAddObjWindow.MeshWindow.bottom);
		mAddObjWindow.ModelWindow.position = VECTOR2(mHierarchyWindow.Border.position.x + 77, g_window_height - mAddObjWindow.ModelWindow.bottom);
		mAddObjWindow.ImageWindow.position = VECTOR2(mHierarchyWindow.Border.position.x + 77, g_window_height - mAddObjWindow.ImageWindow.bottom);
		// �^�u�N���b�N(25�̓^�u�̍���)
		if (mMousePosX >= (mAddObjWindow.MeshWindow.position.x + mAddObjWindow.MeshWindow.left) && mMousePosX <= (mAddObjWindow.MeshWindow.position.x)
			&& mMousePosY >= (mAddObjWindow.MeshWindow.position.y + mAddObjWindow.MeshWindow.top) && mMousePosY <= (mAddObjWindow.MeshWindow.position.y + mAddObjWindow.MeshWindow.top + 25))
		{
			if (MOUSE_ENTER(0))
			{
				RiseUI(mAddObjWindow.ModelWindow);
				for (int i = 0, size = (int)mAddObjWindow.ModelButton.size(); i < size; ++i)
				{
					RiseUI(mAddObjWindow.ModelButton[i]);
				}
				for (int i = 0, size = (int)mAddObjWindow.ModelNameText.size(); i < size; ++i)
				{
					RiseText(mAddObjWindow.ModelNameText[i]);
				}
				mAddObjWindow.MeshOrModelWindow = false;
			}
		}
		else if (mMousePosX >= (mAddObjWindow.ModelWindow.position.x) && mMousePosX <= (mAddObjWindow.ModelWindow.position.x + mAddObjWindow.ModelWindow.right)
			&& mMousePosY >= (mAddObjWindow.ModelWindow.position.y + mAddObjWindow.ModelWindow.top) && mMousePosY <= (mAddObjWindow.ModelWindow.position.y + mAddObjWindow.ModelWindow.top + 25))
		{
			if (MOUSE_ENTER(0))
			{
				RiseUI(mAddObjWindow.MeshWindow);
				RiseUI(mAddObjWindow.BOXButtonOff);
				RiseUI(mAddObjWindow.CylinderButtonOff);
				RiseUI(mAddObjWindow.SphereButtonOff);
				mAddObjWindow.MeshOrModelWindow = true;
			}
		}

		// 2D���[�h�̎�
		if (mHierarchyWindow.m2D)
		{
			// ���ꂼ��̃{�^���ɃJ�[�\�����G��Ă���Ƃ����N���b�N��On
			for (int i = (mAddObjWindow.PageNumber - 1) * 5, k = 0, size = (int)mAddObjWindow.ImageButton.size(); i < size; ++i, ++k)
			{
				if (k == 5)
				{
					break;
				}
				if (RectOnMouse(mAddObjWindow.ImageButton[i]))
				{
					if (MOUSE_ENTER(0))
					{
						mAddObjWindow.ClickImageButton[i] = true;
						AddObj(mSimpleImageName[i].c_str());
						AddComponent(mActor.back(), _T("ImageComponent"), mFolderImageName[i].c_str());
					}
				}
			}

			for (int i = (mAddObjWindow.PageNumber - 1) * 5, k = 0, size = (int)mAddObjWindow.ImageButton.size(); i < size; ++i, ++k)
			{
				if (k == 5)
				{
					break;
				}
				if (!RectOnMouse(mAddObjWindow.ImageButton[i]) || MOUSE_EXIT(0))
				{
					mAddObjWindow.ClickImageButton[i] = false;
				}
			}
		}
		// 3D���[�h�̎�
		else
		{
			// MeshWindow���J����Ă���Ƃ�
			if (!mAddObjWindow.MeshOrModelWindow)
			{
				// ���ꂼ��̃{�^���ɃJ�[�\�����G��Ă���Ƃ����N���b�N��On
				if (RectOnMouse(mAddObjWindow.BOXButton))
				{
					if (MOUSE_ENTER(0))
					{
						mAddObjWindow.ClickBoxButton = true;
						AddObj(mMeshName[Box]);
						AddComponent(mActor.back(), _T("MeshComponent"));
						mActor.back()->GetMeshComponent()->SetObjectType(ObjectType::Box);
						mActor.back()->GetMeshComponent()->SetObject();
					}
				}
				else if (RectOnMouse(mAddObjWindow.CylinderButton))
				{
					if (MOUSE_ENTER(0))
					{
						mAddObjWindow.ClickCylinderButton = true;
						AddObj(mMeshName[Cylinder]);
						AddComponent(mActor.back(), _T("MeshComponent"));
						mActor.back()->GetMeshComponent()->SetObjectType(ObjectType::Cylinder);
						mActor.back()->GetMeshComponent()->SetObject();
					}
				}
				else if (RectOnMouse(mAddObjWindow.SphereButton))
				{
					if (MOUSE_ENTER(0))
					{
						mAddObjWindow.ClickSphereButton = true;
						AddObj(mMeshName[Sphere]);
						AddComponent(mActor.back(), _T("MeshComponent"));
						mActor.back()->GetMeshComponent()->SetObjectType(ObjectType::Sphere);
						mActor.back()->GetMeshComponent()->SetObject();
					}
				}
			}
			else
			{
				// ���ꂼ��̃{�^���ɃJ�[�\�����G��Ă���Ƃ����N���b�N��On
				for (int i = (mAddObjWindow.PageNumber - 1) * 5, k = 0, size = (int)mAddObjWindow.ModelButton.size(); i < size; ++i, ++k)
				{
					if (k == 5)
					{
						break;
					}
					if (RectOnMouse(mAddObjWindow.ModelButton[i]))
					{
						if (MOUSE_ENTER(0))
						{
							mAddObjWindow.ClickModelButton[i] = true;
							AddObj(mSimpleModelName[i].c_str());
							AddComponent(mActor.back(), _T("ModelComponent"), mFolderModelName[i].c_str());
						}
					}
				}
			}

			if (mAddObjWindow.ClickBoxButton)
			{
				// �J�[�\�����{�^������O��邩�����ĂȂ���Off
				if (!RectOnMouse(mAddObjWindow.BOXButton) || MOUSE_EXIT(0))
				{
					mAddObjWindow.ClickBoxButton = false;
				}
			}

			if (mAddObjWindow.ClickCylinderButton)
			{
				// �J�[�\�����{�^������O��邩�����ĂȂ���Off
				if (!RectOnMouse(mAddObjWindow.CylinderButton) || MOUSE_EXIT(0))
				{
					mAddObjWindow.ClickCylinderButton = false;
				}
			}

			if (mAddObjWindow.ClickSphereButton)
			{
				// �J�[�\�����{�^������O��邩�����ĂȂ���Off
				if (!RectOnMouse(mAddObjWindow.SphereButton) || MOUSE_EXIT(0))
				{
					mAddObjWindow.ClickSphereButton = false;
				}
			}

			for (int i = (mAddObjWindow.PageNumber - 1) * 5, k = 0, size = (int)mAddObjWindow.ModelButton.size(); i < size; ++i, ++k)
			{
				if (k == 5)
				{
					break;
				}
				if (!RectOnMouse(mAddObjWindow.ModelButton[i]) || MOUSE_EXIT(0))
				{

					mAddObjWindow.ClickModelButton[i] = false;
				}
			}
		}

		// ----- �y�[�W���� ------
		if (RectOnMouse(mAddObjWindow.LeftArrow))
		{
			if (MOUSE_ENTER(0))
			{
				mAddObjWindow.PageNumber--;
			}		
		}
		else if (RectOnMouse(mAddObjWindow.RightArrow))
		{
			if (MOUSE_ENTER(0))
			{
				mAddObjWindow.PageNumber++;
			}
		}

		// 2D���[�h�̎�
		if (mHierarchyWindow.m2D)
		{
			if (mAddObjWindow.PageNumber == 0)
			{
				mAddObjWindow.PageNumber = mAddObjWindow.ImagePageNumMax;
			}
			else if (mAddObjWindow.PageNumber > mAddObjWindow.ImagePageNumMax)
			{
				mAddObjWindow.PageNumber = 1;
			}
		}
		// 3D���[�h�̎�
		else
		{
			// MeshWindow���J����Ă���Ƃ�
			if (!mAddObjWindow.MeshOrModelWindow)
			{
				if (mAddObjWindow.PageNumber == 0)
				{
					mAddObjWindow.PageNumber = mAddObjWindow.MeshPageNumMax;
				}
				else if (mAddObjWindow.PageNumber > mAddObjWindow.MeshPageNumMax)
				{
					mAddObjWindow.PageNumber = 1;
				}
			}
			else
			{
				if (mAddObjWindow.PageNumber == 0)
				{
					mAddObjWindow.PageNumber = mAddObjWindow.ModelPageNumMax;
				}
				else if (mAddObjWindow.PageNumber > mAddObjWindow.ModelPageNumMax)
				{
					mAddObjWindow.PageNumber = 1;
				}
			}
		}
		
		if (mAddObjWindow.PageNumber != mAddObjWindow.PrevPageNumber)
		{
			swprintf(mAddObjWindow.PageNumText.Text, MAX_PATH, _T("%d"), mAddObjWindow.PageNumber);

			// �O�y�[�W�̃{�^��������
			// 2D���[�h�̎�
			if (mHierarchyWindow.m2D)
			{
				for (int i = (mAddObjWindow.PrevPageNumber - 1) * 5, k = 0, size = (int)mAddObjWindow.ImageButton.size(); i < size; ++i, ++k)
				{
					if (k == 5)
					{
						break;
					}
					RiseUI(mAddObjWindow.ImageButton[i]);
					RiseText(mAddObjWindow.ImageNameText[i]);
				}
			}
			// 3D���[�h�̎�
			else
			{
				// MeshWindow���J����Ă���Ƃ�
				if (!mAddObjWindow.MeshOrModelWindow)
				{

				}
				else
				{
					for (int i = (mAddObjWindow.PrevPageNumber - 1) * 5, k = 0, size = (int)mAddObjWindow.ModelButton.size(); i < size; ++i, ++k)
					{
						if (k == 5)
						{
							break;
						}
						RiseUI(mAddObjWindow.ModelButton[i]);
						RiseText(mAddObjWindow.ModelNameText[i]);
					}
				}
			}

			mAddObjWindow.PrevPageNumber = mAddObjWindow.PageNumber;
		}

		// �E�N���b�N�ŕ���
		if (MOUSE_ENTER(1))
		{
			mAddObjWindow.OpenWindow = false;
		}
	}
	else
	{
		// ��ʊO�ɔ�΂�
		mAddObjWindow.MeshWindow.position = VECTOR2(-100, 0);
		mAddObjWindow.ModelWindow.position = VECTOR2(-100, 0);
		mAddObjWindow.ImageWindow.position = VECTOR2(-100, 0);
		// �{�^���̏�ԏ�����
		mAddObjWindow.ClickBoxButton = false;
		mAddObjWindow.ClickCylinderButton = false;
		mAddObjWindow.ClickSphereButton = false;
	}
}

void ObjectCreate::AddComponentInput()
{
	// ----- AddComponentWindow -----
	// ----------------------------------- �\���ʒu -------------------------------------

	// �O�t���[���ɑI�����Ă����{�^���ƈႤ�Ȃ�I�𒆂̃{�^�����g����R���|�[�l���g������
	if (mPreviousSelectButton != mSelectButton && mSelectButton)
	{
		// �O��񏉊���
		for (int i = 0; i < mAddCW.Button.size(); i++)
		{
			RiseUI(mAddCW.Button[i]);
			RiseText(mAddCW.NameText[i]);
		}
		if (mHierarchyWindow.m2D)
		{
			mUseComponent = mActor[mSelectActorNum]->GetUse2DComponent();
		}
		else
		{
			mUseComponent = mActor[mSelectActorNum]->GetUse3DComponent();
		}
		std::vector<std::wstring> text;
		int size = (int)mUseComponent.size();
		for (int i = 0; i < size; i++)
		{
			text.push_back(mUseComponent[i]);
		}

		mAddCW.SetButton(size);
		mAddCW.SetText(text);
		mAddCW.ClickModelButton.resize(size);

	}

	// +����Ă���͍̂��W����������邽��
	mAddCW.AddButton.position = VECTOR2(mInspectorWindow.Border.position.x - 17, g_window_height - mAddCW.AddButton.bottom);
	if (mAddCW.OpenWindow)
	{
		mAddCW.Window.position = VECTOR2(mInspectorWindow.Border.position.x - 77, g_window_height - mAddCW.Window.bottom);
	}
	else
	{
		// ��ʊO�ɔ�΂�
		mAddCW.Window.position = VECTOR2(-100, 0);
	}
	for (int i = 0, size = (int)mAddCW.Button.size(); i < size; i++)
	{
		int num = i % 5;
		mAddCW.Button[i].position = mAddCW.Window.position + VECTOR2(0, (float)num * 40 - 105);

		mAddCW.NameText[i].position = mAddCW.Button[i].position + VECTOR2(-70, -18);
	}

	mAddCW.LeftArrow.position = mAddCW.Window.position + VECTOR2(-50, 110);
	mAddCW.RightArrow.position = mAddCW.Window.position + VECTOR2(50, 110);
	mAddCW.PageNumText.position = mAddCW.Window.position + VECTOR2(-75, 100);
	// ----------------------------------- ���͏��� -------------------------------------
	// AddComponentButton�ɏd�Ȃ��Ă���Ƃ�
	if (RectOnMouse(mAddCW.AddButton))
	{
		// �E�N���b�N����������
		if (MOUSE_ENTER(0))
		{
			// �~�j�E�B���h�E���o��
			mAddCW.OpenWindow = true;
		}
	}
	// �}�E�X�J�[�\�����E�̃E�B���h�E�ɏd�Ȃ���������
	if (RectOnMouse(mAddCW.Window))
	{
		mAddCW.mPutOnAddCW = true;
	}
	else
	{
		mAddCW.mPutOnAddCW = false;
	}
	if (mAddCW.OpenWindow)
	{
		mAddCW.Window.position = VECTOR2(mInspectorWindow.Border.position.x - 77, g_window_height - mAddCW.Window.bottom);
		// ���ꂼ��̃{�^���ɃJ�[�\�����G��Ă���Ƃ����N���b�N��On
		for (int i = (mAddCW.PageNumber - 1) * 5, k = 0, size = (int)mAddCW.Button.size(); i < size; ++i, ++k)
		{
			if (k == 5)
			{
				break;
			}
			if (RectOnMouse(mAddCW.Button[i]))
			{
				if (MOUSE_ENTER(0))
				{
					mAddCW.ClickModelButton[i] = true;
					AddComponent(mActor[mSelectActorNum], mUseComponent[i]);
				}
			}
		}
		for (int i = (mAddCW.PageNumber - 1) * 5, k = 0, size = (int)mAddCW.Button.size(); i < size; ++i, ++k)
		{
			if (k == 5)
			{
				break;
			}
			if (!RectOnMouse(mAddCW.Button[i]) || MOUSE_EXIT(0))
			{
				mAddCW.ClickModelButton[i] = false;
			}
		}
		// ----- �y�[�W���� ------
		if (RectOnMouse(mAddCW.LeftArrow))
		{
			if (MOUSE_ENTER(0))
			{
				mAddCW.PageNumber--;
			}
		}
		else if (RectOnMouse(mAddCW.RightArrow))
		{
			if (MOUSE_ENTER(0))
			{
				mAddCW.PageNumber++;
			}
		}

		if (mAddCW.PageNumber == 0)
		{
			mAddCW.PageNumber = mAddCW.PageNumMax;
		}
		else if (mAddCW.PageNumber > mAddCW.PageNumMax)
		{
			mAddCW.PageNumber = 1;
		}

		if (mAddCW.PageNumber != mAddCW.PrevPageNumber)
		{
			swprintf(mAddCW.PageNumText.Text, MAX_PATH, _T("%d"), mAddCW.PageNumber);

			for (int i = (mAddCW.PrevPageNumber - 1) * 5, k = 0, size = (int)mAddCW.Button.size(); i < size; ++i, ++k)
			{
				if (k == 5)
				{
					break;
				}
				RiseUI(mAddCW.Button[i]);
				RiseText(mAddCW.NameText[i]);
			}

			mAddCW.PrevPageNumber = mAddCW.PageNumber;
		}
		// �E�N���b�N�ŕ���
		if (MOUSE_ENTER(1))
		{
			mAddCW.OpenWindow = false;
		}
	}	
}

void ObjectCreate::SelectWindowInput()
{
	// �}�E�X�J�[�\�����E�̃E�B���h�E�ɏd�Ȃ���������
	if (RectOnMouse(mSelectWindow.Window))
	{
		mSelectWindow.mPutOnSW = true;
	}
	else
	{
		mSelectWindow.mPutOnSW = false;
	}
	if (mOpenSelectWindow)
	{
		// ----- AddComponentWindow -----
		// ----------------------------------- �\���ʒu -------------------------------------				
		// ������Window�̈�����̍���(x3)
		mSelectWindow.Window.position = VECTOR2(mInspectorWindow.Border.position.x - 77, g_window_height - (mSelectWindow.Window.bottom * 3));

		for (int i = 0; i < mSelectWindow.Button.size(); i++)
		{
			int num = i % 5;
			mSelectWindow.Button[i].position = mSelectWindow.Window.position + VECTOR2(0, (float)num * 40 - 105);

			mSelectWindow.NameText[i].position = mSelectWindow.Button[i].position + VECTOR2(-70, -18);
		}

		// ----------------------------------- ���͏��� -------------------------------------
		// ���ꂼ��̃{�^���ɃJ�[�\�����G��Ă���Ƃ����N���b�N��On
		for (int i = (mSelectWindow.PageNumber - 1) * 5, k = 0, size = (int)mSelectWindow.Button.size(); i < size; ++i, ++k)
		{
			if (k == 5)
			{
				break;
			}
			if (RectOnMouse(mSelectWindow.Button[i]))
			{
				if (MOUSE_ENTER(0))
				{
					mSelectWindow.ClickSelectButton[i] = true;
					mSelectWindow.Decision = true;
					mSelectWindow.DecisionNum = i;
					mOpenSelectWindow = false;
					for (int i = 0, size = (int)mSelectWindow.Button.size(); i < size; ++i)
					{
						RiseUI(mSelectWindow.Button[i]);
						RiseText(mSelectWindow.NameText[i]);
					}
				}
			}
		}
		for (int i = (mSelectWindow.PageNumber - 1) * 5, k = 0, size = (int)mSelectWindow.Button.size(); i < size; ++i, ++k)
		{
			if (k == 5)
			{
				break;
			}
			if (!RectOnMouse(mSelectWindow.Button[i]) || MOUSE_EXIT(0))
			{
				mSelectWindow.ClickSelectButton[i] = false;
			}
		}

		// ----- �y�[�W���� ------
		if (RectOnMouse(mSelectWindow.LeftArrow))
		{
			if (MOUSE_ENTER(0))
			{
				mSelectWindow.PageNumber--;
			}
		}
		else if (RectOnMouse(mSelectWindow.RightArrow))
		{
			if (MOUSE_ENTER(0))
			{
				mSelectWindow.PageNumber++;
			}
		}

		if (mSelectWindow.PageNumber == 0)
		{
			mSelectWindow.PageNumber = mSelectWindow.PageNumMax;
		}
		else if (mSelectWindow.PageNumber > mSelectWindow.PageNumMax)
		{
			mSelectWindow.PageNumber = 1;
		}

		if (mSelectWindow.PageNumber != mSelectWindow.PrevPageNumber)
		{
			swprintf(mSelectWindow.PageNumText.Text, MAX_PATH, _T("%d"), mSelectWindow.PageNumber);

			for (int i = (mSelectWindow.PrevPageNumber - 1) * 5, k = 0, size = (int)mSelectWindow.Button.size(); i < size; ++i, ++k)
			{
				if (k == 5)
				{
					break;
				}
				RiseUI(mSelectWindow.Button[i]);
				RiseText(mSelectWindow.NameText[i]);
			}

			mSelectWindow.PrevPageNumber = mSelectWindow.PageNumber;
		}
	}
	else
	{
		// ��ʊO�ɔ�΂�
		mSelectWindow.Window.position = VECTOR2(-100, 0);
	}
	mSelectWindow.LeftArrow.position = mSelectWindow.Window.position + VECTOR2(-50, 110);
	mSelectWindow.RightArrow.position = mSelectWindow.Window.position + VECTOR2(50, 110);
	mSelectWindow.PageNumText.position = mSelectWindow.Window.position + VECTOR2(-75, 100);
}

// ========================================================== �X�V ======================================================
void ObjectCreate::HierarchyWindowUpdate()
{
	// ----- HierarchyWindow-----
	// UI
	SetUI(mHierarchyWindow.BorderSlider);
	SetUI(mHierarchyWindow.Border);
	SetUI(mHierarchyWindow.Window);
	if (mHierarchyWindow.m2D)
	{
		SetUI(mHierarchyWindow.Tab3D2D[0]);
	}
	else
	{
		SetUI(mHierarchyWindow.Tab3D2D[1]);
	}

	// ��U������
	RiseUI(mHierarchyWindow.SelectButtonBlue);
	RiseUI(mHierarchyWindow.SelectButtonRed);
	RiseUI(mHierarchyWindow.SelectButtonGreen);
	if (!mHierarchyWindow.m2D)
	{
		// Button3D
		for (HierarchyButton& button: mHierarchyWindow.Button3D)
		{
			SetText(button.Text);
			RiseUI(button.RectBlue);
			RiseUI(button.RectRed);
			RiseUI(button.RectGreen);

			if (button.ColorNum == 0)
			{
				// Button��\�����A��������SelectButton���폜
				SetUI(button.RectBlue);
			}
			else if (button.ColorNum == 1)
			{
				// Button��\�����A��������SelectButton���폜
				SetUI(button.RectRed);

			}
			else if (button.ColorNum == 2)
			{
				// Button��\�����A��������SelectButton���폜
				SetUI(button.RectGreen);

			}
		}
	}
	else
	{
		// Button2D
		for (HierarchyButton& button : mHierarchyWindow.Button2D)
		{
			SetText(button.Text);
			RiseUI(button.RectBlue);
			RiseUI(button.RectRed);
			RiseUI(button.RectGreen);

			if (button.ColorNum == 0)
			{
				// Button��\�����A��������SelectButton���폜
				SetUI(button.RectBlue);
			}
			else if (button.ColorNum == 1)
			{
				// Button��\�����A��������SelectButton���폜
				SetUI(button.RectRed);

			}
			else if (button.ColorNum == 2)
			{
				// Button��\�����A��������SelectButton���폜
				SetUI(button.RectGreen);

			}
		}
	}
	if (mSelectButton)
	{
		if (mSelectButton->ColorNum == 0)
		{
			mHierarchyWindow.SelectButtonBlue.position = mSelectButton->RectBlue.position;
			mHierarchyWindow.SelectButtonBlue.scale = mSelectButton->RectBlue.scale;
			// SelectButton��\�����A��������Button���폜
			SetUI(mHierarchyWindow.SelectButtonBlue);
		}
		else if (mSelectButton->ColorNum == 1)
		{
			mHierarchyWindow.SelectButtonRed.position = mSelectButton->RectBlue.position;
			mHierarchyWindow.SelectButtonRed.scale = mSelectButton->RectBlue.scale;
			// SelectButton��\�����A��������Button���폜
			SetUI(mHierarchyWindow.SelectButtonRed);
		}
		else if (mSelectButton->ColorNum == 2)
		{
			mHierarchyWindow.SelectButtonGreen.position = mSelectButton->RectBlue.position;
			mHierarchyWindow.SelectButtonGreen.scale = mSelectButton->RectBlue.scale;
			// SelectButton��\�����A��������Button���폜
			SetUI(mHierarchyWindow.SelectButtonGreen);
		}
	}
}
void ObjectCreate::InspectorWindowUpdate()
{
	// ----- InspectorWindow-----
	//UI
	SetUI(mInspectorWindow.Border);
	SetUI(mInspectorWindow.PullButton);
	SetUI(mInspectorWindow.Window);
	if (mSelectButton)
	{
		// ----------- TopFrame ----------
		SetUI(mInspectorWindow.TopFrame.Frame);
		if (mInspectorWindow.TopFrame.NameBox.Select)
		{
			SetUI(mInspectorWindow.SelectTextBox);
		}
		else
		{
			SetUI(mInspectorWindow.TopFrame.NameBox.Box);
		}

		if (mSelectButton->ColorNum == 0)
		{
			SetUI(mInspectorWindow.TopFrame.ColorBlue);
			RiseUI(mInspectorWindow.TopFrame.ColorRed);
			RiseUI(mInspectorWindow.TopFrame.ColorGreen);
		}
		else if (mSelectButton->ColorNum == 1)
		{
			SetUI(mInspectorWindow.TopFrame.ColorRed);
			RiseUI(mInspectorWindow.TopFrame.ColorBlue);
			RiseUI(mInspectorWindow.TopFrame.ColorGreen);
		}
		else if (mSelectButton->ColorNum == 2)
		{
			SetUI(mInspectorWindow.TopFrame.ColorGreen);
			RiseUI(mInspectorWindow.TopFrame.ColorRed);
			RiseUI(mInspectorWindow.TopFrame.ColorBlue);
		}
		
		SetUI(mInspectorWindow.TopFrame.XButton);
		//Text
		SetText(mInspectorWindow.TopFrame.NameBox.Text);
		// ----------- Transform ----------

		SetUI(mInspectorWindow.Transform.Top);

		// �J���Ă�Ƃ�Bottom�\���A���Ă���Ƃ��\�����Ȃ�
		if (mInspectorWindow.Transform.Open)
		{
			// 3��xyz
			for (int i = 0; i < 3; ++i)
			{
				if (mInspectorWindow.Transform.Position[i].Select)
				{
					SetUI(mInspectorWindow.SelectTextBox);
				}
				else
				{
					SetUI(mInspectorWindow.Transform.Position[i].Box);
				}
				if (mInspectorWindow.Transform.Rotation[i].Select)
				{

					SetUI(mInspectorWindow.SelectTextBox);
				}
				else
				{
					SetUI(mInspectorWindow.Transform.Rotation[i].Box);
				}
				if (mInspectorWindow.Transform.Scale[i].Select)
				{

					SetUI(mInspectorWindow.SelectTextBox);
				}
				else
				{
					SetUI(mInspectorWindow.Transform.Scale[i].Box);
				}
				SetText(mInspectorWindow.Transform.Position[i].Text);
				SetText(mInspectorWindow.Transform.Rotation[i].Text);
				SetText(mInspectorWindow.Transform.Scale[i].Text);
			}
			SetUI(mInspectorWindow.Transform.Bottom);
			SetUI(mInspectorWindow.Transform.DownArrow);
			RiseUI(mInspectorWindow.Transform.RightArrow);

		}
		else
		{
			// 3��xyz
			for (int i = 0; i < 3; i++)
			{
				RiseUI(mInspectorWindow.Transform.Position[i].Box);
				RiseUI(mInspectorWindow.Transform.Rotation[i].Box);
				RiseUI(mInspectorWindow.Transform.Scale[i].Box);
				RiseText(mInspectorWindow.Transform.Position[i].Text);
				RiseText(mInspectorWindow.Transform.Rotation[i].Text);
				RiseText(mInspectorWindow.Transform.Scale[i].Text);
			}
			RiseUI(mInspectorWindow.Transform.Bottom);
			SetUI(mInspectorWindow.Transform.RightArrow);
			RiseUI(mInspectorWindow.Transform.DownArrow);
		}

		// 2D���[�h�̎��͈ꕔ��TextBox��\��
		if (mHierarchyWindow.m2D)
		{
			RiseUI(mInspectorWindow.Transform.Position[2].Box);
			RiseUI(mInspectorWindow.Transform.Rotation[0].Box);
			RiseUI(mInspectorWindow.Transform.Rotation[1].Box);
			RiseUI(mInspectorWindow.Transform.Scale[2].Box);
			RiseText(mInspectorWindow.Transform.Position[2].Text);
			RiseText(mInspectorWindow.Transform.Rotation[0].Text);
			RiseText(mInspectorWindow.Transform.Rotation[1].Text);
			RiseText(mInspectorWindow.Transform.Scale[2].Text);
		}

		// ----------- ColorFrame ----------
		if (mInspectorWindow.mColorFrameFlag)
		{
			SetUI(mInspectorWindow.ColorFrame.Frame);
			// 3��rgb
			for (int i = 0; i < 3; i++)
			{
				if (mInspectorWindow.ColorFrame.rgb[i].Select)
				{
					SetUI(mInspectorWindow.SelectTextBox);
				}
				else
				{
					SetUI(mInspectorWindow.ColorFrame.rgb[i].Box);
				}
				SetText(mInspectorWindow.ColorFrame.rgb[i].Text);
			}
		}
		else
		{
			RiseUI(mInspectorWindow.ColorFrame.Frame);
			// 3��rgb
			for (int i = 0; i < 3; i++)
			{
				RiseUI(mInspectorWindow.ColorFrame.rgb[i].Box);
				RiseText(mInspectorWindow.ColorFrame.rgb[i].Text);
			}
		}

		// ------------- Component ------------
		for (auto& component : mSelectComponent)
		{
			// top��title�͏�ɕ\��
			SetUI(component->Top);
			SetUI(component->XButton);
			SetText(component->Title);
			// Bottom������Ƃ�
			if (component->Bottom.fileName)
			{
				if (component->Open)
				{
					SetUI(component->Bottom);
					SetUI(component->DownArrow);
					RiseUI(component->RightArrow);
				}
				else
				{
					RiseUI(component->Bottom);
					SetUI(component->RightArrow);
					RiseUI(component->DownArrow);
				}
			}
			if (component->Check)
			{
				SetUI(component->CheckBoxOn);
				RiseUI(component->CheckBoxOff);
			}
			else
			{
				SetUI(component->CheckBoxOff);
				RiseUI(component->CheckBoxOn);
			}		
			component->Update(mInspectorWindow.SelectTextBox);
		}
	}
}
void ObjectCreate::AddObjWindowUpdate()
{
	// ----- AddObjWindow -----
	// UI
	if (mHierarchyWindow.m2D)
	{
		// ImageButton
		bool flag = true;// �N���b�N����Ă��郂�f���{�^��������Ȃ����
		for (int i = (mAddObjWindow.PageNumber - 1) * 5, k = 0, size = (int)mAddObjWindow.ImageButton.size(); i < size; ++i, ++k)
		{
			if (k == 5)
			{
				break;
			}
			if (mAddObjWindow.ClickImageButton[i])
			{
				mAddObjWindow.ModelButtonOn.position = mAddObjWindow.ImageButton[i].position;
				SetUI(mAddObjWindow.ModelButtonOn);
				RiseUI(mAddObjWindow.ImageButton[i]);
				flag = false;
			}
			else
			{
				SetUI(mAddObjWindow.ImageButton[i]);
			}
		}
		if (flag)
		{
			RiseUI(mAddObjWindow.ModelButtonOn);
		}
		// ImageText
		for (int i = (mAddObjWindow.PageNumber - 1) * 5, k = 0, size = (int)mAddObjWindow.ImageNameText.size(); i < size; ++i, ++k)
		{
			if (k == 5)
			{
				break;
			}
			SetText(mAddObjWindow.ImageNameText[i]);
		}
		// ImageWidow
		SetUI(mAddObjWindow.ImageWindow);
	}
	else
	{
		// �`������ւ���
		// Text
		SetText(mAddObjWindow.MeshText);
		SetText(mAddObjWindow.ModelText);
		if (!mAddObjWindow.MeshOrModelWindow)
		{
			// BoxButton
			if (mAddObjWindow.ClickBoxButton)
			{
				SetUI(mAddObjWindow.BOXButton);
				RiseUI(mAddObjWindow.BOXButtonOff);
			}
			else
			{
				SetUI(mAddObjWindow.BOXButtonOff);
				RiseUI(mAddObjWindow.BOXButton);
			}
			// CylinderButton
			if (mAddObjWindow.ClickCylinderButton)
			{
				SetUI(mAddObjWindow.CylinderButton);
				RiseUI(mAddObjWindow.CylinderButtonOff);
			}
			else
			{
				SetUI(mAddObjWindow.CylinderButtonOff);
				RiseUI(mAddObjWindow.CylinderButton);
			}
			// SphereButton
			if (mAddObjWindow.ClickSphereButton)
			{
				SetUI(mAddObjWindow.SphereButton);
				RiseUI(mAddObjWindow.SphereButtonOff);
			}
			else
			{
				SetUI(mAddObjWindow.SphereButtonOff);
				RiseUI(mAddObjWindow.SphereButton);
			}
			// MeshWidow
			SetUI(mAddObjWindow.MeshWindow);
		}
		else
		{
			// ModelButton
			bool flag = true;// �N���b�N����Ă��郂�f���{�^��������Ȃ����
			for (int i = (mAddObjWindow.PageNumber - 1) * 5, k = 0, size = (int)mAddObjWindow.ModelButton.size(); i < size; ++i, ++k)
			{
				if (k == 5)
				{
					break;
				}
				if (mAddObjWindow.ClickModelButton[i])
				{
					mAddObjWindow.ModelButtonOn.position = mAddObjWindow.ModelButton[i].position;
					SetUI(mAddObjWindow.ModelButtonOn);
					RiseUI(mAddObjWindow.ModelButton[i]);
					flag = false;
				}
				else
				{
					SetUI(mAddObjWindow.ModelButton[i]);
				}
			}
			if (flag)
			{
				RiseUI(mAddObjWindow.ModelButtonOn);
			}
			// ModelText
			for (int i = (mAddObjWindow.PageNumber - 1) * 5, k = 0, size = (int)mAddObjWindow.ModelButton.size(); i < size; ++i, ++k)
			{
				if (k == 5)
				{
					break;
				}
				SetText(mAddObjWindow.ModelNameText[i]);
			}
			// ModelWidow
			SetUI(mAddObjWindow.ModelWindow);
		}
	}
	SetUI(mAddObjWindow.AddObjectButton);
	SetUI(mAddObjWindow.LeftArrow);
	SetUI(mAddObjWindow.RightArrow);
	SetText(mAddObjWindow.PageNumText);
}

void ObjectCreate::AddComponentUpdate()
{
	if (mSelectButton)
	{
		// Button
		bool flag = true;// �N���b�N����Ă��郂�f���{�^��������Ȃ����
		for (int i = (mAddCW.PageNumber - 1) * 5, k = 0, size = (int)mAddCW.Button.size(); i < size; ++i, ++k)
		{
			if (k == 5)
			{
				break;
			}
			if (mAddCW.ClickModelButton[i])
			{
				mAddCW.ButtonOn.position = mAddCW.Button[i].position;
				SetUI(mAddCW.ButtonOn);
				RiseUI(mAddCW.Button[i]);
				flag = false;
			}
			else
			{
				SetUI(mAddCW.Button[i]);
			}
			if (flag)
			{
				RiseUI(mAddCW.ButtonOn);
			}
		}
		// Text
		for (int i = (mAddCW.PageNumber - 1) * 5, k = 0, size = (int)mAddCW.NameText.size(); i < size; ++i, ++k)
		{
			if (k == 5)
			{
				break;
			}
			SetText(mAddCW.NameText[i]);
		}
	}
	SetUI(mAddCW.AddButton);
	SetUI(mAddCW.Window);
	SetUI(mAddCW.LeftArrow);
	SetUI(mAddCW.RightArrow);
	SetText(mAddCW.PageNumText);
}

void ObjectCreate::SelectWindowUpdate()
{
	if (mOpenSelectWindow)
	{
		// Button
		bool flag = true;// �N���b�N����Ă��郂�f���{�^��������Ȃ����
		for (int i = (mSelectWindow.PrevPageNumber - 1) * 5, k = 0, size = (int)mSelectWindow.Button.size(); i < size; ++i, ++k)
		{
			if (k == 5)
			{
				break;
			}
			if (mSelectWindow.ClickSelectButton[i])
			{
				mSelectWindow.ButtonOn.position = mSelectWindow.Button[i].position;
				SetUI(mSelectWindow.ButtonOn);
				RiseUI(mSelectWindow.Button[i]);
				flag = false;
			}
			else
			{
				SetUI(mSelectWindow.Button[i]);
			}
			if (flag)
			{
				RiseUI(mSelectWindow.ButtonOn);
			}
		}
		// Text
		for (int i = (mSelectWindow.PrevPageNumber - 1) * 5, k = 0, size = (int)mSelectWindow.Button.size(); i < size; ++i, ++k)
		{
			if (k == 5)
			{
				break;
			}
			SetText(mSelectWindow.NameText[i]);
		}
	}
	SetUI(mSelectWindow.Window);
	SetUI(mSelectWindow.LeftArrow);
	SetUI(mSelectWindow.RightArrow);
	SetText(mSelectWindow.PageNumText);
}
// �V�[�����ς��Ƃ���UI�`��폜
void ObjectCreate::DeleteUI()
{
	// ----- HierarchyWindow-----
	RiseUI(mHierarchyWindow.Window);
	RiseUI(mHierarchyWindow.Border);
	RiseUI(mHierarchyWindow.BorderSlider);
	RiseUI(mHierarchyWindow.Tab3D2D[0]);
	RiseUI(mHierarchyWindow.Tab3D2D[1]);
	for (auto& button : mHierarchyWindow.Button3D)
	{
		RiseUI(button.RectBlue);
		RiseUI(button.RectGreen);
		RiseUI(button.RectRed);
		RiseText(button.Text);
	}
	for (auto& button : mHierarchyWindow.Button2D)
	{
		RiseUI(button.RectBlue);
		RiseUI(button.RectGreen);
		RiseUI(button.RectRed);
		RiseText(button.Text);
	}
	RiseUI(mHierarchyWindow.SelectButtonBlue);
	RiseUI(mHierarchyWindow.SelectButtonRed);
	RiseUI(mHierarchyWindow.SelectButtonGreen);
	// ----- InspectorWindow -----
	RiseInspector();
	// ----- AddObjWindow -----
	RiseUI(mAddObjWindow.AddObjectButton);
	RiseUI(mAddObjWindow.LeftArrow);
	RiseUI(mAddObjWindow.RightArrow);
	RiseText(mAddObjWindow.PageNumText);

	// MeshWindow
	RiseUI(mAddObjWindow.MeshWindow);
	RiseText(mAddObjWindow.MeshText);
	RiseUI(mAddObjWindow.BOXButton);
	RiseUI(mAddObjWindow.BOXButtonOff);
	RiseUI(mAddObjWindow.CylinderButton);
	RiseUI(mAddObjWindow.CylinderButtonOff);
	RiseUI(mAddObjWindow.SphereButton);
	RiseUI(mAddObjWindow.SphereButtonOff);
	// ModelWindow
	RiseUI(mAddObjWindow.ModelWindow);
	RiseText(mAddObjWindow.ModelText);
	for (auto& button : mAddObjWindow.ModelButton)
	{
		RiseUI(button);
	}
	for (auto& text : mAddObjWindow.ModelNameText)
	{
		RiseText(text);
	}
	RiseUI(mAddObjWindow.ModelButtonOn);
	// ImageWindow
	RiseUI(mAddObjWindow.ImageWindow);
	for (auto& button : mAddObjWindow.ImageButton)
	{
		RiseUI(button);
	}
	for (auto& text : mAddObjWindow.ImageNameText)
	{
		RiseText(text);
	}

	// ----- AddComponentWindow -----
	RiseUI(mAddCW.AddButton);
	RiseUI(mAddCW.Window);
	RiseUI(mAddCW.LeftArrow);
	RiseUI(mAddCW.RightArrow);
	RiseText(mAddCW.PageNumText);
	for (auto& button : mAddCW.Button)
	{
		RiseUI(button);
	}
	for (auto& text : mAddCW.NameText)
	{
		RiseText(text);
	}
	RiseUI(mAddCW.ButtonOn);

	// ----- SelectWindow -----
	RiseUI(mSelectWindow.Window);
	RiseUI(mSelectWindow.LeftArrow);
	RiseUI(mSelectWindow.RightArrow);
	RiseText(mSelectWindow.PageNumText);
	for (auto& button : mSelectWindow.Button)
	{
		RiseUI(button);
	}
	for (auto& text : mSelectWindow.NameText)
	{
		RiseText(text);
	}
	RiseUI(mSelectWindow.ButtonOn);

	RiseUI(mPlayButton);
	RiseUI(mTitleButton);
	RiseUI(mSaveButton);

	mSavePosition.clear();
	mSaveRotation.clear();
	// Actor�̈ʒu�ۑ�
	for (auto actor : mActor)
	{
		mSavePosition.push_back(actor->GetPosition());
	}
	// Actor�̉�]�ۑ�
	for (auto actor : mActor)
	{
		mSaveRotation.push_back(actor->GetRotation());
	}

	// 3D���[�h�ɂ��Ă���
	mHierarchyWindow.m2D = false;

	mSelectButton = nullptr;
	mPreviousSelectButton = nullptr;
	mSelectActorNum = 0;
	mSelectButtonNum = 0;
}

// �I�u�W�F�N�g�̒ǉ�
void ObjectCreate::AddObj(std::wstring objName)
{
	// =========== UI�֘A ============
	int num = mObjNum[objName];
	HierarchyButton HW = {};
	CreativeUI Rect(300, 100);
	Rect.fileName = "Resouse\\Texture\\UI\\ButtonBlue.png";
	if (mHierarchyWindow.m2D)
	{
		Rect.position = VECTOR2(((mHierarchyWindow.Border.position.x / 2) - 3), 50 + ((float)mHierarchyWindow.Button2D.size() * 100));
	}
	else
	{
		Rect.position = VECTOR2(((mHierarchyWindow.Border.position.x / 2) - 3), 50 + ((float)mHierarchyWindow.Button3D.size() * 100));
	}
	Rect.scale.x = mHierarchyWindow.Border.position.x / 300;
	HW.RectBlue = Rect;
	Rect.fileName = "Resouse\\Texture\\UI\\ButtonRed.png";
	HW.RectRed = Rect;
	Rect.fileName = "Resouse\\Texture\\UI\\ButtonGreen.png";
	HW.RectGreen = Rect;
	// Text
	CreativeText text;
	swprintf(text.Text, MAX_PATH, _T("%s%02d"), objName.c_str(), num);
	text.position.x = 0;
	text.position.y = Rect.position.y - Rect.bottom;
	HW.Text = text;
	HW.ObjType = objName;
	HW.basePos = Rect.position;
	HW.addNum = mButtonNum;

	if (mHierarchyWindow.m2D)
	{
		mHierarchyWindow.Button2D.push_back(HW);
	}
	else
	{
		mHierarchyWindow.Button3D.push_back(HW);
	}

	// �w��Object�̐��𑝂₷
	mObjNum[objName]++;
	mButtonNum++;
	// =========== Obj�{�� ============
	if (mHierarchyWindow.m2D)
	{
		mActor.push_back(new Empty(mGame));
	}
	else
	{
		if (mMeshName[Box]._Equal(objName))
		{
			mActor.push_back(new Empty(mGame));
		}
		else if (mMeshName[Cylinder]._Equal(objName))
		{
			mActor.push_back(new Empty(mGame));
		}
		else if (mMeshName[Sphere]._Equal(objName))
		{
			mActor.push_back(new Empty(mGame));
		}
		// ��L�ȊO�Ȃ�Model
		else
		{
			for (int i = 0, size = (int)mModelName.size(); i < size; ++i)
			{
				if (mSimpleModelName[i]._Equal(objName))
				{
					mActor.push_back(new Empty(mGame));
					break;
				}
			}
		}
	}
	mSelectButton = nullptr;
	RiseInspector();
}

// �I�u�W�F�N�g�폜
void ObjectCreate::DeleteObj()
{
	// ----- TopFram&Button -----
	// UI
	RiseUI(mSelectButton->RectBlue);
	RiseUI(mSelectButton->RectRed);
	RiseUI(mSelectButton->RectGreen);
	RiseUI(mHierarchyWindow.SelectButtonBlue);
	RiseUI(mHierarchyWindow.SelectButtonRed);
	RiseUI(mHierarchyWindow.SelectButtonGreen);
	RiseText(mSelectButton->Text);

	RiseInspector();

	int addNum = mSelectButton->addNum;
	if (!mHierarchyWindow.m2D)
	{
		if (mHierarchyWindow.Button3D.size() == 1 || (int)(mHierarchyWindow.Button3D.size()) == mSelectButtonNum)
		{
			mHierarchyWindow.Button3D.pop_back();
		}
		else
		{
			for (int i = 0, size = (int)(mHierarchyWindow.Button3D.size() - 1); i < size; ++i)
			{
				// �����ԍ��ȏ�̃{�^������������炷
				if (mHierarchyWindow.Button3D[i].addNum >= addNum)
				{
					int NextButton = i + 1;
					mHierarchyWindow.Button3D[i] = mHierarchyWindow.Button3D[NextButton];
					mHierarchyWindow.Button3D[i].basePos.y -= 100;
					mHierarchyWindow.Button3D[i].RectBlue.position.y -= 100;
				}
			}
		}
	}
	else
	{
		if (mHierarchyWindow.Button2D.size() == 1 || (int)(mHierarchyWindow.Button2D.size()) == mSelectButtonNum)
		{
			mHierarchyWindow.Button2D.pop_back();
		}
		else
		{
			for (int i = 0, size = (int)(mHierarchyWindow.Button2D.size() - 1); i < size; ++i)
			{
				// �����ԍ��ȏ�̃{�^������������炷
				if (mHierarchyWindow.Button2D[i].addNum >= addNum)
				{
					int NextButton = i + 1;
					mHierarchyWindow.Button2D[i] = mHierarchyWindow.Button2D[NextButton];
					mHierarchyWindow.Button2D[i].basePos.y -= 100;
					mHierarchyWindow.Button2D[i].RectBlue.position.y -= 100;
				}
			}
		}
	}

	// �ǉ����̏��Ԃ���Â��炷�B
	for (auto& button : mHierarchyWindow.Button3D)
	{
		if (button.addNum > addNum)
		{
			button.addNum--;
		}
	}
	for (auto& button : mHierarchyWindow.Button2D)
	{
		if (button.addNum > addNum)
		{
			button.addNum--;
		}
	}

	// ----- AddComponetCW -----
	for (int i = 0; i < mAddCW.Button.size(); i++)
	{
		RiseUI(mAddCW.Button[i]);
		RiseText(mAddCW.NameText[i]);
	}

	// ----- SelectWindow -----
	RiseUI(mSelectWindow.Window);
	for (auto& button : mSelectWindow.Button)
	{
		RiseUI(button);
	}
	for (auto& text : mSelectWindow.NameText)
	{
		RiseText(text);
	}

	// ----------- ComponentWindow ----------
	for (int i = 0; i < mSelectComponentNum; i++)
	{
		DeleteComponent(i);
	}

	// =============== Component ===================
	for (auto component : mSelectComponent)
	{
		mActor[mSelectActorNum]->DeleateComponent(component->Type);
	}
	// =============== Obj ===================
	if (mActor.size() == 1)
	{
		mActor[addNum]->SetState(ActorState::EDead);
		mActor.pop_back();
	}
	else
	{
		for (int i = 0, size = (int)mActor.size(); i < size; ++i)
		{
			if (i == addNum)
			{
				mActor[i]->SetState(ActorState::EDead);
			}
			// �����ԍ��ȏ�̃{�^������������炷
			if (i >= addNum)
			{
				int NextButton = i + 1;
				// �������{�^�����������popBack
				if (size == NextButton)
				{
					mActor.pop_back();
				}
				else
				{
					mActor[i] = mActor[NextButton];
				}
			}
		}
	}

	mButtonNum--;
	// �Ō�ɕ��������������T�C�Y����
	int ButtonNum;
	if (!mHierarchyWindow.m2D)
	{
		ButtonNum = (int)mHierarchyWindow.Button3D.size() - 1;
		if (ButtonNum > 0)
		{
			mHierarchyWindow.Button3D.resize(ButtonNum);
		}
	}
	else
	{
		ButtonNum = (int)mHierarchyWindow.Button2D.size() - 1;
		if (ButtonNum > 0)
		{
			mHierarchyWindow.Button2D.resize(ButtonNum);
		}
	}
	mActor.resize(mButtonNum);
}
// �R���|�[�l���g�̒ǉ�
void ObjectCreate::AddComponent(Actor* actor, std::wstring componentName, const char* path)
{
	// �I�𒆂̃R���|�[�l���g�̐�
	int num = (int)mSelectComponent.size() - 1;
	bool flag = false;		// ���łɃR���|�[�l���g�����݂��邩

	if (componentName._Equal(_T("PlayerInput")))
	{
		if (!actor->GetComponent(componentName))
		{
			actor->SetComponent(componentName);
			// �{�^�����I�𒆂Ȃ�
			if (mSelectButton)
			{
				mSelectComponent.push_back(new InputUI);
				mSelectComponent.back()->Type = componentName;
				flag = true;
			}
		}
	}
	else if (componentName._Equal(_T("MeshComponent")))
	{
		if (!actor->GetComponent(componentName) && !actor->GetComponent(_T("ModelComponent")))
		{
			actor->SetComponent(componentName);

			// �{�^�����I�𒆂Ȃ�
			if (mSelectButton)
			{
				mSelectComponent.push_back(new MeshUI);
				mSelectComponent.back()->Type = componentName;
				flag = true;
			}
		}
	}
	else if (componentName._Equal(_T("ModelComponent")))
	{
		if (!actor->GetComponent(componentName) && !actor->GetComponent(_T("MeshComponent")))
		{
			actor->SetComponent(componentName);
			actor->GetModelComponent()->SetFileName(path);
			actor->GetModelComponent()->SetObjModel();
			// �{�^�����I�𒆂Ȃ�
			if (mSelectButton)
			{
				mSelectComponent.push_back(new ModelUI);
				mSelectComponent.back()->Type = componentName;
				flag = true;
			}
		}
	}
	else if (componentName._Equal(_T("BoxCollider")))
	{
		if (!actor->GetComponent(componentName) && !actor->GetComponent(_T("CapsuleCollider")))
		{
			actor->SetComponent(componentName);
			actor->SetColliderType(ColliderType::Box);
			// �{�^�����I�𒆂Ȃ�
			if (mSelectButton)
			{
				mSelectComponent.push_back(new BoxColliderUI);
				mSelectComponent.back()->Type = componentName;
				flag = true;
			}
		}
	}
	else if (componentName._Equal(_T("CylinderCollider")))
	{
		actor->SetColliderType(ColliderType::Cylinder);
	}
	else if (componentName._Equal(_T("CapsuleCollider")))
	{
		if (!actor->GetComponent(componentName) && !actor->GetComponent(_T("BoxCollider")))
		{
			actor->SetComponent(componentName);
			actor->SetColliderType(ColliderType::Capsule);
			// �{�^�����I�𒆂Ȃ�
			if (mSelectButton)
			{
				mSelectComponent.push_back(new CapsuleColliderUI);
				mSelectComponent.back()->Type = componentName;
				flag = true;
			}
		}
	}
	else if (componentName._Equal(_T("RigidBody")))
	{
		if (!actor->GetComponent(componentName))
		{
			actor->SetComponent(componentName);
			// �{�^�����I�𒆂Ȃ�
			if (mSelectButton)
			{
				mSelectComponent.push_back(new RigidBodyUI);
				mSelectComponent.back()->Type = componentName;
				flag = true;
			}
		}
	}
	else if (componentName._Equal(_T("ImageComponent")))
	{
		actor->SetComponent(componentName);
		actor->GetImageComponent().back()->SetFileName(path);
		int size = (int)actor->GetImageComponent().size();
		// �{�^�����I�𒆂Ȃ�
		if (mSelectButton)
		{
			mSelectComponent.push_back(new ImageUI);
			mSelectComponent.back()->Type = componentName;
			mSelectComponent.back()->ComponentNum = size;
			flag = true;
		}
	}
	else
	{
		if (!actor->GetComponent(componentName))
		{
			actor->SetComponent(componentName);
			// �{�^�����I�𒆂Ȃ�
			if (mSelectButton && actor->GetComponent(componentName))
			{
				mSelectComponent.push_back(new ScriptUI);
				mSelectComponent.back()->Type = componentName;
				swprintf(mSelectComponent.back()->Title.Text, MAX_PATH, componentName.c_str());
				flag = true;
			}
		}
	}


	if (flag)
	{
		mSelectComponent.back()->SetMetaData();
		if (num == -1)
		{
			if (mInspectorWindow.mColorFrameFlag)
			{
				mSelectComponent.back()->basePos = InspectorFramePos(mSelectComponent.back()->Top, mInspectorWindow.ColorFrame.Frame);
			}
			else
			{
				mSelectComponent.back()->basePos = InspectorFramePos(mSelectComponent.back()->Top, mInspectorWindow.Transform.Bottom);
			}
		}
		else
		{
			if (mSelectComponent[num]->Bottom.fileName)
			{
				// Bottom������Ƃ�
				mSelectComponent.back()->basePos = InspectorFramePos(mSelectComponent.back()->Top, mSelectComponent[num]->Bottom);
			}
			else
			{
				// Bottom���Ȃ��Ƃ�
				mSelectComponent.back()->basePos = InspectorFramePos(mSelectComponent.back()->Top, mSelectComponent[num]->Top);
			}
		}
		mSelectComponentNum++;
	}
}
// ���ꂼ��̃R���|�[�l���g�̓��͏���
void  ObjectCreate::InputComponentData(StructComponent* component)
{
	// �I�𒆂�Actor
	Actor* selectActor = mActor[mSelectActorNum];
	// Input�Ȃ�
	if (component->Type._Equal(_T("PlayerInput")))
	{
		InputUI* PlayerInput = dynamic_cast<InputUI*>(component);
		// MoveComponent
		TextBoxSelect(PlayerInput->MoveSpeed);
		// ���͑O�ɏ�����ꍞ��
		float moveSpeed = selectActor->GetInput()->GetSpeed();
		
		TextBoxInput(PlayerInput->MoveSpeed, moveSpeed);

		selectActor->GetInput()->SetSpeed(moveSpeed);
	}
	// MeshCollider�Ȃ�
	else if (component->Type._Equal(_T("MeshComponent")))
	{
		MeshUI* Mesh = dynamic_cast<MeshUI*>(component);
		// SourceImage
		if (!mOpenSelectWindow)
		{
			if (RectOnMouse(Mesh->SelectButton))
			{
				if (MOUSE_ENTER(0))
				{
					mOpenSelectWindow = true;
					int num = (int)mMeshName.size();
					mSelectWindow.SetButton(num);
					mSelectWindow.SetText(mMeshName);
					mSelectWindow.ClickSelectButton.resize(num);
				}
			}
		}
		else
		{
			if (RectOnMouse(Mesh->SelectButton))
			{
				if (MOUSE_ENTER(0))
				{
					mOpenSelectWindow = false;
					for (int i = 0, size = (int)mSelectWindow.Button.size(); i < size; ++i)
					{
						RiseUI(mSelectWindow.Button[i]);
						RiseText(mSelectWindow.NameText[i]);
					}
				}
			}
		}

		if (selectActor->GetMeshComponent()->GetObjectType()!=ObjectType::TypeNone)
		{
			swprintf(Mesh->SourceImage.Text.Text, MAX_PATH, mSelectButton->ObjType.c_str());
		}
		else
		{
			swprintf(Mesh->SourceImage.Text.Text, MAX_PATH, _T("NULL"));
		}

		// TextBox�ɐ��l����
		for (int i = 0; i < 4; i++)
		{
			// Color
			TextBoxSelect(Mesh->Color[i]);
		}

		// ���͑O�ɏ�����ꍞ��
		float getColor[3] = { selectActor->GetColor().x,selectActor->GetColor().y,selectActor->GetColor().z };
		float getAlpha = selectActor->GetAlpha();

		// RGB
		float num = 0;
		for (int i = 0; i < 3; i++)
		{
			getColor[i] *= 255;
			// ���͒��͍X�V���Ȃ�
			if (!Mesh->Color[i].Select)
			{
				swprintf(Mesh->Color[i].Text.Text, MAX_PATH, _T("%d"), (int)getColor[i]);
			}
		}
		// ����
		for (int i = 0; i < 3; i++)
		{
			if (Mesh->Color[i].Select)
			{
				std::wstring Text = Input(Mesh->Color[i].Text.Text, true, 3).c_str();
				int l = 0;
				int size = (int)Text.size();
				for (; l < size; ++l)
				{
					if (Text[l] != 48)
					{
						break;
					}
				}
				if (l != 0 && size != l)
				{
					for (int k = 0; k < size - 1; ++k)
					{
						// �����ԍ��ȏ�̃{�^������������炷
						if (k >= l - 1)
						{
							int NextButton = k + 1;
							int num = k - (l - 1);
							Text[num] = Text[NextButton];
						}
					}
					int num = size - l;
					Text.resize(num);
				}
				if (size > 0)
				{
					swprintf(Mesh->Color[i].Text.Text, MAX_PATH, Text.c_str());
				}
				else
				{
					swprintf(Mesh->Color[i].Text.Text, MAX_PATH, _T("0"));
				}
				break;// ������Ȃ̂�break
			}
		}
		// ���͂������l��߂�
		for (int i = 0; i < 3; i++)
		{
			swscanf_s(Mesh->Color[i].Text.Text, _T("%f"), &getColor[i]);
			// rgb��0�`255
			if (getColor[i] > 255)
			{
				getColor[i] = 255;
			}
			else if (getColor[i] < 0)
			{
				getColor[i] = 0;
			}
			getColor[i] /= 255;
		}

		// Alpha
		TextBoxInput(Mesh->Color[3], getAlpha);

		if (getAlpha > 1)
		{
			getAlpha = 1;
		}
		else if (getAlpha < 0)
		{
			getAlpha = 0;
		}

		if (mSelectWindow.Decision)
		{
			// SourceImage
			mSelectButton->ObjType = mMeshName[mSelectWindow.DecisionNum];
			selectActor->GetMeshComponent()->DeleteObject();
			switch (mSelectWindow.DecisionNum)
			{
			case 0:
				selectActor->GetMeshComponent()->SetObjectType(ObjectType::Box);
				break;
			case 1:
				selectActor->GetMeshComponent()->SetObjectType(ObjectType::Cylinder);
				break;
			case 2:
				selectActor->GetMeshComponent()->SetObjectType(ObjectType::Sphere);
				break;
			}
			mSelectWindow.Decision = false;
		}
		selectActor->SetColor(VECTOR(getColor[0], getColor[1], getColor[2]));
		selectActor->SetAlpha(getAlpha);
		selectActor->GetMeshComponent()->SetObject();
		// Tab�������ꂽ�玟��TextBox�Ɉړ�
		if (mTab)
		{
			for (int i = 0; i < 4; i++)
			{
				// Color
				if (Mesh->Color[i].Select)
				{
					if (i != 3)
					{
						int num = i + 1;
						TabClickInput(Mesh->Color[i], Mesh->Color[num]);
						break;
					}
				}
			}
		}
	}
	// ModelCollider�Ȃ�
	else if (component->Type._Equal(_T("ModelComponent")))
	{
		ModelUI* Model = dynamic_cast<ModelUI*>(component);
		// SourceImage
		if (!mOpenSelectWindow)
		{
			if (RectOnMouse(Model->SelectButton))
			{
				if (MOUSE_ENTER(0))
				{
					mOpenSelectWindow = true;
					int num = (int)mSimpleModelName.size();
					mSelectWindow.SetButton(num);
					mSelectWindow.SetText(mSimpleModelName);
					mSelectWindow.ClickSelectButton.resize(num);
				}
			}
		}
		else
		{
			if (RectOnMouse(Model->SelectButton))
			{
				if (MOUSE_ENTER(0))
				{
					mOpenSelectWindow = false;
					for (int i = 0, size = (int)mSelectWindow.Button.size(); i < size; ++i)
					{
						RiseUI(mSelectWindow.Button[i]);
						RiseText(mSelectWindow.NameText[i]);
					}
				}
			}
		}

		if (strlen(selectActor->GetModelComponent()->GetFileName()) != 0)
		{
			swprintf(Model->SourceImage.Text.Text, MAX_PATH, mSelectButton->ObjType.c_str());
		}
		else
		{
			swprintf(Model->SourceImage.Text.Text, MAX_PATH, _T("NULL"));
		}

		// TextBox�ɐ��l����
		for (int i = 0; i < 4; i++)
		{
			// Color
			TextBoxSelect(Model->Color[i]);
		}

		// ���͑O�ɏ�����ꍞ��
		float getColor[3] = { selectActor->GetColor().x,selectActor->GetColor().y,selectActor->GetColor().z };
		float getAlpha = selectActor->GetAlpha();

		// RGB
		float num = 0;
		for (int i = 0; i < 3; i++)
		{
			getColor[i] *= 255;
			// ���͒��͍X�V���Ȃ�
			if (!Model->Color[i].Select)
			{
				swprintf(Model->Color[i].Text.Text, MAX_PATH, _T("%d"), (int)getColor[i]);
			}
		}
		// ����
		for (int i = 0; i < 3; i++)
		{
			if (Model->Color[i].Select)
			{
				std::wstring Text = Input(Model->Color[i].Text.Text, true, 3).c_str();
				int l = 0;
				int size = (int)Text.size();
				for (; l < size; ++l)
				{
					if (Text[l] != 48)
					{
						break;
					}
				}
				if (l != 0 && size != l)
				{
					for (int k = 0; k < size - 1; ++k)
					{
						// �����ԍ��ȏ�̃{�^������������炷
						if (k >= l - 1)
						{
							int NextButton = k + 1;
							int num = k - (l - 1);
							Text[num] = Text[NextButton];
						}
					}
					int num = size - l;
					Text.resize(num);
				}
				if (size > 0)
				{
					swprintf(Model->Color[i].Text.Text, MAX_PATH, Text.c_str());
				}
				else
				{
					swprintf(Model->Color[i].Text.Text, MAX_PATH, _T("0"));
				}
				break;// ������Ȃ̂�break
			}
		}
		// ���͂������l��߂�
		for (int i = 0; i < 3; i++)
		{
			swscanf_s(Model->Color[i].Text.Text, _T("%f"), &getColor[i]);
			// rgb��0�`255
			if (getColor[i] > 255)
			{
				getColor[i] = 255;
			}
			else if (getColor[i] < 0)
			{
				getColor[i] = 0;
			}
			getColor[i] /= 255;
		}

		// Alpha
		TextBoxInput(Model->Color[3], getAlpha);

		if (getAlpha > 1)
		{
			getAlpha = 1;
		}
		else if (getAlpha < 0)
		{
			getAlpha = 0;
		}

		if (mSelectWindow.Decision)
		{
			// SourceImage
			mSelectButton->ObjType = mSimpleModelName[mSelectWindow.DecisionNum];
			selectActor->GetModelComponent()->DeleteObjModel();
			selectActor->GetModelComponent()->SetFileName(mFolderModelName[mSelectWindow.DecisionNum].c_str());
			mSelectWindow.Decision = false;
		}
		selectActor->SetColor(VECTOR(getColor[0], getColor[1], getColor[2]));
		selectActor->SetAlpha(getAlpha);
		selectActor->GetModelComponent()->SetObjModel();
		// Tab�������ꂽ�玟��TextBox�Ɉړ�
		if (mTab)
		{
			for (int i = 0; i < 4; i++)
			{
				// Color
				if (Model->Color[i].Select)
				{
					if (i != 3)
					{
						int num = i + 1;
						TabClickInput(Model->Color[i], Model->Color[num]);
						break;
					}
				}
			}
		}
	}
	// BoxCollider�Ȃ�
	else if (component->Type._Equal(_T("BoxCollider")))
	{
		BoxColliderUI* BoxCollider = dynamic_cast<BoxColliderUI*>(component);
		// TextBox�ɐ��l����
		for (int i = 0; i < 3; i++)
		{
			// Size
			TextBoxSelect(BoxCollider->Size[i]);

			// Center
			TextBoxSelect(BoxCollider->Center[i]);

		}
		// ���͑O�ɏ�����ꍞ��

		float getCenter[3] = { selectActor->GetBoxComponent()->GetPositionGap().x,selectActor->GetBoxComponent()->GetPositionGap().y,selectActor->GetBoxComponent()->GetPositionGap().z };

		for (int i = 0; i < 3; i++)
		{

			// Center
			TextBoxInput(BoxCollider->Center[i], getCenter[i]);

		}

		selectActor->GetBoxComponent()->SetPositionGap(VECTOR(getCenter[0], getCenter[1], getCenter[2]));

		// Tab�������ꂽ�玟��TextBox�Ɉړ�
		if (mTab)
		{
			for (int i = 0; i < 3; i++)
			{
				// Size
				if (BoxCollider->Size[i].Select)
				{
					if (i == 2)
					{
						TabClickInput(BoxCollider->Size[i], BoxCollider->Center[0]);
						break;
					}
					else
					{
						int num = i + 1;
						TabClickInput(BoxCollider->Size[i], BoxCollider->Size[num]);
						break;
					}
				}

				// Center
				if (BoxCollider->Center[i].Select)
				{
					if (i != 2)
					{
						int num = i + 1;
						TabClickInput(BoxCollider->Center[i], BoxCollider->Center[num]);
						break;
					}
				}
			}
		}
	}
	// CapsuleCollider�Ȃ�
	else if (component->Type._Equal(_T("CapsuleCollider")))
	{
		CapsuleColliderUI* CapsuleCollider = dynamic_cast<CapsuleColliderUI*>(component);
		// Height
		TextBoxSelect(CapsuleCollider->Height);
		// Radius
		TextBoxSelect(CapsuleCollider->Radius);

		// TextBox�ɐ��l����
		for (int i = 0; i < 3; i++)
		{
			// Center
			TextBoxSelect(CapsuleCollider->Center[i]);

		}
		// ���͑O�ɏ�����ꍞ��
		float getHeight = selectActor->GetCapsuleComponent()->GetHeight();
		float getRadius = selectActor->GetCapsuleComponent()->GetRadius();
		float getCenter[3] = { selectActor->GetCapsuleComponent()->GetPositionGap().x,selectActor->GetCapsuleComponent()->GetPositionGap().y,selectActor->GetCapsuleComponent()->GetPositionGap().z };

		// Height
		TextBoxInput(CapsuleCollider->Height, getHeight);
		// Radius
		TextBoxInput(CapsuleCollider->Radius, getRadius);

		// TextBox�ɐ��l����
		for (int i = 0; i < 3; i++)
		{
			// Center
			TextBoxInput(CapsuleCollider->Center[i], getCenter[i]);
		}

		selectActor->GetCapsuleComponent()->SetHeight(getHeight);
		selectActor->GetCapsuleComponent()->SetRadius(getRadius);
		selectActor->GetCapsuleComponent()->SetPositionGap(VECTOR(getCenter[0], getCenter[1], getCenter[2]));

		// Tab�������ꂽ�玟��TextBox�Ɉړ�
		if (mTab)
		{
			for (int i = 0; i < 3; i++)
			{
				// Height
				if (CapsuleCollider->Height.Select)
				{
					TabClickInput(CapsuleCollider->Height, CapsuleCollider->Radius);
				}

				// Radius
				if (CapsuleCollider->Radius.Select)
				{
					TabClickInput(CapsuleCollider->Radius, CapsuleCollider->Center[0]);
				}

				// Center
				if (CapsuleCollider->Center[i].Select)
				{
					if (i != 2)
					{
						int num = i + 1;
						TabClickInput(CapsuleCollider->Center[i], CapsuleCollider->Center[num]);
						break;
					}
				}
			}
		}
	}
	// ImageCollider�Ȃ�
	else if (component->Type._Equal(_T("ImageComponent")))
	{
		ImageUI* Image = dynamic_cast<ImageUI*>(component);

		int componentNum = Image->ComponentNum - 1;

		// SourceImage
		if (!mOpenSelectWindow)
		{
			if (RectOnMouse(Image->SelectButton))
			{
				if (MOUSE_ENTER(0))
				{
					mOpenSelectWindow = true;
					int num = (int)mSimpleImageName.size();
					mSelectWindow.SetButton(num);
					mSelectWindow.SetText(mSimpleImageName);
					mSelectWindow.ClickSelectButton.resize(num);
					mSelectWindow.ComponentNum = componentNum;
				}
			}
		}
		else
		{
			if (RectOnMouse(Image->SelectButton))
			{
				if (MOUSE_ENTER(0))
				{
					mOpenSelectWindow = false;
					for (int i = 0, size = (int)mSelectWindow.Button.size(); i < size; ++i)
					{
						RiseUI(mSelectWindow.Button[i]);
						RiseText(mSelectWindow.NameText[i]);
					}
				}
			}
		}


		if (strlen(selectActor->GetImageComponent()[componentNum]->GetImageData()->filename) != 0)
		{
			int i = 0;
			for (auto name : mFolderImageName)
			{
				if (name == selectActor->GetImageComponent()[componentNum]->GetImageData()->filename)
				{
					break;
				}
				++i;
			}
			swprintf(Image->SourceImage.Text.Text, MAX_PATH, mSimpleImageName[i].c_str());
		}
		else
		{
			swprintf(Image->SourceImage.Text.Text, MAX_PATH, _T("NULL"));
		}

		// OrderinLayer
		TextBoxSelect(Image->OrderinLayer);

		// TextBox�ɐ��l����
		for (int i = 0; i < 4; i++)
		{
			// Center
			TextBoxSelect(Image->Color[i]);
		}
		
		// ���͑O�ɏ�����ꍞ��
		float getColor[3] = { selectActor->GetColor().x,selectActor->GetColor().y,selectActor->GetColor().z };
		float getAlpha = selectActor->GetAlpha();
		int getDrawOrder = selectActor->GetImageComponent()[componentNum]->GetDrawOrder();

		// RGB
		for (int i = 0; i < 3; i++)
		{
			getColor[i] *= 255;
			// ���͒��͍X�V���Ȃ�
			if (!Image->Color[i].Select)
			{
				swprintf(Image->Color[i].Text.Text, MAX_PATH, _T("%d"), (int)getColor[i]);
			}
		}
		// ����
		for (int i = 0; i < 3; i++)
		{
			if (Image->Color[i].Select)
			{
				std::wstring Text = Input(Image->Color[i].Text.Text, true, 3).c_str();
				int l = 0;
				int size = (int)Text.size();
				for (; l < size; ++l)
				{
					if (Text[l] != 48)
					{
						break;
					}
				}
				if (l != 0 && size != l)
				{
					for (int k = 0; k < size - 1; ++k)
					{
						// �����ԍ��ȏ�̃{�^������������炷
						if (k >= l - 1)
						{
							int NextButton = k + 1;
							int num = k - (l - 1);
							Text[num] = Text[NextButton];
						}
					}
					int num = size - l;
					Text.resize(num);
				}
				if (size > 0)
				{
					swprintf(Image->Color[i].Text.Text, MAX_PATH, Text.c_str());
				}
				else
				{
					swprintf(Image->Color[i].Text.Text, MAX_PATH, _T("0"));
				}
				break;// ������Ȃ̂�break
			}
		}
		// ���͂������l��߂�
		for (int i = 0; i < 3; i++)
		{
			swscanf_s(Image->Color[i].Text.Text, _T("%f"), &getColor[i]);
			// rgb��0�`255
			if (getColor[i] > 255)
			{
				getColor[i] = 255;
			}
			else if (getColor[i] < 0)
			{
				getColor[i] = 0;
			}
			getColor[i] /= 255;
		}

		// Alpha
		TextBoxInput(Image->Color[3], getAlpha);

		if (getAlpha > 1)
		{
			getAlpha = 1;
		}
		else if (getAlpha < 0)
		{
			getAlpha = 0;
		}

		// OrderinLayer
		TextBoxInput(Image->OrderinLayer, getDrawOrder);
		
		if (getDrawOrder < -2)
		{
			getDrawOrder = -2;
		}

		if (mSelectWindow.Decision && mSelectWindow.ComponentNum == componentNum)
		{
			// SourceImage
			selectActor->GetImageComponent()[componentNum]->SetFileName(mFolderImageName[mSelectWindow.DecisionNum].c_str());
			swprintf(Image->SourceImage.Text.Text, MAX_PATH, mSimpleImageName[mSelectWindow.DecisionNum].c_str());
			mSelectWindow.Decision = false;
			mSelectWindow.ComponentNum = 0;
		}
		selectActor->SetColor(VECTOR(getColor[0], getColor[1], getColor[2]));
		selectActor->SetAlpha(getAlpha);
		selectActor->GetImageComponent()[componentNum]->SetDrawOrder(getDrawOrder);

		// Tab�������ꂽ�玟��TextBox�Ɉړ�
		if (mTab)
		{
			for (int i = 0; i < 4; i++)
			{
				// Color
				if (Image->Color[i].Select)
				{
					if (i == 3)
					{
						TabClickInput(Image->Color[i], Image->OrderinLayer);
						break;
					}
					else
					{
						int num = i + 1;
						TabClickInput(Image->Color[i], Image->Color[num]);
						break;
					}
				}
			}
		}
	}
}
// �R���|�[�l���g�폜
void ObjectCreate::DeleteComponent(int num)
{
	// ----------- Component�{�� ------------
	if (mSelectComponent[num]->ComponentNum == 0)
	{
		mActor[mSelectActorNum]->DeleateComponent(mSelectComponent[num]->Type);
	}
	else
	{
		mActor[mSelectActorNum]->DeleateComponent(mSelectComponent[num]->Type, num);
	}
	// ----------- ComponentWindow ----------
	RiseUI(mSelectComponent[num]->Top);
	RiseUI(mSelectComponent[num]->Bottom);
	RiseText(mSelectComponent[num]->Title);
	RiseUI(mSelectComponent[num]->DownArrow);
	RiseUI(mSelectComponent[num]->RightArrow);
	RiseUI(mSelectComponent[num]->CheckBoxOn);
	RiseUI(mSelectComponent[num]->CheckBoxOff);
	RiseUI(mSelectComponent[num]->XButton);
	mSelectComponent[num]->RiseDraw();
	if (mSelectComponent.size() == 1)
	{
		delete mSelectComponent[0];
	}
	else
	{
		delete mSelectComponent[num];
		for (int i = 0; i < mSelectComponent.size() - 1; i++)
		{
			// �����ԍ��ȏ�̃{�^������������炷
			if (i >= num)
			{
				int NextButton = i + 1;
				// �폜����Window�̍����擾
				float height = (mSelectComponent[i]->Top.bottom * 2) + (mSelectComponent[i]->Bottom.bottom * 2);
				mSelectComponent[i] = mSelectComponent[NextButton];
				mSelectComponent[i]->basePos.y -= height;
				if (mSelectComponent[i]->ComponentNum > 1)
				{
					--mSelectComponent[i]->ComponentNum;
				}
			}
		}
	}

	mSelectComponentNum--;
	mSelectComponent.resize(mSelectComponentNum);
}
// �C���X�y�N�^�[��\��
void ObjectCreate::RiseInspector()
{
	RiseUI(mInspectorWindow.Window);
	RiseUI(mInspectorWindow.Border);
	RiseUI(mInspectorWindow.SelectTextBox);
	RiseUI(mInspectorWindow.PullButton);
	RiseUI(mInspectorWindow.TopFrame.Frame);
	RiseUI(mInspectorWindow.TopFrame.XButton);
	RiseUI(mInspectorWindow.TopFrame.ColorBlue);
	RiseUI(mInspectorWindow.TopFrame.ColorRed);
	RiseUI(mInspectorWindow.TopFrame.ColorGreen);
	RiseUI(mInspectorWindow.TopFrame.NameBox.Box);
	RiseText(mInspectorWindow.TopFrame.NameBox.Text);
	RiseUI(mInspectorWindow.ColorFrame.Frame);
	// 3��rgb
	for (int i = 0; i < 3; i++)
	{
		RiseUI(mInspectorWindow.ColorFrame.rgb[i].Box);
		RiseText(mInspectorWindow.ColorFrame.rgb[i].Text);
	}

	RiseUI(mInspectorWindow.Transform.Top);
	RiseUI(mInspectorWindow.Transform.Bottom);
	RiseUI(mInspectorWindow.Transform.RightArrow);
	RiseUI(mInspectorWindow.Transform.DownArrow);
	RiseUI(mInspectorWindow.Transform.XButton);
	RiseUI(mInspectorWindow.Transform.CheckBoxOn);
	RiseUI(mInspectorWindow.Transform.CheckBoxOff);
	RiseText(mInspectorWindow.Transform.Title);
	// 3��xyz
	for (int i = 0; i < 3; i++)
	{
		RiseUI(mInspectorWindow.Transform.Position[i].Box);
		RiseUI(mInspectorWindow.Transform.Rotation[i].Box);
		RiseUI(mInspectorWindow.Transform.Scale[i].Box);
		RiseText(mInspectorWindow.Transform.Position[i].Text);
		RiseText(mInspectorWindow.Transform.Rotation[i].Text);
		RiseText(mInspectorWindow.Transform.Scale[i].Text);
	}
	for (auto& component : mSelectComponent)
	{
		RiseUI(component->Top);
		RiseUI(component->Bottom);
		RiseText(component->Title);
		RiseUI(component->DownArrow);
		RiseUI(component->RightArrow);
		RiseUI(component->XButton);
		RiseUI(component->CheckBoxOn);
		RiseUI(component->CheckBoxOff);
		component->RiseDraw();
	}
}


// �}�E�X��ui�̋�`�Ƃ̓����蔻��
bool ObjectCreate::RectOnMouse(CreativeUI ui)
{
	if (ui.scale.x < 0)
	{
		ui.scale.x *= -1;
	}
	if (ui.scale.y < 0)
	{
		ui.scale.y *= -1;
	}
	if (mMousePosX >= (ui.position.x + (ui.left * ui.scale.x)) && mMousePosX <= (ui.position.x + (ui.right * ui.scale.x))
		&& mMousePosY >= (ui.position.y + (ui.top * ui.scale.y)) && mMousePosY <= (ui.position.y + (ui.bottom * ui.scale.y)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

// ����(numOnly:true�Ő��������擾)
std::wstring ObjectCreate::Input(const TCHAR* text, bool numOnly , int maxSize)
{
	// �����i�[
	std::wstring InputCharacter = text;
	

	if (KEY_ENTER(NUM1_KEY))
	{
		InputCharacter += _T("1");
	}
	if (KEY_ENTER(NUM2_KEY))
	{
		InputCharacter += _T("2");
	}
	if (KEY_ENTER(NUM3_KEY))
	{
		InputCharacter += _T("3");
	}
	if (KEY_ENTER(NUM4_KEY))
	{
		InputCharacter += _T("4");
	}
	if (KEY_ENTER(NUM5_KEY))
	{
		InputCharacter += _T("5");
	}
	if (KEY_ENTER(NUM6_KEY))
	{
		InputCharacter += _T("6");
	}
	if (KEY_ENTER(NUM7_KEY))
	{
		InputCharacter += _T("7");
	}
	if (KEY_ENTER(NUM8_KEY))
	{
		InputCharacter += _T("8");
	}
	if (KEY_ENTER(NUM9_KEY))
	{
		InputCharacter += _T("9");
	}
	if (KEY_ENTER(NUM0_KEY))
	{
		InputCharacter += _T("0");
	}
	if (KEY_ENTER(PERIOD_KEY))
	{
		InputCharacter += _T(".");
	}
	if (KEY_ENTER(MINUS_KEY))
	{
		InputCharacter += _T("-");
	}
	if (!numOnly)
	{
		// �V�t�g�������Ă���Ƃ�
		bool Shift = false;
		if (KEY(LSHIFT_KEY))
		{
			Shift = true;
		}

		// �V�t�g��������Ă�����啶��
		if (KEY_ENTER(A_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("A");
			}
			else
			{
				InputCharacter += _T("a");
			}
		}
		if (KEY_ENTER(B_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("B");
			}
			else
			{
				InputCharacter += _T("b");
			}
		}
		if (KEY_ENTER(C_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("C");
			}
			else
			{
				InputCharacter += _T("c");
			}
		}
		if (KEY_ENTER(D_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("D");
			}
			else
			{
				InputCharacter += _T("d");
			}
		}
		if (KEY_ENTER(E_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("E");
			}
			else
			{
				InputCharacter += _T("e");
			}
		}
		if (KEY_ENTER(F_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("F");
			}
			else
			{
				InputCharacter += _T("f");
			}
		}
		if (KEY_ENTER(G_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("G");
			}
			else
			{
				InputCharacter += _T("g");
			}
		}
		if (KEY_ENTER(H_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("H");
			}
			else
			{
				InputCharacter += _T("h");
			}
		}
		if (KEY_ENTER(I_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("I");
			}
			else
			{
				InputCharacter += _T("i");
			}
		}
		if (KEY_ENTER(J_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("J");
			}
			else
			{
				InputCharacter += _T("j");
			}
		}
		if (KEY_ENTER(K_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("K");
			}
			else
			{
				InputCharacter += _T("k");
			}
		}
		if (KEY_ENTER(L_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("L");
			}
			else
			{
				InputCharacter += _T("l");
			}
		}
		if (KEY_ENTER(M_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("M");
			}
			else
			{
				InputCharacter += _T("m");
			}
		}
		if (KEY_ENTER(N_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("N");
			}
			else
			{
				InputCharacter += _T("n");
			}
		}
		if (KEY_ENTER(O_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("O");
			}
			else
			{
				InputCharacter += _T("o");
			}
		}
		if (KEY_ENTER(P_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("P");
			}
			else
			{
				InputCharacter += _T("p");
			}
		}
		if (KEY_ENTER(Q_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("Q");
			}
			else
			{
				InputCharacter += _T("q");
			}
		}
		if (KEY_ENTER(R_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("R");
			}
			else
			{
				InputCharacter += _T("r");
			}
		}
		if (KEY_ENTER(S_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("S");
			}
			else
			{
				InputCharacter += _T("s");
			}
		}
		if (KEY_ENTER(T_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("T");
			}
			else
			{
				InputCharacter += _T("t");
			}
		}
		if (KEY_ENTER(U_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("U");
			}
			else
			{
				InputCharacter += _T("u");
			}
		}
		if (KEY_ENTER(V_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("V");
			}
			else
			{
				InputCharacter += _T("v");
			}
		}
		if (KEY_ENTER(W_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("W");
			}
			else
			{
				InputCharacter += _T("w");
			}
		}
		if (KEY_ENTER(X_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("X");
			}
			else
			{
				InputCharacter += _T("x");
			}
		}
		if (KEY_ENTER(Y_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("Y");
			}
			else
			{
				InputCharacter += _T("y");
			}
		}
		if (KEY_ENTER(Z_KEY))
		{
			if (Shift)
			{
				InputCharacter += _T("Z");
			}
			else
			{
				InputCharacter += _T("z");
			}
		}
	}

	// ============ BackSpace ================
	// ����������
	if (mInputBackSpace <= 1)
	{
		if (KEY_ENTER(BACK_KEY))
		{
			mBackSpace = true;
			if (InputCharacter.size() > 0)
			{
				InputCharacter.erase(--InputCharacter.end());
			}
		}
	}
	else
	{
		if (KEY(BACK_KEY))
		{
			if (InputCharacter.size() > 0)
			{
				InputCharacter.erase(--InputCharacter.end());
			}
		}
	}
	if (KEY_EXIT(BACK_KEY))
	{
		mBackSpace = false;
		mInputBackSpace = 0;
	}
	if (mBackSpace)
	{
		mInputBackSpace += g_elapsedTime;
	}

	// ============ Tab ================
	if (KEY_ENTER(TAB_KEY))
	{
		mTab = true;
	}

	while (InputCharacter.size()>maxSize)
	{
		InputCharacter.erase(--InputCharacter.end());
	}

	return InputCharacter;
}

// �w��̃t�H���_�p�X�̒��g�𒲂ׂ�
bool ObjectCreate::GetModelFileNames(std::wstring folderPath, std::vector<std::wstring>& file_names)			
{
	HANDLE hFind;
	WIN32_FIND_DATA win32fd;
	std::wstring search_name = folderPath + _T("\\*");

	hFind = FindFirstFile(search_name.c_str(), &win32fd);

	if (hFind == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("file not found");
		return false;
	}

	// �w��̃f�B���N�g���ȉ��̃t�@�C�������t�@�C�����Ȃ��Ȃ�܂Ŏ擾����
	do {
		if (!(win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			// �t�@�C��������������Vector�z��ɕۑ�����
			file_names.push_back(win32fd.cFileName);
			// ���������o��
			auto laseChar = file_names.back().c_str()[file_names.back().size() - 1];
			// obj
			if (laseChar != 'j')
			{
				file_names.pop_back();
			}
		}
	} while (FindNextFile(hFind, &win32fd));

	FindClose(hFind);

	mFolderModelName.resize(file_names.size());
	// �������鏊�ɓn�����߂�path�����
	for (int i = 0, size = (int)file_names.size(); i < size; ++i)
	{
		std::wstring path = mModelFolderPath + _T("\\") + file_names[i];
		// wstring �� SJIS
		int iBufferSize = WideCharToMultiByte(CP_OEMCP, 0, path.c_str()
			, -1, (char*)NULL, 0, NULL, NULL);
		// �o�b�t�@�̎擾
		CHAR* cpMultiByte = new CHAR[iBufferSize];
		// wstring �� SJIS
		WideCharToMultiByte(CP_OEMCP, 0, path.c_str(), -1, cpMultiByte
			, iBufferSize, NULL, NULL);
		// string�̐���
		mFolderModelName[i].assign(cpMultiByte, cpMultiByte + iBufferSize - 1);
		delete[] cpMultiByte;
	}

	mSimpleModelName.resize(file_names.size());
	for (int i = 0, size = (int)file_names.size(); i < size; ++i)
	{
		const size_t strLen = file_names[i].length();
		// .��������4����������
		mSimpleModelName[i] = file_names[i].substr(0, strLen - 4);
	}

	return true;
}

// �w��̉摜�t�H���_�p�X�̒��g�𒲂ׂ�
bool ObjectCreate::GetImageFileNames(std::wstring folderPath, std::vector<std::wstring>& file_names)
{
	HANDLE hFind;
	WIN32_FIND_DATA win32fd;
	std::wstring search_name = folderPath + _T("\\*");

	hFind = FindFirstFile(search_name.c_str(), &win32fd);

	if (hFind == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("file not found");
		return false;
	}

	// �w��̃f�B���N�g���ȉ��̃t�@�C�������t�@�C�����Ȃ��Ȃ�܂Ŏ擾����
	do {
		if (!(win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			// �t�@�C��������������Vector�z��ɕۑ�����
			file_names.push_back(win32fd.cFileName);
			// ���������o��
			auto laseChar = file_names.back().c_str()[file_names.back().size() - 1];
			// jpg,png
			if (laseChar != 'G'&& laseChar != 'g')
			{
				file_names.pop_back();
			}
		}
	} while (FindNextFile(hFind, &win32fd));

	FindClose(hFind);

	mFolderImageName.resize(file_names.size());
	// �������鏊�ɓn�����߂�path�����
	for (int i = 0, size = (int)file_names.size(); i < size; ++i)
	{
		std::wstring path = mImageFolderPath + _T("\\") + file_names[i];
		// wstring �� SJIS
		int iBufferSize = WideCharToMultiByte(CP_OEMCP, 0, path.c_str()
			, -1, (char*)NULL, 0, NULL, NULL);
		// �o�b�t�@�̎擾
		CHAR* cpMultiByte = new CHAR[iBufferSize];
		// wstring �� SJIS
		WideCharToMultiByte(CP_OEMCP, 0, path.c_str(), -1, cpMultiByte
			, iBufferSize, NULL, NULL);
		// string�̐���
		mFolderImageName[i].assign(cpMultiByte, cpMultiByte + iBufferSize - 1);
		delete[] cpMultiByte;
	}

	mSimpleImageName.resize(file_names.size());
	for (int i = 0, size = (int)file_names.size(); i < size; ++i)
	{
		const size_t strLen = file_names[i].length();
		// .��������4����������
		mSimpleImageName[i] = file_names[i].substr(0, strLen - 4);
	}

	return true;
}

// text�t�@�C�������[�h
void ObjectCreate::LoadText(const TCHAR filename[])
{
	// wstring �� SJIS
	int iBufferSize = WideCharToMultiByte(CP_OEMCP, 0, filename
		, -1, (char*)NULL, 0, NULL, NULL);
	// �o�b�t�@�̎擾
	CHAR* cpMultiByte = new CHAR[iBufferSize];
	// wstring �� SJIS
	WideCharToMultiByte(CP_OEMCP, 0, filename, -1, cpMultiByte
		, iBufferSize, NULL, NULL);
	// string�̐���
	std::string newFilename;
	newFilename.assign(cpMultiByte, cpMultiByte + iBufferSize - 1);
	delete[] cpMultiByte;

	FILE* fp = nullptr;	// �t�@�C���|�C���^�[

	// �t�@�C�����J����
	fopen_s(&fp, newFilename.c_str(), "rt");

	if (fp == nullptr)
	{
		OutputDebugString(_T("�t�@�C���̓ǂݍ��݂̎��s\n"));
	}
	else
	{
		CHAR buf[MAX_PATH] = ("");
		TCHAR line[MAX_PATH] = _T("");
		TCHAR keyWord[MAX_PATH] = _T("");
		std::wstring componentName;
		while (!feof(fp))
		{
			::ZeroMemory(buf, sizeof(buf));
			::ZeroMemory(line, sizeof(line));
			::ZeroMemory(keyWord, sizeof(keyWord));
			fgets(buf, MAX_PATH, fp);

#ifdef UNICODE
			MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, buf, (int)strlen(buf), line, (sizeof line) / 2);
#else
			strcpy(line, buf);
#endif
			// �]���ȋ󔒂�ǂݔ�΂�
			INT size = lstrlenW(line);

			for (INT i = 0; i < size; ++i)
			{
				if (line[i] != ' ')
				{
					memcpy_s(keyWord, MAX_PATH, &line[i], sizeof(TCHAR) * size);
					keyWord[size - 1] = '\0';
					break;
				}
			}

			// #,//:�R�����g�A\n:���s�R�[�h
			if (keyWord[0] == '#' ||
				keyWord[0] == '\n'||
				keyWord[0] == '\0'||
				(keyWord[0] == '/' &&
				keyWord[1] == '/'))
			{
				int a = 0;
			}
			else if (keyWord[0] == '2' && keyWord[1] == 'D')
			{
				mHierarchyWindow.m2D = true;
			}
			else if (keyWord[0] == 't' && keyWord[1] == ' ')
			{
				// Actor�����擾
				TCHAR name[MAX_PATH] = {};

				// 2�����ǂݍ��݈ʒu���ړ�������
				for (INT i = 2; i < size; ++i)
				{
					if (keyWord[i] != ' ')
					{
						UINT copySize = static_cast<UINT>(size - i);
						memcpy_s(name, MAX_PATH, &keyWord[i], sizeof(TCHAR) * copySize);
						break;
					}
				}

				AddObj(name);
			}
			else if (keyWord[0] == 'n' && keyWord[1] == ' ')
			{
				// obj�����擾
				TCHAR name[MAX_PATH] = {};

				// 2�����ǂݍ��݈ʒu���ړ�������
				for (INT i = 2; i < size; ++i)
				{
					if (keyWord[i] != ' ')
					{
						UINT copySize = static_cast<UINT>(size - i);
						memcpy_s(name, MAX_PATH, &keyWord[i], sizeof(TCHAR) * copySize);
						break;
					}
				}
				if (mHierarchyWindow.m2D)
				{
					swprintf(mHierarchyWindow.Button2D.back().Text.Text, MAX_PATH, name);
				}
				else
				{
					swprintf(mHierarchyWindow.Button3D.back().Text.Text, MAX_PATH, name);
				}
			}
			else if (keyWord[0] == 'c' && keyWord[1] == 'o' && keyWord[2] == 'l' && keyWord[3] == ' ')
			{
				INT ColorNum = 0;
				INT count = 0;

				count = swscanf_s(keyWord, _T("%*s%d"), &ColorNum);
				
				if (count == 1)
				{
					if (mHierarchyWindow.m2D)
					{
						mHierarchyWindow.Button2D.back().ColorNum = ColorNum;
					}
					else
					{
						mHierarchyWindow.Button3D.back().ColorNum = ColorNum;
					}
				}
			}
			else if (keyWord[0] == 'p'&& keyWord[1] == ' ')
			{
				VECTOR position = {};
				INT count = 0;

				count = swscanf_s(keyWord, _T("%*s%f%f%f")
					, &position.x, &position.y, &position.z);
				
				if (count == 3)
				{
					mActor.back()->SetPosition(position);
				}
			}
			else if (keyWord[0] == 'r' && keyWord[1] == ' ')
			{
				VECTOR rotation = {};
				INT count = 0;

				count = swscanf_s(keyWord, _T("%*s%f%f%f")
					, &rotation.x, &rotation.y, &rotation.z);

				if (count == 3)
				{
					mActor.back()->SetRotation(rotation);
				}
			}
			else if (keyWord[0] == 's' && keyWord[1] == ' ')
			{
				VECTOR scale = {};
				INT count = 0;

				count = swscanf_s(keyWord, _T("%*s%f%f%f")
					, &scale.x, &scale.y, &scale.z);

				if (count == 3)
				{
					mActor.back()->SetScale(scale);
				}
			}
			else if (keyWord[0] == 'c' && keyWord[1] == ' ')
			{
				VECTOR color = {};
				INT count = 0;

				count = swscanf_s(keyWord, _T("%*s%f%f%f")
					, &color.x, &color.y, &color.z);

				if (count == 3)
				{
					mActor.back()->SetColor(color);
				}
			}
			else if (keyWord[0] == 'a' && keyWord[1] == ' ')
			{
				FLOAT alpha = 0;
				INT count = 0;

				count = swscanf_s(keyWord, _T("%*s%f"), &alpha);

				if (count == 1)
				{
					mActor.back()->SetAlpha(alpha);
				}
			}
			// =============== InputComponent ==================
			else if (keyWord[0] == 'P' && keyWord[1] == 'l' && keyWord[2] == 'a' && keyWord[3] == 'y' && keyWord[4] == 'e' && keyWord[5] == 'r' &&
					keyWord[6] == 'I' && keyWord[7] == 'n' && keyWord[8] == 'p' && keyWord[9] == 'u' && keyWord[10] == 't')
			{
				componentName = keyWord;
				AddComponent(mActor.back(), componentName);
			}
			else if (keyWord[0] == 'm' && keyWord[1] == 's')
			{
				FLOAT MoveSpeed = 0;
				INT count = 0;

				count = swscanf_s(keyWord, _T("%*s%f"), &MoveSpeed);

				if (count == 1)
				{
					mActor.back()->GetInput()->SetSpeed(MoveSpeed);
				}
			}
			// =============== MeshComponent ==================
			else if (keyWord[0] == 'M' && keyWord[1] == 'e' && keyWord[2] == 's' && keyWord[3] == 'h')
			{
				// Actor�����擾
				TCHAR componentName[MAX_PATH] = {};
				// ObjType���擾
				TCHAR objType[MAX_PATH] = {};

				// �ǂݍ��݈ʒu���ړ�������
				for (INT i = 0; i < size; ++i)
				{
					if (keyWord[i] == ' ')
					{
						UINT copySize = static_cast<UINT>(i);
						memcpy_s(componentName, MAX_PATH, &keyWord[0], sizeof(TCHAR) * copySize);
						copySize = static_cast<UINT>(size - i - 2);
						memcpy_s(objType, MAX_PATH, &keyWord[i + 1], sizeof(TCHAR) * copySize);
						break;
					}
				}

				AddComponent(mActor.back(), componentName);
				
				int num = 0;
				swscanf_s(objType, _T("%d"), &num);
				switch (num)
				{
				case 0:
					mActor.back()->GetMeshComponent()->SetObjectType(ObjectType::Box);
					break;
				case 1:
					mActor.back()->GetMeshComponent()->SetObjectType(ObjectType::Cylinder);
					break;
				case 2:
					mActor.back()->GetMeshComponent()->SetObjectType(ObjectType::Sphere);
					break;
				}
				mActor.back()->GetMeshComponent()->SetObject();
			}
			else if (keyWord[0] == 'M' && keyWord[1] == 'o' && keyWord[2] == 'd' && keyWord[3] == 'e' && keyWord[4] == 'l'&&
			keyWord[5] == 'C' && keyWord[6] == 'o' && keyWord[7] == 'm' && keyWord[8] == 'p' && keyWord[9] == 'o')
			{
				// Actor�����擾
				TCHAR componentName[MAX_PATH] = {};
				// File�����擾
				TCHAR filePath[MAX_PATH] = {};

				// �ǂݍ��݈ʒu���ړ�������
				for (INT i = 0; i < size; ++i)
				{
					if (keyWord[i] == ' ')
					{
						UINT copySize = static_cast<UINT>(i);
						memcpy_s(componentName, MAX_PATH, &keyWord[0], sizeof(TCHAR) * copySize);
						copySize = static_cast<UINT>(size - i - 2);
						memcpy_s(filePath, MAX_PATH, &keyWord[i + 1], sizeof(TCHAR) * copySize);
						break;
					}
				}

				// wstring �� SJIS
				int iBufferSize = WideCharToMultiByte(CP_OEMCP, 0, filePath
					, -1, (char*)NULL, 0, NULL, NULL);
				// �o�b�t�@�̎擾
				CHAR* cpMultiByte = new CHAR[iBufferSize];
				// wstring �� SJIS
				WideCharToMultiByte(CP_OEMCP, 0, filePath, -1, cpMultiByte
					, iBufferSize, NULL, NULL);

				for (auto& path : mFolderModelName)
				{
					if (path._Equal(cpMultiByte))
					{
						AddComponent(mActor.back(), componentName, path.c_str());
						break;
					}
				}
				delete[] cpMultiByte;
			}
			// =============== BoxComponent ==================
			else if (keyWord[0] == 'B' && keyWord[1] == 'o' && keyWord[2] == 'x' && 
			keyWord[3] == 'C' && keyWord[4] == 'o' && keyWord[5] == 'l' && keyWord[6] == 'l' && keyWord[7] == 'i'&&
			keyWord[8] == 'd' && keyWord[9] == 'e' && keyWord[10] == 'r')
			{
				componentName = keyWord;
				AddComponent(mActor.back(), componentName);
			}
			// =============== CapsuleComponent ==================
			else if (keyWord[0] == 'C' && keyWord[1] == 'a' && keyWord[2] == 'p' &&
				keyWord[3] == 's' && keyWord[4] == 'u' && keyWord[5] == 'l' && keyWord[6] == 'e' &&
				keyWord[7] == 'C' && keyWord[8] == 'o' && keyWord[9] == 'l' && keyWord[10] == 'l' && keyWord[11] == 'i' &&
				keyWord[12] == 'd' && keyWord[13] == 'e' && keyWord[14] == 'r')
			{
				componentName = keyWord;
				AddComponent(mActor.back(), componentName);
			}
			else if (keyWord[0] == 's' && keyWord[1] == 'i')
			{
				VECTOR ColliderSize = {};
				INT count = 0;

				count = swscanf_s(keyWord, _T("%*s%f%f%f")
					, &ColliderSize.x, &ColliderSize.y, &ColliderSize.z);

				if (count == 3)
				{
					if (mActor.back()->GetBoxComponent())
					{
					}
				}
			}
			else if (keyWord[0] == 'c' && keyWord[1] == 'e')
			{
				VECTOR ColliderCenter = {};
				INT count = 0;

				count = swscanf_s(keyWord, _T("%*s%f%f%f")
					, &ColliderCenter.x, &ColliderCenter.y, &ColliderCenter.z);

				if (count == 3)
				{
					if (mActor.back()->GetBoxComponent())
					{
						mActor.back()->GetBoxComponent()->SetPositionGap(ColliderCenter);
					}
					else if (mActor.back()->GetCapsuleComponent())
					{
						mActor.back()->GetCapsuleComponent()->SetPositionGap(ColliderCenter);
					}
				}
			}
			else if (keyWord[0] == 'r' && keyWord[1] == 'a')
			{
				FLOAT Radius = 0;
				INT count = 0;

				count = swscanf_s(keyWord, _T("%*s%f"), &Radius);

				if (count == 1)
				{
					if (mActor.back()->GetCapsuleComponent())
					{
						mActor.back()->GetCapsuleComponent()->SetRadius(Radius);
					}
				}
			}
			else if (keyWord[0] == 'h' && keyWord[1] == 'e')
			{
				FLOAT Height = 0;
				INT count = 0;

				count = swscanf_s(keyWord, _T("%*s%f"), &Height);

				if (count == 1)
				{
					if (mActor.back()->GetCapsuleComponent())
					{
						mActor.back()->GetCapsuleComponent()->SetHeight(Height);
					}
				}
			}
			// =============== RigidBodyComponent ==================
			else if (keyWord[0] == 'R' && keyWord[1] == 'i' && keyWord[2] == 'g' && keyWord[3] == 'i' && keyWord[4] == 'd' && 
			keyWord[5] == 'B' && keyWord[6] == 'o' && keyWord[7] == 'd' && keyWord[8] == 'y')
			{
				componentName = keyWord;
				AddComponent(mActor.back(), componentName);
			}
			// =============== ImageComponent ==================
			else if (keyWord[0] == 'I' && keyWord[1] == 'm' && keyWord[2] == 'a' && keyWord[3] == 'g' && keyWord[4] == 'e')
			{
				// Actor�����擾
				TCHAR componentName[MAX_PATH] = {};
				// File�����擾
				TCHAR filePath[MAX_PATH] = {};

				// �ǂݍ��݈ʒu���ړ�������
				for (INT i = 0; i < size; ++i)
				{
					if (keyWord[i] == ' ')
					{
						UINT copySize = static_cast<UINT>(i);
						memcpy_s(componentName, MAX_PATH, &keyWord[0], sizeof(TCHAR) * copySize);
						copySize = static_cast<UINT>(size - i - 2);
						memcpy_s(filePath, MAX_PATH, &keyWord[i + 1], sizeof(TCHAR)* copySize);
						break;
					}
				}

				// wstring �� SJIS
				int iBufferSize = WideCharToMultiByte(CP_OEMCP, 0, filePath
					, -1, (char*)NULL, 0, NULL, NULL);
				// �o�b�t�@�̎擾
				CHAR* cpMultiByte = new CHAR[iBufferSize];
				// wstring �� SJIS
				WideCharToMultiByte(CP_OEMCP, 0, filePath, -1, cpMultiByte
					, iBufferSize, NULL, NULL);

				for (auto& path : mFolderImageName)
				{
					if (path._Equal(cpMultiByte))
					{
						AddComponent(mActor.back(), componentName, path.c_str());
						break;
					}
				}
				delete[] cpMultiByte;
			}
			else if (keyWord[0] == 'd' && keyWord[1] == 'o' && keyWord[2] == ' ')
			{
				INT order = 0;
				INT count = 0;

				count = swscanf_s(keyWord, _T("%*s%d"), &order);

				if (count == 1)
				{
					if (mActor.back()->GetImageComponent().back())
					{
						mActor.back()->GetImageComponent().back()->SetDrawOrder(order);
					}
				}
			}
			// =============== ���̑� ==================
			else
			{
				componentName = keyWord;
				if (mActor.size() > 0)
				{
					AddComponent(mActor.back(), componentName);
				}
			}
		}
		// �t�@�C�������
		fclose(fp);
	}
	mHierarchyWindow.m2D = false;
	mSavePosition.clear();
	mSaveRotation.clear();
	// Actor�̈ʒu�ۑ�
	for (auto actor : mActor)
	{
		mSavePosition.push_back(actor->GetPosition());
	}
	// Actor�̉�]�ۑ�
	for (auto actor : mActor)
	{
		mSaveRotation.push_back(actor->GetRotation());
	}

	mActorName.clear();
	// mActorName�X�V
	for (auto& button : mHierarchyWindow.Button2D)
	{
		mActorName[button.Text.Text] = button.addNum;
	}
	for (auto& button : mHierarchyWindow.Button3D)
	{
		mActorName[button.Text.Text] = button.addNum;
	}
	mGame->SetActorName(mActorName);
}

// text�t�@�C�����Z�[�u
void ObjectCreate::SaveText()
{
	// .txt�t�@�C���ɕۑ�
	FILE* fp = nullptr;
	fopen_s(&fp, "Resouse\\�Z�[�u.txt", "wt, ccs=UTF-8");

	// �S���
	TCHAR txt[MAX_PATH * 2] = {};

	if (fp == nullptr)
	{
		OutputDebugString(_T("�t�@�C���̓ǂݍ��݂̎��s\n"));
	}
	else
	{
		fputws(L"// ===== 3D =====\n", fp);
		fputws(L"3D\n", fp);
		// 3DObject�̕ۑ�
		for (auto button : mHierarchyWindow.Button3D)
		{
			Actor* actor = mActor[button.addNum];
			// InspectorWindow����Transform�A�F���
			swprintf(txt, MAX_PATH, _T("t %s\nn %s\ncol %d\np %06f %06f %06f\nr %06f %06f %06f\ns %06f %06f %06f\nc %06f %06f %06f\na %06f\n"),
				button.ObjType.c_str(),
				button.Text.Text,
				button.ColorNum,
				actor->GetPosition().x, actor->GetPosition().y, actor->GetPosition().z,
				actor->GetRotation().x, actor->GetRotation().y, actor->GetRotation().z,
				actor->GetScale().x, actor->GetScale().y, actor->GetScale().z,
				actor->GetColor().x, actor->GetColor().y, actor->GetColor().z,
				actor->GetAlpha()
			);

			// �ȉ��eComponent���
			for (auto uc : actor->GetUse3DComponent())
			{
				// component���
				TCHAR ctxt[MAX_PATH] = {};
				if (uc._Equal(_T("PlayerInput")))
				{
					if (actor->GetComponent(uc))
					{
						swprintf(ctxt, MAX_PATH, _T("%s\nms %06f\n"),
							uc.c_str(),
							actor->GetInput()->GetSpeed()
						);
					}
				}
				else if (uc._Equal(_T("MeshComponent")))
				{
					if (actor->GetComponent(uc))
					{
						swprintf(ctxt, MAX_PATH, _T("%s %d\n"),
							uc.c_str(),
							actor->GetMeshComponent()->GetObjectType()
						);
					}
				}
				else if (uc._Equal(_T("ModelComponent")))
				{
					if (actor->GetComponent(uc))
					{
						TCHAR pFileName[256] = {};
#ifdef UNICODE
						MultiByteToWideChar(CP_OEMCP, MB_PRECOMPOSED, actor->GetModelComponent()->GetFileName(), (int)strlen(actor->GetModelComponent()->GetFileName()), pFileName, (sizeof pFileName) / 2);
#else
						strcpy(pFileName, actor->GetModelComponent()->GetFileName());
#endif
						swprintf(ctxt, MAX_PATH, _T("%s %s\n"),
							uc.c_str(),
							pFileName
						);
					}
				}
				else if (uc._Equal(_T("BoxCollider")))
				{
					if (actor->GetComponent(uc))
					{
						swprintf(ctxt, MAX_PATH, _T("%s\nsi %06f %06f %06f\nce %06f %06f %06f\n"),
							uc.c_str(),
							0.f,
							0.f,
							0.f,
							actor->GetBoxComponent()->GetPositionGap().x,
							actor->GetBoxComponent()->GetPositionGap().y,
							actor->GetBoxComponent()->GetPositionGap().z
						);
					}
				}
				else if (uc._Equal(_T("CapsuleCollider")))
				{
					if (actor->GetComponent(uc))
					{
						swprintf(ctxt, MAX_PATH, _T("%s\nhe %06f\nra %06f\nce %06f %06f %06f\n"),
							uc.c_str(),
							actor->GetCapsuleComponent()->GetHeight(),
							actor->GetCapsuleComponent()->GetRadius(),
							actor->GetCapsuleComponent()->GetPositionGap().x,
							actor->GetCapsuleComponent()->GetPositionGap().y,
							actor->GetCapsuleComponent()->GetPositionGap().z
						);
					}
				}
				else if (uc._Equal(_T("RigidBody")))
				{
					if (actor->GetComponent(uc))
					{
						swprintf(ctxt, MAX_PATH, _T("%s\n"), uc.c_str());
					}
				}
				else
				{
					if (actor->GetComponent(uc))
					{
						swprintf(ctxt, MAX_PATH, _T("%s\n"), uc.c_str());
					}
				}
				wcscat_s(txt, ctxt);
			}
			fputws(txt, fp);
			fputws(L"\n", fp);
		}

		fputws(L"// ===== 2D =====\n", fp);
		fputws(L"2D\n", fp);
		// 2DObject�̕ۑ�
		for (auto button : mHierarchyWindow.Button2D)
		{
			Actor* actor = mActor[button.addNum];
			// InspectorWindow����Transform�A�F���
			swprintf(txt, MAX_PATH, _T("t %s\nn %s\ncol %d\np %06f %06f %06f\nr %06f %06f %06f\ns %06f %06f %06f\nc %06f %06f %06f\na %06f\n"),
				button.ObjType.c_str(),
				button.Text.Text,
				button.ColorNum,
				actor->GetPosition().x, actor->GetPosition().y, actor->GetPosition().z,
				actor->GetRotation().x, actor->GetRotation().y, actor->GetRotation().z,
				actor->GetScale().x, actor->GetScale().y, actor->GetScale().z,
				actor->GetColor().x, actor->GetColor().y, actor->GetColor().z,
				actor->GetAlpha()
			);

			// �ȉ��eComponent���
			for (auto uc : actor->GetUse2DComponent())
			{
				// component���
				TCHAR ctxt[MAX_PATH] = {};
				if (uc._Equal(_T("ImageComponent")))
				{
					for(auto comp: actor->GetImageComponent())
					{
						TCHAR pFileName[256] = {};
#ifdef UNICODE
						MultiByteToWideChar(CP_OEMCP, MB_PRECOMPOSED, comp->GetImageData()->filename, (int)strlen(comp->GetImageData()->filename), pFileName, (sizeof pFileName) / 2);
#else
						strcpy(pFileName, actor->GetImageComponent()->GetImageData()->filename);
#endif
						swprintf(ctxt, MAX_PATH, _T("%s %s\ndo %d\n"),
							uc.c_str(),
							pFileName,
							comp->GetDrawOrder()
						);
						wcscat_s(txt, ctxt);
					}
				}
				else
				{
					if (actor->GetComponent(uc))
					{
						swprintf(ctxt, MAX_PATH, _T("%s\n"), uc.c_str());
					}
					wcscat_s(txt, ctxt);
				}
			}
			fputws(txt, fp);
			fputws(L"\n", fp);
		}
		fclose(fp);
	}
}

// ��ɂ���t���[������ɂ��č��̃t���[���̈ʒu�擾
VECTOR2 ObjectCreate::InspectorFramePos(CreativeUI nowFrame, CreativeUI PrevFrame)
{
	// PrevFrame�ʒu����Ƃ��Ĉʒu���o��
	// �����͎����̍���/2����PrevFrame�̍���/2
	return PrevFrame.position + VECTOR2(0, PrevFrame.bottom * PrevFrame.scale.y + nowFrame.bottom * nowFrame.scale.y);
}

// TextBox�̑I��
void ObjectCreate::TextBoxSelect(TextBox& textBox)
{
	if (RectOnMouse(textBox.Box))
	{
		if (MOUSE_ENTER(0))
		{
			textBox.Select = true;
			mInspectorWindow.SelectTextBox.position = textBox.Box.position;
			mInspectorWindow.SelectTextBox.scale = textBox.Box.scale;
			RiseUI(textBox.Box);
		}
	}
	else
	{
		if (MOUSE_ENTER(0))
		{
			textBox.Select = false;
			RiseUI(mInspectorWindow.SelectTextBox);
		}
	}
	if (MOUSE_ENTER(1) || MOUSE_ENTER(2) || KEY_ENTER(RETURN_KEY))
	{
		textBox.Select = false;
		RiseUI(mInspectorWindow.SelectTextBox);
	}
}

// TextBox�̓��͏���(int)
void ObjectCreate::TextBoxInput(TextBox& textBox, int& value)
{
	float num = 0;

	// ���͒��͍X�V���Ȃ�
	if (!textBox.Select)
	{
		swprintf(textBox.Text.Text, MAX_PATH, _T("%d"), value);
	}
	// ����
	else
	{
		std::wstring Text = Input(textBox.Text.Text, true, 6).c_str();
		int i = 0;
		int size = (int)Text.size();
		for (; i < size; ++i)
		{
			if (Text[i] != 48)
			{
				break;
			}
		}
		if (i != 0 && size != i)
		{
			for (int k = 0; k < size - 1; ++k)
			{
				// �����ԍ��ȏ�̃{�^������������炷
				if (k >= i - 1)
				{
					int NextButton = k + 1;
					int num = k - (i - 1);
					Text[num] = Text[NextButton];
				}
			}
			int num = size - i;
			Text.resize(num);
		}
		if (size > 0)
		{
			swprintf(textBox.Text.Text, MAX_PATH, Text.c_str());
		}
		else
		{
			swprintf(textBox.Text.Text, MAX_PATH, _T("0"));
		}
	}

	// ���͂������l��߂�
	swscanf_s(textBox.Text.Text, _T("%d"), &value);
}

// TextBox�̓��͏���(float)
void ObjectCreate::TextBoxInput(TextBox& textBox, float& value)
{
	float num = 0;

	// ���͒��͍X�V���Ȃ�
	if (!textBox.Select)
	{
		num =  value * 1000;
		// 1000�Ŋ���؂��ΐ���
		if (((int)num % 1000) == 0)
		{
			swprintf(textBox.Text.Text, MAX_PATH, _T("%d"), (int)value);
		}
		// 100�Ŋ���؂��Ώ����_����
		else if (((int)num % 100) == 0)
		{
			swprintf(textBox.Text.Text, MAX_PATH, _T("%.1f"), value);
		}
		// 10�Ŋ���؂��Ώ����_����
		else if (((int)num % 10) == 0)
		{
			swprintf(textBox.Text.Text, MAX_PATH, _T("%.2f"), value);
		}
		// ����ȊO�͏����_��O��
		else
		{
			swprintf(textBox.Text.Text, MAX_PATH, _T("%.3f"), value);
		}
	}	
	// ����
	else
	{
		std::wstring Text = Input(textBox.Text.Text, true, 6).c_str();
		int i = 0;
		int size = (int)Text.size();
		for (; i < size; ++i)
		{
			if (Text[i] != 48)
			{
				break;
			}
		}
		if (i != 0 && size != i)
		{
			for (int k = 0; k < size - 1; ++k)
			{
				// �����ԍ��ȏ�̃{�^������������炷
				if (k >= i - 1)
				{
					int NextButton = k + 1;
					int num = k - (i - 1);
					Text[num] = Text[NextButton];
				}
			}
			int num = size - i;
			Text.resize(num);
		}
		if (size > 0)
		{
			swprintf(textBox.Text.Text, MAX_PATH, Text.c_str());
		}
		else
		{
			swprintf(textBox.Text.Text, MAX_PATH, _T("0"));
		}
	}

	// ���͂������l��߂�
	swscanf_s(textBox.Text.Text, _T("%f"), &value);	
}

// ���I�𒆂�TextBox���玟��TextBox�Ɉڂ�
void ObjectCreate::TabClickInput(TextBox& nowSelect, TextBox& nextSelect)
{
	nowSelect.Select = false;
	nextSelect.Select = true;
	mInspectorWindow.SelectTextBox.position = nextSelect.Box.position;
	mInspectorWindow.SelectTextBox.scale = nextSelect.Box.scale;
	RiseUI(nextSelect.Box);
}

// �|���S���Ɛ��̏Փ�
bool ObjectCreate::HitCheckPolygonLine(std::vector<VECTOR> vertices, VECTOR rayStartPos, VECTOR rayEndPos, VECTOR& crossPoint)
{
	const UINT polygonSize = 3;

	VECTOR ray = rayEndPos - rayStartPos;
	// �ʂɊ܂܂��_���璼���̓�_�Ɍ�����Vector
	VECTOR FaceRay1 = rayStartPos - vertices[0];
	// ���ʂ̕�����
	// v1������2���������߂�
	VECTOR v10 = vertices[0] - vertices[1];
	VECTOR v12 = vertices[2] - vertices[1];
	// �O�ς𗘗p���A�@�����擾
	VECTOR normal = cross(v10, v12);
	float dot1 = dot(&ray, &normal);
	float dot2 = dot(&FaceRay1, &normal);
	if (dot1 != 0 || dot2 == 0)
	{
		VECTOR FaceRay2 = rayEndPos - vertices[0];
		float dot3 = dot(&FaceRay2, &normal);
		if (dot2 * dot3 <= 0)
		{
			float a = normal.x;
			float b = normal.y;
			float c = normal.z;
			// �R���_�̂ǂꂩ�̍��W����
			float d = -(a * vertices[0].x + b * vertices[0].y + c * vertices[0].z);


			float t = -(a * rayStartPos.x + b * rayStartPos.y + c * rayStartPos.z
				+ d) / (a * ray.x + b * ray.y + c * ray.z);

			crossPoint = rayStartPos + ray * t;
			// ��_�����C��ɑ��݂��邩�m�F
			bool onLine = false;
			if (dot(crossPoint - rayStartPos, crossPoint - rayEndPos) < 0.0f)
			{
				onLine = true;
			}

			// ��_���|���S���ɓ���Ă邩�m�F
			if (onLine)
			{
				// �|���S���̊O���x�N�g��
				VECTOR outLine[polygonSize] = {};
				for (int i = 0; i < polygonSize; ++i)
				{
					outLine[i] = vertices[(i + 1) % polygonSize] - vertices[i];
				}
				// �|���S���̊e���_�����_���̃x�N�g��
				VECTOR inLine[polygonSize] = {};
				for (int i = 0; i < polygonSize; ++i)
				{
					inLine[i] = crossPoint - vertices[i];
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