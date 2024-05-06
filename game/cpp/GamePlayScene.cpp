#include "GamePlayScene.h"
#include"BossScene.h"
#include"SceneManager.h"
#include"ClearScene.h"
void GamePlayScene::Initialize(DirectXCommon* dxCommon, ImguiManager* imgui)
{

	dxCommon_ = dxCommon;
	imgui_ = imgui;
	input_ = Input::GetInstance();

	eye = XMFLOAT3(0, 0, 5);	//視点座標
	target = XMFLOAT3(0, 0, 10);	//注視点座標
	up = XMFLOAT3(0, 1, 0);		//上方向ベクトル
	//カメラ
	camera->Initialize();
	camera->SetEye(eye);
	camera->SetTarget(target);
	camera->SetUp(up);
	camera->Update();

	//ワイヤーオブジェクト
	WireObject::SetCamera(camera);
	//オブジェクト3dカメラ
	Object3d::SetCamera(camera);
	//パーティクルマネージャカメラセット
	ParticleManager::SetCamera(camera);

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

	//スプライト共通部の初期化
	spriteCommon->Initialize(dxCommon);

	//リソースマネージャ
	resorcemanager = ResourceManager::Getinstance();
	resorcemanager->LoadObj("blackcube");
	resorcemanager->LoadObj("redcube");
	resorcemanager->LoadObj("floor");

	//プレイヤー
	player->Initialize();

	//敵
	Enemy::SetPlayer(player);
	Enemy::SetDxCommon(dxCommon);
	enemycsv->LoadCSV("Resources/csv/enemy.csv");
	//スプライト共通部の初期化
	spriteCommon->Initialize(dxCommon);

	//テクスチャ読み込み
	spriteCommon->LoadTexture(0, "hit.png");
	spriteCommon->LoadTexture(1, "mario.jpg");
	spriteCommon->LoadTexture(2, "menu.png");
	spriteCommon->LoadTexture(3, "boss.png");
	spriteCommon->LoadTexture(4, "title.png");
	spriteCommon->LoadTexture(5, "start.png");
	spriteCommon->LoadTexture(6, "black1x1.png");
	spriteCommon->LoadTexture(7, "white1x1.png");
	spriteCommon->LoadTexture(8, "gameover.png");
	spriteCommon->LoadTexture(9, "clear.png");
	spriteCommon->LoadTexture(10, "stickR.png");
	spriteCommon->LoadTexture(11, "stickL.dds");
	spriteCommon->LoadTexture(12, "buttonRT.png");
	spriteCommon->LoadTexture(13, "buttonLt.png");
	//スプライトにテクスチャ割り当て
	hitSprite->Initialize(spriteCommon, 0);
	marioSprite->Initialize(spriteCommon, 1);
	menuSprite->Initialize(spriteCommon, 2);
	titleSprite->Initialize(spriteCommon, 4);
	startSprite->Initialize(spriteCommon, 5);
	blackSprite->Initialize(spriteCommon, 6);
	transitionWhiteSprite->Initialize(spriteCommon, 7);
	gameOverSprite->Initialize(spriteCommon, 8);
	clearWhiteSprite->Initialize(spriteCommon, 7);
	clearSprite->Initialize(spriteCommon, 9);
	stickRSprite->Initialize(spriteCommon, 10);
	stickLSprite->Initialize(spriteCommon, 11);
	buttonRTSprite->Initialize(spriteCommon, 12);
	buttonLTSprite->Initialize(spriteCommon, 13);


	//スプライト初期位置
	const XMFLOAT2 center = { 0.5f,0.5f };
	marioSprite->SetPosition({ 800,0 });
	marioSprite->Update();

	//右スティック
	stickRSprite->SetAnchorPoint(center);
	stickRSprite->SetPosition({ WinApp::window_width - stickRSprite->GetSize().x / 2,stickRSprite->GetSize().y / 1.5f });
	//右トリガー
	buttonRTSprite->SetAnchorPoint(center);
	buttonRTSprite->SetPosition({ WinApp::window_width - buttonRTSprite->GetSize().x / 2,buttonRTSprite->GetSize().y * 2.0f });

	//左スティック
	stickLSprite->SetAnchorPoint(center);
	stickLSprite->SetPosition({ stickLSprite->GetSize().x / 2,stickLSprite->GetSize().y / 1.5f });
	//左トリガー
	buttonLTSprite->SetAnchorPoint(center);
	buttonLTSprite->SetPosition({ buttonLTSprite->GetSize().x / 2,buttonLTSprite->GetSize().y * 2.0f });

	//画面遷移用スプライト
	transitionWhiteSprite->SetAnchorPoint(center);
	transitionWhiteSprite->SetSize(XMFLOAT2(WinApp::window_width, WinApp::window_height));

	//クリア用スプライト
	clearWhiteSprite->SetAnchorPoint(center);
	clearWhiteSprite->SetPosition(XMFLOAT2(WinApp::window_width / 2, WinApp::window_height / 2));


	//3Dモデル
	//モデルデータをマップに入れる
	models.insert(std::make_pair("floor", resorcemanager->LoadObj("blackcube")));
	models.insert(std::make_pair("heightLine", resorcemanager->LoadObj("block")));
	models.insert(std::make_pair("widthLine", resorcemanager->LoadObj("block")));

	menuSprite->SetAnchorPoint(center);
	menuSprite->SetPosition(XMFLOAT2((float)easeOutQuad(maxTime, start, end - start, time), WinApp::window_height / 2));

	////地面の配置(json読み込み)
	//地面を繰り返し描画するために二つ読み込む(json読み込み)

	// レベルデータの読み込み
	floorData = LoadFile::LoadFileData("floor");
	// レベルデータからオブジェクトを生成、配置
	for (auto& objectData : floorData->objects) {
		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
			model = it->second;
		}
		//そのまま描画したいもの
		if (objectData.fileName == "floor") {
			//3Dオブジェクトを生成
			Object3d* newObject = Object3d::Create();
			newObject->SetModel(model);
			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.translation);
			newObject->SetPosition(pos);

			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rotation);
			newObject->SetRotation(rot);

			// サイズ
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scaling);
			newObject->SetScale(scale);


			// 配列に登録
			objects.push_back(newObject);
		}
		//ワイヤーで描画したいもの
		else if (objectData.fileName == "heightLine") {
			//3Dオブジェクトを生成
			WireObject* newObject = WireObject::Create();
			newObject->SetModel(model);
			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.translation);
			newObject->SetPosition(pos);

			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rotation);
			newObject->SetRotation(rot);

			// 座標
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scaling);
			newObject->SetScale(scale);

			// 配列に登録
			heightWireObjects.push_back(newObject);
			heightLineEndScale = (newObject->GetScale());
		}
		else if (objectData.fileName == "widthLine") {
			//3Dオブジェクトを生成
			WireObject* newObject = WireObject::Create();
			newObject->SetModel(model);
			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.translation);
			newObject->SetPosition(pos);

			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rotation);
			newObject->SetRotation(rot);

			// 座標
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scaling);
			newObject->SetScale(scale);

			// 配列に登録
			widthWireObjects.push_back(newObject);
			widthLineEndScale = (newObject->GetScale());
		}
	}

	// レベルデータの読み込み
	floorData = LoadFile::LoadFileData("floors");
	// レベルデータからオブジェクトを生成、配置
	for (auto& objectData : floorData->objects) {
		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
			model = it->second;
		}
		//そのまま描画したいもの
		if (objectData.fileName == "floor") {
			//3Dオブジェクトを生成
			Object3d* newObject = Object3d::Create();
			newObject->SetModel(model);
			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.translation);
			newObject->SetPosition(pos);

			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rotation);
			newObject->SetRotation(rot);

			// 座標
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scaling);
			newObject->SetScale(scale);

			// 配列に登録
			objects.push_back(newObject);
		}
		//ワイヤーで描画したいもの
		else if (objectData.fileName == "heightLine") {
			//3Dオブジェクトを生成
			WireObject* newObject = WireObject::Create();
			newObject->SetModel(model);
			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.translation);
			newObject->SetPosition(pos);

			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rotation);
			newObject->SetRotation(rot);

			// 座標
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scaling);
			newObject->SetScale(scale);

			// 配列に登録
			heightWireObjects.push_back(newObject);
			heightLineEndScale = (newObject->GetScale());
		}
		else if (objectData.fileName == "widthLine") {
			//3Dオブジェクトを生成
			WireObject* newObject = WireObject::Create();
			newObject->SetModel(model);
			// 座標
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.translation);
			newObject->SetPosition(pos);

			// 回転角
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rotation);
			newObject->SetRotation(rot);

			// 座標
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scaling);
			newObject->SetScale(scale);

			// 配列に登録
			widthWireObjects.push_back(newObject);
			widthLineEndScale = (newObject->GetScale());
		}
	}


	//地面の線
	//ライン初期化
	lineModel->Initialize(dxCommon->GetDevice(), 0, 0);
	lineModel->SetImageData(XMFLOAT4(255, 255, 255, 1));

	//パーティクル
	particles->Initialize("line.png");

	redParticles->Initialize("redline.png");

	//敵読み込み
	for (int i = 0; i < enemysize; i++) {
		std::unique_ptr<Enemy>newObject = std::make_unique<Enemy>();
		newObject->Initialize();
		newObject->SetPosition(enemycsv->GetPosition(i));
		newObject->SetRotation(enemycsv->GetRotation(i));
		newObject->SetScale(enemycsv->GetScale(i));
		newObject->SetType(enemycsv->Getmove(i));
		newObject->SetShotType(enemycsv->GetShotType(i));
		newObject->Update();
		enemys.push_back(std::move(newObject));
	}
	//白線を元の位置に戻す
	lineObjects.clear();
	for (int i = 0; i < maxLine; i++) {
		std::unique_ptr<LineObject>newobject = std::make_unique<LineObject>();
		newobject->Initialize();
		newobject->SetModel(lineModel);
		newobject->SetStartPosition(XMFLOAT3(linePosX, -linePosY, (float)lineSpaceZ * i + lineSpaceZ));
		newobject->SetEndPosition(XMFLOAT3(-linePosX, -linePosY, (float)lineSpaceZ * i + lineSpaceZ));
		newobject->SetRotation(XMFLOAT3(0.0f, 0.0f, XMConvertToRadians(90.0f)));
		floorEndScale = newobject->GetScale();
		lineObjects.push_back(std::move(newobject));
	}

	//ボス
	Boss::SetPlayer(player);
	Boss::SetDxCommon(dxCommon);
	boss->Initialize();
	boss->SetRotation(XMFLOAT3(0, 0, 0));
	boss->Reset();
}

