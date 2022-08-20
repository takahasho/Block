#ifndef OBJECT_H_
#define OBJECT_H_
#pragma once
// ------------------------ Object基底クラス(h) ----------------------------
// --------------------- Include File --------------------
#include "../Global.h"
#include "ObjectStruct.h"

// ---------------------- クラス宣言 ---------------------
class BaseClass
{
private:
	// 1ピクセル当たりのバイト数
	const UINT bytePerPixel = 4;
	const UINT64 dataSize = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT * D3D12_TEXTURE_DATA_PITCH_ALIGNMENT * 4;
protected:
	// ------------------ メンバ変数 ---------------------
	ComPtr<ID3D12RootSignature> _rootSignature;		// ルートシグネチャー
	ComPtr<ID3D12PipelineState> _pipelineState;		// グラフィックパイプラインステートオブジェクト

	ComPtr<ID3D12Resource> _texWhiteBuffer;					// 白テクスチャーバッファー
	ComPtr<ID3D12Resource> _texBlackBuffer;					// 黒テクスチャーバッファー
	IWICImagingFactory* _imagingFactory = NULL;	// イメージーファクトリー
	ComPtr<ID3D12Resource> _uploadBuffer;			// アップロードバッファー
	std::map<std::string, ComPtr<ID3D12Resource>*>	_resourceTable;
protected:
	HRESULT CreateWhiteTexture();		// 白色テクスチャーの生成
	HRESULT CreateBlackTexture();		// 黒色テクスチャーの生成
	HRESULT LoadTextureFromFile(const char* fileName, BYTE** pImageData, SIZE* pSize);	// テクスチャ読み込み(char*)
	HRESULT LoadTextureFromFile(const TCHAR* pFileName,BYTE** pImageData, SIZE* pSize);	// テクスチャ読み込み(TCHAR*)
	// アライメントにそろえたサイズを返す
	// @param size 元のサイズ
	// @param alignment アライメントサイズ
	// @param アライメントそろえたサイズ
	size_t AlignmentSize(size_t size, size_t alignment);
	std::wstring GetWideStringFromString(const std::string& str);			// 識別子を取得
	HRESULT BlobErrorMessange(HRESULT hr,
		ID3DBlob* errorBlob);				// エラーメッセージ
public:
	BaseClass() {};				// コンストラクター
	~BaseClass() {};		// デストラクター

	// ゲッター、セッター
	ComPtr<ID3D12RootSignature> GetRootSignature() const {
		return _rootSignature;
	}
	ComPtr<ID3D12PipelineState> GetPipelineState() const {
		return _pipelineState;
	}
};

#endif /* __OBJECT_H__ */