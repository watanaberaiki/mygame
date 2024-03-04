#include "Player.h"
Input* Player::input = nullptr;
DirectXCommon* Player::dxcommon = nullptr;

void Player::Initialize()
{
	resource = ResourceManager::Getinstance();
	//fbx
	playerFbxModel = FbxLoader::GetInstance()->LoadModelFromFile("bonetest");
	playerFbxObj = new FbxObject3D();
	playerFbxObj->Initialize();
	playerFbxObj->SetModel(playerFbxModel);


	//3dオブジェクト
	playerModel_hp[0] = Model::LoadFromObj("player_hp1");
	playerModel_hp[1] = Model::LoadFromObj("player_hp2");
	playerModel_hp[2] = Model::LoadFromObj("player_hp3");
	playerModel_hp[3] = Model::LoadFromObj("player_hp4");
	
	playerObj = WireObject::Create();
	playerObj->SetModel(playerModel_hp[0]);

	//当たり判定キューブモデル
	cubeModel = new CubeModel();
	cubeModel->CreateBuffers(dxcommon->GetDevice());
	cubeModel->SetImageData(XMFLOAT4(255, 0, 0, 1));

	collisionBox = new CubeObject3D();
	collisionBox->Initialize();
	collisionBox->SetModel(cubeModel);
	collisionBox->Update();

	//レティクル初期化
	reticleModel = new LineModel();
	reticleModel->Initialize(dxcommon->GetDevice(), 0, 0);
	reticleModel->SetImageData(XMFLOAT4(0, 255, 0, 1));
	for (int i = 0; i < maxLine; i++) {
		reticleObject[i] = new LineObject();
		reticleObject[i]->Initialize();
		reticleObject[i]->SetModel(reticleModel);
		if (i%2== 0) {
			reticleObject[i]->SetRotation(XMFLOAT3(0.0f, 0.0f, XMConvertToRadians(90.0f)));
		}
	}

	//レティクルの位置を横の壁でわかりやすく
	reticleLineModel = new LineModel();
	reticleLineModel->Initialize(dxcommon->GetDevice(), 0, 0);
	reticleLineModel->SetImageData(XMFLOAT4(0, 255, 0, 1));
	for (int i = 0; i < maxWidthLine;i++) {
		reticleLineObject[i] = new LineObject();
		reticleLineObject[i]->Initialize();
		reticleLineObject[i]->SetModel(reticleLineModel);
	}

	//まっすぐのレティクル
	reticleLineModelStraight = new LineModel();
	reticleLineModelStraight->Initialize(dxcommon->GetDevice(), 0.0f, 0.0f);
	reticleLineModelStraight->SetImageData(XMFLOAT4(0, 255, 0, 0.5f));
	reticleLineObjectStraight = new LineObject();
	reticleLineObjectStraight->Initialize();
	reticleLineObjectStraight->SetModel(reticleLineModelStraight);
	reticleLineObjectStraight->SetStartPosition({ 0.0f,0.0f,0.0f });

	//レティクルの位置
	frontdepth = 5;
	backdepth = 30;

	input = Input::GetInstance();

}