void GamePlayScene::Finalize()
{
}

void GamePlayScene::Update()
{
	//遷移演出用
	if (isTransition || isBackTransition) {
		Transition(nextScene);
	}

	//メニュー画面
	if (isMenu) {
		menuSprite->Update();
		//メニューから戻るとき
		if (backMenu) {
			menuSprite->SetPosition(XMFLOAT2((float)easeOutQuad(maxTime, end, start - end, backtime), WinApp::window_height / 2));
			if (backtime < maxTime) {
				backtime++;
			}
		}
		else {
			menuSprite->SetPosition(XMFLOAT2((float)easeOutQuad(maxTime, start, end - start, time), WinApp::window_height / 2));
			if (time < maxTime) {
				time++;
			}
		}
		if (input_->TriggerKey(DIK_SPACE)) {
			if (time == maxTime) {
				time = 0;
				backMenu = true;
			}
		}
		if (backMenu) {
			if (backtime == maxTime) {
				isMenu = false;
				backMenu = false;
				backtime = 0;
			}
		}

	}
	else {
		//ゲームオーバー演出
		if (isGameOver) {
			//死んだ瞬間
			if (gameOverTime == 0) {
				Particle(player->GetPosition());
			}
			//時間を増やす
			gameOverTime++;
			//横の白線が散る演出
			if (gameOverTime == gameOverMaxTime) {
				//縦向きの白線
				for (auto& wireobject : heightWireObjects) {
					linepos = wireobject->GetPosition();
					//linepos.z = lineZ;
					Particle(linepos);
				}
				lineLose = true;
			}
			if (nextSceneTime == gameOverTime) {
				particles->Update();
				//戻っている最中は押しても反応しない
				if (isBackTransition) {

				}
				//ゲームオーバーに移行
				else
				{
					nextScene = Dead;
					isTransition = true;
					isGameOver = false;
					gameOverTime = 0;
				}
			}

		}


		//メニュー
		if (input_->TriggerKey(DIK_M)) {
			menuSprite->SetPosition(XMFLOAT2((float)easeOutQuad(maxTime, start, end - start, time), WinApp::window_height / 2));
			isMenu = true;
			time = 0;
		}
		//操作ガイド
		OperationGuide();
		//右スティック
		stickRSprite->Update();
		//左スティック
		stickLSprite->Update();
		//右トリガー
		buttonRTSprite->Update();
		//左トリガー
		buttonLTSprite->Update();
		//雑魚敵が生きてる
		if (isEnemyAlive) {
			eye.z += 0.05f;
			target.z = eye.z + 1;

			camera->SetEye(eye);
			camera->SetTarget(target);
			camera->Update();

			//地面
			for (auto& object : objects) {
				object->Update();
			}
			//横向きの白線
			for (auto& wireobject : heightWireObjects) {
				wireobject->Update();
			}
			//縦向きの白線
			for (auto& wireobject : widthWireObjects) {
				wireobject->Update();
			}

			//地面の真ん中の白線
			for (auto& lineobject : lineObjects) {
				lineobject->Update();
				//カメラより後ろに行ったら一番後ろの白線の続きの座標になる
				if (lineobject->GetStartPosition().z <= eye.z) {
					//一時的な変数
					XMFLOAT3 pos = {};
					//一番後ろの座標の続きの座標
					for (auto& searchlineobject : lineObjects) {
						if (pos.z <= searchlineobject->GetStartPosition().z) {
							pos = searchlineobject->GetStartPosition();
						}
					}
					pos.z += lineSpaceZ;
					lineobject->SetStartPosition(pos);
					pos.x = -linePosX;
					lineobject->SetEndPosition(pos);
				}
			}

			//プレイヤー
			player->SetPositionZ(eye.z + playerSpaceZ);
			//プレイヤーが動くように判定を送る
			player->SetIsTitle(isTitle);
			player->Update();
			//死んだ判定
			if (player->GetIsDead()) {
				isGameOver = true;
			}

			//敵
			for (std::unique_ptr<Enemy>& enemy : enemys)
			{
				enemy->Update();
				//死んだ際のパーティクル
				if (enemy->GetisDead()) {
					EnemyParticle(enemy->GetPos());
				}
			}
			//敵の死んだ処理
			enemys.remove_if([](std::unique_ptr<Enemy>& enemy) {
				return enemy->GetisDead();
				});

			//パーティクル
			particles->Update();
			redParticles->Update();
			//当たり判定
			AllCollision();

			//敵の全滅判定
			if (enemys.empty()) {
				isEnemyAlive = false;
			}

		}
		//ボスシーンに移行
		else {
			//ボス戦に移行
			isBoss = true;

			////ボスシーン生成
			//BaseScene* changeScene = new BossScene();
			//sceneManager_->SetNextScene(changeScene);
		}

		//ボス戦
		if (isBoss) {
			//地面
			for (auto& object : objects) {
				object->Update();
			}
			//横向きの白線
			for (auto& wireobject : heightWireObjects) {
				wireobject->Update();
			}
			//縦向きの白線
			for (auto& wireobject : widthWireObjects) {
				wireobject->Update();
			}
			//地面の真ん中の白線
			for (auto& lineobject : lineObjects) {
				lineobject->Update();
			}
			//パーティクル
			particles->Update();
			redParticles->Update();
			//カメラ更新
			camera->Update();
			//プレイヤー更新
			player->SetPositionZ(eye.z + playerSpaceZ);
			player->SetIsTitle(isTitle);
			player->Update();

			//死んだ判定
			if (player->GetIsDead()) {
				isGameOver = true;
			}
			//ボス更新
			boss->Update();
			//判定
			AllCollision();

			//ボスの死亡
			if (boss->GetisDead()) {
				//クリア演出
				ClearTransition();
			}
		}
	}

}

