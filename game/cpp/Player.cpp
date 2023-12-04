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

	//ライン初期化
	lineModel = new LineModel();
	lineModel->Initialize(dxcommon->GetDevice(), 0.2f, -0.2f);
	lineModel->SetImageData(XMFLOAT4(0, 255, 0, 1));
	for (int i = 0; i < maxLine; i++) {
		lineObject[i] = new LineObject();
		lineObject[i]->Initialize();
		lineObject[i]->SetModel(lineModel);
		if (i%2== 0) {
			lineObject[i]->SetRotation(XMFLOAT3(0.0f, 0.0f, XMConvertToRadians(90.0f)));
		}
	}
	////XMConvertToDegrees
	//lineObject[0]->SetRotation(XMFLOAT3(0.0f, 0.0f, XMConvertToRadians(90.0f)));
	//lineObject[2]->SetRotation(XMFLOAT3(0.0f, 0.0f, XMConvertToRadians(90.0f)));

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
	reticleVec = { backReticlepos.x - (position.x), backReticlepos.y - (position.y), backReticlepos.z - position.z };

	//弾の回転用
	lengthFrontVec = bulletFrontVec.length();
	lengthreticleVec = reticleVec.length();
	bulletRotation = acos(bulletFrontVec.dot(reticleVec) / (lengthFrontVec * lengthreticleVec));

	//正規化
	reticleVec.normalize();
	//手前レティクル
	frontVec = reticleVec * frontdepth;
	frontReticlepos = XMFLOAT3(frontVec.x + position.x, frontVec.y + position.y, frontVec.z + position.z);
	//手前の後ろ
	frontBackVec = reticleVec * ((backdepth-frontdepth)/2);
	frontBackReticlepos = XMFLOAT3(frontBackVec.x + position.x, frontBackVec.y + position.y, frontBackVec.z + position.z);
	//奥側の前
	backFrontVec = reticleVec * ((backdepth - frontdepth)/2+frontdepth);
	backFrontReticlepos= XMFLOAT3(backFrontVec.x + position.x, backFrontVec.y + position.y, backFrontVec.z + position.z);

	//奥側のレティクル
	//backVec= reticleVec * backdepth;
	//backReticlepos= XMFLOAT3(backVec.x, backVec.y, backVec.z);
	backReticlepos.z = backdepth+position.z;

	for (int i = 0; i < maxLine; i++) {
		//手前
		if (i==0||i==1) {
			lineObject[i]->SetPosition(frontReticlepos);
		}
		//手前の後ろ
		else if (i == 2 || i == 3) {
			lineObject[i]->SetPosition(frontBackReticlepos);
		}
		//奥側の前
		else if (i == 4 || i == 5) {
			lineObject[i]->SetPosition(backFrontReticlepos);
		}
		//奥側
		else if (i == 6 || i == 7) {
			lineObject[i]->SetPosition(backReticlepos);
		}
		lineObject[i]->Update();
	}
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

	for (int i = 0; i < maxLine; i++) {
		lineObject[i]->Draw(cmdList);
	}
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
			if (backReticlepos.y< reticleUpLimit) {
				backReticlepos.y += 0.3f;
			}
		}
		else if (input->PushKey(DIK_DOWN) || input->RStickDown()) {
			if (backReticlepos.y >-reticleDownLimit ) {
				backReticlepos.y -= 0.3f;
			}
		}
	}

	if (input->PushKey(DIK_LEFT) || input->PushKey(DIK_RIGHT) || input->RStickLeft() || input->RStickRight()) {
		if (input->PushKey(DIK_LEFT) || input->RStickLeft()) {
			if (backReticlepos.x > -reticleLeftLimit) {
				backReticlepos.x -= 0.3f;
			}
		}
		else if (input->PushKey(DIK_RIGHT) || input->RStickRight()) {
			if (backReticlepos.x < reticleRightLimit) {
				backReticlepos.x += 0.3f;
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
