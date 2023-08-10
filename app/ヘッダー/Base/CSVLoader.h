#pragma once
#include "array"
#include "DirectXMath.h"
#include "vector"
#include "string"

class CSVLoader
{
private:	//�G�C���A�X
	//DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//csv�t�@�C����ǂݍ���ŕϐ��ɑ������
	void LoadCSV(const std::string fileName);


	//�Q�b�^�[
	XMFLOAT3 GetPosition(int num) { return position[num]; };
	XMFLOAT3 GetRotation(int num) { return rotation[num]; };
	XMFLOAT3 GetScale(int num) { return scale[num]; };
	int Getmove(int num) { return move[num]; }

private:
	//�ǂݍ��ރI�u�W�F�N�g�̐�
	int objectNum = 0;

	std::vector<XMFLOAT3> position;
	std::vector<XMFLOAT3> rotation;
	std::vector<XMFLOAT3> scale;
	std::vector<int> move;
};