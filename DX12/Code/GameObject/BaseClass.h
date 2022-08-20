#ifndef OBJECT_H_
#define OBJECT_H_
#pragma once
// ------------------------ Object���N���X(h) ----------------------------
// --------------------- Include File --------------------
#include "../Global.h"
#include "ObjectStruct.h"

// ---------------------- �N���X�錾 ---------------------
class BaseClass
{
private:
	// 1�s�N�Z��������̃o�C�g��
	const UINT bytePerPixel = 4;
	const UINT64 dataSize = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT * D3D12_TEXTURE_DATA_PITCH_ALIGNMENT * 4;
protected:
	// ------------------ �����o�ϐ� ---------------------
	ComPtr<ID3D12RootSignature> _rootSignature;		// ���[�g�V�O�l�`���[
	ComPtr<ID3D12PipelineState> _pipelineState;		// �O���t�B�b�N�p�C�v���C���X�e�[�g�I�u�W�F�N�g

	ComPtr<ID3D12Resource> _texWhiteBuffer;					// ���e�N�X�`���[�o�b�t�@�[
	ComPtr<ID3D12Resource> _texBlackBuffer;					// ���e�N�X�`���[�o�b�t�@�[
	IWICImagingFactory* _imagingFactory = NULL;	// �C���[�W�[�t�@�N�g���[
	ComPtr<ID3D12Resource> _uploadBuffer;			// �A�b�v���[�h�o�b�t�@�[
	std::map<std::string, ComPtr<ID3D12Resource>*>	_resourceTable;
protected:
	HRESULT CreateWhiteTexture();		// ���F�e�N�X�`���[�̐���
	HRESULT CreateBlackTexture();		// ���F�e�N�X�`���[�̐���
	HRESULT LoadTextureFromFile(const char* fileName, BYTE** pImageData, SIZE* pSize);	// �e�N�X�`���ǂݍ���(char*)
	HRESULT LoadTextureFromFile(const TCHAR* pFileName,BYTE** pImageData, SIZE* pSize);	// �e�N�X�`���ǂݍ���(TCHAR*)
	// �A���C�����g�ɂ��낦���T�C�Y��Ԃ�
	// @param size ���̃T�C�Y
	// @param alignment �A���C�����g�T�C�Y
	// @param �A���C�����g���낦���T�C�Y
	size_t AlignmentSize(size_t size, size_t alignment);
	std::wstring GetWideStringFromString(const std::string& str);			// ���ʎq���擾
	HRESULT BlobErrorMessange(HRESULT hr,
		ID3DBlob* errorBlob);				// �G���[���b�Z�[�W
public:
	BaseClass() {};				// �R���X�g���N�^�[
	~BaseClass() {};		// �f�X�g���N�^�[

	// �Q�b�^�[�A�Z�b�^�[
	ComPtr<ID3D12RootSignature> GetRootSignature() const {
		return _rootSignature;
	}
	ComPtr<ID3D12PipelineState> GetPipelineState() const {
		return _pipelineState;
	}
};

#endif /* __OBJECT_H__ */