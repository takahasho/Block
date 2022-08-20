#include "Effect.h"

Effect* Effect::_instance = nullptr;			// インスタンスの実態
// コンストラクター
Effect::Effect()
{
}
// デストラクター
Effect::~Effect()
{
	for (auto effect : effects)
	{
		EffekseerDestroy(effect);
	}
}
// 生成
void Effect::Create()
{
	if (!_instance)
	{
		_instance = new Effect();
	}
}
// 破棄
void Effect::Destroy()
{
	delete _instance;
}
// Effekseer生成
EFFEKSEER* Effect::EffekseerCreate(const char* EffectName)
{
	effects.push_back(new EFFEKSEER);
	TCHAR pFileName[256] = {};
#ifdef UNICODE
	MultiByteToWideChar(CP_OEMCP, MB_PRECOMPOSED, EffectName, (int)strlen(EffectName), pFileName, (sizeof pFileName) / 2);
#else
	strcpy(pFileName, EffectName);
#endif

	XAudio2Create(&effects.back()->g_xa2);
	effects.back()->g_xa2->CreateMasteringVoice(&effects.back()->g_xa2_master);
	auto format = DXGI_FORMAT_R8G8B8A8_UNORM;
	effects.back()->renderer = ::EffekseerRendererDX12::Create(DX12::GetInstance()->GetDevice().Get(), DX12::GetInstance()->CommandQueue().Get(), 3, &format, 1, DXGI_FORMAT_UNKNOWN, false, 1000);

	// メモリプールの作成
	effects.back()->sfMemoryPoolEfk = EffekseerRenderer::CreateSingleFrameMemoryPool(effects.back()->renderer->GetGraphicsDevice());

	// コマンドリストの作成
	effects.back()->commandListEfk = EffekseerRenderer::CreateCommandList(effects.back()->renderer->GetGraphicsDevice(), effects.back()->sfMemoryPoolEfk);

	// エフェクトのマネージャーの作成
	effects.back()->manager = ::Effekseer::Manager::Create(1000);

	// 描画モジュールの設定
	effects.back()->manager->SetSpriteRenderer(effects.back()->renderer->CreateSpriteRenderer());
	effects.back()->manager->SetRibbonRenderer(effects.back()->renderer->CreateRibbonRenderer());
	effects.back()->manager->SetRingRenderer(effects.back()->renderer->CreateRingRenderer());
	effects.back()->manager->SetTrackRenderer(effects.back()->renderer->CreateTrackRenderer());
	effects.back()->manager->SetModelRenderer(effects.back()->renderer->CreateModelRenderer());

	// テクスチャ、モデル、カーブ、マテリアルローダーの設定する。
	// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
	effects.back()->manager->SetTextureLoader(effects.back()->renderer->CreateTextureLoader());
	effects.back()->manager->SetModelLoader(effects.back()->renderer->CreateModelLoader());
	effects.back()->manager->SetMaterialLoader(effects.back()->renderer->CreateMaterialLoader());
	effects.back()->manager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

	// 下記コメントアウトを外せばEffekseerのSoundも読み込まれます。ただ、メモリリークの原因がどうしてもわからない。
	//// サウンドモジュールの設定
	//effect1.sound = ::EffekseerSound::Sound::Create(effect1.g_xa2, 16, 16);

	//// 音再生用インスタンスから再生機能を指定
	//effect1.manager->SetSoundPlayer(effect1.sound->CreateSoundPlayer());

	//// サウンドデータの読込機能を設定する。
	//// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
	//effect1.manager->SetSoundLoader(effect1.sound->CreateSoundLoader());

	// エフェクトの読込
	effects.back()->effect = Effekseer::Effect::Create(effects.back()->manager, (const EFK_CHAR*)pFileName, 1.0f);

	return effects.back();
}
// Effekseer再生
void Effect::EffekseerDraw(EFFEKSEER* effect, VECTOR position, VECTOR angle, VECTOR scale, float speed)
{
	// 視点位置を確定
	Effekseer::Matrix44 fkViewMat;
	Effekseer::Matrix44 fkProjMat;

	XMMATRIX world_matrix;
	XMMATRIX translate = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX rotate_x = XMMatrixRotationX(DirectX::XMConvertToRadians(angle.x));
	XMMATRIX rotate_y = XMMatrixRotationY(DirectX::XMConvertToRadians(angle.y));
	XMMATRIX rotate_z = XMMatrixRotationZ(DirectX::XMConvertToRadians(angle.z));
	XMMATRIX scale_mat = XMMatrixScaling(scale.x, scale.y, scale.z);
	world_matrix = scale_mat * rotate_x * rotate_y * rotate_z * translate;

	// =====ビュー行列=====
	XMMATRIX viewMat = Camera::GetInstance()->GetView();
	// =====プロジェクション行列=====
	XMMATRIX projMat = Camera::GetInstance()->GetProjection();

	XMMATRIX matrix = world_matrix * viewMat;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			fkViewMat.Values[i][j] = matrix.r[i].m128_f32[j];
			fkProjMat.Values[i][j] = projMat.r[i].m128_f32[j];
		}
	}

	// 投影行列を設定
	effect->renderer->SetProjectionMatrix(fkProjMat);
	// カメラ行列を設定
	effect->renderer->SetCameraMatrix(fkViewMat);

	// フレームの開始時に呼ぶ
	effect->sfMemoryPoolEfk->NewFrame();

	// コマンドリストを開始する。
	EffekseerRendererDX12::BeginCommandList(effect->commandListEfk, DX12::GetInstance()->CommandList().Get());

	effect->renderer->SetCommandList(effect->commandListEfk);

	if (effect->playType == PlayType::STOP)
	{
		// エフェクトの停止
		effect->manager->StopEffect(effect->handle);
	}
	else if (effect->playType == PlayType::ONESHOT)
	{
		if (effect->time == 0)
		{
			// エフェクトの再生
			effect->handle = effect->manager->Play(effect->effect, 0, 0, 0);
		}
	}
	else if (effect->playType == PlayType::LOOP)
	{
		if (effect->time >= 10)
		{
			effect->time = 0;
		}
		if (effect->time == 0)
		{
			// エフェクトの再生
			effect->handle = effect->manager->Play(effect->effect, 0, 0, 0);
		}
	}

	effect->time += (g_elapsedTime * speed);

	// エフェクトの移動
	effect->manager->AddLocation(effect->handle, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f));

	// マネージャーの更新
	effect->manager->Update();

	// 時間を更新する
	effect->renderer->SetTime(effect->time / 60.0f);

	// エフェクトの描画開始処理を行う。
	effect->renderer->BeginRendering();

	// エフェクトの描画を行う。
	effect->manager->Draw();

	// エフェクトの描画終了処理を行う。
	effect->renderer->EndRendering();
	// コマンドリストを終了する。
	effect->renderer->SetCommandList(nullptr);
	EffekseerRendererDX12::EndCommandList(effect->commandListEfk);
}
// Effekseer終了
void Effect::EffekseerDestroy(EFFEKSEER* effect)
{
	// ---- エフェクト関連 ----
	// マネージャーの破棄
	effect->manager.Reset();
	// サウンドの破棄
	effect->sound.Reset();
	// レンダラーの破棄
	effect->renderer.Reset();
	// Release XAudio2
	// XAudio2の解放
	if (effect->g_xa2_master != nullptr)
	{
		effect->g_xa2_master->DestroyVoice();
		effect->g_xa2_master = nullptr;
	}
	ES_SAFE_RELEASE(effect->g_xa2);
	delete effect;
}