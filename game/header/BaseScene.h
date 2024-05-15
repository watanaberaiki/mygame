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
#pragma once

class SceneManager;

class BaseScene
{
protected:
	enum Scene {
		Title,
		GamePlay,
		BossFight,
		Clear,
		Dead
	};
public://メンバ関数
	//デストラクタ
	virtual ~BaseScene() = default;
	//初期化
	virtual void Initialize(DirectXCommon* dxCommon, ImguiManager* imgui);
	//終了処理
	virtual void Finalize();
	//更新処理
	virtual void Update();
	//描画処理
	virtual void Draw();
	//画面遷移
	virtual void Transition(Scene nextScene);
	//画面遷移時に使うパーティクル
	virtual void TransitionParticle(XMFLOAT3 pos);
	virtual void TransitionBackParticle(XMFLOAT3 pos, int num);
	//イージング
	double easeOutQuad(double time, double start, double difference, double totaltime);
	//セッター
	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }
protected:
	//シーンマネージャ(借りてくる)
	SceneManager* sceneManager_ = nullptr;

protected://ゲーム処理(仮の引っ越し)
	//ポインタ
	Input* input_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;
	ImguiManager* imgui_ = nullptr;

	//パーティクル
	ParticleManager* particles = new ParticleManager();
	ParticleManager* redParticles = new ParticleManager();
	//スプライト共通部
	SpriteCommon* spriteCommon = new SpriteCommon();
	//スプライト
	Sprite* hitSprite = new Sprite();
	Sprite* marioSprite = new Sprite();
	Sprite* menuSprite = new Sprite();
	Sprite* titleSprite = new Sprite();
	Sprite* startSprite = new Sprite();
	Sprite* blackSprite = new Sprite();
	Sprite* transitionWhiteSprite = new Sprite();
	Sprite* gameOverSprite = new Sprite();
	Sprite* clearWhiteSprite = new Sprite();
	Sprite* clearSprite = new Sprite();
	Sprite* stickRSprite = new Sprite();
	Sprite* stickLSprite = new Sprite();
	Sprite* buttonRTSprite = new Sprite();
	Sprite* buttonLTSprite = new Sprite();



	/*OBJからモデルデータを読み込む*/
	//3Dモデル
	Model* sphereModel = nullptr;
	Model* blockModel = nullptr;
	Model* testModel = nullptr;
	//3Dモデルの当たり判定
	XMFLOAT3 minsphereModel = {}, maxsphereModel = {};

	//3Dオブジェクト
	Object3d* sphereObj = nullptr;
	Object3d* blockObj = nullptr;

	//カメラ
	Camera* camera = new Camera();

	//カメラ用
	XMFLOAT3 eye = { 0,0,0 };		//視点座標
	XMFLOAT3 target = { 0,0,0 };	//注視点座標
	XMFLOAT3 up = { 0,0,0 };		//上方向ベクトル
	XMMATRIX matView = {};

	//FBXModel
	FbxModel* boneTestModel = nullptr;
	FbxModel* cube = nullptr;

	static const int bonetestsize = 5;
	FbxObject3D* bonetest[bonetestsize] = {};

	//地面jsonファイル読み込み
	LevelData* floorData = nullptr;

	//配置オブジェクト情報
	std::map<std::string, Model*> models;
	std::vector<Object3d*> objects;
	std::vector<WireObject*> heightWireObjects;
	std::vector<WireObject*> widthWireObjects;


	//敵
	static const int enemysize = 11;
	std::list<std::unique_ptr<Enemy>>enemys;
	//プレイヤー
	Player* player = new Player();
	const float playerSpaceZ = 4.0f;

	//ボス
	Boss* boss = new Boss();
	const float bossSpaceZ = 12.0f;

	////マップ用
	int enemyCount = 0;

	//ヒット確認
	bool isHit = false;

	//ライン
	static const int maxLine = 5;
	LineModel* lineModel = new LineModel();
	const float linePosX = 4.0f;
	const float linePosY = 1.8f;
	const float lineSpaceZ = 20.0f;
	std::list <std::unique_ptr<LineObject>>lineObjects;


	ResourceManager* resorcemanager = nullptr;

	//判定をまとめて管理
	std::list<std::unique_ptr<CubeObject3D>>collisionBoxs;

	CSVLoader* enemycsv = new CSVLoader();

	//メニュー
	bool isMenu = false;
	bool backMenu = false;
	double time = 0.0;
	double backtime = 0.0;
	double maxTime = 50.0;
	double start = -WinApp::window_width / 2;
	double end = WinApp::window_width / 2;
	int menunum = 0;

	//地面
	Object3d* floorObj = nullptr;

	//シーン切り替え
	Scene scene = Title;
	bool isEnemyAlive = true;

	//タイトルパーティクル用
	XMFLOAT3 partPos = {};
	int particleTime = 0;
	const int particleMaxTime = 5;
	//タイトル判定
	bool isTitle = false;
	//スタート演出用
	bool isStart = false;
	double totalTime = 0.0;
	double startMaxTime = 40.0;
	double startSizeX = 1280;
	double startSizeY = 720;
	double endSize = 0;

	double startPos = 0;
	double endPosx = WinApp::window_width / 2;
	double endPosY = WinApp::window_height / 2;

	//スタート時に白線関連をだんだん大きくしていく演出
	double directionTime = 0;
	double directionMaxTime = 50;
	//プレイヤーのスケール
	double playerStartScale = 0;
	double playerEndScale = 0.1;
	float playerDirectionScale = 0;
	//地面の白線のスケール
	XMFLOAT3 floorStartScale = { 0.0f,0.0f,0.0f };
	XMFLOAT3 floorEndScale = { 0.0f,0.0f,0.0f };
	XMFLOAT3 floorDirectionScale = {};
	float floorStartPosX = 0.0f;
	float floorEndPosX = 4.0f;
	float floorDirectionPosX = 0;
	//横の壁の白線のスケール
	XMFLOAT3 lineStartScale = { 0.0f,0.0f,0.0f };
	XMFLOAT3 heightLineEndScale = {};
	XMFLOAT3 widthLineEndScale = {};


	XMFLOAT3 lineDirectionScale = { 0.0f,0.0f,0.0f };



	//画面遷移
	Scene nextScene = {};
	//遷移中判定
	bool isTransition = false;
	//戻る遷移判定
	bool isBackTransition = false;

	double totalTransitionTime = 0.0;
	double MaxTransitionTime = 100.0;
	double transitionStartPosY = WinApp::window_height + WinApp::window_height / 2;
	double transitionEndposY = WinApp::window_height / 2;

	int MaxDelay = 20;
	int delayTime = 0;

	//ゲームオーバー演出用
	bool isGameOver = false;
	bool lineLose = false;
	int gameOverTime = 0;
	const int gameOverMaxTime = 30;
	const int nextSceneTime = 60;
	//白線の消滅エフェクト用
	float lineZ = 10;
	XMFLOAT3 linepos = {};

	//クリア演出用
	bool isClearBack = false;
	int clearTime = 0;
	int clearBackTime = 0;
	const int clearMaxTime = 60;
	const int clearChangeSceneTime = 80;

	//リセット用
	XMFLOAT3 eyeReset = { 0,0,5 };
	XMFLOAT3 targetReset = { 0,0,10 };

	DxInput* dxinput = new DxInput();

	//デバッグ
	XMFLOAT3 debugEye = { 0,0,0 };
	XMFLOAT3 debugTarget = { 0,0,0 };

	//地面の繰り返し描画処理用
	int floorCount = 0;


	//操作方法のガイド
	bool isStickR = false;
	bool isStickL = false;
	bool isButtonRT = false;
	int stickRCount = 0;
	int stickLCount = 0;
	int buttonRTCount = 0;
	int buttonLTCount = 0;
	int stickRNotCount = 0;
	int stickLNotCount = 0;
	int buttonRTNotCount = 0;
	int buttonLTNotCount = 0;
	float stickRAlpha = 1.0f;
	float stickLAlpha = 1.0f;
	float buttonRTAlpha = 1.0f;
	float buttonLTAlpha = 1.0f;
	const int maxGuideCount = 30;
	const int maxGuideNotCount = 240;
};

