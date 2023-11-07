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
#include"ImguiManager.h"
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
	void Initialize(DirectXCommon* dxCommon,ImguiManager*imgui);
	//更新
	void Update();
	//描画
	void Draw();
	//判定一覧
	void AllCollision();
	//パーティクル処理
	void Particle(XMFLOAT3 pos);
	//タイトルシーンのパーティクル処理
	void TitleParticle(XMFLOAT3 pos);
	//画面遷移時に使うパーティクル
	void TransitionParticle(XMFLOAT3 pos);
	void TransitionBackParticle(XMFLOAT3 pos,int num);
	//画面遷移
	void Transition(Scene nextScene);
	//イージング
	double easeOutQuad(double time,double start,double difference,double totaltime);
	//クリア演出
	void ClearTransition();
	//クリアバック
	void ClearBackTransition();
private:
	//ポインタ
	Input* input_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;
	ImguiManager* imgui_ = nullptr;

	//パーティクル
	std::list<std::unique_ptr<ParticleManager>> particles;

	//スプライト共通部
	SpriteCommon* spriteCommon = nullptr;
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

	/*OBJからモデルデータを読み込む*/
	//3Dモデル
	Model* sphereModel=nullptr;
	Model* blockModel = nullptr;
	Model* testModel = nullptr;
	//3Dモデルの当たり判定
	XMFLOAT3 minsphereModel = {}, maxsphereModel = {};
	
	//3Dオブジェクト
	Object3d* sphereObj=nullptr;
	Object3d* blockObj = nullptr;

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

	//地面jsonファイル読み込み
	LevelData* floorData = nullptr;

	//配置オブジェクト情報
	std::map<std::string, Model*> models;
	std::vector<Object3d*> objects;
	std::vector<WireObject*> heightWireObjects;
	std::vector<WireObject*> widthWireObjects;


	//敵
	static const int enemysize = 10;
	std::list<std::unique_ptr<Enemy>>enemys;
	/*const std::list<std::unique_ptr<EnemyBullet>>& enemyBullets;*/
	//プレイヤー
	Player* player = nullptr;

	//ボス
	Boss* boss = nullptr;
	////マップ用
	int enemyCount = 0;

	//ヒット確認
	bool isHit = false;

	//ライン
	static const int maxLine = 5;
	LineModel* lineModel = nullptr;
	std::list <std::unique_ptr<LineObject>>lineObjects;
	
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
	Object3d* floorObj = nullptr;

	//シーン切り替え
	Scene scene = Title;
	bool isEnemyAlive = true;

	//タイトルパーティクル用
	XMFLOAT3 partPos = {};
	int particleTime = 0;
	const int particleMaxTime = 5;

	//スタート演出用
	bool isStart = false;
	double totalTime = 0.0;
	double startMaxTime = 40.0;
	double startSizeX = 1280;
	double startSizeY = 720;
	double endSize =0;

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
	XMFLOAT3 floorStartScale = {0.0f,0.0f,0.0f};
	XMFLOAT3 floorEndScale = {0.0f,0.0f,0.0f};
	XMFLOAT3 floorDirectionScale = {};
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
	double transitionStartPosY = WinApp::window_height+WinApp::window_height/2;
	double transitionEndposY = WinApp::window_height/2;

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


};

