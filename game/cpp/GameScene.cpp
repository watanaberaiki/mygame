

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
	for (Object3d*& object : objects) {
		delete object;
	}
	for (WireObject*& wireobject : wireobjects) {
		delete wireobject;
	}
	////レベルデータ解放
	//delete leveldata;
	//delete floorobj;
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

	eye = XMFLOAT3(0, 0, 5);	//視点座標
	target = XMFLOAT3(0, 0, 10);	//注視点座標
	up = XMFLOAT3(0, 1, 0);		//上方向ベクトル
	//カメラ
	camera = new Camera();
	camera->Initialize(input_);
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
	Player::SetInput(input);
	Player::SetDxCommon(dxCommon);
	player = new Player;
	player->Initialize();



	//敵読み込み
	enemycsv = new CSVLoader();
	enemycsv->LoadCSV("Resources/csv/enemy.csv");
	Enemy::SetPlayer(player);
	Enemy::SetDxCommon(dxCommon);
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
	//スプライトにテクスチャ割り当て
	hitsprite->Initialize(spriteCommon, 0);
	mariosprite->Initialize(spriteCommon, 1);
	menu->Initialize(spriteCommon, 2);
	title->Initialize(spriteCommon, 4);
	startsprite->Initialize(spriteCommon, 5);
	blacksprite->Initialize(spriteCommon, 6);
	whitesprite->Initialize(spriteCommon, 7);

	//スプライト初期位置
	mariosprite->SetPosition({ 800,0 });
	mariosprite->Update();

	//画面遷移用スプライト
	whitesprite->SetAnchorPoint(XMFLOAT2(0.5, 0.5));
	whitesprite->SetSize(XMFLOAT2(WinApp::window_width, WinApp::window_height));

	//3Dモデル
	//モデルデータをマップに入れる
	models.insert(std::make_pair("floor", resorcemanager->LoadObj("blackcube")));
	models.insert(std::make_pair("line", resorcemanager->LoadObj("block")));

	menu->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
	menu->SetPosition(XMFLOAT2((float)easeOutQuad(maxTime, start, end - start, time), WinApp::window_height / 2));

	////地面の配置(json読み込み)
	//地面を繰り返し描画するために二つ読み込む(json読み込み)

	// レベルデータの読み込み
	floordata = LoadFile::LoadFileData("test");
	// レベルデータからオブジェクトを生成、配置
	for (auto& objectData : floordata->objects) {
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
		else if (objectData.fileName == "line") {
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
			wireobjects.push_back(newObject);
		}
	}

	// レベルデータの読み込み
	floordata = LoadFile::LoadFileData("test1");
	// レベルデータからオブジェクトを生成、配置
	for (auto& objectData : floordata->objects) {
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
		else if (objectData.fileName == "line") {
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
			wireobjects.push_back(newObject);
			lineEndScales.push_back(newObject->GetScale());
		}
	}


	//地面の線
	//ライン初期化
	linemodel = new LineModel();
	linemodel->Initialize(dxCommon->GetDevice(), 5.0f, -5.0f);
	linemodel->SetImageData(XMFLOAT4(255, 255, 255, 1));
	for (int i = 0; i < maxLine; i++) {
		std::unique_ptr<LineObject>newobject = std::make_unique<LineObject>();
		newobject->Initialize();
		newobject->SetModel(linemodel);
		newobject->SetPosition(XMFLOAT3(0.0f, -1.8f, (float)20.0f * i + 20));
		newobject->SetRotation(XMFLOAT3(0.0f, 0.0f, XMConvertToRadians(90.0f)));
		newobject->Update();
		floorEndScale = newobject->GetScale();
		lineObjects.push_back(std::move(newobject));
	}
}

