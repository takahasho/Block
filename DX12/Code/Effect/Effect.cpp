#include "Effect.h"

Effect* Effect::_instance = nullptr;			// �C���X�^���X�̎���
// �R���X�g���N�^�[
Effect::Effect()
{
}
// �f�X�g���N�^�[
Effect::~Effect()
{
	for (auto effect : effects)
	{
		EffekseerDestroy(effect);
	}
}
// ����
void Effect::Create()
{
	if (!_instance)
	{
		_instance = new Effect();
	}
}
// �j��
void Effect::Destroy()
{
	delete _instance;
}
// Effekseer����
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

	// �������v�[���̍쐬
	effects.back()->sfMemoryPoolEfk = EffekseerRenderer::CreateSingleFrameMemoryPool(effects.back()->renderer->GetGraphicsDevice());

	// �R�}���h���X�g�̍쐬
	effects.back()->commandListEfk = EffekseerRenderer::CreateCommandList(effects.back()->renderer->GetGraphicsDevice(), effects.back()->sfMemoryPoolEfk);

	// �G�t�F�N�g�̃}�l�[�W���[�̍쐬
	effects.back()->manager = ::Effekseer::Manager::Create(1000);

	// �`�惂�W���[���̐ݒ�
	effects.back()->manager->SetSpriteRenderer(effects.back()->renderer->CreateSpriteRenderer());
	effects.back()->manager->SetRibbonRenderer(effects.back()->renderer->CreateRibbonRenderer());
	effects.back()->manager->SetRingRenderer(effects.back()->renderer->CreateRingRenderer());
	effects.back()->manager->SetTrackRenderer(effects.back()->renderer->CreateTrackRenderer());
	effects.back()->manager->SetModelRenderer(effects.back()->renderer->CreateModelRenderer());

	// �e�N�X�`���A���f���A�J�[�u�A�}�e���A�����[�_�[�̐ݒ肷��B
	// ���[�U�[���Ǝ��Ŋg���ł���B���݂̓t�@�C������ǂݍ���ł���B
	effects.back()->manager->SetTextureLoader(effects.back()->renderer->CreateTextureLoader());
	effects.back()->manager->SetModelLoader(effects.back()->renderer->CreateModelLoader());
	effects.back()->manager->SetMaterialLoader(effects.back()->renderer->CreateMaterialLoader());
	effects.back()->manager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

	// ���L�R�����g�A�E�g���O����Effekseer��Sound���ǂݍ��܂�܂��B�����A���������[�N�̌������ǂ����Ă��킩��Ȃ��B
	//// �T�E���h���W���[���̐ݒ�
	//effect1.sound = ::EffekseerSound::Sound::Create(effect1.g_xa2, 16, 16);

	//// ���Đ��p�C���X�^���X����Đ��@�\���w��
	//effect1.manager->SetSoundPlayer(effect1.sound->CreateSoundPlayer());

	//// �T�E���h�f�[�^�̓Ǎ��@�\��ݒ肷��B
	//// ���[�U�[���Ǝ��Ŋg���ł���B���݂̓t�@�C������ǂݍ���ł���B
	//effect1.manager->SetSoundLoader(effect1.sound->CreateSoundLoader());

	// �G�t�F�N�g�̓Ǎ�
	effects.back()->effect = Effekseer::Effect::Create(effects.back()->manager, (const EFK_CHAR*)pFileName, 1.0f);

	return effects.back();
}
// Effekseer�Đ�
void Effect::EffekseerDraw(EFFEKSEER* effect, VECTOR position, VECTOR angle, VECTOR scale, float speed)
{
	// ���_�ʒu���m��
	Effekseer::Matrix44 fkViewMat;
	Effekseer::Matrix44 fkProjMat;

	XMMATRIX world_matrix;
	XMMATRIX translate = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX rotate_x = XMMatrixRotationX(DirectX::XMConvertToRadians(angle.x));
	XMMATRIX rotate_y = XMMatrixRotationY(DirectX::XMConvertToRadians(angle.y));
	XMMATRIX rotate_z = XMMatrixRotationZ(DirectX::XMConvertToRadians(angle.z));
	XMMATRIX scale_mat = XMMatrixScaling(scale.x, scale.y, scale.z);
	world_matrix = scale_mat * rotate_x * rotate_y * rotate_z * translate;

	// =====�r���[�s��=====
	XMMATRIX viewMat = Camera::GetInstance()->GetView();
	// =====�v���W�F�N�V�����s��=====
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

	// ���e�s���ݒ�
	effect->renderer->SetProjectionMatrix(fkProjMat);
	// �J�����s���ݒ�
	effect->renderer->SetCameraMatrix(fkViewMat);

	// �t���[���̊J�n���ɌĂ�
	effect->sfMemoryPoolEfk->NewFrame();

	// �R�}���h���X�g���J�n����B
	EffekseerRendererDX12::BeginCommandList(effect->commandListEfk, DX12::GetInstance()->CommandList().Get());

	effect->renderer->SetCommandList(effect->commandListEfk);

	if (effect->playType == PlayType::STOP)
	{
		// �G�t�F�N�g�̒�~
		effect->manager->StopEffect(effect->handle);
	}
	else if (effect->playType == PlayType::ONESHOT)
	{
		if (effect->time == 0)
		{
			// �G�t�F�N�g�̍Đ�
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
			// �G�t�F�N�g�̍Đ�
			effect->handle = effect->manager->Play(effect->effect, 0, 0, 0);
		}
	}

	effect->time += (g_elapsedTime * speed);

	// �G�t�F�N�g�̈ړ�
	effect->manager->AddLocation(effect->handle, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f));

	// �}�l�[�W���[�̍X�V
	effect->manager->Update();

	// ���Ԃ��X�V����
	effect->renderer->SetTime(effect->time / 60.0f);

	// �G�t�F�N�g�̕`��J�n�������s���B
	effect->renderer->BeginRendering();

	// �G�t�F�N�g�̕`����s���B
	effect->manager->Draw();

	// �G�t�F�N�g�̕`��I���������s���B
	effect->renderer->EndRendering();
	// �R�}���h���X�g���I������B
	effect->renderer->SetCommandList(nullptr);
	EffekseerRendererDX12::EndCommandList(effect->commandListEfk);
}
// Effekseer�I��
void Effect::EffekseerDestroy(EFFEKSEER* effect)
{
	// ---- �G�t�F�N�g�֘A ----
	// �}�l�[�W���[�̔j��
	effect->manager.Reset();
	// �T�E���h�̔j��
	effect->sound.Reset();
	// �����_���[�̔j��
	effect->renderer.Reset();
	// Release XAudio2
	// XAudio2�̉��
	if (effect->g_xa2_master != nullptr)
	{
		effect->g_xa2_master->DestroyVoice();
		effect->g_xa2_master = nullptr;
	}
	ES_SAFE_RELEASE(effect->g_xa2);
	delete effect;
}