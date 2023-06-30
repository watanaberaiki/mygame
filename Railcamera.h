#pragma once
#include "Camera.h"
#include"Input.h"
class Railcamera
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
public://�����o�֐�
	//������
	void Initialize(Camera*camera);
	//�X�V
	void Update();

	//�Q�b�^�[
	const XMMATRIX& GetMatWorld()const { return matWorld; }
private://�����o�ϐ�
	// ���[�J���X�P�[��
	XMFLOAT3 scale = XMFLOAT3(1,1,1);
	// X,Y,Z�����̃��[�J����]�p
	XMFLOAT3 rotation = XMFLOAT3( 0,0,0 );
	// ���[�J�����W
	XMFLOAT3 position = XMFLOAT3(0,0,0 );
	// ���[�J�����[���h�ϊ��s��
	XMMATRIX matWorld;

private://�ÓI�����o�ϐ�
	//�J����
	static Camera* camera;
};

