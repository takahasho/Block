#include "Game.h"
// .txt�ɏo�͂��邽��
#include<fstream>
#include "../DX12System.h"
#include "../Camera/Camera.h"
#include "../Input/DInput.h"
#include "../GameObject/Field.h"
#include "../GameObject/Mesh.h"
#include "../GameObject/Texture.h"
#include "../GameObject/PmdModel.h"
#include "../GameObject/ObjModel.h"
#include "../GameObject/PointLine.h"
#include "../Effect/Effect.h"
#include "../Sound/CSoundManager.h"

#include "ObjectCreate.h"
#include "Scene.h"
#include "CameraComponent.h"
#include "../rand.h"
#include "../MATRIX.h"

// �����X�e�[�g
GameState Game::mGameState = GameState::ETitle;

void Game::Initialize(WNDCLASSEX w)
{
	// DirectX12����&������
	DX12::Create();

	// DirectInput�̎���
	DInput::Create(g_hwnd, w.hInstance);

	// �J�����̐���
	Camera::Create();

	// �t�B�[���h�̐���&������
	Field::Create();

	// ���b�V���̐���&������
	Mesh::Create();

	// �e�N�X�`���̐���&������
	Texture::Create();

	// pmd���f���ǂݍ��ݗp�̐���&������
	PmdModel::Create();

	// obj���f���ǂݍ��ݗp�̐���&������
	ObjModel::Create();

	// Effect�̐���&������
	Effect::Create();

	// PointLine�̐���&������
	PointLine::Create();

	 // �^����������
	setRandSeed(timeGetTime());

	// �t�@�C���ǂݍ���
	LoadFile();

	// �I�u�W�F�N�g����
	CreateObject();

}
void Game::LoadFile()
{
	pSoundManager = new CSoundManager(
		g_hwnd);
	if (!pSoundManager->GetDirectSound()->Create(g_hwnd))
	{
		SAFE_DELETE(pSoundManager);
	}
	const UINT NumSound = 1;    //�ǂݍ��ސ�
	char SoundFileName[NumSound][MAX_PATH] =
	{
		"Resouse\\Sound\\music.wav"
	};
	//�T�E���h�̓ǂݍ���
	for (UINT i = 0; i < NumSound; i++)
	{
		pSoundManager->LoadWave(i, SoundFileName[i]);
	}

	// �J�������ǂݍ���
	FILE* fp = nullptr;	// �t�@�C���|�C���^�[

	// �t�@�C�����J����
	fopen_s(&fp, "Resouse\\�Z�[�u(�J����).txt", "rt");

	float zoom = 0;
	if (fp == nullptr)
	{
		OutputDebugString(_T("�t�@�C���̓ǂݍ��݂̎��s\n"));
	}
	else
	{
		TCHAR line[MAX_PATH] = _T("");
		TCHAR keyWord[MAX_PATH] = _T("");
		std::wstring componentName;
		while (!feof(fp))
		{
			::ZeroMemory(line, sizeof(line));
			::ZeroMemory(keyWord, sizeof(keyWord));
			fgetws(line, MAX_PATH, fp);

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
			swscanf_s(keyWord, _T("%f"), &zoom);
		}	
		// �t�@�C�������
		fclose(fp);
	}

	if (zoom == 0)
	{
		Camera::GetInstance()->SetZoom(20);
	}
	else
	{
		Camera::GetInstance()->SetZoom(zoom);
	}

	int num = -1;
	Mesh::GetInstance()->SetCircleMetaData(num, VECTOR(0, 0, 0), VECTOR(0, 0, 0), VECTOR(1, 1, 1), VECTOR(1, 0, 0));
}

void Game::CreateObject()
{
	mObjectCreate = new ObjectCreate(this);
	
	mCsvText = new std::vector<std::wstring>;
	mScene = new Scene(this);
}

void Game::RunLoop()
{
	// �L�[�{�[�h���͏��̍X�V
	DInput::GetInstance()->UpdateKeyboardState();
	// �}�E�X���͏��̍X�V
	DInput::GetInstance()->UpdateMouseState();

	ProcessInput();
	UpdateGame();
	GenerateOutput();
}

