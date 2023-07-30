#include "GameScene.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	delete spriteCommon;
	//3dオブジェクト解放
	delete sphereobj;
	delete blockobj;
	//3Dモデル解放
	delete spheremodel;
	delete blockmodel;
	FBX_SAFE_DELETE(boneTestModel);
	FBX_SAFE_DELETE(cube);
	for (int i = 0; i < bonetestsize; i++) {
		FBX_SAFE_DELETE(bonetest[i]);
	}
	delete enemycsv;
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input)
{
	this->dxCommon_ = dxCommon;
	this->input_ = input;

	//モデル名を指定してファイル読み込み
	/*FbxLoader::GetInstance()->LoadModelFromFile("cube");*/

	eye = XMFLOAT3(0, 0, -10);	//視点座標
	target = XMFLOAT3(0, 0, 0);	//注視点座標
	up = XMFLOAT3(0, 1, 0);		//上方向ベクトル
	//カメラ
	camera = new Camera();
	camera->Initialize(input_);
	camera->SetEye(eye);
	camera->SetTarget(target);
	camera->SetUp(up);
	camera->Update();

	//オブジェクト3dカメラ
	Object3d::SetCamera(camera);
	//当たり判定キューブオブジェクト
	CubeObject3D::SetCamera(camera);
	CubeObject3D::SetDevice(dxCommon_->GetDevice());
	CubeObject3D::CreateGraphicsPipeline();

	//デバイスをセット
	FbxObject3D::SetDevice(dxCommon_->GetDevice());
	//カメラをセット
	FbxObject3D::SetCamera(camera);
	//グラフィックスパイプライン生成
	FbxObject3D::CreateGraphicsPipeline();

	//ライン初期化
	LineObject::SetCamera(camera);
	LineObject::SetDevice(dxCommon->GetDevice());
	LineObject::CreateGraphicsPipeline();

	//読み込み
	resorcemanager = ResourceManager::Getinstance();
	boneTestModel = resorcemanager->LoadFbx("boneTest");
	cube = resorcemanager->LoadFbx("fbxcube");
	resorcemanager->LoadObj("blackcube");
	resorcemanager->LoadObj("redcube");



	//プレイヤー
	Player::SetInput(input);
	Player::SetDxCommon(dxCommon);
	player = new Player;
	player->Initialize();



	//敵
	enemycsv = new CSVLoader();
	enemycsv->LoadCSV("Resources/csv/enemy.csv");
	Enemy::SetDxCommon(dxCommon);
	for (int i = 0; i < enemysize; i++) {
		std::unique_ptr<Enemy>newObject = std::make_unique<Enemy>();
		newObject->Initialize();
		newObject->SetPosition(enemycsv->GetPosition(i));
		newObject->SetRotation(enemycsv->GetRotation(i));
		newObject->SetScale(enemycsv->GetScale(i));

		//newObject->SetPosition(XMFLOAT3((float)(i*0.2),(float)(i*0.2),(float)i*20));
		newObject->Update();
		enemys.push_back(std::move(newObject));
	}
	
	//パーティクル


	//スプライト共通部の初期化
	spriteCommon = new SpriteCommon;
	spriteCommon->Initialize(dxCommon);

	//テクスチャ読み込み
	spriteCommon->LoadTexture(0, "hit.png");
	spriteCommon->LoadTexture(1, "mario.jpg");

	//スプライトにテクスチャ割り当て
	hitsprite->Initialize(spriteCommon, 0);
	mariosprite->Initialize(spriteCommon, 1);
	//スプライト初期位置
	mariosprite->SetPosition({ 800,0 });
	mariosprite->Update();

	////3Dモデル
	//spheremodel = Model::LoadFromObj("Skydome");
	//blockmodel = Model::LoadFromObj("redcube");
	////当たり判定
	//minsphereModel = spheremodel->GetminModel();
	//maxsphereModel = spheremodel->GetmaxModel();

	////球
	//sphereobj = Object3d::Create();
	//sphereobj->SetModel(spheremodel);
	//sphereobj->SetPosition({ 0,0,0 });

	////ブロック
	//blockobj = Object3d::Create();
	//blockobj->SetModel(blockmodel);
	//blockobj->SetPosition({ 0,5,0 });

	/*railCamera->Initialize(camera);
	railCamera->Update();
	blockobj->SetParentCamera(railCamera);*/


	////ライン
	//linemodel = new LineModel();
	//linemodel->Initialize(dxCommon->GetDevice(), 1.5, -1.5);
	//linemodel->SetImageData(XMFLOAT4(255, 255, 255, 1));
	//lineobject = new LineObject();
	//lineobject->Initialize();
	//lineobject->SetModel(linemodel);
	//lineobject->SetPosition(XMFLOAT3(0, 0, 0));
	//lineobject->SetScale(XMFLOAT3(10.0, 1.0, 1.0));
}

