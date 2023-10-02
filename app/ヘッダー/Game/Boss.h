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
#include"Player.h"

class Boss
{
public://�����o�֐�
	//������
	void Initialize();
	//�X�V����
	void Update();
	//�`�揈��
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//�����蔻��
	void DebugDraw(ID3D12GraphicsCommandList* cmdList);
	//����
	void Move();
	//����
	void Fire();
	//�R�[���o�b�N�֐�
	void OnCollision();

	//�Z�b�^�[
	void SetPosition(XMFLOAT3 position) { this->position = position; }
	void SetPositionZ(float positionz) { this->position.z = positionz; }
	void SetScale(XMFLOAT3 scale) { this->scale = scale; }
	void SetRotation(XMFLOAT3 rotation) { this->rotation = rotation; }
	void SetType(int type) { this->type = type; }
	static void SetDxCommon(DirectXCommon* dxcommon) { Boss::dxcommon = dxcommon; }
	static void SetPlayer(Player* player) { Boss::player = player; }

	//�Q�b�^�[
	CubeObject3D* GetCubeObject() { return collisionBox; }
	const std::list<std::unique_ptr<EnemyBullet>>& GetBullet() { return bullets; }
	bool GetisDead() { return isdead; }
	XMFLOAT3 GetPos() { return position; }


private://�ÓI�����o�ϐ�
	static DirectXCommon* dxcommon;
	static Player* player;

private://�����o�ϐ�
	//�ʒu�A�傫���A��]
	XMFLOAT3 position = { 0,0,0 };
	XMFLOAT3 scale = { (float)0.1,(float)0.1,(float)0.1 };
	XMFLOAT3 rotation = { 0,0,0 };


	//FBX���f��
	FbxModel* bossfbxmodel = nullptr;
	//FBX�I�u�W�F�N�g
	FbxObject3D* bossfbxobj = nullptr;

	//3D���f��
	Model* bossmodel = nullptr;
	//3D�I�u�W�F�N�g
	WireObject* bossobj = nullptr;

	//����p
	CubeModel* cubeModel = nullptr;
	CubeObject3D* collisionBox = nullptr;

	//�X�s�[�h
	float speedZ = 0.2f;
	float speedY = 0.02f;
	float speedX = 0.02f;

	int time = 0;
	const int MaxTime = 60;

	//�e
	std::list<std::unique_ptr<EnemyBullet>>bullets;
	ResourceManager* resource = nullptr;

	//���S����
	bool isdead = false;

	//����
	int type = 0;
	bool plusX = false;
	bool plusY = false;
	const float MoveX = 1.5f;
	const float MoveY = 1.5f;

	//���@��_���e�p
	Vector3 velocityvec;
	XMFLOAT3 velocity;


	//hp
	int life = 10;
};