void GamePlayScene::Draw()
{
	//オブジェクト描画
	Object3d::PreDraw(dxCommon_->GetCommandlist());
	if (!isGameOver) {
		//プレイヤー
		player->Draw();
	}

	//敵
	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		enemy->Draw();
	}
	
	//ボス
	if (isBoss) {
		boss->Draw();
	}

	////地面
	for (auto& object : objects) {
		object->Draw();
	}

	//地面の真ん中の白線
	for (auto& lineobject : lineObjects) {
		lineobject->Draw(dxCommon_->GetCommandlist());
	}



	Object3d::PostDraw();

	//ワイヤーオブジェクト描画
	WireObject::PreDraw(dxCommon_->GetCommandlist());
	if (!isGameOver) {
		//プレイヤー
		player->WireDraw();
	}

	//敵
	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		enemy->WireDraw();
	}

	//ボス
	if (isBoss) {
		boss->WireDraw();
	}
	if (!lineLose) {
		//縦向きの線
		for (auto& wireobject : heightWireObjects) {
			wireobject->Draw();
		}
		//横向きの白線
		for (auto& wireobject : widthWireObjects) {
			wireobject->Draw();
		}
	}
	WireObject::PostDraw();

	//パーティクル
	particles->Draw();
	redParticles->Draw();

	//デバッグ表示
	//プレイヤー
	player->DebugDraw(dxCommon_->GetCommandlist());

	//敵
	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		enemy->DebugDraw(dxCommon_->GetCommandlist());
	}

	//ボス
	if (isBoss) {
		boss->LineDraw(dxCommon_->GetCommandlist());
	}
	//スプライト描画
	spriteCommon->PreDraw();
	if (isMenu) {
		menuSprite->Draw();
	}

	if (isHit) {
		hitSprite->Draw();
	}

	//操作ガイド
	if (stickRAlpha > 0) {
		stickRSprite->Draw();
	}
	if (stickLAlpha > 0) {
		stickLSprite->Draw();
	}
	if (buttonRTAlpha > 0) {
		buttonRTSprite->Draw();
	}
	if (buttonLTAlpha > 0) {
		buttonLTSprite->Draw();
	}
	//遷移演出
	transitionWhiteSprite->Draw();
	//クリア演出
	if (boss->GetisDead()) {
		clearWhiteSprite->Draw();
	}
	spriteCommon->PostDraw();
}

