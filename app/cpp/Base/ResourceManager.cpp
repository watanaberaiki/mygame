#include "ResourceManager.h"
ResourceManager*ResourceManager::instance;

void ResourceManager::Initialize()
{
}

FbxModel* ResourceManager::LoadFbx(const string modelName)
{
	//読み込まれたテクスチャを参照
	auto it = fbxmodels.find(modelName);
	if (it != fbxmodels.end()) {
		model = it->second;
		return model;
	}

	model = FbxLoader::GetInstance()->LoadModelFromFile(modelName);
	fbxmodels[modelName] = model;
	return model;
}

Model* ResourceManager::LoadObj(const string modelName)
{
	//読み込まれたテクスチャを参照
	auto it = objmodels.find(modelName);
	if (it != objmodels.end()) {
		objmodel = it->second;
		return objmodel;
	}

	objmodel = Model::LoadFromObj(modelName);
	objmodels[modelName] = objmodel;
	return objmodel;
}
