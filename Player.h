#pragma once
#include"FbxObject3D.h"
#include"FbxModel.h"
#include"PlayerBullet.h"
#include"Object3d.h"
#include"Model.h"

class Player
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
	//�Z�b�^�[
	void SetPosition(XMFLOAT3 position) { this->position = position; }
	void SetScale(XMFLOAT3 scale) { this->position = scale; }
	void SetRotation(XMFLOAT3 rotation) { this->position = rotation; }
	static void SetInput(Input* input) { Player::input = input; }

private://�ÓI�����o�ϐ�
	//�L�[�{�[�h
	static Input* input;

private://�����o�ϐ�
	//�ʒu�A�傫���A��]
	XMFLOAT3 position = { 0,0,0 };
	XMFLOAT3 scale = { (float)0.1,(float)0.1,(float)0.1 };
	XMFLOAT3 rotation = { 0,0,0 };


	//FBX���f��
	FbxModel* playerfbxmodel = nullptr;
	//FBX�I�u�W�F�N�g
	FbxObject3D* playerfbxobj = nullptr;

	//3D���f��
	Model* playermodel = nullptr;
	//3D�I�u�W�F�N�g
	Object3d* playerobj = nullptr;


	//�X�s�[�h
	float speed = 0.2f;

	//�e
	size_t bulletsize = 50;
	std::list<std::unique_ptr<PlayerBullet>>bullets;
};

