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
#pragma warning(push)
#pragma warning(disable:4267)
#include<map>
#include<utility>
#pragma warning(pop)
struct LevelData;
enum Scene {
	Title,
	Game,
	Boss
};
class GameScene
{
//メンバ関数
public:
	GameScene();
	~GameScene();
	//初期化
	void Initialize(DirectXCommon* dxCommon, Input* input);
	//更新
	void Update();
	//描画
	void Draw();
	//判定一覧
	void AllCollision();
	//パーティクル処理
	void Particle(XMFLOAT3 pos);
	//イージング
	double easeOutQuad(double time,double start,double difference,double totaltime);
private:
	//ポインタ
	Input* input_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;

	//パーティクル
	std::list<std::unique_ptr<ParticleManager>> particles;

	//スプライト共通部
	SpriteCommon* spriteCommon = nullptr;
	//スプライト
	Sprite* hitsprite = new Sprite();
	Sprite* mariosprite = new Sprite();
	Sprite* menu = new Sprite();
	Sprite* boss = new Sprite();


	/*OBJからモデルデータを読み込む*/
	//3Dモデル
	Model* spheremodel=nullptr;
	Model* blockmodel = nullptr;
	Model* testmodel = nullptr;
	//3Dモデルの当たり判定
	XMFLOAT3 minsphereModel = {}, maxsphereModel = {};
	
	//3Dオブジェクト
	Object3d* sphereobj=nullptr;
	Object3d* blockobj = nullptr;

	//カメラ
	Camera* camera = nullptr;

	//カメラ用
	XMFLOAT3 eye = {0,0,0};		//視点座標
	XMFLOAT3 target = {0,0,0};	//注視点座標
	XMFLOAT3 up = {0,0,0};		//上方向ベクトル
	XMMATRIX matView = {};

	//FBXModel
	FbxModel* boneTestModel = nullptr;
	FbxModel* cube = nullptr;

	static const int bonetestsize = 5;
	FbxObject3D* bonetest[bonetestsize] = {};

	//ファイル読み込み
	LevelData* leveldata = nullptr;
	//配置オブジェクト情報
	std::map<std::string, Model*> models;
	std::vector<Object3d*> objects;
	std::vector<WireObject*> wireobjects;

	//敵
	static const int enemysize = 10;
	std::list<std::unique_ptr<Enemy>>enemys;
	/*const std::list<std::unique_ptr<EnemyBullet>>& enemyBullets;*/
	//プレイヤー
	Player* player = nullptr;
	////マップ用
	int enemycount = 0;

	//ヒット確認
	bool isHit = false;

	//ライン
	static const int maxLine = 10;
	LineModel* linemodel = nullptr;
	LineObject* lineObject[maxLine] = {};

	ResourceManager* resorcemanager = nullptr;

	//判定をまとめて管理
	std::list<std::unique_ptr<CubeObject3D>>collisionBoxs;

	CSVLoader* enemycsv = nullptr;

	//メニュー
	bool isMenu = false;
	bool backMenu = false;
	double time = 0.0;
	double backtime = 0.0;
	double maxTime = 50.0;
	double start = -WinApp::window_width/2;
	double end = WinApp::window_width / 2;
	int menunum = 0;

	//地面
	Object3d* floorobj = nullptr;

	//シーン切り替え
	Scene scene = Title;
	bool isEnemyAlive = true;

};

