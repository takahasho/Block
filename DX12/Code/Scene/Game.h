#pragma once
// --------------------- Include File --------------------
#include "../DX12System.h"
#include <vector>
#include <map>
// ---------------------- �N���X�錾 ---------------------
// �ȈՓI�ȃQ�[���X�e�[�g
enum class GameState { ETitle, EPlay, ECreative, EPouse };

class Game
{
	// ---------------- �����o�֐��錾 ----------------
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
	// �������Z�G���W��
	class Bullet* bullet;
	class Bullet* GetBullet()const { return bullet; }
private:
	void LoadFile();
	void CreateObject();
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void SaveCSV();

	std::vector<class Actor*>mActors;	// Actor�N���X�ŕ\��(HierarchyButton�̏��ԂɑΉ�)
	std::map<std::wstring, int> mActorName;	// Actor�̖��O�Ɣԍ��Ή�

	class CSoundManager* pSoundManager = nullptr;	// �T�E���h

	// Update���t���b�O
	bool mUpdatingActors = false;
	std::vector<class Actor*> mPendingActors;

	bool mEnd = false;
	// ����Game�ŗL�̋L�q
public:
	std::vector<std::wstring>* GetCsvText() { return mCsvText; }
private:
	class ObjectCreate* mObjectCreate = {};
	class Scene* mScene = {};
	std::vector<std::wstring>* mCsvText;
};