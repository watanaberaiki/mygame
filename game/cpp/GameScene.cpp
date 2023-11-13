

#include "GameScene.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	delete spriteCommon;
	//3dオブジェクト解放
	delete sphereObj;
	delete blockObj;
	for (Object3d*& object : objects) {
		delete object;
	}
	for (WireObject*& wireobject : heightWireObjects) {
		delete wireobject;
	}
	////レベルデータ解放
	//delete leveldata;
	//delete floorobj;
	//3Dモデル解放
	delete sphereModel;
	delete blockModel;
	FBX_SAFE_DELETE(boneTestModel);
	FBX_SAFE_DELETE(cube);
	for (int i = 0; i < bonetestsize; i++) {
		FBX_SAFE_DELETE(bonetest[i]);
	}
	delete enemycsv;
}

void GameScene::Initialize(DirectXCommon* dxCommon, ImguiManager* imgui)
{
	dxCommon_ = dxCommon;
	input_ = Input::GetInstance();

	imgui_ = imgui;

	//モデル名を指定してファイル読み込み
	/*FbxLoader::GetInstance()->LoadModelFromFile("cube");*/

	eye = XMFLOAT3(0, 0, 5);	//視点座標
	target = XMFLOAT3(0, 0, 10);	//注視点座標
	up = XMFLOAT3(0, 1, 0);		//上方向ベクトル
	//カメラ
	camera = new Camera();
	camera->Initialize();
	camera->SetEye(eye);
	camera->SetTarget(target);
	camera->SetUp(up);
	camera->Update();

	//ワイヤーオブジェクト
	WireObject::SetCamera(camera);
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
	resorcemanager->LoadObj("blackcube");
	resorcemanager->LoadObj("redcube");
	resorcemanager->LoadObj("floor");

	//プレイヤー
	Player::SetDxCommon(dxCommon);
	player = new Player;
	player->Initialize();



	//敵
	enemycsv = new CSVLoader();
	enemycsv->LoadCSV("Resources/csv/enemy.csv");
	Enemy::SetPlayer(player);
	Enemy::SetDxCommon(dxCommon);
	
	//ボス
	Boss::SetPlayer(player);
	Boss::SetDxCommon(dxCommon);
	boss = new Boss();
	boss->Initialize();
	boss->SetRotation(XMFLOAT3(0, 0, 0));
	boss->SetScale(XMFLOAT3(0.1f, 0.4f, 0.1f));
	//スプライト共通部の初期化
	spriteCommon = new SpriteCommon;
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

	//スプライト初期位置
	marioSprite->SetPosition({ 800,0 });
	marioSprite->Update();

	//画面遷移用スプライト
	transitionWhiteSprite->SetAnchorPoint(XMFLOAT2(0.5, 0.5));
	transitionWhiteSprite->SetSize(XMFLOAT2(WinApp::window_width, WinApp::window_height));

	//クリア用スプライト
	clearWhiteSprite->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
	clearWhiteSprite->SetPosition(XMFLOAT2(WinApp::window_width / 2, WinApp::window_height / 2));

	//3Dモデル
	//モデルデータをマップに入れる
	models.insert(std::make_pair("floor", resorcemanager->LoadObj("blackcube")));
	models.insert(std::make_pair("heightLine", resorcemanager->LoadObj("block")));
	models.insert(std::make_pair("widthLine", resorcemanager->LoadObj("block")));



	menuSprite->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
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
	lineModel = new LineModel();
	lineModel->Initialize(dxCommon->GetDevice(), 5.0f, -5.0f);
	lineModel->SetImageData(XMFLOAT4(255, 255, 255, 1));
	for (int i = 0; i < maxLine; i++) {
		std::unique_ptr<LineObject>newobject = std::make_unique<LineObject>();
		newobject->Initialize();
		newobject->SetModel(lineModel);
		newobject->SetPosition(XMFLOAT3(0.0f, -1.8f, (float)20.0f * i + 20));
		newobject->SetRotation(XMFLOAT3(0.0f, 0.0f, XMConvertToRadians(90.0f)));
		floorEndScale = newobject->GetScale();
		lineObjects.push_back(std::move(newobject));
	}
}

