#include "Player.h"
Input* Player::input = nullptr;
void Player::Initialize()
{
	playermodel = FbxLoader::GetInstance()->LoadModelFromFile("bonetest");
	playerobj = new FbxObject3D();
	playerobj->Initialize();
	playerobj->SetModel(playermodel);

	for (int i = 0; i < bulletsize; i++) {
		std::unique_ptr<PlayerBullet>newObject = std::make_unique<PlayerBullet>();
		newObject->Initialize();
		bullets.push_back(std::move(newObject));
	}

}

void Player::Update()
{
	if (input->TriggerKey(DIK_SPACE)) {
		Fire();
	}

	Move();
	playerobj->SetPosition(position);
	playerobj->SetScale(scale);
	playerobj->SetRotation(rotation);
	playerobj->Update();

	for (std::unique_ptr<PlayerBullet>& bullet : bullets)
	{
		if (bullet->GetIsFIre() == false) 
		{
			bullet->SetPosition(position);
		}
		bullet->Update();
	}
}

void Player::Draw(ID3D12GraphicsCommandList* cmdList)
{
	playerobj->Draw(cmdList);
	for (std::unique_ptr<PlayerBullet>& bullet : bullets)
	{
		if (bullet->GetIsFIre())
		{
			bullet->Draw(cmdList);
		}
	}

}

void Player::Move()
{
	//キーボードでの移動
	if (input->PushKey(DIK_W) || input->PushKey(DIK_S)) {
		if (input->PushKey(DIK_W)) {
			position.y += 0.1f;
		}
		else if (input->PushKey(DIK_S)) {
			position.y -= 0.1f;
		}
	}

	if (input->PushKey(DIK_A) || input->PushKey(DIK_D)) {
		if (input->PushKey(DIK_A)) {
			position.x -= 0.1f;
		}
		else if (input->PushKey(DIK_D)) {
			position.x += 0.1f;
		}
	}
}

void Player::Fire()
{
	for (std::unique_ptr<PlayerBullet>& bullet : bullets)
	{
		if (bullet->GetIsFIre()==false) {
			bullet->SetIsFire(true);
			break;
		}
	}
}
