#pragma once
#include "fbxsdk.h"
#include "FbxModel.h"
#include <d3d12.h>
#include <d3dx12.h>
#include <string>
class FbxLoader
{
private://エイリアス
	//std::を省略
	using string = std::string;
public://定数
	//モデル格納ルートパス
	static const string baseDirectory;
	//テクスチャがない場合の標準テクスチャファイル名
	static const string defaultTextureFileName;

public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static FbxLoader* GetInstance();


	//FBXの行列をXMMatrixに変換
	static void ConvertMatrixFromFbx(DirectX::XMMATRIX* dst, const FbxAMatrix& src);
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ID3D12Device* device);

	/// <summary>
	/// 後始末
	/// </summary>
	void Finalize();

	/// <summary>
	/// ファイルからFBXモデル読込
	/// </summary>
	/*void LoadModelFromFile(const string& modelName);*/
	FbxModel* LoadModelFromFile(const string& modelName);
	/// <summary>
	/// 再帰的にノード構成を解析
	/// </summary>
	void ParseNodeRecursive(FbxModel* fbxModel, FbxNode* fbxNode, Node* parent = nullptr);

	/// <summary>
	/// メッシュ読み取り
	/// </summary>
	void ParseMesh(FbxModel* fbxModel, FbxNode* fbxNode);

	//頂点座標読み取り
	void ParseMeshVertices(FbxModel* fbxModel, FbxMesh* fbxMesh);
	//面情報読み取り
	void ParseMeshFaces(FbxModel* fbxModel, FbxMesh* fbxMesh);
	//マテリアル読み取り
	void ParseMaterial(FbxModel* fbxModel, FbxNode* fbxNode);
	//テクスチャ読み込み
	void LoadTexture(FbxModel* fbxModel, const std::string& fullpath);
	//ディレクトリを含んだファイルパスからファイル名を注室する
	std::string ExtractFileName(const std::string& path);
	//スキニング情報の読み取り
	void ParseSkin(FbxModel* fbxModel, FbxMesh* fbxMesh);

private:
	// privateなコンストラクタ（シングルトンパターン）
	FbxLoader() = default;
	// privateなデストラクタ（シングルトンパターン）
	~FbxLoader() = default;
	// コピーコンストラクタを禁止（シングルトンパターン）
	FbxLoader(const FbxLoader& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const FbxLoader& obj) = delete;

private:
	//D3D12デバイス
	ID3D12Device* device = nullptr;
	//FBXマネージャ
	FbxManager* fbxManager = nullptr;
	//FBXインポータ
	FbxImporter* fbxImporter = nullptr;
};