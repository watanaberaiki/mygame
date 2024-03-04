#include "TitleScene.h"
#include"GamePlayScene.h"
#include"SceneManager.h"
void TitleScene::Initialize(DirectXCommon* dxCommon, ImguiManager* imgui)
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

	//読み込み
	resorcemanager = ResourceManager::Getinstance();
	resorcemanager->LoadObj("blackcube");
	resorcemanager->LoadObj("redcube");
	resorcemanager->LoadObj("floor");

	//プレイヤー
	Player::SetDxCommon(dxCommon);
	player->Initialize();

	//敵
	enemycsv->LoadCSV("Resources/csv/enemy.csv");
	Enemy::SetPlayer(player);
	Enemy::SetDxCommon(dxCommon);

	//ボス
	Boss::SetPlayer(player);
	Boss::SetDxCommon(dxCommon);
	boss->Initialize();
	boss->SetRotation(XMFLOAT3(0, 0, 0));
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


	//スプライト初期位置
	const XMFLOAT2 center = { 0.5f,0.5f };
	marioSprite->SetPosition({ 800,0 });
	marioSprite->Update();

	//右スティック
	stickRSprite->SetAnchorPoint(center);
	stickRSprite->SetPosition({ WinApp::window_width - stickRSprite->GetSize().x / 2,stickRSprite->GetSize().y / 2 });
	//左スティック
	stickLSprite->SetAnchorPoint(center);
	stickLSprite->SetPosition({ WinApp::window_width - stickRSprite->GetSize().x / 2,stickRSprite->GetSize().y * 1.5f });
	//右トリガー
	buttonRTSprite->SetAnchorPoint(center);
	buttonRTSprite->SetPosition({ WinApp::window_width - stickRSprite->GetSize().x / 2,stickRSprite->GetSize().y * 2.5f });

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
	lineModel->Initialize(dxCommon->GetDevice(), 0, 0);
	lineModel->SetImageData(XMFLOAT4(255, 255, 255, 1));

	//パーティクル
	particles->Initialize("line.png");

	redParticles->Initialize("redline.png");


	isTitle = true;
	eye = eyeReset;
	target = targetReset;
	camera->SetEye(eye);
	camera->SetTarget(target);
	camera->Update();
}
void TitleScene::Finalize()
{

}
void TitleScene::Update() {
	//遷移演出用
	if (isTransition || isBackTransition) {
		Transition(nextScene);
	}
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
	floorDirectionPosX = (float)easeOutQuad(directionMaxTime, floorStartPosX, floorEndPosX - floorStartPosX, directionTime);
	for (auto& lineobject : lineObjects) {
		lineobject->SetStartPositionX(floorDirectionPosX);
		lineobject->SetEndPositionX(-floorDirectionPosX);
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
		if (input_->TriggerKey(DIK_SPACE) || input_->TriggerPadButton(XINPUT_GAMEPAD_A)) {
			//初期化関連
			isGameOver = false;
			isStart = true;
			isEnemyAlive = true;
			lineLose = false;
			stickRCount = 0;
			stickLCount = 0;
			buttonRTCount = 0;
			stickRAlpha = 1.0f;
			stickLAlpha = 1.0f;
			buttonRTAlpha = 1.0f;
			//右スティック
			stickRSprite->Update();
			//左スティック
			stickLSprite->Update();
			//右トリガー
			buttonRTSprite->Update();
			enemys.clear();
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
		}
	}
	//パーティクル更新処理
	particles->Update();

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
			//ゲームシーンにチェンジ
			else if (directionMaxTime < directionTime) {
				isStart = false;
				totalTime = 0;
				directionTime = 0;
				isTitle = false;
				//ゲームシーン生成
				BaseScene* changeScene = new GamePlayScene();
				sceneManager_->SetNextScene(changeScene);
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
}
//描画
void TitleScene::Draw()
{
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
	particles->Draw();


	//スプライト
	spriteCommon->PreDraw();

	titleSprite->Draw();

	startSprite->Draw();

	//遷移演出
	transitionWhiteSprite->Draw();

	spriteCommon->PostDraw();

}
//タイトルシーンのパーティクル処理
void TitleScene::TitleParticle(XMFLOAT3 pos_)
{
	//パーティクル
	/*std::unique_ptr<ParticleManager>newparticle = std::make_unique<ParticleManager>();
	newparticle->Initialize("line.png");*/
	//パーティクル
	particles->SetEmitterPos(pos_);
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
		particles->Add(life, pos, vel, acc, 0.5f, 0.1f, color, false);
	}
	particles->Update();
}