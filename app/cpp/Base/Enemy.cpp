#include "Enemy.h"
DirectXCommon* Enemy::dxcommon = nullptr;
Player* Enemy::player = nullptr;

void Enemy::Initialize()
{
	resource = ResourceManager::Getinstance();

	//fbx
	enemyfbxmodel = resource->LoadFbx("boneTest");
	enemyfbxobj = new FbxObject3D();
	enemyfbxobj->Initialize();
	enemyfbxobj->SetModel(enemyfbxmodel);

	//3dオブジェクト
	enemymodel = resource->LoadObj("redcube");
	enemyobj = WireObject::Create();
	enemyobj->SetModel(enemymodel);

	//当たり判定キューブモデル
	cubeModel = new CubeModel();
	cubeModel->CreateBuffers(dxcommon->GetDevice());
	cubeModel->SetImageData(XMFLOAT4(255, 0, 0, 1));

	collisionBox = new CubeObject3D();
	collisionBox->Initialize();
	collisionBox->SetModel(cubeModel);
	collisionBox->Update();

}

void Enemy::Update()
{
	Move();

	if (player->GetPosition().z -position.z>=10.0f) {
		isdead = true;
	}


	//デスフラグの立った球を削除
	bullets.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {
		return bullet->GetIsDeath();
		});

	//弾の発射
	time++;
	if (time >= MaxTime) {
		//プレイヤーの裏に行ったら弾を撃たなくなる
		if (player->GetPosition().z < position.z) {
			if (position.z- player->GetPosition().z<=40.0f) {
				Fire();
				time = 0;
			}
		}
	}
	//弾
	for (std::unique_ptr<EnemyBullet>& bullet : bullets)
	{
		bullet->Update();
	}

	////fbx
	//enemyfbxobj->SetPosition(position);
	//enemyfbxobj->SetScale(scale);
	//enemyfbxobj->SetRotation(rotation);
	//enemyfbxobj->Update();


	//オブジェクト
	enemyobj->SetPosition(position);
	enemyobj->SetScale(scale);
	enemyobj->SetRotation(rotation);
	enemyobj->Update();

	//判定
	collisionBox->SetPosition(position);
	collisionBox->SetScale(XMFLOAT3(scale.x * 2, scale.y * 2, scale.z * 2));
	collisionBox->SetRotation(rotation);
	collisionBox->Update();
}

void Enemy::Draw()
{
	//fbx
	//enemyfbxobj->Draw(cmdList);

	////オブジェクト
	enemyobj->Draw();

	//弾
	for (std::unique_ptr<EnemyBullet>& bullet : bullets)
	{
		bullet->Draw();
	}

}

void Enemy::DebugDraw(ID3D12GraphicsCommandList* cmdList)
{
	//弾
	for (std::unique_ptr<EnemyBullet>& bullet : bullets)
	{
		bullet->DebugDraw(cmdList);
	}

	collisionBox->Draw(cmdList);
}

void Enemy::Move()
{
	switch (type)
	{
		//z方向のみ
	case Move_z:
		position.z -= speedZ;
		break;
		//xz方向
	case Move_xz:
		position.z -= speedZ;

		if (plusX) {
			if (position.x < MoveX) {
				position.x += speedX;
			}
			else {
				plusX = false;
			}
		}
		else {
			if (position.x > -MoveX) {
				position.x -= speedX;
			}
			else {
				plusX = true;
			}
		}
		break;
		//yz方向
	case Move_yz:
		position.z -= speedZ;
		if (plusY) {
			if (position.y < MoveY) {
				position.y += speedY;
			}
			else {
				plusY = false;
			}
		}
		else {
			if (position.y > -MoveY) {
				position.y -= speedY;
			}
			else {
				plusY = true;
			}
		}



		break;
		//xyz方向
	case Move_xyz:
		position.z -= speedZ;

		if (plusX) {
			if (position.x < MoveX) {
				position.x += speedX;
			}
			else {
				plusX = false;
			}
		}
		else {
			if (position.x > -MoveX) {
				position.x -= speedX;
			}
			else {
				plusX = true;
			}
		}

		if (plusY) {
			if (position.y < MoveY) {
				position.y += speedY;
			}
			else {
				plusY = false;
			}
		}
		else {
			if (position.y > -MoveY) {
				position.y -= speedY;
			}
			else {
				plusY = true;
			}
		}

		break;
	default:
		break;
	}
}

void Enemy::Fire()
{
	velocityvec = { player->GetPosition().x - position.x,player->GetPosition().y - position.y, player->GetPosition().z - position.z };
	velocityvec.normalize();

	velocity = XMFLOAT3(velocityvec.x, velocityvec.y, velocityvec.z);
	std::unique_ptr<EnemyBullet>newObject = std::make_unique<EnemyBullet>();
	newObject->Initialize(dxcommon, resource, velocity);
	newObject->SetPosition(position);
	bullets.push_back(std::move(newObject));
}

void Enemy::OnCollision()
{
	isdead = true;
}
