#pragma once
#include"FbxObject3D.h"
#include"FbxModel.h"
#include"Object3d.h"
#include"Model.h"
#include"CubeModel.h"
#include"CubeObject3D.h"
#include"ResourceManager.h"
#include"WireObject.h"
class EnemyBullet
{
public://�����o�֐�
	//������
	void Initialize(DirectXCommon* dxcommon, ResourceManager*resource);
	//�X�V����
	void Update();
	//�`�揈��
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//�����蔻��
	void DebugDraw(ID3D12GraphicsCommandList* cmdList);
	//����
	void Move();
	//����
	void OnCollision();
	//�Z�b�^�[
	void SetPosition(XMFLOAT3 position) { this->position = position; }
	void SetScale(XMFLOAT3 scale) { this->position = scale; }
	void SetRotation(XMFLOAT3 rotation) { this->position = rotation; }
	void SetIsFire(bool isfire) { this->isfire = isfire; }

	//�Q�b�^�[
	bool GetIsFIre() { return isfire; }
	bool GetIsDeath()const { return isdeath; }
	CubeObject3D* GetCubeObject() { return collisionBox; }
private://�ÓI�����o�ϐ�

private://�����o�ϐ�
	//�ʒu�A�傫���A��]
	XMFLOAT3 position = { 0,0,0 };
	XMFLOAT3 scale = { (float)0.1,(float)0.1,(float)0.1 };
	XMFLOAT3 rotation = { 0,0,0 };


	//FBX���f��
	FbxModel* bulletfbxmodel = nullptr;
	//FBX�I�u�W�F�N�g
	FbxObject3D* bulletfbxobj = nullptr;
	//3D���f��
	Model* bulletmodel = nullptr;
	//3D�I�u�W�F�N�g
	WireObject* bulletobj = nullptr;


	//�X�s�[�h
	float speed = 0.5f;
	//�t���O
	bool isfire = false;
	bool isdeath = false;

	//����
	static const int32_t kLifeTime = 60 * 5;
	//�f�X�^�C�}�[
	int32_t deathTimer_ = kLifeTime;

	//����p
	CubeModel* cubeModel = nullptr;
	CubeObject3D* collisionBox = nullptr;

};