void Player::Update()
{
	//死んでる判定
	if (life <= 0) {
		isDead = true;
	}
	else {
		isDead = false;
	}

	//ダメージを食らった後の無敵時間
	if (isInvincible) {
		if (maxInvincibleTime > invincibleTime) {
			invincibleTime++;
		}
		else {
			invincibleTime = 0;
			isInvincible = false;
		}
	}

	//デスフラグの立った球を削除
	bullets.remove_if([](std::unique_ptr<PlayerBullet>& bullet) {
		return bullet->GetIsDeath();
		});
	//自機の動き
	Move();
	//弾の発射処理
	Fire();
	//レティクルの動き
	MoveReticle();


	//弾
	for (std::unique_ptr<PlayerBullet>& bullet : bullets)
	{
		bullet->Update();
	}

	////fbx
	//playerfbxobj->SetPosition(position);
	//playerfbxobj->SetScale(scale);
	//playerfbxobj->SetRotation(rotation);
	//playerfbxobj->Update();


	//オブジェクト
	playerObj->SetPosition(position);
	playerObj->SetScale(scale);
	playerObj->SetRotation(rotation);
	playerObj->Update();

	//判定
	collisionBox->SetPosition(position);
	collisionBox->SetScale(XMFLOAT3(scale.x * 2, scale.y * 2, scale.z * 2));
	collisionBox->SetRotation(rotation);
	collisionBox->Update();


	//弾の回転用
	bulletFrontVec = { position.x - position.x,position.y - position.y,backReticlepos.z - position.z };

	//レティクル
	reticleVec = { frontReticlepos.x - (position.x), frontReticlepos.y - (position.y), frontReticlepos.z - position.z };

	//弾の回転用
	lengthFrontVec = bulletFrontVec.length();
	lengthreticleVec = reticleVec.length();
	bulletRotation = acos(bulletFrontVec.dot(reticleVec) / (lengthFrontVec * lengthreticleVec));

	//正規化
	reticleVec.normalize();
	////手前レティクル
	//frontVec = reticleVec * frontdepth;
	//frontReticlepos = XMFLOAT3(frontVec.x + position.x, frontVec.y + position.y, frontVec.z + position.z);
	frontReticlepos.z = frontdepth + position.z;
	//手前の後ろ
	frontBackVec = reticleVec * ((backdepth-frontdepth)/2);
	frontBackReticlepos = XMFLOAT3(frontBackVec.x + position.x, frontBackVec.y + position.y, frontBackVec.z + position.z);
	//奥側の前
	backFrontVec = reticleVec * ((backdepth - frontdepth)/2+frontdepth);
	backFrontReticlepos= XMFLOAT3(backFrontVec.x + position.x, backFrontVec.y + position.y, backFrontVec.z + position.z);

	//奥側のレティクル
	backVec= reticleVec * backdepth;
	backReticlepos= XMFLOAT3(backVec.x + position.x, backVec.y + position.y, backVec.z + position.z);

	//backReticlepos.z = backdepth+position.z;

	for (int i = 0; i < maxLine; i++) {
		XMFLOAT3 reticlePos = {};
		//手前
		if (i == 0 || i == 1) {
			reticlePos = frontReticlepos;
			if (i == 1) {
				reticlePos.x -= linePos;
				reticleObject[i]->SetStartPosition(reticlePos);
				reticlePos = frontReticlepos;
				reticlePos.x += linePos;
				reticleObject[i]->SetEndPosition(reticlePos);
			}
			else {
				reticlePos.y -= linePos;
				reticleObject[i]->SetStartPosition(reticlePos);
				reticlePos = frontReticlepos;
				reticlePos.y += linePos;
				reticleObject[i]->SetEndPosition(reticlePos);
			}
		}
		//手前の後ろ
		else if (i == 2 || i == 3) {
			reticlePos = frontBackReticlepos;
			if (i == 2) {
				reticlePos.x -= linePos;
				reticleObject[i]->SetStartPosition(reticlePos);
				reticlePos = frontBackReticlepos;
				reticlePos.x += linePos;
				reticleObject[i]->SetEndPosition(reticlePos);
			}
			else {
				reticlePos.y -= linePos;
				reticleObject[i]->SetStartPosition(reticlePos);
				reticlePos = frontBackReticlepos;
				reticlePos.y += linePos;
				reticleObject[i]->SetEndPosition(reticlePos);
			}
		}
		//奥側の前
		else if (i == 4 || i == 5) {
			reticlePos = backFrontReticlepos;
			if (i == 4) {
				reticlePos.x -= linePos;
				reticleObject[i]->SetStartPosition(reticlePos);
				reticlePos = backFrontReticlepos;
				reticlePos.x += linePos;
				reticleObject[i]->SetEndPosition(reticlePos);
			}
			else {
				reticlePos.y -= linePos;
				reticleObject[i]->SetStartPosition(reticlePos);
				reticlePos = backFrontReticlepos;
				reticlePos.y += linePos;
				reticleObject[i]->SetEndPosition(reticlePos);
			}
		}
		//奥側
		else if (i == 6 || i == 7) {
			reticlePos = backReticlepos;
			if (i == 6) {
				reticlePos.x -= linePos;
				reticleObject[i]->SetStartPosition(reticlePos);
				reticlePos = backReticlepos;
				reticlePos.x += linePos;
				reticleObject[i]->SetEndPosition(reticlePos);
			}
			else {
				reticlePos.y -= linePos;
				reticleObject[i]->SetStartPosition(reticlePos);
				reticlePos = backReticlepos;
				reticlePos.y += linePos;
				reticleObject[i]->SetEndPosition(reticlePos);
			}
		}
		reticleObject[i]->Update();
	}

	//レティクルの横の壁に表示
	const float widthLineY = 0.5f;
	for (int i = 0; i < maxWidthLine;i++) {
		XMFLOAT3 pos = {};
		pos = reticleObject[i]->GetStartPosition();
		pos.y = -widthLineY;
		pos.x = 0;
		XMFLOAT3 widthLinePos = {};
		if (i%2==0) {
			pos.x += widthSpace;
			widthLinePos = pos;
			widthLinePos.y += linePos;
			reticleLineObject[i]->SetStartPosition(widthLinePos);
			widthLinePos = pos;
			widthLinePos.y -= linePos;
			reticleLineObject[i]->SetEndPosition(widthLinePos);
		}
		else {
			pos.x -= widthSpace;
			widthLinePos = pos;
			widthLinePos.y += linePos;
			reticleLineObject[i]->SetStartPosition(widthLinePos);
			widthLinePos = pos;
			widthLinePos.y -= linePos;
			reticleLineObject[i]->SetEndPosition(widthLinePos);
		}
		reticleLineObject[i]->Update();
	}

	//判定
	if (isEnemyReticleCol) {
		reticleLineModelStraight->SetImageData(XMFLOAT4(0, 255, 255, 0.5f));
		reticleModel->SetImageData(XMFLOAT4(0, 255, 255, 1));
		reticleLineModel->SetImageData(XMFLOAT4(0, 255, 255, 1));
	}
	else {
		reticleLineModelStraight->SetImageData(XMFLOAT4(0, 255, 0, 0.5f));
		reticleModel->SetImageData(XMFLOAT4(0, 255, 0, 1));
		reticleLineModel->SetImageData(XMFLOAT4(0, 255, 0, 1));
	}

	//まっすぐのレティクル
	reticleLineObjectStraight->SetStartPosition(position);
	reticleLineObjectStraight->SetEndPosition(backReticlepos);
	reticleLineObjectStraight->Update();
}

