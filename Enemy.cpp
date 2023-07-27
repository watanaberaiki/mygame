#include "Enemy.h"
DirectXCommon* Enemy::dxcommon = nullptr;

void Enemy::Initialize()
{
	resource=ResourceManager::Getinstance();

	//fbx
	enemyfbxmodel = resource->LoadFbx("boneTest");
	enemyfbxobj = new FbxObject3D();
	enemyfbxobj->Initialize();
	enemyfbxobj->SetModel(enemyfbxmodel);

	//3dオブジェクト
	enemymodel = resource->LoadObj("redcube");
	enemyobj = Object3d::Create();
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

	//デスフラグの立った球を削除
	bullets.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {
		return bullet->GetIsDeath();
		});

	//弾の発射
	time++;
	if (time >= MaxTime) {
		Fire();
		time = 0;
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

void Enemy::Draw(ID3D12GraphicsCommandList* cmdList)
{
	//fbx
	//enemyfbxobj->Draw(cmdList);

	////オブジェクト
	enemyobj->Draw();

	//弾
	for (std::unique_ptr<EnemyBullet>& bullet : bullets)
	{
		bullet->Draw(cmdList);
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
	position.z -= speed;
}

void Enemy::Fire()
{
	std::unique_ptr<EnemyBullet>newObject = std::make_unique<EnemyBullet>();
	newObject->Initialize(dxcommon,resource);
	newObject->SetPosition(position);
	bullets.push_back(std::move(newObject));
}

void Enemy::OnCollision()
{

}