void GameScene::Update()
{
	//デバッグ用
	if (scene != Clear && scene != Dead) {
		if (input_->TriggerKey(DIK_1)) {
			isTransition = true;
			nextScene = Clear;
		}
		if (input_->TriggerKey(DIK_2)) {
			isTransition = true;
			nextScene = Dead;
		}
	}

	if (isTransition || isBackTransition) {
		Transition(nextScene);
	}



	switch (scene)
	{
	case Title:

		//地面
		for (auto& object : objects) {
			object->Update();
		}

		//スプライト
		title->Update();

		startsprite->Update();

		//戻っている最中は押しても反応しない
		if (isBackTransition) {

		}
		else
		{
			//ゲームシーンへのシーンチェンジ
			if (input_->TriggerKey(DIK_SPACE)) {
				isstart = true;

			}



		}
		//パーティクル更新処理
		for (std::unique_ptr<ParticleManager>& particle : particles)
		{
			particle->Update();
		}

		//スタート演出
		if (isstart) {
			//スプライトのサイズ、座標変更
			title->SetSize(
				XMFLOAT2(
					(float)easeOutQuad(startMaxTime, startsizeX, endsize - startsizeX, totaltime),
					(float)easeOutQuad(startMaxTime, startsizeY, endsize - startsizeY, totaltime)
				)
			);
			title->SetPosition(
				XMFLOAT2(
					(float)easeOutQuad(startMaxTime, startpos, end - startpos, totaltime),
					(float)easeOutQuad(startMaxTime, startpos, endposY - startpos, totaltime)
				)
			);

			startsprite->SetSize(
				XMFLOAT2(
					(float)easeOutQuad(startMaxTime, startsizeX, endsize - startsizeX, totaltime),
					(float)easeOutQuad(startMaxTime, startsizeY, endsize - startsizeY, totaltime)
				)
			);
			startsprite->SetPosition(
				XMFLOAT2((float)easeOutQuad(startMaxTime, startpos, end - startpos, totaltime),
					(float)easeOutQuad(startMaxTime, startpos, endposY - startpos, totaltime)
				)
			);

			if (startMaxTime >= totaltime) {
				totaltime += 1;
			}
			else if (startMaxTime < totaltime) {
				//プレイヤー
				player->SetPositionZ(eye.z + 4.0f);
				//スケールがだんだん元の大きさになる
				//プレイヤー
				playerDirectionScale = (float)easeOutQuad(directionMaxTime, playerStartScale, playerEndScale - playerStartScale, directionTime);
				player->SetScale(XMFLOAT3(playerDirectionScale, playerDirectionScale, playerDirectionScale));
				//地面の白線
				floorDirectionScale.x = (float)easeOutQuad(directionMaxTime, floorStartScale.x, floorEndScale.x - floorStartScale.x, directionTime);
				floorDirectionScale.y = (float)easeOutQuad(directionMaxTime, floorStartScale.y, floorEndScale.y - floorStartScale.y, directionTime);
				floorDirectionScale.z = (float)easeOutQuad(directionMaxTime, floorStartScale.z, floorEndScale.z - floorStartScale.z, directionTime);
				for (auto& lineobject : lineObjects) {
					lineobject->SetScale(floorDirectionScale);
					lineobject->Update();
				}
				//横の壁の白線
				int endCount = 0;
				int wireCount = 0;
				for (auto& endscale : lineEndScales) {
					lineDirectionScale.x = (float)easeOutQuad(directionMaxTime, lineStartScale.x, endscale.x - lineStartScale.x, directionTime);
					lineDirectionScale.y = (float)easeOutQuad(directionMaxTime, lineStartScale.y, endscale.y - lineStartScale.y, directionTime);
					lineDirectionScale.z = (float)easeOutQuad(directionMaxTime, lineStartScale.z, endscale.z - lineStartScale.z, directionTime);
					for (auto& wireobject : wireobjects) {
						if (endCount==wireCount) {
							wireobject->SetScale(lineDirectionScale);
						}
						wireobject->Update();
						wireCount++;
					}
					endCount++;
				}


				//プレイヤーが動かないように判定を送る
				player->SetIsStart(isstart);
				player->Update();
				if (directionMaxTime >= directionTime) {
					directionTime += 1;
				}
				else if (directionMaxTime < directionTime) {
					scene = Game;
					isstart = false;
				}
			}
		}
		else {
			//パーティクル生成
			partpos = eye;
			partpos.y = eye.y + 5;
			partpos.z = eye.z + 2;
			particletime++;
			if (particleMaxtime <= particletime) {
				TitleParticle(partpos);
				particletime = 0;
			}
		}

		break;
	case Game:
		if (isMenu) {
			menu->Update();


			//メニューから戻るとき
			if (backMenu) {
				menu->SetPosition(XMFLOAT2((float)easeOutQuad(maxTime, end, start - end, backtime), WinApp::window_height / 2));
				if (backtime < maxTime) {
					backtime++;
				}
			}
			else {
				menu->SetPosition(XMFLOAT2((float)easeOutQuad(maxTime, start, end - start, time), WinApp::window_height / 2));
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
			//メニュー
			if (input_->TriggerKey(DIK_M)) {
				menu->SetPosition(XMFLOAT2((float)easeOutQuad(maxTime, start, end - start, time), WinApp::window_height / 2));
				isMenu = true;
				time = 0;
			}
			//仮のシーンチェンジ
			if (input_->TriggerKey(DIK_RETURN)) {
				scene = Title;
			}

			if (isEnemyAlive) {
				eye.z += 0.05f;
				target.z = eye.z + 1;
				camera->SetEye(eye);
				camera->SetTarget(target);
				camera->Update();
				matView = camera->GetmatView();

				//地面
				for (auto& object : objects) {
					object->Update();
				}
				//地面の横の白線
				for (auto& wireobject : wireobjects) {
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
				player->SetIsStart(isstart);
				player->Update();

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
				matView = camera->GetmatView();
				//プレイヤー
				player->SetPositionZ(eye.z + 4.0f);
				player->Update();

				//ボス
				boss->SetPosition(player->GetPosition());
				boss->SetPositionZ(player->GetPosition().z + 10.0f);

			}

		}
		break;
		//ボス戦
	case BossFight:
		if (isMenu) {
			menu->Update();


			//メニューから戻るとき
			if (backMenu) {
				menu->SetPosition(XMFLOAT2((float)easeOutQuad(maxTime, end, start - end, backtime), WinApp::window_height / 2));
				if (backtime < maxTime) {
					backtime++;
				}
			}
			else {
				menu->SetPosition(XMFLOAT2((float)easeOutQuad(maxTime, start, end - start, time), WinApp::window_height / 2));
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
			//メニュー
			if (input_->TriggerKey(DIK_M)) {
				menu->SetPosition(XMFLOAT2((float)easeOutQuad(maxTime, start, end - start, time), WinApp::window_height / 2));
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
			for (auto& wireobject : wireobjects) {
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

			player->Update();
			boss->Update();
			AllCollision();

			//ボスの死亡
			if (boss->GetisDead()) {
				scene = Clear;
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
			if (input_->TriggerKey(DIK_SPACE)) {
				isTransition = true;
				nextScene = Title;
			}
		}


		break;
	case Dead:

		for (std::unique_ptr<ParticleManager>& particle : particles)
		{
			particle->Update();
		}
		//戻っている最中は押しても反応しない
		if (isBackTransition) {

		}
		else
		{
			if (input_->TriggerKey(DIK_SPACE)) {
				isTransition = true;
				nextScene = Title;
			}
		}
		break;
	}


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

		//地面の線
		for (auto& wireobject : wireobjects) {
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

		title->Draw();

		startsprite->Draw();

		spriteCommon->PostDraw();

		break;
	case Game:
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


		//敵
		for (std::unique_ptr<Enemy>& enemy : enemys)
		{
			enemy->Draw();
		}
		//地面の線
		for (auto& wireobject : wireobjects) {
			wireobject->Draw();
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
			menu->Draw();
		}

		if (isHit) {
			hitsprite->Draw();
		}

		spriteCommon->PostDraw();
		break;
		//ボス戦
	case BossFight:
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

		boss->Draw();

		//地面の線
		for (auto& wireobject : wireobjects) {
			wireobject->Draw();
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
			menu->Draw();
		}

		if (isHit) {
			hitsprite->Draw();
		}

		spriteCommon->PostDraw();
		break;

	case Clear:

		break;
	}

	//パーティクル
	for (std::unique_ptr<ParticleManager>& particle : particles)
	{
		particle->Draw();
	}

	//スプライト描画
	spriteCommon->PreDraw();

	//遷移演出
	whitesprite->Draw();

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

void GameScene::EnemyParticle(XMFLOAT3 pos_)
{
	XMFLOAT3 posA = pos_;
	posA.z += 5;
	//パーティクル
	std::unique_ptr<ParticleManager>newparticle = std::make_unique<ParticleManager>();
	newparticle->Initialize("line.png");
	newparticle->SetEmitterPos(posA);
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
		partpos = eye;
		partpos.y = eye.y + 5;
		partpos.z = eye.z;

		//パーティクル
		TransitionParticle(partpos);
		if (MaxDelay <= delayTime) {
			//白い画像の位置
			whitesprite->SetPosition(XMFLOAT2(WinApp::window_width / 2, (float)easeOutQuad(MaxTransitionTime, transitionStartPosY, transitionEndposY - transitionStartPosY, totalTransitionTime)));
			whitesprite->Update();

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
		whitesprite->Update();
		if (MaxDelay <= delayTime) {
			//パーティクルの位置
			partpos = eye;
			partpos.y = eye.y - 5;
			partpos.z = eye.z;
			//後半は出ないようにしておく
			if (totalTransitionTime > MaxTransitionTime - 40.0) {

			}
			else {
				//パーティクル
				TransitionBackParticle(partpos, (int)(MaxTransitionTime - totalTransitionTime * 2));
			}

			//白い画像の位置
			whitesprite->SetPosition(XMFLOAT2(WinApp::window_width / 2, (float)easeOutQuad(MaxTransitionTime, transitionEndposY, transitionStartPosY - transitionEndposY, totalTransitionTime)));

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