void GameScene::Update()
{
	camera->Update();
	matView = camera->GetmatView();
	//プレイヤー
	player->SetPositionZ(eye.z + 3.0f);
	player->Update();

	//敵
	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		enemy->Update();
		//死んだ際のパーティクル
		if (enemy->GetisDead()) {
			Particle(enemy->GetPos());
		}
	}
	//敵の死んだ処理
	enemys.remove_if([](std::unique_ptr<Enemy>& enemy) {
		return enemy->GetisDead();
		});

	//パーティクル
	for (std::unique_ptr<ParticleManager>& particle : particles)
	{
		particle->Update();
	}

	AllCollision();
}

void GameScene::Draw()
{
	//オブジェクト描画
	Object3d::PreDraw(dxCommon_->GetCommandlist());
	//プレイヤー
	player->Draw(dxCommon_->GetCommandlist());

	//敵
	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		enemy->Draw(dxCommon_->GetCommandlist());
	}
	Object3d::PostDraw();

	//パーティクル
	for (std::unique_ptr<ParticleManager>& particle : particles)
	{
		particle->Draw();
	}

	//デバッグ表示
	//プレイヤー
	player->DebugDraw(dxCommon_->GetCommandlist());

	////敵
	//for (std::unique_ptr<Enemy>& enemy : enemys)
	//{
	//	enemy->DebugDraw(dxCommon_->GetCommandlist());
	//}

	//スプライト描画
	spriteCommon->PreDraw();
	if (isHit) {
		hitsprite->Draw();
	}

	

	spriteCommon->PostDraw();
}

void GameScene::AllCollision()
{
	//自弾リストの取得
	const std::list<std::unique_ptr<PlayerBullet>>& playerBullets = player->GetBullet();
	//敵弾リストの取得
	int i = 0;
	
	/*for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		enemyBullets=enemy->GetBullet();
		i++;
	}*/

	////自キャラと敵弾の判定
	//for (int i = 0; i < enemysize; i++) {
	//	for (const std::unique_ptr<EnemyBullet>& bullet : enemyBullets) {
	//		if (player->GetCubeObject()->CheakCollision(bullet->GetCubeObject())) {
	//			bullet->OnCollision();
	//			player->OnCollision();
	//		}
	//	}
	//}

	//自キャラと敵の判定
	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		if (player->GetCubeObject()->CheakCollision(enemy->GetCubeObject())) {
			player->OnCollision();
		}
	}

	for (std::unique_ptr<Enemy>& enemy : enemys) {
		for (const std::unique_ptr<EnemyBullet>& enemybullet : enemy->GetBullet()) {
			if (player->GetCubeObject()->CheakCollision(enemybullet->GetCubeObject())) {
				enemybullet->OnCollision();
				player->OnCollision();
			}
		}

	}

	////自弾と敵弾の判定
	//for (int i = 0; i < enemysize; i++) {
	//	for (const std::unique_ptr<EnemyBullet>& enemybullet : enemyBullets[i]) {
	//		for (const std::unique_ptr<PlayerBullet>& playerbullet : playerBullets) {
	//			if (playerbullet->GetCubeObject()->CheakCollision(enemybullet->GetCubeObject())) {
	//				playerbullet->OnCollision();
	//				enemybullet->OnCollision();
	//			}
	//		}
	//	}
	//}

	//自弾と敵の判定
	for (const std::unique_ptr<PlayerBullet>& playerbullet : playerBullets) {
		for (std::unique_ptr<Enemy>& enemy : enemys) {
			if (playerbullet->GetCubeObject()->CheakCollision(enemy->GetCubeObject())) {
				playerbullet->OnCollision();
				enemy->OnCollision();
			}
		}
	}

}

void GameScene::Particle(XMFLOAT3 pos)
{
	//パーティクル
	std::unique_ptr<ParticleManager>newparticle = std::make_unique<ParticleManager>();
	newparticle->Initialize("line.png");
	newparticle->SetEmitterPos(pos);
	for (int i = 0; i < 50; i++) {
		//X,Y,Z全て[-5.0f,+5.0f]でランダムに分布
		const float rnd_pos = 1.0f;
		XMFLOAT3 pos{};
		pos.x = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;
		pos.y = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;
		pos.z = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;

		//X,Y,Z全て[-0.05f,+0.05f]でランダムに分布
		const float rnd_vel = 0.1f;
		XMFLOAT3 vel{};
		vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.y = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		//重力に見立ててYのみ[-0.001f,0]でランダムに分布
		const float rnd_acc = 0.001f;
		XMFLOAT3 acc{};
		acc.y = -(float)rand() / RAND_MAX * rnd_acc;

		//色
		const float rnd_color = 1.0f;
		XMFLOAT4 color{  };
		color.x = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;
		color.y = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;
		color.z = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;
		color.w = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;
		//追加
		newparticle->Add(20, pos, vel, acc, 1.0f, 0.0f, color);
	}
	newparticle->Update();
	particles.push_back(std::move(newparticle));
}
