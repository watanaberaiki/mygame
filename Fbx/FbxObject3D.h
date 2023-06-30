#pragma once

#include"FbxModel.h"
#include"Camera.h"

#include<Windows.h>
#include<wrl.h>
#include<d3d12.h>
#include<d3dx12.h>
#include<DirectXMath.h>
#include<string>
#include"FbxLoader.h"

class FbxObject3D
{

protected:	//�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	//�萔
	//�{�[���̍ő吔
	static const int MAX_BONES = 32;

public:	//�T�u�N���X
	//�萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferDataTransform
	{
		XMMATRIX viewproj;		//�r���[�v���W�F�N�V�����s��
		XMMATRIX world;			//���[���h�s��
		XMFLOAT3 cameraPos;		//�J�������W(���[���h���W)
	};
	//�萔�o�b�t�@�p�\����(�X�L�j���O)
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[MAX_BONES];
	};

public:	//�ÓI�����o�֐�
	//setter
	static void SetDevice(ID3D12Device* device) { FbxObject3D::device = device; }
	static void SetCamera(Camera* camera) { FbxObject3D::camera = camera; }
	//�O���t�B�b�N�X�p�C�v���C���̐���
	static void CreateGraphicsPipeline();
private://�ÓI�����o�ϐ�
	//�f�o�C�X
	static ID3D12Device* device;
	//�J����
	static Camera* camera;
	//���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature>rootsignature;
	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState>pipelinestate;
public://�����o�֐�
	//������
	void Initialize();
	//���t���[������
	void Update();
	//�`��
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//�A�j���[�V�����J�n
	void PlayAnimation();

	///�Z�b�^�[
	//���f���̃Z�b�g
	void SetModel(FbxModel* fbxModel) { this->fbxModel = fbxModel; }
	void SetPosition(XMFLOAT3 position) { this->position = position; }
	void SetRoatation(XMFLOAT3 rotation) { this->rotation = rotation; }
	//�Q�b�^�[

protected:	//�����o�ϐ�
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource>constBuffTransform;
	//�萔�o�b�t�@(�X�L��)
	ComPtr<ID3D12Resource>constBuffSkin;
	//���[�J���X�P�[��
	XMFLOAT3 scale = { 1,1,1 };
	//X,Y,Z������̃��[�J����]�p
	XMFLOAT3 rotation = { 0,0,0 };
	//���[�J�����W
	XMFLOAT3 position = { 0,0,0 };
	//���[�J�����[���h�ϊ��s��
	XMMATRIX matWorld = {};
	//���f��
	FbxModel* fbxModel = nullptr;
	//1�t���[���̎���
	FbxTime frameTime;
	//�A�j���[�V�����J�n����
	FbxTime startTime;
	//�A�j���[�V�����I������
	FbxTime endTime;
	//���ݎ���
	FbxTime currentTime;
	//�A�j���[�V�����Đ���
	bool isPlay = false;
};