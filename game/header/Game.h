//ゲームの処理のクラス
#include"SceneManager.h"
#include"BaseScene.h"
#include"TitleScene.h"

#include"Vector3.h"
#include "Input.h"
#include"WinApp.h"
#include"DirectXCommon.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include <DirectXMath.h>
#include<math.h>
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include"Camera.h"
#include <math.h>
#include"ParticleManager.h"
#include"FbxLoader.h"
#include"FbxObject3D.h"
#include"LoadFile.h"
#include<map>
#include"LineModel.h"
#include"LineObject.h"
#include"Enemy.h"
#include"Player.h"
#include"ResourceManager.h"
#include"EnemyBullet.h"
#include"CSVLoader.h"
#include"Boss.h"
#include"ImguiManager.h"
#include"DxInput.h"
#pragma warning(push)
#pragma warning(disable:4267)
#include<map>
#include<utility>
#pragma warning(pop)
using namespace RaikiEngine;
enum Scene {
	Title,
	GamePlay,
	BossFight,
	Clear,
	Dead
};
class Game
{
	//メンバ関数
public:
	Game();
	~Game();
	//初期化
	void Initialize(DirectXCommon* dxCommon, ImguiManager* imgui);
	//更新
	void Update();
	//描画
	void Draw();
private:
	//ポインタ
	Input* input_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;
	ImguiManager* imgui_ = nullptr;

	SceneManager* sceneManager_=nullptr;
	
};

