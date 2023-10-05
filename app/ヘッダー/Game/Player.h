#pragma once
#include"FbxObject3D.h"
#include"FbxModel.h"
#include"PlayerBullet.h"
#include"Object3d.h"
#include"Model.h"
#include"CubeModel.h"
#include"CubeObject3D.h"
#include"DirectXCommon.h"
#include <memory>
#include"ResourceManager.h"
#include"LineModel.h"
#include"LineObject.h"
#include<DirectXMath.h>
#include"Vector3.h"
#include"WireObject.h"
class Player
{
public://�����o�֐�
	//������
	void Initialize();
	//�X�V����
	void Update();
	//�`�揈��
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//���C���[�t���[��
	void WireDraw();
	//�����蔻��
	void DebugDraw(ID3D12GraphicsCommandList* cmdList);
	//����
	void Move();
	//���e�B�N���𓮂���
	void MoveReticle();
	//����
	void Fire();
	//�R�[���o�b�N�֐�
	void OnCollision();
	//�Z�b�^�[
	void SetPosition(XMFLOAT3 position) { this->position = position; }
	void SetPositionZ(float positionz) { this->position.z = positionz; }
	void SetScale(XMFLOAT3 scale) { this->scale = scale; }
	void SetRotation(XMFLOAT3 rotation) { this->rotation = rotation; }
	void SetIsStart(bool isstart) { this->isstart = isstart; }
	static void SetInput(Input* input) { Player::input = input; }
	static void SetDxCommon(DirectXCommon* dxcommon) { Player::dxcommon = dxcommon; }
	//�Q�b�^�[
	CubeObject3D* GetCubeObject() { return collisionBox; }
	XMFLOAT3 GetPosition() { return position; }
	const std::list<std::unique_ptr<PlayerBullet>>& GetBullet() { return bullets; }
private://�ÓI�����o�ϐ�
	//�L�[�{�[�h
	static Input* input;
	static DirectXCommon* dxcommon;
private://�����o�ϐ�
	//�ʒu�A�傫���A��]
	XMFLOAT3 position = { 0,0,0 };
	XMFLOAT3 scale = { (float)0.1,(float)0.1,(float)0.1 };
	XMFLOAT3 rotation = { 0,0,0 };

	//���e�B�N���̈ʒu
	XMFLOAT3 frontReticlepos = { 0,0,0 };
	XMFLOAT3 backReticlepos = { 0,0,0 };
	float frontdepth = 0.0f;
	float backdepth = 0.0f;


	//FBX���f��
	FbxModel* playerfbxmodel = nullptr;
	//FBX�I�u�W�F�N�g
	FbxObject3D* playerfbxobj = nullptr;

	//3D���f��
	Model* playermodel = nullptr;
	//3D�I�u�W�F�N�g
	WireObject* playerobj = nullptr;

	//����p
	CubeModel* cubeModel = nullptr;
	CubeObject3D* collisionBox = nullptr;

	//�X�s�[�h
	float speed = 0.05f;

	//�e
	std::list<std::unique_ptr<PlayerBullet>>bullets;

	//���\�[�X
	ResourceManager* resource = nullptr;

	//���C��
	LineModel* linemodel = nullptr;
	LineObject* lineobject[4] = {};

	//���e�B�N���p�x�N�g��
	Vector3 reticleVec;
	Vector3 frontVec;
	Vector3 backVec;
	XMFLOAT3 velocity;

	//�X�^�[�g���o���ɓ������Ȃ��悤�ɂ���
	bool isstart = false;
};

