#include "Enemy.h"

void Enemy::Initialize()
{
	enemymodel= FbxLoader::GetInstance()->LoadModelFromFile("boneTest");
	enemyobj = new FbxObject3D();
	enemyobj->Initialize();
	enemyobj->SetModel(enemymodel);
}

void Enemy::Update()
{
	Move();
	enemyobj->SetPosition(position);
	enemyobj->SetScale(scale);
	enemyobj->SetRotation(rotation);
	enemyobj->Update();
}

void Enemy::Draw(ID3D12GraphicsCommandList* cmdList)
{
	enemyobj->Draw(cmdList);
}

void Enemy::Move()
{
	position.z -= speed;
}