//判定一覧
void GamePlayScene::AllCollision()
{
	//自弾リストの取得
	const std::list<std::unique_ptr<PlayerBullet>>& playerBullets = player->GetBullet();

	//自キャラと敵の判定
	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		if (player->GetCubeObject()->CheakCollision(enemy->GetCubeObject())) {
			player->OnCollision();
			Particle(player->GetPosition());
		}
	}

	//自キャラと敵の弾
	for (std::unique_ptr<Enemy>& enemy : enemys) {
		for (const std::unique_ptr<EnemyBullet>& enemybullet : enemy->GetBullet()) {
			if (player->GetCubeObject()->CheakCollision(enemybullet->GetCubeObject())) {
				enemybullet->OnCollision();
				player->OnCollision();
				Particle(player->GetPosition());

			}
		}

	}

	//自弾と敵弾の判定
	for (std::unique_ptr<Enemy>& enemy : enemys) {
		for (const std::unique_ptr<EnemyBullet>& enemybullet : enemy->GetBullet()) {
			for (const std::unique_ptr<PlayerBullet>& playerbullet : playerBullets) {
				if (playerbullet->GetCubeObject()->CheakCollision(enemybullet->GetCubeObject())) {
					playerbullet->OnCollision();
					enemybullet->OnCollision();
				}
			}
		}
	}


	//自弾と敵の判定
	for (const std::unique_ptr<PlayerBullet>& playerbullet : playerBullets) {
		for (std::unique_ptr<Enemy>& enemy : enemys) {
			if (playerbullet->GetCubeObject()->CheakCollision(enemy->GetCubeObject())) {
				playerbullet->OnCollision();
				enemy->OnCollision();
			}
		}
	}


	//レティクルと敵の当たり判定
	for (std::unique_ptr<Enemy>& enemy : enemys) {
		if (EnemyLineCollision(player->GetPosition(), player->GetEndPosition(), enemy->GetPos(), enemy->GetScale())) {
			player->SetIsEnemyReticleCol(true);
			break;
		}
		else {
			player->SetIsEnemyReticleCol(false);
		}
	}

	//自キャラとボス弾
	for (const std::unique_ptr<EnemyBullet>& bossybullet : boss->GetBullet()) {
		if (player->GetCubeObject()->CheakCollision(bossybullet->GetCubeObject())) {
			bossybullet->OnCollision();
			player->OnCollision();
			Particle(player->GetPosition());

		}
	}

	//自弾とボス弾
	for (const std::unique_ptr<EnemyBullet>& bossybullet : boss->GetBullet()) {
		for (const std::unique_ptr<PlayerBullet>& playerbullet : playerBullets) {
			if (playerbullet->GetCubeObject()->CheakCollision(bossybullet->GetCubeObject())) {
				playerbullet->OnCollision();
				bossybullet->OnCollision();
			}
		}
	}

	//自弾とボスの判定
	for (const std::unique_ptr<PlayerBullet>& playerbullet : playerBullets) {
		if (playerbullet->GetCubeObject()->CheakCollision(boss->GetCubeObject())) {
			playerbullet->OnCollision();
			boss->OnCollision();
			EnemyParticle(boss->GetPos());
		}
	}

	//レティクルとボスの当たり判定
	if (isBoss) {
		if (EnemyLineCollision(player->GetPosition(), player->GetEndPosition(), boss->GetPos(), boss->GetScale())) {
			player->SetIsEnemyReticleCol(true);
		}
		else {
			player->SetIsEnemyReticleCol(false);
		}
	}

}