void GameScene::Update()
{

	//遷移演出用
	if (isTransition || isBackTransition) {
		Transition(nextScene);
	}

	//パッド更新
	dxinput->Update();




	switch (scene)
	{
	case Title:
		isTitle = true;
		eye = eyeReset;
		target = targetReset;
		camera->SetEye(eye);
		camera->SetTarget(target);
		camera->Update();

		//地面
		for (auto& object : objects) {
			object->Update();
		}

		//スプライト
		titleSprite->Update();

		startSprite->Update();

		//スタート演出
		//スプライトのサイズ、座標変更
		titleSprite->SetSize(
			XMFLOAT2(
				(float)easeOutQuad(startMaxTime, startSizeX, endSize - startSizeX, totalTime),
				(float)easeOutQuad(startMaxTime, startSizeY, endSize - startSizeY, totalTime)
			)
		);
		titleSprite->SetPosition(
			XMFLOAT2(
				(float)easeOutQuad(startMaxTime, startPos, end - startPos, totalTime),
				(float)easeOutQuad(startMaxTime, startPos, endPosY - startPos, totalTime)
			)
		);

		startSprite->SetSize(
			XMFLOAT2(
				(float)easeOutQuad(startMaxTime, startSizeX, endSize - startSizeX, totalTime),
				(float)easeOutQuad(startMaxTime, startSizeY, endSize - startSizeY, totalTime)
			)
		);
		startSprite->SetPosition(
			XMFLOAT2((float)easeOutQuad(startMaxTime, startPos, end - startPos, totalTime),
				(float)easeOutQuad(startMaxTime, startPos, endPosY - startPos, totalTime)
			)
		);

		//スケールがだんだん元の大きさになる
		//プレイヤー
		playerDirectionScale = (float)easeOutQuad(directionMaxTime, playerStartScale, playerEndScale - playerStartScale, directionTime);
		player->SetScale(XMFLOAT3(playerDirectionScale, playerDirectionScale, playerDirectionScale));
		//プレイヤーが動かないように判定を送る
		player->SetIsTitle(isTitle);
		player->Update();
		//地面の白線
		floorDirectionScale.x = (float)easeOutQuad(directionMaxTime, floorStartScale.x, floorEndScale.x - floorStartScale.x, directionTime);
		floorDirectionScale.y = (float)easeOutQuad(directionMaxTime, floorStartScale.y, floorEndScale.y - floorStartScale.y, directionTime);
		floorDirectionScale.z = (float)easeOutQuad(directionMaxTime, floorStartScale.z, floorEndScale.z - floorStartScale.z, directionTime);
		for (auto& lineobject : lineObjects) {
			lineobject->SetScale(floorDirectionScale);
			lineobject->Update();
		}
		//縦向きの白線
		lineDirectionScale.x = (float)easeOutQuad(directionMaxTime, lineStartScale.x, heightLineEndScale.x - lineStartScale.x, directionTime);
		lineDirectionScale.y = (float)easeOutQuad(directionMaxTime, lineStartScale.y, heightLineEndScale.y - lineStartScale.y, directionTime);
		lineDirectionScale.z = (float)easeOutQuad(directionMaxTime, lineStartScale.z, heightLineEndScale.z - lineStartScale.z, directionTime);
		for (auto& wireobject : heightWireObjects) {
			wireobject->SetScale(lineDirectionScale);
			wireobject->Update();
		}

		//横向きの白線
		lineDirectionScale.x = (float)easeOutQuad(directionMaxTime, lineStartScale.x, widthLineEndScale.x - lineStartScale.x, directionTime);
		lineDirectionScale.y = (float)easeOutQuad(directionMaxTime, lineStartScale.y, widthLineEndScale.y - lineStartScale.y, directionTime);
		lineDirectionScale.z = (float)easeOutQuad(directionMaxTime, lineStartScale.z, widthLineEndScale.z - lineStartScale.z, directionTime);
		for (auto& wireobject : widthWireObjects) {
			wireobject->SetScale(lineDirectionScale);
			wireobject->Update();
		}


		//戻っている最中は押しても反応しない
		if (isBackTransition) {

		}
		else
		{
			//ゲームシーンへのシーンチェンジ
			if (input_->TriggerKey(DIK_SPACE)||input_->TriggerPadButton(XINPUT_GAMEPAD_A)) {
				isGameOver = false;
				isStart = true;
				isEnemyAlive = true;
				//敵読み込み
				for (int i = 0; i < enemysize; i++) {
					std::unique_ptr<Enemy>newObject = std::make_unique<Enemy>();
					newObject->Initialize();
					newObject->SetPosition(enemycsv->GetPosition(i));
					newObject->SetRotation(enemycsv->GetRotation(i));
					newObject->SetScale(enemycsv->GetScale(i));
					newObject->SetType(enemycsv->Getmove(i));
					newObject->Update();
					enemys.push_back(std::move(newObject));
				}
			}
		}
		//パーティクル更新処理
		for (std::unique_ptr<ParticleManager>& particle : particles)
		{
			particle->Update();
		}

		//スタート演出
		if (isStart) {
			
			//時間を増やす
			if (startMaxTime >= totalTime) {
				totalTime += 1;
			}
			//スタート演出
			else if (startMaxTime < totalTime) {
				//プレイヤー
				player->SetPositionZ(eye.z + 4.0f);
				//時間を増やす
				if (directionMaxTime >= directionTime) {
					directionTime += 1;
				}
				else if (directionMaxTime < directionTime) {
					scene = Game;
					isStart = false;
					totalTime = 0;
					directionTime = 0;
					isTitle = false;
				}
			}
		}
		else {
			//パーティクル生成
			partPos = eye;
			partPos.y = eye.y + 5;
			partPos.z = eye.z + 2;
			particleTime++;
			if (particleMaxTime <= particleTime) {
				TitleParticle(partPos);
				particleTime = 0;
			}
		}

		break;
	case Game:
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
					for (std::unique_ptr<ParticleManager>& particle : particles)
					{
						particle->Update();
					}
					//戻っている最中は押しても反応しない
					if (isBackTransition) {

					}
					else
					{
						isTransition = true;
						nextScene = Dead;
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
					if (lineobject->GetPosition().z <= eye.z) {
						//一時的な変数
						XMFLOAT3 pos = {};
						//一番後ろの座標の続きの座標
						for (auto& searchlineobject : lineObjects) {
							if (pos.z <= searchlineobject->GetPosition().z) {
								pos = searchlineobject->GetPosition();
							}
						}
						//pos = lineObjects.back().get()->GetPosition();
						pos.z += 20.0f;
						lineobject->SetPosition(pos);
					}
				}

				//プレイヤー
				player->SetPositionZ(eye.z + 4.0f);
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

				//敵の全滅判定
				if (enemys.empty()) {
					isEnemyAlive = false;
				}

			}
			else {
				scene = BossFight;
				eye.z += 10.0f;
				target.z = eye.z + 1;
				camera->SetEye(eye);
				camera->SetTarget(target);
				camera->Update();
				//プレイヤー
				player->SetPositionZ(eye.z + 4.0f);
				player->Update();

				//ボス
				boss->SetPosition(player->GetPosition());
				boss->SetPositionZ(player->GetPosition().z + 10.0f);
				boss->Reset();
			}

		}
		break;
		//ボス戦
	case BossFight:
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
					for (std::unique_ptr<ParticleManager>& particle : particles)
					{
						particle->Update();
					}
					//戻っている最中は押しても反応しない
					if (isBackTransition) {

					}
					else
					{
						isTransition = true;
						nextScene = Dead;
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
			//仮のシーンチェンジ
			if (input_->TriggerKey(DIK_RETURN)) {
				scene = Title;
			}

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
			for (std::unique_ptr<ParticleManager>& particle : particles)
			{
				particle->Update();
			}
			//カメラ更新
			camera->Update();
			//プレイヤー更新
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
				ClearTransition();
			}
		}
		break;
	case Clear:

		for (std::unique_ptr<ParticleManager>& particle : particles)
		{
			particle->Update();
		}
		//戻っている最中は押しても反応しない
		if (isBackTransition) {

		}
		else
		{
			if (input_->TriggerKey(DIK_SPACE)||input_->TriggerPadButton(XINPUT_GAMEPAD_A)) {
				isTransition = true;
				nextScene = Title;
				player->Reset();
			}
		}
		//戻り演出
		ClearBackTransition();

		clearSprite->Update();

		break;
	case Dead:
		//スプライト
		gameOverSprite->Update();

		for (std::unique_ptr<ParticleManager>& particle : particles)
		{
			particle->Update();
		}
		//戻っている最中は押しても反応しない
		if (isBackTransition) {

		}
		else
		{
			if (input_->TriggerKey(DIK_SPACE)|| input_->TriggerPadButton(XINPUT_GAMEPAD_A)) {
				isTransition = true;
				nextScene = Title;
				player->Reset();
			}
		}
		break;
	}

	//imgui更新
	imgui_->Begin();
	//表示項目
	ImGui::Text("eye:%f", eye.z);
	ImGui::Text("player:%f", player->GetPosition().z);
	//ボタンを押したら
	if (ImGui::Button("Save")) {

	}
	//デモウィンドウ
	ImGui::ShowDemoWindow();

	imgui_->End();
}

