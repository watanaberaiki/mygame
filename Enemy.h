#pragma once
#include"FbxObject3D.h"
#include"FbxModel.h"
#include"Object3d.h"
#include"Model.h"
#include"CubeModel.h"
#include"CubeObject3D.h"
#include"DirectXCommon.h"
#include"EnemyBullet.h"
#include"ResourceManager.h"

class Enemy
{
public://�����o�֐�
	//������
	void Initialize();
	//�X�V����
	void Update();
	//�`�揈��
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//����
	void Move();
	//����
	void Fire();
	//�R�[���o�b�N�֐�
	void OnCollision();
	
	//�Z�b�^�[
	void SetPosition(XMFLOAT3 position) { this->position = position; }
	void SetScale(XMFLOAT3 scale) { this->position = scale; }
	void SetRotation(XMFLOAT3 rotation) { this->position = rotation; }
	static void SetDxCommon(DirectXCommon* dxcommon) { Enemy::dxcommon = dxcommon; }
	//�Q�b�^�[
	CubeObject3D* GetCubeObject() { return collisionBox; }
	const std::list<std::unique_ptr<EnemyBullet>>& GetBullet() { return bullets; }


private://�ÓI�����o�ϐ�
	static DirectXCommon* dxcommon;

private://�����o�ϐ�
	//�ʒu�A�傫���A��]
	XMFLOAT3 position = { 0,0,0 };
	XMFLOAT3 scale = { (float)0.1,(float)0.1,(float)0.1 };
	XMFLOAT3 rotation = { 0,0,0 };


	//FBX���f��
	FbxModel* enemyfbxmodel = nullptr;
	//FBX�I�u�W�F�N�g
	FbxObject3D* enemyfbxobj = nullptr;

	//3D���f��
	Model* enemymodel = nullptr;
	//3D�I�u�W�F�N�g
	Object3d* enemyobj = nullptr;

	//����p
	CubeModel* cubeModel = nullptr;
	CubeObject3D* collisionBox = nullptr;

	//�X�s�[�h
	float speed = 0.2f;

	int time = 0;
	const int MaxTime = 60;

	//�e
	std::list<std::unique_ptr<EnemyBullet>>bullets;
	ResourceManager* resource = nullptr;
};

