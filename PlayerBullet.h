#pragma once
#include"FbxObject3D.h"
#include"FbxModel.h"
class PlayerBullet
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
	void SetIsFire(bool isfire) { this->isfire = isfire; }

	//�Q�b�^�[
	bool GetIsFIre() { return isfire; }
private://�ÓI�����o�ϐ�
	//�L�[�{�[�h
	static Input* input;

private://�����o�ϐ�
	//�ʒu�A�傫���A��]
	XMFLOAT3 position = { 0,0,0 };
	XMFLOAT3 scale = { (float)0.05,(float)0.01,(float)0.05 };
	XMFLOAT3 rotation = { 0,0,0 };


	//FBX���f��
	FbxModel* bulletmodel = nullptr;
	//FBX�I�u�W�F�N�g
	FbxObject3D* bulletobj = nullptr;

	//�X�s�[�h
	float speed = 0.2f;
	//�t���O
	bool isfire = false;
};

