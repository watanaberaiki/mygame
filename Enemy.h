#pragma once
#include"FbxObject3D.h"
#include"FbxModel.h"
#include"Object3d.h"
#include"Model.h"
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
	//�Z�b�^�[
	void SetPosition(XMFLOAT3 position) { this->position = position; }
	void SetScale(XMFLOAT3 scale) { this->position = scale; }
	void SetRotation(XMFLOAT3 rotation) { this->position = rotation; }
private://�ÓI�����o�ϐ�


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

	//�X�s�[�h
	float speed = 0.2f;
};

