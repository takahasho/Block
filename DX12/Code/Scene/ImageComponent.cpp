#include "ImageComponent.h"
#include "Actor.h"
#include "../GameObject/Texture.h"
#include "ObjectCreate.h"
#include "Game.h"

ImageComponent::ImageComponent(Actor* actor, int drawOrder)
	:Component(actor)
	, mDrawOrder(drawOrder), mImageData(nullptr)
{

}
// �f�X�g���N�^�[
ImageComponent::~ImageComponent()
{
	DeleateImage();
	delete mImageData;
}

void ImageComponent::SetFileName(const char* fileName)
{

	DeleateImage();
	delete mImageData;
	mImageData = new ImageData(fileName);
	if (strlen(fileName) != 0)
	{
		// �f�[�^�o�^
		VECTOR position = mOwner->GetPosition();
		VECTOR rotation = mOwner->GetRotation();
		VECTOR scale = mOwner->GetScale();
		VECTOR color = mOwner->GetColor();
		float a = mOwner->GetAlpha();

		Texture::GetInstance()->SetTextureMetaData(
			mImageData->entryPos,								// �V�F�[�_�[�ɑ���C���X�^���X�ԍ��i-1�Ȃ�܂����܂��Ă��Ȃ��j
			position,												// �ʒu�i���オ���_�j
			rotation,												// ��]�i�O�`�R�U�O�x�j
			scale,													// �傫��(1�Ȃ�摜�̌��T�C�Y�Ɠ���)
			color,													// �F
			a,														// ��
			mDrawOrder,												// �`�揇
			false,													// UI������
			mImageData->filename);

		DeleateImage();

		// �T�C�Y�擾
		Texture::GetInstance()->GetSize(mImageData->filename, mImageData->width, mImageData->height);
	}
}

void ImageComponent::DeleateImage()
{
	if (mImageData && strlen(mImageData->filename) != 0)
	{
		Texture::GetInstance()->TextureDeleate(mImageData->entryPos, mImageData->filename);
	}
}

void ImageComponent::Update()
{
	if (mImageData && strlen(mImageData->filename) != 0)
	{
		VECTOR position = mOwner->GetPosition();
		VECTOR rotation = mOwner->GetRotation();
		VECTOR scale = mOwner->GetScale();
		VECTOR color = mOwner->GetColor();
		float a = mOwner->GetAlpha();

		if (mOwner->GetGame()->GetGameState() == GameState::ECreative)
		{
			if (mOwner->GetGame()->GetObjectCreate()->Get2DMode())
			{
				// ����ɂȂ�悤�ɂ��炷
				position.x += (mImageData->width / 2);
				position.y *= -1;
				position.y += g_window_height - (mImageData->height / 2);
				Texture::GetInstance()->SetTextureMetaData(
					mImageData->entryPos,								// �V�F�[�_�[�ɑ���C���X�^���X�ԍ��i-1�Ȃ�܂����܂��Ă��Ȃ��j
					position,												// �ʒu�i���オ���_�j
					rotation,												// ��]�i�O�`�R�U�O�x�j
					scale,													// �傫��(1�Ȃ�摜�̌��T�C�Y�Ɠ���)
					color,													// �F
					a,														// ��
					mDrawOrder,												// �`�揇
					false,													// UI������
					mImageData->filename);
			}
			else
			{
				DeleateImage();
			}
		}
		else if (mOwner->GetGame()->GetGameState() == GameState::EPlay)
		{
			position.x += (mImageData->width / 2);
			position.y += (mImageData->height / 2);

			Texture::GetInstance()->SetTextureMetaData(
				mImageData->entryPos,								// �V�F�[�_�[�ɑ���C���X�^���X�ԍ��i-1�Ȃ�܂����܂��Ă��Ȃ��j
				position,												// �ʒu�i���オ���_�j
				rotation,												// ��]�i�O�`�R�U�O�x�j
				scale,													// �傫��(1�Ȃ�摜�̌��T�C�Y�Ɠ���)
				color,													// �F
				a,														// ��
				mDrawOrder,												// �`�揇
				true,													// UI������
				mImageData->filename);
		}
		else
		{
			DeleateImage();
		}
	}
}

void ImageComponent::Draw()
{

}