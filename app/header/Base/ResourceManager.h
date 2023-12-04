//リソース関連を読み込み保管しておけるクラス

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
	//std::を省略
	using string = std::string;
public:
	//初期化
	void Initialize();
	//読み込み
	FbxModel* LoadFbx(const string& modelName);
	Model* LoadObj(const string& modelName);

	//インスタンス取得
	static ResourceManager* Getinstance() { if (instance == NULL)instance = new ResourceManager(); return instance; }

private:
	FbxModel* model=nullptr;
	Model* objModel = nullptr;
	std::map<string, FbxModel*>fbxModels;
	std::map<string, Model*>objModels;
	/*std::map<char, SpriteCommon>fbxmodels;*/

	//インスタンス
private:
	static ResourceManager* instance;

};