void Game::Shutdown()
{	
	// �J�������ۑ�
	// .txt�t�@�C���ɕۑ�
	std::fstream myFile;
	myFile.open("Resouse\\�Z�[�u(�J����).txt", std::ios::out);
	char str[MAX_PATH];
	if (myFile.is_open()) {
		sprintf_s(str, sizeof(str), "%06f\n", mScene->GetCamera()->GetZoom3D());
		myFile << str;
	}
	myFile.close();
	

	while (!mActors.empty())
	{
		delete mActors.back();
	}
	SAFE_DELETE(pSoundManager);
	delete mCsvText;
	delete mObjectCreate;
	DInput::Destroy();
	Camera::Destroy();
	DX12::Destroy();
	Field::Destroy();
	Mesh::Destroy();
	Texture::Destroy();
	PmdModel::Destroy();
	ObjModel::Destroy();
	Effect::Destroy();
	PointLine::Destroy();
	
}

void Game::AddActor(Actor* actor)
{
	//Update���Ȃ�A�ǉ���Update��ɉ�������
	if (mUpdatingActors)
	{
		mPendingActors.emplace_back(actor);
	}
	else
	{
		mActors.emplace_back(actor);
	}
}

Actor* Game::GetActor(std::wstring name)
{ 
	bool flg = false;
	for (auto actorname : mActorName)
	{
		if (actorname.first._Equal(name))
		{
			flg = true;
			break;
		}
	}
	if (flg)
	{
		int num = mActorName[name] + 1;
		return mActors[num];
	}
	else
	{
		return nullptr;
	}
}

void Game::RemoveActor(Actor* actor)
{
	//����actor��mActors�ɂ��邩�T��
	auto iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		//����Actor�ƃP�c��Actor�����ւ���(������R�s�[����������邽��)
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}
}
void Game::ProcessInput()
{
	DInput::GetInstance()->MouseStateUpdate();
	DInput::GetInstance()->KeyStateUpdate();

	if (KEY(ESK_KEY))
	{
		mEnd = true;
	}
	// -------------------------------------------------------------------
	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->ProcessInput();
	}

	mUpdatingActors = false;
}

void Game::UpdateGame()
{
	// mActors�X�V(�X�V����new���ꂽActor��mPendingActors�ɒǉ������)
	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->Update();
	}
	mUpdatingActors = false;

	// �ǉ�����������Actor��mActors�ɒǉ�����
	for (auto pending : mPendingActors)
	{
		mActors.emplace_back(pending);
	}
	mPendingActors.clear();

	// Dead��Ԃ�Actor�𒼉���deadActors�ɒ��o����
	std::vector<Actor*> deadActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == ActorState::EDead)
		{
			deadActors.emplace_back(actor);
		}
	}
	// deadActors����������(mActors�������菜�����)
	for (auto actor : deadActors)
	{
		delete actor;
	}
}

void Game::GenerateOutput()
{
	DX->BeginDraw();

	Field::GetInstance()->Draw();
	PointLine::GetInstance()->DrawLine();
	PointLine::GetInstance()->DrawPoint();
	Mesh::GetInstance()->DrawBox();
	Mesh::GetInstance()->DrawCylinder();
	Mesh::GetInstance()->DrawSphere();
	Mesh::GetInstance()->DrawCircle();
	ObjModel::GetInstance()->Draw();
	Texture::GetInstance()->Draw();
	DX->DrawTextFormat();
	DX->EndDraw();
}

// CSV��������
void Game::SaveCSV()
{
	// .txt�t�@�C���ɕۑ�
	FILE* fp = nullptr;
	fopen_s(&fp, "Resouse\\CSV\\test.csv", "wt, ccs=UTF-8");

	if (fp == nullptr)
	{
		OutputDebugString(_T("�t�@�C���̓ǂݍ��݂̎��s\n"));
	}
	else
	{
		fputws(_T("\"FN�ԍ�\",\"����\",\"�b��\"\n"), fp);
		for (int i = 0, size = (int)mCsvText->size(); i < size; i++)
		{
			auto s = mCsvText[0][i].c_str();
			fputws(s , fp);
			fputws(_T("\n"), fp);
		}
		fclose(fp);
	}

}