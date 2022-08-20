#include "Game.h"
// .txtに出力するため
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

// 初期ステート
GameState Game::mGameState = GameState::ETitle;

void Game::Initialize(WNDCLASSEX w)
{
	// DirectX12生成&初期化
	DX12::Create();

	// DirectInputの実装
	DInput::Create(g_hwnd, w.hInstance);

	// カメラの生成
	Camera::Create();

	// フィールドの生成&初期化
	Field::Create();

	// メッシュの生成&初期化
	Mesh::Create();

	// テクスチャの生成&初期化
	Texture::Create();

	// pmdモデル読み込み用の生成&初期化
	PmdModel::Create();

	// objモデル読み込み用の生成&初期化
	ObjModel::Create();

	// Effectの生成&初期化
	Effect::Create();

	// PointLineの生成&初期化
	PointLine::Create();

	 // 疑似乱数生成
	setRandSeed(timeGetTime());

	// ファイル読み込み
	LoadFile();

	// オブジェクト生成
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
	const UINT NumSound = 1;    //読み込む数
	char SoundFileName[NumSound][MAX_PATH] =
	{
		"Resouse\\Sound\\music.wav"
	};
	//サウンドの読み込み
	for (UINT i = 0; i < NumSound; i++)
	{
		pSoundManager->LoadWave(i, SoundFileName[i]);
	}

	// カメラ情報読み込み
	FILE* fp = nullptr;	// ファイルポインター

	// ファイルを開ける
	fopen_s(&fp, "Resouse\\セーブ(カメラ).txt", "rt");

	float zoom = 0;
	if (fp == nullptr)
	{
		OutputDebugString(_T("ファイルの読み込みの失敗\n"));
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

			// 余分な空白を読み飛ばす
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
		// ファイルを閉じる
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
	// キーボード入力情報の更新
	DInput::GetInstance()->UpdateKeyboardState();
	// マウス入力情報の更新
	DInput::GetInstance()->UpdateMouseState();

	ProcessInput();
	UpdateGame();
	GenerateOutput();
}

void Game::Shutdown()
{	
	// カメラ情報保存
	// .txtファイルに保存
	std::fstream myFile;
	myFile.open("Resouse\\セーブ(カメラ).txt", std::ios::out);
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
	//Update中なら、追加をUpdate後に延期する
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
	//このactorがmActorsにあるか探す
	auto iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		//このActorとケツのActorを入れ替える(消去後コピー処理を避けるため)
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
	// mActors更新(更新中にnewされたActorはmPendingActorsに追加される)
	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->Update();
	}
	mUpdatingActors = false;

	// 追加を延期したActorをmActorsに追加する
	for (auto pending : mPendingActors)
	{
		mActors.emplace_back(pending);
	}
	mPendingActors.clear();

	// Dead状態のActorを直下のdeadActorsに抽出する
	std::vector<Actor*> deadActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == ActorState::EDead)
		{
			deadActors.emplace_back(actor);
		}
	}
	// deadActorsを消去する(mActorsからも取り除かれる)
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

// CSV書き込み
void Game::SaveCSV()
{
	// .txtファイルに保存
	FILE* fp = nullptr;
	fopen_s(&fp, "Resouse\\CSV\\test.csv", "wt, ccs=UTF-8");

	if (fp == nullptr)
	{
		OutputDebugString(_T("ファイルの読み込みの失敗\n"));
	}
	else
	{
		fputws(_T("\"FN番号\",\"拍数\",\"秒数\"\n"), fp);
		for (int i = 0, size = (int)mCsvText->size(); i < size; i++)
		{
			auto s = mCsvText[0][i].c_str();
			fputws(s , fp);
			fputws(_T("\n"), fp);
		}
		fclose(fp);
	}

}