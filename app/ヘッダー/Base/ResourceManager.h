#pragma once
#include<map>
#include"FbxModel.h"
#include"Model.h"
#include <string>
#include"FbxLoader.h"
//#include"SpriteCommon.h"
#pragma warning(push)
#pragma warning(disable:4267)
#include<map>
#pragma warning(pop)
class ResourceManager
{
private:
	//std::���ȗ�
	using string = std::string;
public:
	//������
	void Initialize();
	//�ǂݍ���
	FbxModel* LoadFbx(const string modelName);
	Model* LoadObj(const string modelName);

	//�C���X�^���X�擾
	static ResourceManager* Getinstance() { if (instance == NULL)instance = new ResourceManager(); return instance; }

private:
	FbxModel* model=nullptr;
	Model* objmodel = nullptr;
	std::map<string, FbxModel*>fbxmodels;
	std::map<string, Model*>objmodels;
	/*std::map<char, SpriteCommon>fbxmodels;*/

	//�C���X�^���X
private:
	static ResourceManager* instance;

};

