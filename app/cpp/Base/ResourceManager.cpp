#include "ResourceManager.h"
ResourceManager*ResourceManager::instance;

void ResourceManager::Initialize()
{
}

FbxModel* ResourceManager::LoadFbx(const string modelName)
{
	//読み込まれたテクスチャを参照
	auto it = fbxModels.find(modelName);
	if (it != fbxModels.end()) {
		model = it->second;
		return model;
	}

	model = FbxLoader::GetInstance()->LoadModelFromFile(modelName);
	fbxModels[modelName] = model;
	return model;
}

Model* ResourceManager::LoadObj(const string modelName)
{
	//読み込まれたテクスチャを参照
	auto it = objModels.find(modelName);
	if (it != objModels.end()) {
		objModel = it->second;
		return objModel;
	}

	objModel = Model::LoadFromObj(modelName);
	objModels[modelName] = objModel;
	return objModel;
}