//敵とレティクルの当たり判定
bool GamePlayScene::EnemyLineCollision(XMFLOAT3 lineStartPos, XMFLOAT3 lineEndPos, XMFLOAT3 enemyPos, XMFLOAT3 enemyScale)
{
	// 線分の方向ベクトル
	Vector3 dir = { lineEndPos.x - lineStartPos.x, lineEndPos.y - lineStartPos.y, lineEndPos.z - lineStartPos.z };

	// 線分の始点から球の中心までのベクトル
	Vector3 lineToSphere = { enemyPos.x - lineStartPos.x, enemyPos.y - lineStartPos.y, enemyPos.z - lineStartPos.z };

	// 線分の方向ベクトルの長さの二乗
	float dirLengthSq = dir.x * dir.x + dir.y * dir.y + dir.z * dir.z;

	// 線分の方向ベクトルがゼロベクトルでないか確認
	if (dirLengthSq > 0.0f) {
		// 線分の始点から球の中心へのベクトルと線分の方向ベクトルの内積
		float t = (lineToSphere.x * dir.x + lineToSphere.y * dir.y + lineToSphere.z * dir.z) / dirLengthSq;

		// 最寄りの点が線分上にあるか確認
		if (t >= 0.0f && t <= 1.0f) {
			// 線分上の最寄りの点
			Vector3 closestPoint = {
				lineStartPos.x + t * dir.x,
				lineStartPos.y + t * dir.y,
				lineStartPos.z + t * dir.z
			};

			// その点と球の中心との距離の二乗
			float distanceSq = (closestPoint.x - enemyPos.x) * (closestPoint.x - enemyPos.x) +
				(closestPoint.y - enemyPos.y) * (closestPoint.y - enemyPos.y) +
				(closestPoint.z - enemyPos.z) * (closestPoint.z - enemyPos.z);

			// 球の半径の二乗
			float radiusSq = enemyScale.x * enemyScale.x;

			// 距離の二乗が半径の二乗以下ならば交差している
			return distanceSq <= radiusSq;
		}
	}

	// 線分がゼロベクトルならば交差しない
	return false;
}

