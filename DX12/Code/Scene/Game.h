#pragma once
// --------------------- Include File --------------------
#include "../DX12System.h"
#include <vector>
#include <map>
// ---------------------- クラス宣言 ---------------------
// 簡易的なゲームステート
enum class GameState { ETitle, EPlay, ECreative, EPouse };

class Game
{
	// ---------------- メンバ関数宣言 ----------------
public:
	void Initialize(WNDCLASSEX w);
	void RunLoop();
	void Shutdown();

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);

	bool GetGameFlag() { return mEnd; }
	void SetGameFlag(bool flag) { mEnd = flag; }
	GameState GetGameState() { return mGameState; }
	void SetGameState(GameState state) { mGameState = state; }
	std::vector<class Actor*> GetActors() { return mActors; }
	std::map<std::wstring, int> GetActorName() { return mActorName; }
	void SetActorName(std::map<std::wstring, int> actorName) { mActorName = actorName; }
	class Actor* GetActor(std::wstring name);
	class CSoundManager* GetSoundManager() { return pSoundManager; }

	static GameState mGameState;
	class ObjectCreate* GetObjectCreate()const { return mObjectCreate; }
	// 物理演算エンジン
	class Bullet* bullet;
	class Bullet* GetBullet()const { return bullet; }
private:
	void LoadFile();
	void CreateObject();
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void SaveCSV();

	std::vector<class Actor*>mActors;	// Actorクラスで表示(HierarchyButtonの順番に対応)
	std::map<std::wstring, int> mActorName;	// Actorの名前と番号対応

	class CSoundManager* pSoundManager = nullptr;	// サウンド

	// Update中フラッグ
	bool mUpdatingActors = false;
	std::vector<class Actor*> mPendingActors;

	bool mEnd = false;
	// このGame固有の記述
public:
	std::vector<std::wstring>* GetCsvText() { return mCsvText; }
private:
	class ObjectCreate* mObjectCreate = {};
	class Scene* mScene = {};
	std::vector<std::wstring>* mCsvText;
};