void GameScene::Draw()
{
	switch (scene)
	{
	case Title:
		//オブジェクト描画
		Object3d::PreDraw(dxCommon_->GetCommandlist());

		//プレイヤー
		player->Draw();

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

		//プレイヤー
		player->WireDraw();

		//縦向きの線
		for (auto& wireobject : heightWireObjects) {
			wireobject->Draw();
		}
		//横向きの白線
		for (auto& wireobject : widthWireObjects) {
			wireobject->Draw();
		}

		WireObject::PostDraw();

		//パーティクル
		for (std::unique_ptr<ParticleManager>& particle : particles)
		{
			particle->Draw();
		}

		//スプライト
		spriteCommon->PreDraw();

		titleSprite->Draw();

		startSprite->Draw();

		spriteCommon->PostDraw();

		break;
	case Game:
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
		if (isMenu) {
			menuSprite->Draw();
		}

		if (isHit) {
			hitSprite->Draw();
		}

		spriteCommon->PostDraw();
		break;
		//ボス戦
	case BossFight:
		//オブジェクト描画
		Object3d::PreDraw(dxCommon_->GetCommandlist());
		if (!isGameOver) {
			//プレイヤー
			player->Draw();
		}

		boss->Draw();

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

		boss->WireDraw();
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
		for (std::unique_ptr<ParticleManager>& particle : particles)
		{
			particle->Draw();
		}

		//デバッグ表示
		//プレイヤー
		player->DebugDraw(dxCommon_->GetCommandlist());



		//スプライト描画
		spriteCommon->PreDraw();
		if (isMenu) {
			menuSprite->Draw();
		}

		if (isHit) {
			hitSprite->Draw();
		}

		//クリア演出用
		if (boss->GetisDead()) {
			clearWhiteSprite->Draw();
		}

		spriteCommon->PostDraw();
		break;

	case Clear:
		//スプライト描画
		spriteCommon->PreDraw();

		//演出白背景
		clearWhiteSprite->Draw();

		//クリア
		clearSprite->Draw();

		spriteCommon->PostDraw();
		break;
	case Dead:

		//スプライト描画
		spriteCommon->PreDraw();

		gameOverSprite->Draw();

		spriteCommon->PostDraw();
	}

	//パーティクル
	for (std::unique_ptr<ParticleManager>& particle : particles)
	{
		particle->Draw();
	}

	//スプライト描画
	spriteCommon->PreDraw();

	//遷移演出
	transitionWhiteSprite->Draw();

	spriteCommon->PostDraw();


}