//パーティクル処理
void GamePlayScene::Particle(XMFLOAT3 pos_)
{
	//パーティクル
	particles->SetEmitterPos(pos_);
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
		//const float rnd_acc = 0.001f;
		XMFLOAT3 acc{};
		//acc.y = -(float)rand() / RAND_MAX * rnd_acc;

		//色
		const float rnd_color = 1.0f;
		XMFLOAT4 color{  };
		color.x = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;
		color.y = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;
		color.z = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;
		color.w = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;
		//時間
		const int rnd_life = 40;
		int life = 0;
		life = rand() % rnd_life + 1;
		life += 20;
		//追加
		particles->Add(life, pos, vel, acc, 0.2f, 0.0f, color, true);
	}
	particles->Update();
}

//敵のパーティクル処理
void GamePlayScene::EnemyParticle(XMFLOAT3 pos_)
{
	//パーティクル
	redParticles->SetEmitterPos(pos_);
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
		//const float rnd_acc = 0.001f;
		XMFLOAT3 acc{};
		//acc.y = -(float)rand() / RAND_MAX * rnd_acc;

		//色
		const float rnd_color = 1.0f;
		XMFLOAT4 color{  };
		color.x = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;
		color.y = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;
		color.z = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;
		color.w = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;
		//時間
		const int rnd_life = 40;
		int life = 0;
		life = rand() % rnd_life + 1;
		life += 20;
		//追加
		redParticles->Add(life, pos, vel, acc, 0.5f, 0.0f, color, true);
	}
	redParticles->Update();
}

