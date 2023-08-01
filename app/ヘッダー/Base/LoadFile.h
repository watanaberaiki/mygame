#pragma once

#include <string>
#include <vector>
#include <DirectXMath.h>

// ���x���f�[�^
struct LevelData {

	struct ObjectData {
		// �t�@�C����
		std::string fileName;
		// ���s�ړ�
		DirectX::XMVECTOR translation;
		// ��]�p
		DirectX::XMVECTOR rotation;
		// �X�P�[�����O
		DirectX::XMVECTOR scaling;
	};

	// �I�u�W�F�N�g�z��
	std::vector<ObjectData> objects;
};

// ���x���f�[�^�̃��[�_�[
class LoadFile {

public:// �萔
	// �f�t�H���g�̓ǂݍ��݃f�B���N�g��
	static const std::string kDefaultBaseDirectory;
	// �t�@�C���g���q
	static const std::string kExtension;

public:// �����o�֐�

	/// <summary>
	/// ���x���f�[�^�t�@�C���̓ǂݍ���
	/// </summary>
	/// <param name="fileName">�t�@�C����</param>
	static LevelData* LoadFileData(const std::string& fileName);
};

