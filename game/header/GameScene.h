//ゲームの処理のクラス

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
#pragma warning(push)
#pragma warning(disable:4267)
#include<map>
#include<utility>
#pragma warning(pop)
struct LevelData;
enum Scene {
	Title,
	Game,
	BossFight,
	Clear,
	Dead
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
	//敵のパーティクル処理
	void EnemyParticle(XMFLOAT3 pos);
	//タイトルシーンのパーティクル処理
	void TitleParticle(XMFLOAT3 pos);
	//画面遷移時に使うパーティクル
	void TransitionParticle(XMFLOAT3 pos);
	void TransitionBackParticle(XMFLOAT3 pos,int num);
	//画面遷移
	void Transition(Scene nextScene);
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
	Sprite* title = new Sprite();
	Sprite* startsprite = new Sprite();
	Sprite* blacksprite = new Sprite();
	Sprite* whitesprite = new Sprite();

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

	//ボス
	Boss* boss = nullptr;
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

	//タイトルパーティクル用
	XMFLOAT3 partpos = {};
	int particletime = 0;
	const int particleMaxtime = 5;

	//スタート演出用
	bool isstart = false;
	double totaltime = 0.0;
	double startMaxTime = 40.0;
	double startsizeX = 1280;
	double startsizeY = 720;
	double endsize =0;

	double startpos = 0;
	double endposx = WinApp::window_width / 2;
	double endposY = WinApp::window_height / 2;

	//自機登場演出
	double playerDirectionTime = 0;
	double playerDirectionMaxTime = 50;
	double startscale = 0;
	double endscale = 0.1;
	float directionscale = 0;

	//画面遷移
	Scene nextScene = {};
	//遷移中判定
	bool isTransition = false;
	//戻る遷移判定
	bool isBackTransition = false;
	
	double totalTransitionTime = 0.0;
	double MaxTransitionTime = 150.0;
	double transitionStartPosY = WinApp::window_height+WinApp::window_height/2;
	double transitionEndposY = WinApp::window_height/2;

	int MaxDelay = 20;
	int delayTime = 0;
};

