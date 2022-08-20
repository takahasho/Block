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
// デストラクター
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
		// データ登録
		VECTOR position = mOwner->GetPosition();
		VECTOR rotation = mOwner->GetRotation();
		VECTOR scale = mOwner->GetScale();
		VECTOR color = mOwner->GetColor();
		float a = mOwner->GetAlpha();

		Texture::GetInstance()->SetTextureMetaData(
			mImageData->entryPos,								// シェーダーに送るインスタンス番号（-1ならまだ決まっていない）
			position,												// 位置（左上が原点）
			rotation,												// 回転（０～３６０度）
			scale,													// 大きさ(1なら画像の元サイズと同じ)
			color,													// 色
			a,														// α
			mDrawOrder,												// 描画順
			false,													// UI化する
			mImageData->filename);

		DeleateImage();

		// サイズ取得
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
				// 左上になるようにずらす
				position.x += (mImageData->width / 2);
				position.y *= -1;
				position.y += g_window_height - (mImageData->height / 2);
				Texture::GetInstance()->SetTextureMetaData(
					mImageData->entryPos,								// シェーダーに送るインスタンス番号（-1ならまだ決まっていない）
					position,												// 位置（左上が原点）
					rotation,												// 回転（０～３６０度）
					scale,													// 大きさ(1なら画像の元サイズと同じ)
					color,													// 色
					a,														// α
					mDrawOrder,												// 描画順
					false,													// UI化する
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
				mImageData->entryPos,								// シェーダーに送るインスタンス番号（-1ならまだ決まっていない）
				position,												// 位置（左上が原点）
				rotation,												// 回転（０～３６０度）
				scale,													// 大きさ(1なら画像の元サイズと同じ)
				color,													// 色
				a,														// α
				mDrawOrder,												// 描画順
				true,													// UI化する
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