void GameScene::AllCollision()
{
	//自弾リストの取得
	const std::list<std::unique_ptr<PlayerBullet>>& playerBullets = player->GetBullet();

	//自キャラと敵の判定
	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		if (player->GetCubeObject()->CheakCollision(enemy->GetCubeObject())) {
			player->OnCollision();
		}
	}

	//自キャラと敵の弾
	for (std::unique_ptr<Enemy>& enemy : enemys) {
		for (const std::unique_ptr<EnemyBullet>& enemybullet : enemy->GetBullet()) {
			if (player->GetCubeObject()->CheakCollision(enemybullet->GetCubeObject())) {
				enemybullet->OnCollision();
				player->OnCollision();
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

	//自キャラとボス弾
	for (const std::unique_ptr<EnemyBullet>& bossybullet : boss->GetBullet()) {
		if (player->GetCubeObject()->CheakCollision(bossybullet->GetCubeObject())) {
			bossybullet->OnCollision();
			player->OnCollision();
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
		}
	}

}

void GameScene::Particle(XMFLOAT3 pos_)
{
	//XMFLOAT3 posA = pos_;
	//posA.z += 5;
	//パーティクル
	std::unique_ptr<ParticleManager>newparticle = std::make_unique<ParticleManager>();
	newparticle->Initialize("line.png");
	newparticle->SetEmitterPos(pos_);
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

void GameScene::TitleParticle(XMFLOAT3 pos_)
{
	//パーティクル
	std::unique_ptr<ParticleManager>newparticle = std::make_unique<ParticleManager>();
	newparticle->Initialize("line.png");
	newparticle->SetEmitterPos(pos_);
	for (int i = 0; i < 3; i++) {
		//X,Y,Z全て[-5.0f,+5.0f]でランダムに分布
		const float rnd_pos = 20.0f;
		XMFLOAT3 pos{};
		pos.x = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;
		pos.y = (float)rand() / RAND_MAX * rnd_pos / 2.0f;
		pos.z = (float)rand() / RAND_MAX * rnd_pos / 2.0f;

		//X,Y,Z全て[-0.05f,+0.05f]でランダムに分布
		const float rnd_vel = 0.1f;
		XMFLOAT3 vel{};
		//vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.y = -rnd_vel;
		//vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;

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
		const int rnd_life = 80;
		int life = 0;
		life = rand() % rnd_life + 1;
		life += 20;

		//追加
		newparticle->Add(life, pos, vel, acc, 0.5f, 0.1f, color);
	}
	newparticle->Update();
	particles.push_back(std::move(newparticle));
}

void GameScene::TransitionParticle(XMFLOAT3 pos_)
{
	//パーティクル
	std::unique_ptr<ParticleManager>newparticle = std::make_unique<ParticleManager>();
	newparticle->Initialize("line.png");
	newparticle->SetEmitterPos(pos_);
	for (int i = 0; i < 30; i++) {
		//X,Y,Z全て[-5.0f,+5.0f]でランダムに分布
		const float rnd_pos = 20.0f;
		XMFLOAT3 pos{};
		pos.x = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;
		pos.y = (float)rand() / RAND_MAX * rnd_pos / 2.0f;
		//pos.z = (float)rand() / RAND_MAX * rnd_pos / 2.0f;

		//X,Y,Z全て[-0.05f,+0.05f]でランダムに分布
		const float rnd_vel = 0.1f;
		XMFLOAT3 vel{};
		//vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.y = -rnd_vel;
		//vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;

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
		const int rnd_life = 80;
		int life = 0;
		life = rand() % rnd_life + 1;
		life += 20;

		//追加
		newparticle->Add(life, pos, vel, acc, 0.5f, 0.1f, color);
	}
	newparticle->Update();
	particles.push_back(std::move(newparticle));
}

void GameScene::TransitionBackParticle(XMFLOAT3 pos_, int num)
{
	//パーティクル
	std::unique_ptr<ParticleManager>newparticle = std::make_unique<ParticleManager>();
	newparticle->Initialize("line.png");
	newparticle->SetEmitterPos(pos_);
	for (int i = 0; i < num; i++) {
		//X,Y,Z全て[-5.0f,+5.0f]でランダムに分布
		const float rnd_pos = 20.0f;
		XMFLOAT3 pos{};
		pos.x = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;
		pos.y = (float)rand() / RAND_MAX * rnd_pos / 2.0f;
		//pos.z = (float)rand() / RAND_MAX * rnd_pos / 2.0f;

		//X,Y,Z全て[-0.05f,+0.05f]でランダムに分布
		const float rnd_vel = 0.1f;
		XMFLOAT3 vel{};
		//vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.y = rnd_vel;
		//vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;

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
		newparticle->Add(life, pos, vel, acc, 0.5f, 0.1f, color);
	}
	newparticle->Update();
	particles.push_back(std::move(newparticle));
}

void GameScene::Transition(Scene nextScene_)
{
	//遷移中
	if (isTransition) {
		//パーティクル座標
		partPos = eye;
		partPos.y = eye.y + 5;
		partPos.z = eye.z;

		//パーティクル
		TransitionParticle(partPos);
		if (MaxDelay <= delayTime) {
			//白い画像の位置
			transitionWhiteSprite->SetPosition(XMFLOAT2(WinApp::window_width / 2, (float)easeOutQuad(MaxTransitionTime, transitionStartPosY, transitionEndposY - transitionStartPosY, totalTransitionTime)));
			transitionWhiteSprite->Update();

			//トータルのタイムを増やす
			if (MaxTransitionTime > totalTransitionTime) {
				totalTransitionTime++;
			}

			//MaxTimeを超えたら戻る演出を入れる
			if (MaxTransitionTime <= totalTransitionTime) {
				isTransition = false;
				isBackTransition = true;
				totalTransitionTime = 0;
				scene = nextScene_;
				delayTime = 0;
			}
		}
		delayTime++;

	}
	//遷移戻り
	else if (isBackTransition) {
		transitionWhiteSprite->Update();
		if (MaxDelay <= delayTime) {
			//パーティクルの位置
			partPos = eye;
			partPos.y = eye.y - 5;
			partPos.z = eye.z;
			//後半は出ないようにしておく
			if (totalTransitionTime > MaxTransitionTime - 40.0) {

			}
			else {
				//パーティクル
				TransitionBackParticle(partPos, (int)(MaxTransitionTime - totalTransitionTime * 2));
			}

			//白い画像の位置
			transitionWhiteSprite->SetPosition(XMFLOAT2(WinApp::window_width / 2, (float)easeOutQuad(MaxTransitionTime, transitionEndposY, transitionStartPosY - transitionEndposY, totalTransitionTime)));

			//トータルのタイムを増やす
			if (MaxTransitionTime > totalTransitionTime) {
				totalTransitionTime++;
			}
			//MaxTimeを超えたら戻る
			if (MaxTransitionTime <= totalTransitionTime) {
				isTransition = false;
				isBackTransition = false;
				totalTransitionTime = 0;
				delayTime = 0;
			}
		}
		delayTime++;
	}

	for (std::unique_ptr<ParticleManager>& particle : particles)
	{
		particle->Update();
	}
}

double GameScene::easeOutQuad(double time_, double start_, double difference, double totaltime_)
{
	double x = totaltime_ / time_;
	double v = 1 - (1 - x) * (1 - x);
	double ret = difference * v + start_;
	return ret;
}

void GameScene::ClearTransition()
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
		scene = Clear;
		isClearBack = true;
		clearTime = 0;
	}
	else {
		//タイム
		clearTime++;
	}



	clearWhiteSprite->Update();

}

void GameScene::ClearBackTransition()
{
	float sizeX = 0.0f;
	float sizeY = 0.0f;
	//戻り
	if (isClearBack) {
		if (clearBackTime <= clearMaxTime) {
			//イージング
			sizeX = (float)easeOutQuad(clearMaxTime, WinApp::window_width, sizeX - WinApp::window_width, clearBackTime);
			sizeY = (float)easeOutQuad(clearMaxTime, WinApp::window_height, sizeY - WinApp::window_height, clearBackTime);
			//座標代入
			clearWhiteSprite->SetSize(XMFLOAT2(sizeX, sizeY));
			//タイム
			clearBackTime++;
		}
		else {
			isClearBack = false;
			clearBackTime = 0;
		}
	}
	clearWhiteSprite->Update();
}