//操作ガイド
void GamePlayScene::OperationGuide()
{
	//左スティック
	if (input_->LStickUp() || input_->LStickDown() || (input_->LStickLeft() || input_->LStickRight())) {
		stickLCount++;
		stickLNotCount = 0;

	}//長く触れていない場合
	else {
		stickLNotCount++;
		if (maxGuideNotCount <= stickLNotCount) {
			stickLCount = 0;
			//だんだんアルファ値をあげていく
			if (stickLAlpha < 1.0f) {
				stickLAlpha += 0.05f;
			}
		}
	}
	//一定時間触れるとアルファ値を下げていく
	if (stickLCount >= maxGuideCount) {
		if (stickLAlpha > 0) {
			stickLAlpha -= 0.05f;
		}
	}
	//アルファ値
	stickLSprite->SetAlpha(stickLAlpha);

	//右スティック
	if (input_->RStickUp() || input_->RStickDown() || (input_->RStickLeft() || input_->RStickRight())) {
		stickRCount++;
		stickRNotCount = 0;
	}//長く触れていない場合
	else {
		stickRNotCount++;
		if (maxGuideNotCount <= stickRNotCount) {
			stickRCount = 0;
			//だんだんアルファ値をあげていく
			if (stickRAlpha < 1.0f) {
				stickRAlpha += 0.05f;
			}
		}
	}
	//一定時間触れるとアルファ値を下げていく
	if (stickRCount >= maxGuideCount) {
		if (stickRAlpha > 0) {
			stickRAlpha -= 0.05f;
		}
	}
	//アルファ値
	stickRSprite->SetAlpha(stickRAlpha);

	//右トリガー
	if (input_->PushRButton()) {
		buttonRTCount++;
		buttonRTNotCount = 0;
	}//長く触れていない場合
	else {
		buttonRTNotCount++;
		if (maxGuideNotCount <= buttonRTNotCount) {
			buttonRTCount = 0;
			//だんだんアルファ値をあげていく
			if (buttonRTAlpha < 1.0f) {
				buttonRTAlpha += 0.05f;
			}
		}
	}
	//一定時間触れるとアルファ値を下げていく
	if (buttonRTCount >= maxGuideCount) {
		if (buttonRTAlpha > 0) {
			buttonRTAlpha -= 0.05f;
		}
	}
	//アルファ値
	buttonRTSprite->SetAlpha(buttonRTAlpha);

	//左トリガー
	if (input_->PushLButton()) {
		buttonLTCount++;
		buttonLTNotCount = 0;
	}//長く触れていない場合
	else {
		buttonLTNotCount++;
		if (maxGuideNotCount <= buttonLTNotCount) {
			buttonLTNotCount = 0;
			//だんだんアルファ値をあげていく
			if (buttonLTAlpha < 1.0f) {
				buttonLTAlpha += 0.05f;
			}
		}
	}
	//一定時間触れるとアルファ値を下げていく
	if (buttonLTCount >= maxGuideCount) {
		if (buttonLTAlpha > 0) {
			buttonLTAlpha -= 0.05f;
		}
	}
	//アルファ値
	buttonLTSprite->SetAlpha(buttonLTAlpha);

}

//クリア演出
void GamePlayScene::ClearTransition()
{
	float startSize = 0.0f;
	float sizeX = 0.0f;
	float sizeY = 0.0f;
	//	遷移中
	if (clearTime <= clearMaxTime) {
		//イージング
		sizeX = (float)easeOutQuad(clearMaxTime, startSize, WinApp::window_width - sizeX, clearTime);
		sizeY = (float)easeOutQuad(clearMaxTime, startSize, WinApp::window_height - sizeY, clearTime);
		//座標代入
		clearWhiteSprite->SetSize(XMFLOAT2(sizeX, sizeY));

		//タイム
		clearTime++;

	}
	//遷移後のシーンチェンジの時間
	else if (clearTime >= clearChangeSceneTime) {
		//シーンチェンジ
		//クリアシーン生成
		BaseScene* changeScene = new ClearScene();
		sceneManager_->SetNextScene(changeScene);

		isClearBack = true;
		clearTime = 0;
	}
	else {
		//タイム
		clearTime++;
	}



	clearWhiteSprite->Update();

}