void Player::Draw()
{
	////fbx
	//playerfbxobj->Draw(cmdList);



	//弾
	for (std::unique_ptr<PlayerBullet>& bullet : bullets)
	{
		bullet->Draw();
	}


}

void Player::WireDraw()
{
	//無敵の時の点滅
	if (invincibleTime % 2 == 0) {
		//オブジェクト
		playerObj->Draw();
	}
}

void Player::DebugDraw(ID3D12GraphicsCommandList* cmdList)
{
	////弾
	//for (std::unique_ptr<PlayerBullet>& bullet : bullets)
	//{
	//	//bullet->DebugDraw(cmdList);
	//}

	//collisionBox->Draw(cmdList);

	//レティクル
	for (int i = 0; i < maxLine; i++) {
		reticleObject[i]->Draw(cmdList);
	}
	//レティクルの横の壁
	for (int i = 0; i < maxWidthLine; i++) {
		reticleLineObject[i]->Draw(cmdList);
	}
	//まっすぐレティクル
	reticleLineObjectStraight->Draw(cmdList);
}

void Player::Move()
{
	if (isTitle == false) {
		//キーボードでの移動
		if (input->PushKey(DIK_W) || input->PushKey(DIK_S) || input->LStickUp() || input->LStickDown()) {
			if (input->PushKey(DIK_W) || input->LStickUp()) {
				if (position.y < 1.5) {
					position.y += 0.1f;
				}
			}
			else if (input->PushKey(DIK_S) || input->LStickDown()) {
				if (position.y > -1.5) {
					position.y -= 0.1f;
				}
			}
		}

		if (input->PushKey(DIK_A) || input->PushKey(DIK_D) || input->LStickLeft() || input->LStickRight()) {
			if (input->PushKey(DIK_A) || input->LStickLeft()) {
				if (position.x > -2.5) {
					position.x -= 0.1f;
				}
			}
			else if (input->PushKey(DIK_D) || input->LStickRight()) {
				if (position.x < 2.5) {
					position.x += 0.1f;
				}
			}
		}
	}
	else {

	}

}

void Player::MoveReticle()
{
	//レティクルの移動
	if (input->PushKey(DIK_UP) || input->PushKey(DIK_DOWN)||input->RStickUp()||input->RStickDown()) {
		if (input->PushKey(DIK_UP)|| input->RStickUp()) {
			if (frontReticlepos.y< reticleUpLimit) {
				frontReticlepos.y += 0.05f;
			}
		}
		else if (input->PushKey(DIK_DOWN) || input->RStickDown()) {
			if (frontReticlepos.y >-reticleDownLimit ) {
				frontReticlepos.y -= 0.05f;
			}
		}
	}

	if (input->PushKey(DIK_LEFT) || input->PushKey(DIK_RIGHT) || input->RStickLeft() || input->RStickRight()) {
		if (input->PushKey(DIK_LEFT) || input->RStickLeft()) {
			if (frontReticlepos.x > -reticleLeftLimit) {
				frontReticlepos.x -= 0.05f;
			}
		}
		else if (input->PushKey(DIK_RIGHT) || input->RStickRight()) {
			if (frontReticlepos.x < reticleRightLimit) {
				frontReticlepos.x += 0.05f;
			}
		}
	}
}

void Player::Fire()
{
	//弾のフルオートの間隔
	if (bulletDireyMaxTime > bulletDireyTime) {
		bulletDireyTime++;
	}

	if (isTitle == false) {
		velocity = XMFLOAT3(reticleVec.x, reticleVec.y, reticleVec.z);
		if (input->PushKey(DIK_SPACE) || input->PushRButton()) {
			//弾フルオート
			if (bulletDireyTime >= bulletDireyMaxTime) {
				std::unique_ptr<PlayerBullet>newObject = std::make_unique<PlayerBullet>();
				newObject->Initialize(dxcommon, resource, velocity, bulletRotation);
				newObject->SetPosition(position);
				bullets.push_back(std::move(newObject));
				bulletDireyTime = 0;
			}
		}
	}
}

void Player::OnCollision()
{
	if (!isInvincible) {
		life--;
		isInvincible = true;
		if (life != 0) {
			playerObj->SetModel(playerModel_hp[maxLife - life]);
		}
	}
	else {

	}
	

}

void Player::Reset()
{
	life = maxLife;
	position = { 0,0,0 };
	playerObj->SetModel(playerModel_hp[maxLife - life]);
}
