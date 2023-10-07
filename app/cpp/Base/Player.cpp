#include "Player.h"
Input* Player::input = nullptr;
DirectXCommon* Player::dxcommon = nullptr;

void Player::Initialize()
{
	resource = ResourceManager::Getinstance();
	//fbx
	playerfbxmodel = FbxLoader::GetInstance()->LoadModelFromFile("bonetest");
	playerfbxobj = new FbxObject3D();
	playerfbxobj->Initialize();
	playerfbxobj->SetModel(playerfbxmodel);


	//3dオブジェクト
	playermodel = Model::LoadFromObj("block");
	playerobj = WireObject::Create();
	playerobj->SetModel(playermodel);

	//当たり判定キューブモデル
	cubeModel = new CubeModel();
	cubeModel->CreateBuffers(dxcommon->GetDevice());
	cubeModel->SetImageData(XMFLOAT4(255, 0, 0, 1));

	collisionBox = new CubeObject3D();
	collisionBox->Initialize();
	collisionBox->SetModel(cubeModel);
	collisionBox->Update();

	//ライン初期化
	linemodel = new LineModel();
	linemodel->Initialize(dxcommon->GetDevice(), 0.2f, -0.2f);
	linemodel->SetImageData(XMFLOAT4(255, 255, 255, 1));
	for (int i = 0; i < 4; i++) {
		lineobject[i] = new LineObject();
		lineobject[i]->Initialize();
		lineobject[i]->SetModel(linemodel);
	}
	//XMConvertToDegrees
	lineobject[0]->SetRotation(XMFLOAT3(0.0f, 0.0f, XMConvertToRadians(90.0f)));
	lineobject[2]->SetRotation(XMFLOAT3(0.0f, 0.0f, XMConvertToRadians(90.0f)));

	//レティクルの位置
	frontdepth = 10;
	backdepth = 25;

}

void Player::Update()
{
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
	playerobj->SetPosition(position);
	playerobj->SetScale(scale);
	playerobj->SetRotation(rotation);
	playerobj->Update();

	//判定
	collisionBox->SetPosition(position);
	collisionBox->SetScale(XMFLOAT3(scale.x * 2, scale.y * 2, scale.z * 2));
	collisionBox->SetRotation(rotation);
	collisionBox->Update();


	//レティクル
	reticleVec = { backReticlepos.x - (position.x), backReticlepos.y - (position.y), backReticlepos.z - position.z };
	//正規化
	reticleVec.normalize();
	//手前レティクル
	frontVec = reticleVec * frontdepth;
	frontReticlepos = XMFLOAT3(frontVec.x + position.x, frontVec.y + position.y, frontVec.z + position.z);	
	//奥側のレティクル
	//backVec= reticleVec * backdepth;
	//backReticlepos= XMFLOAT3(backVec.x, backVec.y, backVec.z);
	backReticlepos.z = backdepth+position.z;

	for (int i = 0; i < 4; i++) {
		if (i < 2) {
			lineobject[i]->SetPosition(frontReticlepos);
		}
		else {
			lineobject[i]->SetPosition(backReticlepos);
		}
		lineobject[i]->Update();
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
	//オブジェクト
	playerobj->Draw();
}

void Player::DebugDraw(ID3D12GraphicsCommandList* cmdList)
{
	////弾
	//for (std::unique_ptr<PlayerBullet>& bullet : bullets)
	//{
	//	//bullet->DebugDraw(cmdList);
	//}

	//collisionBox->Draw(cmdList);

	for (int i = 0; i < 4; i++) {
		lineobject[i]->Draw(cmdList);
	}
}

void Player::Move()
{
	if (isstart == false) {
		//キーボードでの移動
		if (input->PushKey(DIK_W) || input->PushKey(DIK_S)) {
			if (input->PushKey(DIK_W)) {
				if (position.y < 1.5) {
					position.y += 0.1f;
				}
			}
			else if (input->PushKey(DIK_S)) {
				if (position.y > -1.5) {
					position.y -= 0.1f;
				}
			}
		}

		if (input->PushKey(DIK_A) || input->PushKey(DIK_D)) {
			if (input->PushKey(DIK_A)) {
				if (position.x > -2.5) {
					position.x -= 0.1f;
				}
			}
			else if (input->PushKey(DIK_D)) {
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
	//キーボードでの移動
	if (input->PushKey(DIK_UP) || input->PushKey(DIK_DOWN)) {
		if (input->PushKey(DIK_UP)) {
			backReticlepos.y += 0.3f;
		}
		else if (input->PushKey(DIK_DOWN)) {
			backReticlepos.y -= 0.3f;
		}
	}

	if (input->PushKey(DIK_LEFT) || input->PushKey(DIK_RIGHT)) {
		if (input->PushKey(DIK_LEFT)) {
			backReticlepos.x -= 0.3f;
		}
		else if (input->PushKey(DIK_RIGHT)) {
			backReticlepos.x += 0.3f;
		}
	}
}

void Player::Fire()
{
	velocity = XMFLOAT3(reticleVec.x, reticleVec.y, reticleVec.z);
	if (input->TriggerKey(DIK_SPACE)) {
		std::unique_ptr<PlayerBullet>newObject = std::make_unique<PlayerBullet>();
		newObject->Initialize(dxcommon, resource, velocity);
		newObject->SetPosition(position);
		bullets.push_back(std::move(newObject));
	}
}

void Player::OnCollision()
{
}
